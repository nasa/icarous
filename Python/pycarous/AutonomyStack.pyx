# distutils: language=c++

from AutonomyStackDef cimport *
from libc.string cimport strcpy
from ichelper import ConvertVnedToTrkGsVs
from libcpp.string cimport string
from libcpp.map cimport map

cdef class AutonomyStack:
    cdef void* TrafficMonitor
    cdef void* Cognition
    cdef void* Guidance
    cdef void* TrajManager
    cdef void* Geofence
    cdef void* TfMonitor
    cdef void* Merger
    cdef string callsign
    cdef string intCallsign
    cdef int verbose
    cdef string activePlan
    cdef bint etaFP1
    cdef int nextWP1
    cdef int nextWP2
    cdef map[string,float] params
    cdef GuidanceMode guidanceMode
    cdef double position[3]
    cdef double trkgsvs[3]
    cdef int numSecPlan
    cdef double windFrom, windSpeed
    cdef double controlInput[3]
    cdef int missionPlanSize
    cdef int secPlanSize
    cdef bint land
    cdef double planOffsets[6]
    cdef double sigmaPos[6]
    cdef double sigmaVel[6]
    cdef bands_t gsBand
    cdef bands_t altBand
    cdef bands_t vsBand
    cdef bands_t trkBand

    def __cinit__(self,opts):
        cdef GuidanceParams_t guidParams
        cdef double geoParams[5]
        self.callsign = opts['callsign'].encode('utf-8')
        daaConfig = opts['daaConfig']
        daaLog = opts['daaLog']
        self.verbose = opts['verbose']
        self.TrafficMonitor = newDaidalusTrafficMonitor(<char*>self.callsign.c_str(),daaConfig.encode('utf-8'),daaLog); 
        self.Cognition = CognitionInit(<char*>self.callsign.c_str())
        self.Guidance = InitGuidance(&guidParams) 
        self.TrajManager = new_TrajManager(<char*>self.callsign.c_str())
        self.Geofence = new_GeofenceMonitor(geoParams)
        self.Merger = MergerInit(<char*>self.callsign.c_str(),opts['vehicleID'])
        self.activePlan = b''
        self.etaFP1 = False
        self.nextWP1 = 1
        self.nextWP2 = 1
        self.guidanceMode = GUIDE_NOOP
        self.numSecPlan = 0
        self.windFrom = 0
        self.windSpeed = 0
        self.controlInput = [0.0,0.0,0.0]
        self.missionPlanSize = 0
        self.secPlanSize = 0
        self.windFrom = 0
        self.windSpeed = 0

    def SetGuidanceParams(self):
        cdef GuidanceParams_t guidParams
        guidParams.defaultWpSpeed = self.params[b'DEF_WP_SPEED'] 
        guidParams.captureRadiusScaling = self.params[b'CAP_R_SCALING']
        guidParams.guidanceRadiusScaling = self.params[b'GUID_R_SCALING']
        guidParams.climbFpAngle = self.params[b'CLIMB_ANGLE']
        guidParams.climbAngleVRange = self.params[b'CLIMB_ANGLE_VR']
        guidParams.climbAngleHRange = self.params[b'CLIMB_ANGLE_HR']
        guidParams.climbRateGain = self.params[b'CLIMB_RATE_GAIN']
        guidParams.maxClimbRate = self.params[b'MAX_VS'] * 0.00508
        guidParams.minClimbRate = self.params[b'MIN_VS'] * 0.00508
        guidParams.maxCap = self.params[b'MAX_CAP']
        guidParams.minCap = self.params[b'MIN_CAP']
        guidParams.maxSpeed = self.params[b'MAX_GS'] * 0.5
        guidParams.minSpeed = self.params[b'MIN_GS'] * 0.5
        guidParams.yawForward = True if self.params[b'YAW_FORWARD'] == 1 else False
        guidParams.turnRateGain = self.params[b'TURNRATE_GAIN']
        guidSetParams(self.Guidance,&guidParams)

    def SetTrajectoryParams(self):

        cdef DubinsParams_t dubinsParams
        dubinsParams.turnRate = self.params[b'TURN_RATE']
        dubinsParams.zSections = self.params[b'ALT_DH'] 
        dubinsParams.vertexBuffer = self.params[b'OBSBUFFER']
        dubinsParams.maxGS = self.params[b'MAX_GS'] * 0.5
        dubinsParams.minGS = self.params[b'MIN_GS'] * 0.5
        dubinsParams.maxVS = self.params[b'MAX_VS'] * 0.00508
        dubinsParams.minVS = self.params[b'MIN_VS'] * 0.00508
        dubinsParams.hAccel = self.params[b'HORIZONTAL_ACCL'] 
        dubinsParams.vAccel = self.params[b'VERTICAL_ACCL'] 
        dubinsParams.hDaccel = self.params[b'HORIZONTAL_ACCL']*-0.8
        dubinsParams.vDaccel = self.params[b'VERTICAL_ACCL']*-1
        dubinsParams.climbgs = self.params[b'CLMB_SPEED']
        dubinsParams.maxH = self.params[b'MAX_ALT']*0.3 
        dubinsParams.wellClearDistH = self.params[b'DET_1_WCV_DTHR']/3
        dubinsParams.wellClearDistV = self.params[b'DET_1_WCV_ZTHR']/3

        TrajManager_UpdateDubinsPlannerParameters(self.TrajManager,&dubinsParams)

    def SetCognitionParams(self):

        cdef cognition_params_t cogParams
        cogParams.resolutionType = int(self.params[b'RES_TYPE'])
        cogParams.allowedXtrackDeviation = self.params[b'XTRKDEV']
        cogParams.DTHR = self.params[b'DET_1_WCV_DTHR']/3
        cogParams.ZTHR = self.params[b'DET_1_WCV_ZTHR']/3
        cogParams.lookaheadTime = self.params[b'LOOKAHEAD_TIME']
        cogParams.persistenceTime = self.params[b'PERSIST_TIME']
        cogParams.return2NextWP = int(self.params[b'RETURN_WP'])
        cogParams.active = True if cogParams.resolutionType >= 0 else False
        InputParameters(self.Cognition,&cogParams)

    def SetGeofenceParams(self):
        cdef double params[5]

        params[0] = self.params[b'LOOKAHEAD']
        params[1] = self.params[b'HTHRESHOLD']
        params[2] = self.params[b'VTHRESHOLD']
        params[3] = self.params[b'HSTEPBACK']
        params[4] = self.params[b'VSTEPBACK']

        GeofenceMonitor_SetGeofenceParameters(self.Geofence, params);
    
    def SetMergerParams(self):
        MergerSetVehicleConstraints(self.Merger,self.params[b'MIN_GS']*0.5,
                                                self.params[b'MAX_GS']*0.5,
                                                self.params[b'MAX_TURN_RADIUS'])
        MergerSetFixParams(self.Merger,self.params[b'MIN_SEP_TIME'],
                                       self.params[b'COORD_ZONE'],
                                       self.params[b'SCHEDULE_ZONE'],
                                       self.params[b'ENTRY_RADIUS'],
                                       self.params[b'CORRIDOR_WIDTH'])

    def SetParams(self,params):
        for key in params.keys():
            self.params[key.encode('utf-8')] = params[key]
        self.SetCognitionParams()
        self.SetGuidanceParams()
        self.SetGeofenceParams()
        self.SetTrajectoryParams()
        self.SetMergerParams()

    def InputWind(self,windFrom,windSpeed):
        self.windFrom = windFrom
        self.windSpeed = windSpeed
        guidSetWindData(self.Guidance,windFrom,windSpeed)

    def InputMissionFlightPlan(self,waypoints,repair=False,eta=False):
        cdef waypoint_t inputWPs[500]; 
        cdef bint _repair = repair
        self.etaFP1 = eta
        self.missionPlanSize = len(waypoints)
        for i,wp in enumerate(waypoints):
            inputWPs[i].time = wp.time
            inputWPs[i].latitude = wp.latitude
            inputWPs[i].longitude = wp.longitude
            inputWPs[i].altitude = wp.altitude
            inputWPs[i].tcp[0] = wp.tcp[0]
            inputWPs[i].tcp[1] = wp.tcp[1]
            inputWPs[i].tcp[2] = wp.tcp[2]
            inputWPs[i].tcpValue[0] = wp.tcpValue[0]
            inputWPs[i].tcpValue[1] = wp.tcpValue[1]
            inputWPs[i].tcpValue[2] = wp.tcpValue[2]
            strcpy(inputWPs[i].info,wp.info)

        InputFlightPlanData(self.Cognition,b'Plan0',inputWPs,self.missionPlanSize,0,_repair,self.params[b'TURN_RATE'])
        guidInputFlightplanData(self.Guidance,b'Plan0',inputWPs,self.missionPlanSize,0,_repair,self.params[b'TURN_RATE'])
        TrajManager_InputFlightPlan(self.TrajManager,b'Plan0',inputWPs,self.missionPlanSize,0,_repair,self.params[b'TURN_RATE'])

    def SetIntersectionData(self,fp):
        cdef int n,fid
        cdef double pos[3]
        n = len(fp)
        for i in range(n):
            fid = fp[i][0]
            for j in range(3):
                pos[j] = fp[i][1][j]
            MergerSetIntersectionData(self.Merger,i,fid,pos)

    def InputMergerLogs(self,logs):
        cdef dataLog_t datalogs;
        cdef int n

        n = len(logs)
        datalogs.nodeRole = 1
        datalogs.totalNodes = n
        for i,key in enumerate(logs.keys()):
            datalogs.intersectionID = logs[key]['intersectionID']
            datalogs.log[i].aircraftID = logs[key]['aircraftID'] 
            datalogs.log[i].intersectionID = logs[key]['intersectionID']
            datalogs.log[i].earlyArrivalTime = logs[key]['earlyArrivalTime']
            datalogs.log[i].currentArrivalTime = logs[key]['currentArrivalTime']
            datalogs.log[i].lateArrivalTime = logs[key]['lateArrivalTime']
            datalogs.log[i].numSchedulesComputed = logs[key]['numSchedulesComputed']
            datalogs.log[i].zoneStatus = logs[key]['zoneStatus']

        MergerSetNodeLog(self.Merger,&datalogs)
    
    def GetArrivalTimes(self):
        cdef mergingData_t arrData
        cdef bint out
        out = MergerGetArrivalTimes(self.Merger,&arrData)
        return (out,{
            'aircraftID':arrData.aircraftID,
            'intersectionID':arrData.intersectionID,
            'earlyArrivalTime':arrData.earlyArrivalTime,
            'currentArrivalTime':arrData.currentArrivalTime,
            'lateArrivalTime':arrData.lateArrivalTime,
            'numSchedulesComputed':arrData.numSchedulesComputed,
            'zoneStatus':arrData.zoneStatus
        })
            

    def GetKinematicPlan(self,planID):
        cdef waypoint_t wp
        class Waypoint:
            pass
        id = 0
        totalWP = 1
        fp = []
        while id < totalWP:
            totalWP = guidGetWaypoint(self.Guidance,planID.encode('utf-8'),id,&wp)
            output = Waypoint();
            output.time = wp.time
            output.latitude = wp.latitude
            output.longitude = wp.longitude
            output.altitude = wp.altitude
            output.tcp = [wp.tcp[0],wp.tcp[1],wp.tcp[2]]
            output.tcpValue = [wp.tcpValue[0],wp.tcpValue[1],wp.tcpValue[2]]
            output.info = wp.info.decode('utf-8')
            id += 1
            fp.append(wp)
          
        return fp

    def InputOwnshipState(self,time,position,velocity,sigmaPos,sigmaVel):
        for i in range(3):
            self.position[i] = position[i]
            self.trkgsvs[i] = velocity[i]

        for i in range(6):
            self.sigmaPos[i] = sigmaPos[i]
            self.sigmaVel[i] = sigmaVel[i]

        InputVehicleState(self.Cognition,self.position,self.trkgsvs,self.trkgsvs[0])
        guidSetAircraftState(self.Guidance,self.position,self.trkgsvs)
        TrafficMonitor_InputOwnshipData(self.TrafficMonitor,self.position,self.trkgsvs,time,self.sigmaPos,self.sigmaVel)
        MergerSetAircraftState(self.Merger,self.position,self.trkgsvs)


    def InputIntruderState(self,time,callsign,pos,vel,sumPos=[0,0,0,0,0,0],sumVel=[0,0,0,0,0,0]):
        cdef double position[3]
        cdef double velocity[3]
        cdef double sigmaPos[6]
        cdef double sigmaVel[6]

        for i in range(3):
            position[i] = pos[i]
            velocity[i] = vel[i]

        for i in range(6):
            sigmaPos[i] = sumPos[i]
            sigmaVel[i] = sumVel[i]

        self.intCallsign = callsign.encode('utf-8')

        TrafficMonitor_InputIntruderData(self.TrafficMonitor,0,<char*>self.intCallsign.c_str(),position,velocity,time,sigmaPos,sigmaVel)
        TrajManager_InputTraffic(self.TrajManager,<char*>self.intCallsign.c_str(),position,velocity,time)

    def InputGeofence(self,fenceList):
        
        cdef double Vert[50][2]
        cdef int numV
        for fence in fenceList:
            numV = len(fence['vertices'])
            for i in range(numV):
                Vert[i][0] = fence['vertices'][i][0]
                Vert[i][1] = fence['vertices'][i][1]

            GeofenceMonitor_InputGeofenceData(self.Geofence,
                                              0 if fence['type'] == 'KEEPIN' else 1,
                                              fence['id'],
                                              numV,
                                              fence['floor'],
                                              fence['roof'],
                                              Vert)
                                              
            TrajManager_InputGeofenceData(self.TrajManager,
                                          0 if fence['type'] == 'KEEPIN' else 1,
                                          fence['id'],
                                          numV,
                                          fence['floor'],
                                          fence['roof'],
                                          Vert)
    def StartFlight(self):
        StartMission(self.Cognition,0,0)

    def _RunCognition_(self,time):
        cdef Command cmd
        cdef waypoint_t wp[50]
        cdef double currPosition[3]
        cdef double trkGsVsCmd[3]
        cdef double distance
        cdef int numWP
        cdef int retVal

        for i in range(3):
            currPosition[i] = self.position[i]

        # Run the cognition module for the current time
        retVal = RunCognition(self.Cognition,time) 
        if retVal == -2:
            self.land = True 
        else:
            self.land = False

        # Handle outputs from cognition
        while(GetCognitionOutput(self.Cognition,&cmd) > 0):
            if cmd.commandType == FP_CHANGE:
                self.guidanceMode = FLIGHTPLAN
                self.activePlan =  cmd.fpChange.name
                nextWP = cmd.fpChange.wpIndex
                eta = False
                if self.activePlan == b"Plan0":
                    self.nextWP1 = nextWP
                    eta = self.etaFP1
                else:
                    self.nextWP2 = nextWP
                SetGuidanceMode(self.Guidance,FLIGHTPLAN,cmd.fpChange.name,cmd.fpChange.wpIndex,False)
                if self.verbose > 0:
                    print(self.callsign.decode('utf-8'), ": active plan = ",self.activePlan.decode('utf-8'))
            elif cmd.commandType == P2P_COMMAND:
                self.guidanceMode = POINT2POINT
                point = cmd.p2PCommand.point
                speed = cmd.p2PCommand.speed

                # Set first wp as current position
                wp[0].latitude = currPosition[0] 
                wp[0].longitude = currPosition[1] 
                wp[0].altitude = currPosition[2] 
                wp[0].time = 0

                # Second wp is destination
                wp[1].latitude = point[0]
                wp[1].longitude = point[1]
                wp[1].altitude = point[2]
                wp[2].time = ComputeDistance(currPosition,point)/speed;
 
                guidInputFlightplanData(self.Guidance,b'P2P',wp,2,0,False,self.params[b'TURN_RATE'])
                self.activePlan = b'P2P'
                self.nextWP2 = 1
                SetGuidanceMode(self.Guidance,POINT2POINT,b'P2P',1,False)
                if self.verbose > 0:
                    print(self.callsign.decode('utf-8'), ": active plan = ",self.activePlan.decode('utf-8'))
            elif cmd.commandType == VELOCITY_COMMAND:
                self.guidanceMode = VECTOR
                vn = cmd.velocityCommand.vn
                ve = cmd.velocityCommand.ve
                vu = -cmd.velocityCommand.vu
                trkGsVs = ConvertVnedToTrkGsVs(vn,ve,vu)
                for i in range(3):
                    trkGsVsCmd[i] = trkGsVs[i]
                SetGuidanceMode(self.Guidance,self.guidanceMode,"",0,False)
                guidInputVelocityCmd(self.Guidance,trkGsVsCmd)
            elif cmd.commandType == TAKEOFF_COMMAND:
                self.guidanceMode = TAKEOFF
            elif cmd.commandType == SPEED_CHANGE_COMMAND:
                self.etaFP1 = False
                speedChange =  cmd.speedChange.speed
                planID= cmd.speedChange.name
                nextWP = self.nextWP1 if planID == b"Plan0" else self.nextWP2
                ChangeWaypointSpeed(self.Guidance,planID,nextWP,speedChange,False)
            elif cmd.commandType == ALT_CHANGE_COMMAND:
                altChange = cmd.altChange.altitude
                planID= cmd.altChange.name
                nextWP = self.nextWP1 if planID == b"Plan0" else self.nextWP2
                ChangeWaypointAlt(self.Guidance,planID,nextWP,altChange,cmd.altChange.hold)
            elif cmd.commandType == STATUS_MESSAGE:
                if self.verbose > 0:
                    message = cmd.statusMessage.buffer
                    print(self.callsign.decode('utf-8'),":",message.decode('utf-8'))
            elif cmd.commandType == FP_REQUEST:
                self.numSecPlan += 1
                
                # Find the new path
                numWP = TrajManager_FindPath(self.TrajManager,
                                             cmd.fpRequest.name,
                                             cmd.fpRequest.fromPosition,
                                             cmd.fpRequest.toPosition,
                                             cmd.fpRequest.startVelocity,
                                             cmd.fpRequest.endVelocity)
                self.secPlanSize = numWP 
                if numWP > 0:
                    if self.verbose > 0:
                        print("%s : At %f s, Computed flightplan %s with %d waypoints" % (self.callsign.decode('utf-8'),time,cmd.fpRequest.name.decode('utf-8'),numWP))
                    # offset the new path with current time (because new paths start with t=0) 
                    TrajManager_SetPlanOffset(self.TrajManager,cmd.fpRequest.name,0,time)

                    # Combine new plan with rest of old plan
                    TrajManager_CombinePlan(self.TrajManager,cmd.fpRequest.name,"Plan0",-1)

                    for i in range(numWP):
                        TrajManager_GetWaypoint(self.TrajManager,cmd.fpRequest.name,i,&wp[i])

                    InputFlightPlanData(self.Cognition,cmd.fpRequest.name,wp,numWP,0,False,self.params[b'TURN_RATE'])
                    guidInputFlightplanData(self.Guidance,cmd.fpRequest.name,wp,numWP,0,False,self.params[b'TURN_RATE'])

                    # Set guidance mode with new flightplan and wp 0 to offset plan times
                    SetGuidanceMode(self.Guidance,FLIGHTPLAN,cmd.fpChange.name,0,False)

                else:
                    if self.verbose > 0:
                        print(self.callsign.decode('utf-8'),"Error finding path")    


    def _RunGuidance_(self,time):
        cdef GuidanceOutput_t guidOutput
        cdef int nextWP

        if self.guidanceMode is GUIDE_NOOP:
            return
        if self.guidanceMode is TAKEOFF:
            ReachedWaypoint(self.Cognition,b"Takeoff",0)
            return

        RunGuidance(self.Guidance,time)

        guidGetOutput(self.Guidance,&guidOutput)
        for i in range(3):
            self.controlInput[i] = guidOutput.velCmd[i]

        nextWP = guidOutput.nextWP
        if self.guidanceMode is not VECTOR:
            if self.activePlan == b"Plan0":
                if self.nextWP1 < nextWP:
                    ReachedWaypoint(self.Cognition,b"Plan0",nextWP-1)
                    self.nextWP1 = nextWP
                    if self.verbose > 0 and self.nextWP1 < self.missionPlanSize:
                        print("%s : Proceeding to waypoint %d on %s" % (self.callsign.decode('utf-8'),nextWP,self.activePlan.decode('utf-8')))
            else:
                if self.nextWP2 < nextWP:
                    self.nextWP2 = nextWP
                    ReachedWaypoint(self.Cognition,<char*>self.activePlan.c_str(),nextWP-1)
                    if self.verbose > 0 and self.nextWP2 < self.secPlanSize:
                        print("%s : Proceeding to waypoint %d on %s" % (self.callsign.decode('utf-8'),nextWP,self.activePlan.decode('utf-8')))
        else:
            pass

    def _RunGeofenceMonitor_(self):
        cdef geofenceConflict_t gfConflictData
        cdef uint8_t conflict
        cdef uint8_t violation
        cdef int id,
        cdef uint8_t type
        
        GeofenceMonitor_CheckViolation(self.Geofence,self.position,self.trkgsvs[0],self.trkgsvs[1],self.trkgsvs[2])

        gfConflictData.numConflicts = GeofenceMonitor_GetNumConflicts(self.Geofence)
        for i in range(gfConflictData.numConflicts):
            GeofenceMonitor_GetConflict(self.Geofence,i,&id,&conflict,&violation,
                                        gfConflictData.recoveryPosition,
                                        &type)
            gfConflictData.conflictFenceIDs[i] = id
            gfConflictData.conflictTypes[i] = type

        InputGeofenceConflictData(self.Cognition,&gfConflictData)

    def _RunTrajectoryMonitor_(self,time):
        cdef int nextWP
        cdef string planID
        cdef trajectoryMonitorData_t tjMonData
        nextWP = self.nextWP1 if self.activePlan == b"Plan0" else self.nextWP2
        planID = b"Plan+" if self.activePlan != b"Plan0" else b"Plan0"
        tjMonData = TrajManager_MonitorTrajectory(self.TrajManager,time,<char*>planID.c_str(),self.position,self.trkgsvs,self.nextWP1,nextWP)
        InputTrajectoryMonitorData(self.Cognition,&tjMonData)
        for i in range(3):
            self.planOffsets[i] = tjMonData.offsets1[i]
            self.planOffsets[3+i] = tjMonData.offsets2[i]

    def _RunTrafficMonitor_(self,time):
        cdef char callsign[25]
        cdef int alert
        cdef int numAlerts
        cdef double wind[2]

        wind[0] = self.windFrom
        wind[1] = self.windSpeed
        TrafficMonitor_MonitorTraffic(self.TrafficMonitor,wind)

        TrafficMonitor_GetAltBands(self.TrafficMonitor,&self.altBand) 
        TrafficMonitor_GetSpeedBands(self.TrafficMonitor,&self.gsBand) 
        TrafficMonitor_GetTrackBands(self.TrafficMonitor,&self.trkBand) 
        TrafficMonitor_GetVerticalSpeedBands(self.TrafficMonitor,&self.vsBand) 

        numAlerts = TrafficMonitor_GetTrafficAlerts(self.TrafficMonitor,0,callsign,&alert)
        if numAlerts > 0:
            InputTrafficAlert(self.Cognition,callsign,alert)
            for i in range(1,numAlerts):
                strcpy(callsign,b'')
                TrafficMonitor_GetTrafficAlerts(self.TrafficMonitor,0,callsign,&alert)
                InputTrafficAlert(self.Cognition,callsign,alert)

        InputTrackBands(self.Cognition,&self.trkBand)
        InputSpeedBands(self.Cognition,&self.gsBand)
        InputAltBands(self.Cognition,&self.altBand)
        InputVSBands(self.Cognition,&self.vsBand)

    def _RunMerger_(self,time):
        cdef int mergingActive
        cdef double trk,gs,vs
        mergingActive = MergerRun(self.Merger,time)
        InputMergeStatus(self.Cognition,mergingActive)
        if mergingActive == 3:
            MergerOutputVelocity(self.Merger,&trk,&gs,&vs)
            ChangeWaypointSpeed(self.Guidance,<char*>b'Plan0',self.nextWP1,gs,False)

    def Run(self,time):
        self._RunCognition_(time)
        self._RunGuidance_(time)
        self._RunTrafficMonitor_(time)
        self._RunTrajectoryMonitor_(time)
        self._RunMerger_(time)
        self._RunGeofenceMonitor_()

    def GetOutput(self):
        return self.controlInput

    def IsMissionComplete(self):
        return self.land

    def GetAllSecondaryPlans(self):
        cdef string planid
        cdef int n
        cdef waypoint_t wp
        cdef list fp = []
        cdef list fps = []
        for i in range(self.numSecPlan+1):
            fp = []
            planid = ('Plan'+str(i)).encode('utf-8')
            n = TrajManager_GetTotalWaypoints(self.TrajManager,<char*>planid.c_str())
            for j in range(n):
                TrajManager_GetWaypoint(self.TrajManager,<char*>planid.c_str(),j,&wp)
                fp.append({
                    'time': wp.time,
                    'latitude': wp.latitude,
                    'longitude': wp.longitude,
                    'altitude': wp.altitude,
                    'tcp': [wp.tcp[k] for k in range(3)],
                    'tcpValue': [wp.tcpValue[k] for k in range(3)],
                    'info': wp.info.decode('utf-8')})

            fps.append(fp) 

        return fps
            
    def GetPlanOffsets(self):
        return self.planOffsets

    def GetBands(self,bandType):
        cdef bands_t band
        if bandType == 'track':
            band = self.trkBand
        elif bandType == 'gs':
            band = self.gsBand
        elif bandType == 'vs':
            band = self.vsBand
        else:
            band = self.altBand

        return {
            'currentConflictBand': band.currentConflictBand,
            'resUp': band.resUp,
            'resDown': band.resDown,
            'numBands': band.numBands,
            'type': [band.type[i] for i in range(band.numBands)],
            'min':[band.min[i] for i in range(band.numBands)],
            'max':[band.max[i] for i in range(band.numBands)]
        }