from ctypes import byref
import numpy as np


from pyCognition import gCog,mCognition
from pyGuidance import (mGuidance,
                      GuidanceInput,
                      GuidanceOutput,
                      GuidanceTable,
                      GuidanceCommands)
from pyGeofence import (Getfence,GeofenceMonitor)
from pyTrajectory import Trajectory
from pyTrafficMonitor import TrafficMonitor
from quadsim import QuadSim
from VehicleSim import (VehicleSim,
                        StartTraffic,
                        RunTraffic)
from ichelper import (ConvertTrkGsVsToVned,
                      distance,
                      gps_offset,
                      ConvertVnedToTrkGsVs,
                      ComputeHeading,
                      LoadIcarousParams)
import time

class Icarous():
    def __init__(self, initialPos, simtype="UAS_ROTOR", fasttime = True, logfile = None):
        self.fasttime = fasttime
        self.logfile = logfile
        self.home_pos = [initialPos[0], initialPos[1], initialPos[2]]
        self.traffic = []
        if simtype == "UAM_VTOL":
            self.ownship = VehicleSim(0.0,0.0,0.0,0.0,0.0,0.0)
        else:
            self.ownship = QuadSim()

        # Modules
        self.cog = gCog
        self.Cognition = mCognition
        self.Guidance = mGuidance
        self.Geofence = GeofenceMonitor([3,2,2,20,20])
        self.Trajectory = Trajectory(5.0,250)
        self.tfMonitor = TrafficMonitor(False)

        # Aircraft data
        self.flightplan1 = []
        self.etaFP1      = False
        self.flightplan2 = []
        self.controlInput = [0.0,0.0,0.0]
        self.fenceList   = []

        # Guidance related
        self.guidIn = GuidanceInput()
        self.guidOut = GuidanceOutput()
        self.guidTbl = GuidanceTable()
        self.reachedStatusUpdated = False;
        self.guidanceMode = GuidanceCommands.NOOP
        self.guidCommParams = []

        self.position = self.home_pos
        self.velocity = [0.0,0.0,0.0]
        self.trkgsvs  = [0.0,0.0,0.0]

        self.positionLog = []
        self.emergbreak = False
        self.currTime   = 0

    def setpos_uncertainty(self,xx,yy,zz,xy,yz,xz,coeff=0.8):
        self.ownship.setpos_uncertainty(xx,yy,zz,xy,yz,xz,coeff)


    def InputTraffic(self,id,position,velocity):
        self.tfMonitor.input_traffic(id,position,velocity,self.currTime)

    def InputFlightplan(self,fp,scenarioTime,eta=False):
        
        self.flightplan1 = fp 
        self.flightplan2 = []
        self.etaFP1 = eta
        self.cog.primaryFPReceived = True
        self.cog.scenarioTime = scenarioTime
        self.cog.missionStart = 0
        self.cog.num_waypoints = len(self.flightplan1)
        self.cog.wpMetricTime = False;

    def InputGeofence(self,filename):
        self.fenceList = Getfence(filename)
        for fence in self.fenceList:
            self.Geofence.InputData(fence)
            self.Trajectory.InputGeofenceData(fence)

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
        daaconfig = "DaidalusQuadConfig.txt"

        # RRT Params
        Nstep = int(params['RRT_NITERATIONS'])
        dt    = params['RRT_DT'] 
        Dt    = int(params['RRT_MACROSTEPS'])
        capR  = params['RRT_CAPR']

        self.Trajectory.UpdateAstarParams(enable3d,gridSize,resSpeed,lookahead,daaconfig)
        self.Trajectory.UpdateRRTParams(resSpeed,Nstep,dt,Dt,capR,daaconfig)

    def SetGuidanceParams(self,params):
        self.guidTbl.defaultWpSpeed = params['DEF_WP_SPEED'] 
        self.guidTbl.captureRadiusScaling = params['CAP_R_SCALING']
        self.guidTbl.guidanceRadiusScaling = params['GUID_R_SCALING']
        self.guidTbl.xtrkDev = params['XTRKDEV']
        self.guidTbl.climbFpAngle = params['CLIMB_ANGLE']
        self.guidTbl.climbAngleVRange = params['CLIMB_ANGLE_VR']
        self.guidTbl.climbAngleHRange = params['CLIMB_ANGLE_HR']
        self.guidTbl.climbRateGain = params['CLIMB_RATE_GAIN']
        self.guidTbl.maxClimbRate = params['MAX_CLIMB_RATE']
        self.guidTbl.minClimbRate = params['MIN_CLIMB_RATE']
        self.guidTbl.yawForward = True if params['YAW_FORWARD'] == 1 else False

    def SetCognitionParams(self,params):
        self.cog.resolutionSpeed = params['RESSPEED']
        self.cog.searchType = int(params['SEARCHALGORITHM'])
        self.cog.resolutionTypeCmd = int(params['RES_TYPE'])

    def SetTrafficParams(self,params):
        paramString = "" \
        +"lookahead_time="+str(params['LOOKAHEAD_TIME'])+ "[s];"\
        +"left_trk="+str(params['LEFT_TRK'])+ "[deg];"\
        +"right_trk="+str(params['RIGHT_TRK'])+ "[deg];"\
        +"min_gs="+str(params['MIN_GS'])+ "[m/s];"\
        +"max_gs="+str(params['MAX_GS'])+ "[m/s];"\
        +"min_vs="+str(params['MIN_VS'])+ "[fpm];"\
        +"max_vs="+str(params['MAX_VS'])+ "[fpm];"\
        +"min_alt="+str(params['MIN_ALT'])+ "[ft];"\
        +"max_alt="+str(params['MAX_ALT'])+ "[ft];"\
        +"trk_step="+str(params['TRK_STEP'])+ "[deg];"\
        +"gs_step="+str(params['GS_STEP'])+ "[m/s];"\
        +"vs_step="+str(params['VS_STEP'])+ "[fpm];"\
        +"alt_step="+str(params['ALT_STEP'])+ "[m];"\
        +"horizontal_accel="+str(params['HORIZONTAL_ACCL'])+ "[m/s^2];"\
        +"vertical_accel="+str(params['VERTICAL_ACCL'])+ "[m/s^2];"\
        +"turn_rate="+str(params['TURN_RATE'])+ "[deg/s];"\
        +"bank_angle="+str(params['BANK_ANGLE'])+ "[deg];"\
        +"vertical_rate="+str(params['VERTICAL_RATE'])+ "[m/s];"\
        +"recovery_stability_time="+str(params['RECOV_STAB_TIME'])+ "[s];"\
        +"min_horizontal_recovery="+str(params['MIN_HORIZ_RECOV'])+ "[m];"\
        +"min_vertical_recovery="+str(params['MIN_VERT_RECOV'])+ "[m];"\
        +"recovery_trk="+( "true;" if params['RECOVERY_TRK'] == 1 else "false;" )\
        +"recovery_gs="+( "true;" if params['RECOVERY_GS'] == 1 else "false;" )\
        +"recovery_vs="+( "true;" if params['RECOVERY_VS'] == 1 else "false;" )\
        +"recovery_alt="+( "true;" if params['RECOVERY_ALT'] == 1 else "false;" )\
        +"ca_bands="+( "true;" if params['CA_BANDS'] == 1 else "false;" )\
        +"ca_factor="+str(params['CA_FACTOR'])+ ";"\
        +"horizontal_nmac="+str(params['HORIZONTAL_NMAC'])+ "[m];"\
        +"vertical_nmac="+str(params['VERTICAL_NMAC'])+ "[m];"\
        +"conflict_crit="+( "true;" if params['CONFLICT_CRIT'] == 1 else "false;" )\
        +"recovery_crit="+( "true;" if params['RECOVERY_CRIT'] == 1 else "false;" )\
        +"contour_thr="+str(params['CONTOUR_THR'])+ "[deg];"\
        +"alert_1_alerting_time="+str(params['AL_1_ALERT_T'])+ "[s];"\
        +"alert_1_detector="+"det_1;"\
        +"alert_1_early_alerting_time="+str(params['AL_1_E_ALERT_T'])+ "[s];"\
        +"alert_1_region="+"NEAR;"\
        +"alert_1_spread_alt="+str(params['AL_1_SPREAD_ALT'])+ "[m];"\
        +"alert_1_spread_gs="+str(params['AL_1_SPREAD_GS'])+ "[m/s];"\
        +"alert_1_spread_trk="+str(params['AL_1_SPREAD_TRK'])+ "[deg];"\
        +"alert_1_spread_vs="+str(params['AL_1_SPREAD_VS'])+ "[fpm];"\
        +"conflict_level="+str(params['CONFLICT_LEVEL'])+ ";"\
        +"det_1_WCV_DTHR="+str(params['DET_1_WCV_DTHR'])+ "[ft];"\
        +"det_1_WCV_TCOA="+str(params['DET_1_WCV_TCOA'])+ "[s];"\
        +"det_1_WCV_TTHR="+str(params['DET_1_WCV_TTHR'])+ "[s];"\
        +"det_1_WCV_ZTHR="+str(params['DET_1_WCV_ZTHR'])+ "[ft];"\
        +"load_core_detection_det_1="+"gov.nasa.larcfm.ACCoRD.WCV_TAUMOD;"
        daa_log = True if params['LOGDAADATA'] == 1 else False
        self.tfMonitor.SetParameters(paramString,daa_log)
        
    def SetParameters(self,params):
        self.SetGuidanceParams(params)
        self.SetGeofenceParams(params)
        self.SetTrajectoryParams(params)
        self.SetCognitionParams(params)
        self.SetTrafficParams(params)

    def RunOwnship(self):
        self.ownship.input(self.controlInput[1],self.controlInput[0],self.controlInput[2])
        self.ownship.step()

        opos = self.ownship.getOutputPosition()
        ovel = self.ownship.getOutputVelocity()

        self.positionLog.append(opos)

        (ogx, ogy) = gps_offset(self.home_pos[0], self.home_pos[1], opos[0], opos[1])
       
        self.position = [ogx, ogy, opos[2]]
        self.velocity = [ovel[1],ovel[0],ovel[2]]           

        self.trkgsvs = ConvertVnedToTrkGsVs(ovel[0],ovel[1],ovel[2])
        self.trkband = None
        self.gsband = None
        self.altband = None
        self.vsband = None

    def RunCogntiion(self):

        for i in range(3):
            self.cog.position[i] = self.position[i]
            self.cog.velocity[i] = self.velocity[i]

        self.cog.hdg = self.trkgsvs[0]

        nextWP = 0
        for i in range(self.cog.nextPrimaryWP,len(self.flightplan1)):
            nextWP = i
            wp = self.flightplan1[i]
            conflict = self.Geofence.CheckViolation(wp[:3],0,0,0)
            if not conflict:
                break

        if self.cog.nextPrimaryWP < len(self.flightplan1):
            wp = self.flightplan1[nextWP]
            self.cog.nextFeasibleWP1 = nextWP
            check1 = self.Geofence.CheckWPFeasibility(self.position,wp[:3])
            check2 = self.tfMonitor.monitor_wp_feasibility(self.position,self.trkgsvs,wp[:3])
            self.cog.directPathToFeasibleWP1 = check1 and check2
            for i in range(3):
                self.cog.wpNextFb1[i] = wp[i]

            for i in range(3):
                self.cog.wpPrev1[i] = self.flightplan1[self.cog.nextPrimaryWP - 1][i]
                self.cog.wpNext1[i] = self.flightplan1[self.cog.nextPrimaryWP][i]

        # Check for feasiblity of path
        if len(self.flightplan2) > 0:
            nextWP = 0
            for i in range(self.cog.nextSecondaryWP,len(self.flightplan2)):
                nextWP = i
                wp = self.flightplan2[i]
                conflict = self.Geofence.CheckViolation(wp[:3],0,0,0)
                if not conflict:
                    break
            wp = self.flightplan2[nextWP]
            self.cog.nextFeasibleWP2 = nextWP
            check1 = self.Geofence.CheckWPFeasibility(self.position,wp[:3])
            check2 = self.tfMonitor.monitor_wp_feasibility(self.position,self.trkgsvs,wp[:3])
            self.cog.directPathToFeasibleWP2 = check1 and check2
            for i in range(3):
                self.cog.wpNextFb2[i] = wp[i]

            for i in range(3):
                self.cog.wpPrev2[i] = self.flightplan2[self.cog.nextSecondaryWP - 1][i]
                self.cog.wpNext2[i] = self.flightplan2[self.cog.nextSecondaryWP][i]



        self.Cognition.FlightPhases()

        if self.cog.sendCommand:
            self.cog.sendCommand = False
            if self.cog.guidanceCommand is not GuidanceCommands.SPEED_CHANGE:
                self.guidCommParams = self.cog.cmdparams
                self.guidanceMode = self.cog.guidanceCommand
                if self.guidanceMode == GuidanceCommands.PRIMARY_FLIGHTPLAN:
                    self.flightplan2 = []
                    if not self.etaFP1:
                        self.guidIn.speed = self.flightplan1[self.guidIn.nextWP][3]
                    else:
                        self.guidIn.speed = self.guidTbl.defaultWpSpeed
            else:
                self.guidIn.speed = self.cog.cmdparams[0]

        if self.cog.sendStatusTxt:
            self.cog.sendStatusTxt = False;
            print(self.cog.statusBuf.decode('utf-8'))

        if self.cog.pathRequest:
            self.cog.pathRequest = False
            numWP = self.Trajectory.FindPath(
                    self.cog.searchType,
                    "Plan1",
                    self.cog.startPosition,
                    self.cog.stopPosition,
                    self.cog.startVelocity)

            if numWP > 0:
                for i in range(numWP):
                    wp = self.Trajectory.GetWaypoint("Plan1",i)
                    self.flightplan2.append([wp[0],wp[1],wp[2],self.cog.resolutionSpeed])
                self.cog.request = 2
                self.cog.nextSecondaryWP = 1
                self.cog.nextWP          = 1
            else:
                print("Error finding path")


    def RunGuidance(self):
        self.guidIn.reachedStatusUpdated = self.guidOut.reachedStatusUpdated
       
        for i in range(3):
            self.guidIn.velCmd[i] = self.controlInput[i]

        for i in range(3):
            self.guidIn.position[i] = self.position[i]
            self.guidIn.velocity[i] = self.velocity[i]


        if self.guidanceMode == GuidanceCommands.PRIMARY_FLIGHTPLAN:
            self.guidIn.nextWP = self.cog.nextWP
            self.guidIn.num_waypoints = len(self.flightplan1)
            if(self.guidIn.nextWP < self.guidIn.num_waypoints):
                for i in range(3):
                    self.guidIn.prev_waypoint[i] = self.flightplan1[self.guidIn.nextWP-1][i]
                    self.guidIn.curr_waypoint[i] = self.flightplan1[self.guidIn.nextWP][i]

                self.guidIn.speed = self.flightplan1[self.guidIn.nextWP][3]
            self.Guidance.ComputeFlightplanGuidanceInput(byref(self.guidIn),byref(self.guidOut),byref(self.guidTbl))

        elif self.guidanceMode == GuidanceCommands.SECONDARY_FLIGHTPLAN:

            self.guidIn.nextWP = self.cog.nextWP
            self.guidIn.num_waypoints = len(self.flightplan2)
            for i in range(3):
                self.guidIn.prev_waypoint[i] = self.flightplan2[self.guidIn.nextWP-1][i]
                self.guidIn.curr_waypoint[i] = self.flightplan2[self.guidIn.nextWP][i]

            self.guidIn.speed = self.flightplan2[self.guidIn.nextWP][3]
            self.Guidance.ComputeFlightplanGuidanceInput(byref(self.guidIn),byref(self.guidOut),byref(self.guidTbl))

        elif self.guidanceMode == GuidanceCommands.TAKEOFF:
            self.cog.takeoffComplete = 1

        elif self.guidanceMode == GuidanceCommands.VECTOR:
            for i in range(3):
                self.guidOut.velCmd[i] = self.guidCommParams[i]

        elif self.guidanceMode == GuidanceCommands.POINT2POINT:
            self.guidIn.speed = self.guidCommParams[3]
            self.guidIn.nextWP = 1
            self.guidIn.num_waypoints = 2

            for i in range(3):
                self.guidIn.prev_waypoint[i] = self.position[i]
                self.guidIn.curr_waypoint[i] = self.guidCommParams[i]

            self.Guidance.ComputeFlightplanGuidanceInput(byref(self.guidIn),byref(self.guidOut),byref(self.guidTbl))

        else:
            pass

        # Handle Guidance outputs
        for i in range(3):
            self.controlInput[i] = self.guidOut.velCmd[i]
        
        if self.guidOut.reachedStatusUpdated:
            if self.guidanceMode == GuidanceCommands.PRIMARY_FLIGHTPLAN:
                if self.guidOut.newNextWP <= len(self.flightplan1):
                    self.cog.nextPrimaryWP = self.guidOut.newNextWP
                    self.cog.nextWP        = self.guidOut.newNextWP

            elif self.guidanceMode == GuidanceCommands.SECONDARY_FLIGHTPLAN:
                if(self.guidOut.newNextWP < len(self.flightplan2)):
                    self.cog.nextSecondaryWP = self.guidOut.newNextWP
                    self.cog.nextWP          = self.guidOut.newNextWP
                else:
                    self.cog.fp2complete = True

            elif self.guidanceMode == GuidanceCommands.POINT2POINT:
                self.controlInput = [0.0,0.0,0.0]
                self.cog.p2pcomplete = True


    def RunGeofenceMonitor(self):
        self.cog.keepInConflict = False
        self.cog.keepOutConflict = False
        self.Geofence.CheckViolation(self.position,*self.trkgsvs)

        num = self.Geofence.GetNumConflicts()
        if num > 0:
            (id,conflict,violation,recPos,ftype) = self.Geofence.GetConflict(0)
            self.cog.recoveryPosition = recPos
            if ftype == 0:
                self.cog.keepInConflict = conflict
            else:
                self.cog.keepOutConflict = conflict

    def RunTrafficMonitor(self):
        self.tfMonitor.monitor_traffic(self.position,self.trkgsvs,self.currTime)
       
        self.trkband = self.tfMonitor.GetTrackBands()
        self.gsband = self.tfMonitor.GetGSBands()
        self.altband = self.tfMonitor.GetAltBands()
        self.vsband = self.tfMonitor.GetVSBands()

        # Manage track bands
        if self.trkband.currentConflict == 1:
            self.cog.trafficTrackConflict = True
            if self.trkband.respref >=0:
                self.cog.preferredTrack = self.trkband.respref
            else:
                self.cog.preferredTrack = -1000
        else:
            self.cog.trafficTrackConflict = False
            self.cog.preferredTrack = -10000

        self.cog.trkbandType = self.trkband.bandTypes
        self.cog.trkBandNum = self.trkband.numBands
        self.cog.trkBandMin = self.trkband.low
        self.cog.trkBandMax = self.trkband.high

        if self.cog.nextPrimaryWP < len(self.flightplan1):
            self.cog.nextWPFeasibility1 = self.tfMonitor.monitor_wp_feasibility(self.position,
                                                                                self.trkgsvs,
                                                                                self.flightplan1[self.cog.nextPrimaryWP])
            self.cog.nextWPFeasibility1 &= self.tfMonitor.monitor_wp_feasibility(self.position,
                                                                                [self.trkgsvs[0],self.cog.resolutionSpeed,self.trkgsvs[2]],
                                                                                self.flightplan1[self.cog.nextPrimaryWP])
            

        if len(self.flightplan2) > 0:
            self.cog.nextWPFeasibility2 = self.tfMonitor.monitor_wp_feasibility(self.position,
                                                                                self.trkgsvs,
                                                                                self.flightplan2[self.cog.nextSecondaryWP])
            self.cog.nextWPFeasibility2 &= self.tfMonitor.monitor_wp_feasibility(self.position,
                                                                                [self.trkgsvs[0],self.cog.resolutionSpeed,self.trkgsvs[2]],
                                                                                self.flightplan2[self.cog.nextSecondaryWP])

        else:
            self.cog.nextWPFeasibility2 = True

        # Manage ground speed bands
        fac = 0
        if abs(self.gsband.respref - self.gsband.resdown) < 1e-3:
            fac = 0.8
        else:
            fac = 1.2

        if self.gsband.currentConflict == 1:
            self.cog.trafficSpeedConflict = True
            if not np.isinf(self.gsband.respref):
                self.cog.preferredSpeed = self.gsband.respref * fac
            else:
                self.cog.preferredSpeed = -10000
        else:
            if self.cog.Plan0 and self.cog.nextWPFeasibility1:
                self.cog.trafficSpeedConflict = False
            elif self.cog.Plan1 and self.cog.nextWPFeasibility2:
                self.cog.trafficSpeedConflict = False
            else:
                if self.cog.trafficSpeedConflict:
                    self.cog.trafficSpeedConflict = True
            self.cog.preferredSpeed = -1000

        # Manage altitude bands
        if self.altband.currentConflict == 1:
            self.cog.trafficAltConflict = True
            if not np.isinf(self.altband.respref):
                self.cog.preferredAlt = self.altband.respref
            else:
                self.cog.preferredAlt = -10000
        else:
            self.cog.trafficAltConflict = False
            self.cog.preferredAlt = -1000

        # Manage vertical speed bands
        self.cog.resVUp = self.vsband.resup
        self.cog.resVDown = self.vsband.resdown
        self.cog.vsBandsNum = self.vsband.numBands


    def CheckMissionComplete(self):
        if self.cog.missionStart == -2:
            return True
        else:
            return False

    def Run(self):

        time_now = time.time()
        if not self.fasttime:
            if time_now - self.currTime >= 0.05:
                self.currTime = time_now
            else:
                return False
        else:
            self.currTime += 0.05

        self.RunCogntiion()

        self.RunTrafficMonitor()

        self.RunGeofenceMonitor()

        self.RunGuidance()

        self.RunOwnship()

        return True

