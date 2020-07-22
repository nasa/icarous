from ctypes import byref
import numpy as np

from Interfaces import (CommandTypes,GeofenceConflict)
from Cognition import (Cognition,
                        CognitionParams)
from Guidance import (Guidance,
                      GuidanceParam,
                      GuidanceMode)
from Geofence import (Getfence,GeofenceMonitor)
from Trajectory import Trajectory
from TrafficMonitor import TrafficMonitor
from Merger import (MAX_NODES,Merger,LogData,MergerData)
from VehicleSim import (VehicleSim,
                        StartTraffic,
                        RunTraffic)
from ichelper import (ConvertTrkGsVsToVned,
                      distance,
                      gps_offset,
                      ConvertVnedToTrkGsVs,
                      ComputeHeading,
                      LoadIcarousParams,
                      ReadFlightplanFile)
import time

class Icarous():
    def __init__(self, initialPos, simtype="UAS_ROTOR", vehicleID = 0,fasttime = True, verbose=0,logName = ""):
        self.fasttime = fasttime
        self.logName = logName
        self.verbose = verbose
        self.home_pos = [initialPos[0], initialPos[1], initialPos[2]]
        self.traffic = []
        if simtype == "UAM_VTOL":
            self.ownship = VehicleSim(vehicleID,0.0,0.0,0.0,0.0,0.0,0.0)
        else:
            from quadsim import QuadSim
            self.ownship = QuadSim()

        self.vehicleID = vehicleID
        self.Cog = Cognition("vehicle"+str(vehicleID))
        self.Guidance = Guidance(GuidanceParam())
        self.Geofence = GeofenceMonitor([3,2,2,20,20])
        self.Trajectory = Trajectory(5.0,250)
        self.tfMonitor = TrafficMonitor(False)
        self.Merger = Merger("AC",vehicleID,self.logName)
        self.daafile = "data/DaidalusQuadConfig.txt"

        # Aircraft data
        self.flightplan1 = []
        self.etaFP1      = False
        self.etaFP2      = False
        self.flightplan2 = []
        self.controlInput = [0.0,0.0,0.0]
        self.fenceList   = []

        self.guidanceMode = GuidanceMode.NOOP

        # Merger
        self.arrTime = None
        self.logLatency = 0
        self.prevLogUpdate = 0
        self.mergerLog = LogData()

        self.position = self.home_pos
        self.velocity = [0.0,0.0,0.0]
        self.trkgsvs  = [0.0,0.0,0.0]

        self.positionLog = []
        self.ownshipLog = {"t": [], "position": [], "velocityNED": [], "positionNED": [], 
                           "trkbands":[],"gsbands":[],"altbands":[],"vsbands":[]}
        self.trafficLog = {}
        self.emergbreak = False
        if self.fasttime:
            self.currTime = 0
        else:
            self.currTime = time.time()
        self.numSecPlan = 0
        self.plans = []
        self.localPlans = []
        self.localFences= []
        self.daa_radius = 0
        self.startSent = False
        self.nextWP1 = 1
        self.nextWP2 = 1
        self.numFences = 0
        self.resSpeed = 0
        self.defaultWPSpeed = 1
        self.missionComplete = False
        self.fphases = -1
        self.land    = False
        self.activePlan = "Plan0"
        self.windFrom = 0
        self.windSpeed = 0

    def setpos_uncertainty(self,xx,yy,zz,xy,yz,xz,coeff=0.8):
        self.ownship.setpos_uncertainty(xx,yy,zz,xy,yz,xz,coeff)

    def InputWind(self,windFrom,windSpeed):
        self.windFrom = windFrom
        self.windSpeed = windSpeed

    def InputTraffic(self,idx,position,velocity,localPos):
        if idx is not self.vehicleID:
            trkgsvs = ConvertVnedToTrkGsVs(velocity[1],velocity[0],velocity[2])
            self.tfMonitor.input_traffic(idx,position,trkgsvs,self.currTime)
            self.Trajectory.InputTrafficData(idx,position,velocity)
            self.RecordTraffic(idx, position, velocity, localPos)

    def InputFlightplan(self,fp,scenarioTime,eta=False):
        
        self.flightplan1 = fp 
        self.flightplan2 = []
        self.etaFP1 = eta
        self.plans.append(fp)
        self.localPlans.append(self.GetLocalFlightPlan(fp))
        self.Trajectory.InputFlightplan("Plan0",fp,eta)
        self.Cog.InputFlightplanData("Plan0",scenarioTime,fp,eta)
        self.Guidance.InputFlightplanData("Plan0",scenarioTime,fp,eta)

    def InputFlightplanFromFile(self,filename,scenarioTime=0,eta=False):
        wp, time, speed = ReadFlightplanFile(filename)
        fp = []
        if eta:
            combine = time
        else:
            combine = speed
        for w,i in zip(wp,combine):
            if not eta and i < 0:
                i = self.defaultWPSpeed
            fp.append(w + [i])
        self.InputFlightplan(fp,scenarioTime,eta)

    def ConvertToLocalCoordinates(self,pos):
        dh = lambda x, y: abs(distance(self.home_pos[0],self.home_pos[1],x,y))
        if pos[1] > self.home_pos[1]:
            sgnX = 1
        else:
            sgnX = -1

        if pos[0] > self.home_pos[0]:
            sgnY = 1
        else:
            sgnY = -1
        dx = dh(self.home_pos[0],pos[1])
        dy = dh(pos[0],self.home_pos[1])
        return [dy*sgnY,dx*sgnX,pos[2]]

    def GetLocalFlightPlan(self,fp):
        local = []
        for wp in fp:
            local.append(self.ConvertToLocalCoordinates(wp))
        return local

    def InputGeofence(self,filename):
        self.fenceList = Getfence(filename)
        for fence in self.fenceList:
            self.Geofence.InputData(fence)
            self.Trajectory.InputGeofenceData(fence)
            self.numFences += 1

            localFence = []
            for vertex in fence['Vertices']:
                localFence.append(self.ConvertToLocalCoordinates([*vertex,0]))

            self.localFences.append(localFence)


    def InputMergeFixes(self,filename):
        wp, ind, _ = ReadFlightplanFile(filename)
        self.Merger.SetIntersectionData(list(zip(ind,wp)))


    def SetGeofenceParams(self,params):
        gfparams = [params['LOOKAHEAD'],
                    params['HTHRESHOLD'],
                    params['VTHRESHOLD'],
                    params['HSTEPBACK'],
                    params['VSTEPBACK']]
        self.Geofence.SetParameters(gfparams)

    def SetTrajectoryParams(self,params):
        # Astar params
        enable3d = True if params['ASTAR_ENABLE3D'] == 1 else False
        gridSize = params['ASTAR_GRIDSIZE'] 
        resSpeed = params['ASTAR_RESSPEED']
        lookahead= params['ASTAR_LOOKAHEAD']
        daaconfig = self.daafile
        self.Trajectory.UpdateAstarParams(enable3d,gridSize,resSpeed,lookahead,daaconfig)

        # RRT Params
        Nstep = int(params['RRT_NITERATIONS'])
        dt    = params['RRT_DT'] 
        Dt    = int(params['RRT_MACROSTEPS'])
        capR  = params['RRT_CAPR']
        resSpeed = params['RRT_RESSPEED']

        self.Trajectory.UpdateRRTParams(resSpeed,Nstep,dt,Dt,capR,daaconfig)

    def SetGuidanceParams(self,params):
        guidParams = GuidanceParam()
        guidParams.defaultWpSpeed = params['DEF_WP_SPEED'] 
        guidParams.captureRadiusScaling = params['CAP_R_SCALING']
        guidParams.guidanceRadiusScaling = params['GUID_R_SCALING']
        guidParams.xtrkDev = params['XTRKDEV']
        guidParams.climbFpAngle = params['CLIMB_ANGLE']
        guidParams.climbAngleVRange = params['CLIMB_ANGLE_VR']
        guidParams.climbAngleHRange = params['CLIMB_ANGLE_HR']
        guidParams.climbRateGain = params['CLIMB_RATE_GAIN']
        guidParams.maxClimbRate = params['MAX_CLIMB_RATE']
        guidParams.minClimbRate = params['MIN_CLIMB_RATE']
        guidParams.maxCap = params['MAX_CAP']
        guidParams.minCap = params['MIN_CAP']
        guidParams.maxSpeed = params['MAX_GS'] * 0.5
        guidParams.minSpeed = params['MIN_GS'] * 0.5
        guidParams.yawForward = True if params['YAW_FORWARD'] == 1 else False
        self.defaultWPSpeed = guidParams.defaultWpSpeed
        self.Guidance.SetGuidanceParams(guidParams)

    def SetCognitionParams(self,params):
        cogParams = CognitionParams()
        cogParams.resolutionSpeed = params['RESSPEED']
        cogParams.searchType =int(params['SEARCHALGORITHM']) 
        cogParams.resolutionType = int(params['RES_TYPE'])
        cogParams.DTHR = params['DET_1_WCV_DTHR']/3
        cogParams.ZTHR = params['DET_1_WCV_ZTHR']/3
        self.Cog.InputParameters(cogParams)
        self.resSpeed = params["RESSPEED"]


    def SetTrafficParams(self,params):
        paramString = "" \
        +"lookahead_time="+str(params['LOOKAHEAD_TIME'])+ "[s];"\
        +"left_hdir="+str(params['LEFT_TRK'])+ "[deg];"\
        +"right_hdir="+str(params['RIGHT_TRK'])+ "[deg];"\
        +"min_hs="+str(params['MIN_GS'])+ "[knot];"\
        +"max_hs="+str(params['MAX_GS'])+ "[knot];"\
        +"min_vs="+str(params['MIN_VS'])+ "[fpm];"\
        +"max_vs="+str(params['MAX_VS'])+ "[fpm];"\
        +"min_alt="+str(params['MIN_ALT'])+ "[ft];"\
        +"max_alt="+str(params['MAX_ALT'])+ "[ft];"\
        +"step_hdir="+str(params['TRK_STEP'])+ "[deg];"\
        +"step_hs="+str(params['GS_STEP'])+ "[knot];"\
        +"step_vs="+str(params['VS_STEP'])+ "[fpm];"\
        +"step_alt="+str(params['ALT_STEP'])+ "[ft];"\
        +"horizontal_accel="+str(params['HORIZONTAL_ACCL'])+ "[m/s^2];"\
        +"vertical_accel="+str(params['VERTICAL_ACCL'])+ "[m/s^2];"\
        +"turn_rate="+str(params['TURN_RATE'])+ "[deg/s];"\
        +"bank_angle="+str(params['BANK_ANGLE'])+ "[deg];"\
        +"vertical_rate="+str(params['VERTICAL_RATE'])+ "[fpm];"\
        +"recovery_stability_time="+str(params['RECOV_STAB_TIME'])+ "[s];"\
        +"min_horizontal_recovery="+str(params['MIN_HORIZ_RECOV'])+ "[ft];"\
        +"min_vertical_recovery="+str(params['MIN_VERT_RECOV'])+ "[ft];"\
        +"recovery_hdir="+( "true;" if params['RECOVERY_TRK'] == 1 else "false;" )\
        +"recovery_hs="+( "true;" if params['RECOVERY_GS'] == 1 else "false;" )\
        +"recovery_vs="+( "true;" if params['RECOVERY_VS'] == 1 else "false;" )\
        +"recovery_alt="+( "true;" if params['RECOVERY_ALT'] == 1 else "false;" )\
        +"ca_bands="+( "true;" if params['CA_BANDS'] == 1 else "false;" )\
        +"ca_factor="+str(params['CA_FACTOR'])+ ";"\
        +"horizontal_nmac="+str(params['HORIZONTAL_NMAC'])+ "[ft];"\
        +"vertical_nmac="+str(params['VERTICAL_NMAC'])+ "[ft];"\
        +"conflict_crit="+( "true;" if params['CONFLICT_CRIT'] == 1 else "false;" )\
        +"recovery_crit="+( "true;" if params['RECOVERY_CRIT'] == 1 else "false;" )\
        +"contour_thr="+str(params['CONTOUR_THR'])+ "[deg];"\
        +"alerters = default;"\
        +"default_alert_1_alerting_time="+str(params['AL_1_ALERT_T'])+ "[s];"\
        +"default_alert_1_detector="+"det_1;"\
        +"default_alert_1_early_alerting_time="+str(params['AL_1_E_ALERT_T'])+ "[s];"\
        +"default_alert_1_region="+"NEAR;"\
        +"default_alert_1_spread_alt="+str(params['AL_1_SPREAD_ALT'])+ "[ft];"\
        +"default_alert_1_spread_hs="+str(params['AL_1_SPREAD_GS'])+ "[knot];"\
        +"default_alert_1_spread_hdir="+str(params['AL_1_SPREAD_TRK'])+ "[deg];"\
        +"default_alert_1_spread_vs="+str(params['AL_1_SPREAD_VS'])+ "[fpm];"\
        +"default_det_1_WCV_DTHR="+str(params['DET_1_WCV_DTHR'])+ "[ft];"\
        +"default_det_1_WCV_TCOA="+str(params['DET_1_WCV_TCOA'])+ "[s];"\
        +"default_det_1_WCV_TTHR="+str(params['DET_1_WCV_TTHR'])+ "[s];"\
        +"default_det_1_WCV_ZTHR="+str(params['DET_1_WCV_ZTHR'])+ "[ft];"\
        +"default_load_core_detection_det_1="+"gov.nasa.larcfm.ACCoRD.WCV_TAUMOD;"
        daa_log = True if params['LOGDAADATA'] == 1 else False
        self.tfMonitor.SetParameters(paramString,daa_log)
        self.Trajectory.UpdateDAAParams(paramString)
        self.daa_radius = params['DET_1_WCV_DTHR']/3

    def SetMergerParams(self,params):
        self.Merger.SetVehicleConstraints(params["MIN_GS"]*0.5,
                                          params["MAX_GS"]*0.5,
                                          params["MAX_TURN_RADIUS"])
        self.Merger.SetFixParams(params["MIN_SEP_TIME"],
                                 params["COORD_ZONE"],
                                 params["SCHEDULE_ZONE"],
                                 params["ENTRY_RADIUS"],
                                 params["CORRIDOR_WIDTH"])

        
    def SetParameters(self,params):
        self.params = params
        self.SetGuidanceParams(params)
        self.SetGeofenceParams(params)
        self.SetTrajectoryParams(params)
        self.SetCognitionParams(params)
        self.SetTrafficParams(params)
        self.SetMergerParams(params)

    def RunOwnship(self):
        self.ownship.input(self.controlInput[1],self.controlInput[0],self.controlInput[2])
        self.ownship.step(windFrom=self.windFrom,windSpeed=self.windSpeed)

        opos = self.ownship.getOutputPosition()
        ovel = self.ownship.getOutputVelocity()

        self.positionLog.append(opos)

        (ogx, ogy) = gps_offset(self.home_pos[0], self.home_pos[1], opos[0], opos[1])
       
        self.position = [ogx, ogy, opos[2]]
        self.velocity = [ovel[1],ovel[0],-ovel[2]]
        self.trkgsvs = ConvertVnedToTrkGsVs(ovel[1],ovel[0],-ovel[2])
        self.trkband = None
        self.gsband = None
        self.altband = None
        self.vsband = None


        self.RecordOwnship()

    def RunCognition(self):


        self.Cog.InputVehicleState(self.position,self.trkgsvs,self.trkgsvs[0])
        
        nextWP1 = self.nextWP1
        if self.nextWP1 >= len(self.flightplan1):
            nextWP1 = len(self.flightplan1) - 1
        dist = distance(self.position[0],self.position[1],self.flightplan1[nextWP1][0],self.flightplan1[nextWP1][1])

        if self.verbose > 1:
            if self.activePlan == 'Plan0':
                print("IC: %d, %s, Distance to wp %d: %f" %(self.vehicleID,self.activePlan,self.nextWP1,dist))
            else:
                print("IC: %d, %s, Distance to wp %d: %f" %(self.vehicleID,self.activePlan,self.nextWP2,dist))

        self.fphase = self.Cog.RunFlightPhases(self.currTime)
        if self.fphase == 8:
            self.land = True

        n, cmd = self.Cog.GetOutput()

        if n > 0:
            if cmd.commandType == CommandTypes.FP_CHANGE:
                self.guidanceMode = GuidanceMode.FLIGHTPLAN
                self.activePlan =  cmd.commandU.fpChange.name.decode('utf-8')
                nextWP = cmd.commandU.fpChange.wpIndex
                if self.activePlan == "Plan0":
                    self.flightplan2 = []
                    self.nextWP1 = nextWP
                else:
                    self.nextWP2 = nextWP
                self.Guidance.SetGuidanceMode(self.guidanceMode,self.activePlan,nextWP)
                if self.verbose > 0:
                    print("active plan = %s" % self.activePlan)
            elif cmd.commandType == CommandTypes.P2P:
                self.guidanceMode = GuidanceMode.POINT2POINT
                point = cmd.commandU.p2pCommand.point
                speed = cmd.commandU.p2pCommand.speed
                fp = [(*self.position,0),(*point,speed)]
                self.Guidance.InputFlightplanData("P2P",0,fp)
                self.activePlan = "P2P"
                self.nextWP2 = 1
                self.Guidance.SetGuidanceMode(self.guidanceMode,self.activePlan,1)
                if self.verbose > 0:
                    print("active plan = %s" % self.activePlan)
            elif cmd.commandType == CommandTypes.VELOCITY:
                self.guidanceMode = GuidanceMode.VECTOR
                vn = cmd.commandU.velocityCommand.vn
                ve = cmd.commandU.velocityCommand.ve
                vu = cmd.commandU.velocityCommand.vu
                trkGsVs = ConvertVnedToTrkGsVs(vn,ve,vu)
                self.Guidance.InputVelocityCmd(trkGsVs)
                self.Guidance.SetGuidanceMode(self.guidanceMode,"",0)
            elif cmd.commandType == CommandTypes.TAKEOFF:
                self.guidanceMode = GuidanceMode.TAKEOFF
            elif cmd.commandType == CommandTypes.SPEED_CHANGE:
                self.etaFP1 = False
                speedChange =  cmd.commandU.speedChange.speed
                planID= cmd.commandU.speedChange.name.decode('utf-8')
                nextWP = self.nextWP1 if planID == "Plan0" else self.nextWP2
                self.Guidance.ChangeWaypointSpeed(planID,nextWP,speedChange,False)
            elif cmd.commandType == CommandTypes.STATUS_MESSAGE:
                if self.verbose > 0:
                    message = cmd.commandU.statusMessage.buffer
                    print(message.decode('utf-8'))
            elif cmd.commandType == CommandTypes.FP_REQUEST:
                self.flightplan2 = []
                self.numSecPlan += 1
                searchType = cmd.commandU.fpRequest.searchType
                startPos = cmd.commandU.fpRequest.fromPosition
                stopPos = cmd.commandU.fpRequest.toPosition
                startVel = cmd.commandU.fpRequest.startVelocity
                planID = cmd.commandU.fpRequest.name.decode('utf-8')
                numWP = self.Trajectory.FindPath(
                        searchType,
                        planID,
                        startPos,
                        stopPos,
                        startVel)

                if numWP > 0:
                    if self.verbose > 0:
                        print("%f: Computed flightplan %s with %d waypoints" % (self.currTime,planID,numWP))
                    for i in range(numWP):
                        wp = self.Trajectory.GetWaypoint(planID,i)
                        self.flightplan2.append([wp[0],wp[1],wp[2],self.resSpeed])
                    self.Cog.InputFlightplanData(planID,0,self.flightplan2)
                    self.Guidance.InputFlightplanData(planID,0,self.flightplan2)
                    self.plans.append(self.flightplan2)
                    self.localPlans.append(self.GetLocalFlightPlan(self.flightplan2))
                else:
                    if self.verbose > 0:
                        print("Error finding path")


    def RunGuidance(self):

        if self.guidanceMode is GuidanceMode.NOOP:
            return

        if self.guidanceMode is GuidanceMode.TAKEOFF:
            self.Cog.InputReachedWaypoint("Takeoff",0); 
            return

        self.Guidance.SetAircraftState(self.position,self.trkgsvs)

        self.Guidance.RunGuidance(self.currTime)

        guidOutput = self.Guidance.GetOutput()
        self.controlInput = ConvertTrkGsVsToVned(guidOutput.velCmd[0],
                                                 guidOutput.velCmd[1],
                                                 guidOutput.velCmd[2])


        nextWP = guidOutput.nextWP
        if self.guidanceMode is not GuidanceMode.VECTOR:
            if self.activePlan == "Plan0":
                if self.nextWP1 < nextWP:
                    self.Cog.InputReachedWaypoint("Plan0",nextWP-1)
                    self.nextWP1 = nextWP
                    if self.verbose > 0 and self.nextWP1 < len(self.flightplan1):
                        print("Proceeding to waypoint %d on %s" % (nextWP,self.activePlan))
            else:
                if self.nextWP2 < nextWP:
                    self.nextWP2 = nextWP
                    self.Cog.InputReachedWaypoint(self.activePlan,nextWP-1)
                    if self.verbose > 0 and self.nextWP2 < len(self.flightplan2):
                        print("Proceeding to waypoint %d on %s" % (nextWP,self.activePlan))
        else:
            pass
         
    def RunGeofenceMonitor(self):


        gfConflictData = GeofenceConflict()
        
        self.Geofence.CheckViolation(self.position,*self.trkgsvs)

        gfConflictData.numConflicts = self.Geofence.GetNumConflicts()
        gfConflictData.numFences = self.numFences
        for i in range(gfConflictData.numConflicts):
            (ids,conflict,violation,recPos,ftype) = self.Geofence.GetConflict(i)
            gfConflictData.conflictFenceIDs[i] = ids
            gfConflictData.conflictTypes[i] = ftype
            gfConflictData.recoveryPosition = recPos

        for i,fp in enumerate(self.flightplan1):
            wp = fp[:3]
            check1 = self.Geofence.CheckViolation(wp,0,0,0)
            check2 = self.Geofence.CheckWPFeasibility(self.position,wp)
            gfConflictData.waypointConflict1[i] = check1
            gfConflictData.directPathToWaypoint1[i] = check2

        for i,fp in enumerate(self.flightplan2):
            wp = fp[:3]
            check1 = self.Geofence.CheckViolation(wp,0,0,0)
            check2 = self.Geofence.CheckWPFeasibility(self.position,wp)
            gfConflictData.waypointConflict2[i] = check1
            gfConflictData.directPathToWaypoint2[i] = check2

        self.Cog.InputGeofenceConflictData(gfConflictData)



    def RunTrafficMonitor(self):
        self.tfMonitor.monitor_traffic(self.position,self.trkgsvs,self.currTime)
       
        trkband = self.tfMonitor.GetTrackBands()
        gsband = self.tfMonitor.GetGSBands()
        altband = self.tfMonitor.GetAltBands()
        vsband = self.tfMonitor.GetVSBands()

        trkband.time = self.currTime
        gsband.time = self.currTime
        altband.time = self.currTime
        vsband.time = self.currTime

        for i,fp in enumerate(self.flightplan1):
            wp = fp[:3]
            feasibility = self.tfMonitor.monitor_wp_feasibility(wp,-1)
            feasibility &= self.tfMonitor.monitor_wp_feasibility(wp,self.resSpeed)
            trkband.wpFeasibility1[i] = feasibility
            gsband.wpFeasibility1[i] = feasibility
            altband.wpFeasibility1[i] = feasibility
            vsband.wpFeasibility1[i] = feasibility
        
        for i,fp in enumerate(self.flightplan2):
            wp = fp[:3]
            feasibility = self.tfMonitor.monitor_wp_feasibility(wp,-1)
            feasibility &= self.tfMonitor.monitor_wp_feasibility(wp,self.resSpeed)
            trkband.wpFeasibility2[i] = feasibility
            gsband.wpFeasibility2[i] = feasibility
            altband.wpFeasibility2[i] = feasibility
            vsband.wpFeasibility2[i] = feasibility
         

        self.Cog.InputBands(trkband,gsband,altband,vsband) 

        getbandlog = lambda bands: {} if bands is None else {"conflict": bands.currentConflictBand, 
                                                             "resup": bands.resUp, 
                                                             "resdown": bands.resDown,
                                                             "numBands": bands.numBands,
                                                             "bandTypes": [bands.type[i] for i in range(20)],
                                                             "low": [bands.min[i] for i in range(20)],
                                                             "high": [bands.max[i] for i in range(20)]}

        self.ownshipLog["trkbands"].append(getbandlog(trkband))
        self.ownshipLog["gsbands"].append(getbandlog(gsband))
        self.ownshipLog["altbands"].append(getbandlog(altband))
        self.ownshipLog["vsbands"].append(getbandlog(vsband))

 

    def RunMerger(self):
        self.Merger.SetAircraftState(self.position,self.velocity)
        if self.currTime - self.prevLogUpdate > self.logLatency:
            self.Merger.SetMergerLog(self.mergerLog)
            self.prevLogUpdate = self.currTime

        mergingActive = self.Merger.Run(self.currTime)
        self.Cog.InputMergeStatus(mergingActive)
        outAvail, arrTime =  self.Merger.GetArrivalTimes()
        if outAvail:
            self.arrTime = arrTime
        if mergingActive == 3:
            velCmd = self.Merger.GetVelocityCmd()
            speedChange = velCmd[1]
            nextWP = self.nextWP1
            self.Guidance.ChangeWaypointSpeed("Plan0",nextWP,speedChange,False)



    def InputMergeLogs(self,logs,delay):
        self.mergerLog = logs
        self.logLatency = delay


    def CheckMissionComplete(self):
        if self.land and self.fphase == 0:
            self.missionComplete = True
        return self.missionComplete


    def RecordOwnship(self):
        self.ownshipLog["t"].append(self.currTime)
        self.ownshipLog["position"].append(self.position)
        self.ownshipLog["velocityNED"].append(self.velocity)
        self.ownshipLog["positionNED"].append([self.positionLog[-1][1],
                                               self.positionLog[-1][0],
                                               self.positionLog[-1][2]])

    def RecordTraffic(self, id, position, velocity, positionLoc):
        if id not in self.trafficLog:
            self.trafficLog[id] = {"t": [], "position": [], "velocityNED": [], "positionNED": []}
        self.trafficLog[id]["t"].append(self.currTime)
        self.trafficLog[id]["position"].append(list(position))
        self.trafficLog[id]["velocityNED"].append([velocity[1], velocity[0], velocity[2]])
        self.trafficLog[id]["positionNED"].append([positionLoc[1],positionLoc[0],positionLoc[2]])

    def WriteLog(self, logname="simoutput.json"):
        if self.logName != "":
             logname = self.logName + '.json'

        import json
        if self.verbose > 0:
            print("writing log: %s" % logname)
        log_data = {"ownship": self.ownshipLog,
                    "traffic": self.trafficLog,
                    "waypoints": self.flightplan1,
                    "geofences": self.fenceList,
                    "parameters": self.params,
                    "sim_type": "pyIcarous"}

        with open(logname, 'w') as f:
            json.dump(log_data, f)


    def Run(self):

        time_now = time.time()
        if not self.fasttime:
            if time_now - self.currTime >= 0.05:
                self.currTime = time_now
            else:
                return False
        else:
            self.currTime += 0.05

        #time_cog_in = time.time()
        self.RunCognition()
        #time_cog_out = time.time()

        #time_traff_in = time.time()
        self.RunTrafficMonitor()
        #time_traff_out = time.time()

        #time_geof_in = time.time()
        self.RunGeofenceMonitor()
        #time_geof_out = time.time()

        #time_guid_in = time.time()
        self.RunGuidance()
        #time_guid_out = time.time()


        self.RunMerger()

        #time_ship_in = time.time()
        self.RunOwnship()
        #time_ship_out = time.time()

        #print("cog     %f" % (time_cog_out - time_cog_in))
        #print("traffic %f" % (time_traff_out - time_traff_in))
        #print("geofence %f" % (time_geof_out - time_geof_in))
        #print("ownship %f" % (time_ship_out - time_ship_in))

        return True

def ExchangeArrivalTimes(icInstances,delay):
    arrTimes = []
    log = []
    for ic in icInstances:
        if ic.arrTime is not None:
            arrTimes.append(ic.arrTime)


    for arr in arrTimes:
        fid = arr.intersectionID
        if fid <= 0:
            continue
        avail = False
        for g in log:
            if fid == g[0].intersectionID:
                avail = True
                g.append(arr)
        if not avail:
            log.append([arr])

    for lg in log:
        if len(lg) == 0:
            continue

        datalog = LogData()
        datalog.intersectionID = lg[0].intersectionID
        datalog.nodeRole = 1
        datalog.totalNodes = len(lg)
        mg = MergerData*MAX_NODES
        datalog.log = mg(*lg)
        for ic in icInstances:
            ic.InputMergeLogs(datalog,delay)
            

def RunSimulation(icInstances,trafficVehicles,startDelay=[],timeLimit=[],commDelay=0,wind=[(0,0)]):
    # Run simulation until mission is complete
    simComplete = False
    prevTime = [ic.currTime for ic in icInstances]
    count = 0
    windfrom  = wind[count][0]
    windspeed = wind[count][1]
    while not simComplete:
        status = False
        simComplete = True
        delay = False # with a large number of vehicles, some not starting if delay was longer than time limit.
        if len(wind) > 1:
            windfrom = wind[count][0]
            windspeed = wind[count][1]
        for i,ic in enumerate(icInstances):
            ic.InputWind(windfrom,windspeed)
            if not ic.startSent:
                if len(startDelay) == 0:
                    ic.Cog.InputStartMission(0,0)
                    ic.startSent = True
                    print(ic.vehicleID, 'Start Sent', ic.currTime)
                else:
                    if ic.currTime - prevTime[i] > startDelay[i]:
                        ic.Cog.InputStartMission(0,0)
                        ic.startSent = True
                        prevTime[i] = ic.currTime
                        print(ic.vehicleID, 'Start Sent', prevTime[i])
                    else:
                        delay = True
            
            if not ic.CheckMissionComplete(): # don't run vehicle after completion of flight
                status |= ic.Run()
                simComplete &= ic.CheckMissionComplete()
                pos = ic.position
                locpos = ic.positionLog[-1]
                vel = [ic.velocity[1],ic.velocity[0],-ic.velocity[2]]

                for tf in icInstances:
                    if tf.startSent and ic.startSent: # don't pass info before takeoff
                        tf.InputTraffic(ic.vehicleID,pos,vel,locpos)
            else: 
                # update log for analysis
                ic.RecordOwnship()

            if len(timeLimit) > 0 and not delay and not ic.missionComplete:
                if ic.currTime - prevTime[i] > timeLimit[i]:
                    ic.missionComplete = True
                    print('Time limit Reached', ic.vehicleID, ic.currTime, ic.currTime - prevTime[i])
                    
        if status:
            RunTraffic(trafficVehicles,windfrom,windspeed)
            for ic in icInstances:
                for tf in trafficVehicles:
                    ic.InputTraffic(tf.vehicleID,tf.pos_gps,tf.vel,tf.pos)

        ExchangeArrivalTimes(icInstances,commDelay)
        count += 1

    # Transfer all vehicle to a common reference frame
    for i,ic in enumerate(icInstances):
        if i > 0:
            ic.ownshipLog["positionNED"] = list(map(icInstances[0].ConvertToLocalCoordinates,ic.ownshipLog["position"]))
            ic.localPlans[0] = list(map(icInstances[0].ConvertToLocalCoordinates,ic.flightplan1))

        #ic.WriteLog()

def VisualizeSimData(ic,allplans=False,xmin=-100,ymin=-100,xmax=100,ymax=100,interval=30):
    '''
    ic: icarous object
    allplans: True/False - plot all computed plans
    xmin,ymin : plot axis min values
    xmax,ymax : plot axis max values
    interval  : Interval between frames
    '''
    from Animation import AgentAnimation
    anim= AgentAnimation(xmin,ymin, xmax,ymax,interval)
    anim.AddAgent('ownship',2,'r',ic.ownshipLog,show_circle=True,circle_rad=10)

    for i,pln in enumerate(ic.localPlans):
        if i == 0:
            anim.AddPath(np.array(pln),'k--')

        if i > 0 and allplans:
            anim.AddPath(np.array(pln),'k--')
    tfids = ic.trafficLog.keys()
    for key in tfids:
        anim.AddAgent('traffic'+str(key),2,'b',ic.trafficLog[key])
    for fence in ic.localFences:
        fence.append(fence[0])
        anim.AddFence(np.array(fence),'c-.')
    anim.run()
