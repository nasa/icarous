from libc.stdint cimport int8_t,uint8_t,uint16_t,int32_t,uint32_t

cdef extern from "Commands.hpp":
    ctypedef struct VelocityCommand:
        double vn
        double ve
        double vu
    
    ctypedef struct P2PCommand:
        double point[3]
        double speed
    
    ctypedef struct SpeedChange:
        char name[25]
        double speed
        int hold
    
    ctypedef struct AltChange:
        char name[25]
        double altitude
        int hold
    
    ctypedef struct TakeoffCommand:
        char temp
    
    ctypedef struct LandCommand:
        char temp
    
    ctypedef struct DitchCommand:
        char temp
    
    ctypedef struct FpChange:
        char name[25]
        int wpIndex
        int nextFeasibleWp
    
    ctypedef struct FpRequest:
        char name[25]
        double fromPosition[3]
        double toPosition[3]
        double startVelocity[3]
        double endVelocity[3]
    
    ctypedef struct StatusMessage:
        int severity
        char buffer[250]
    
    ctypedef enum CommandType_e:
        VELOCITY_COMMAND
        P2P_COMMAND
        SPEED_CHANGE_COMMAND
        ALT_CHANGE_COMMAND
        TAKEOFF_COMMAND
        LAND_COMMAND
        DITCH_COMMAND
        FP_CHANGE
        FP_REQUEST
        STATUS_MESSAGE
    
    ctypedef union CommandVal:
        VelocityCommand velocityCommand
        P2PCommand p2PCommand
        SpeedChange speedChange
        AltChange altChange
        TakeoffCommand takeoffCommand
        LandCommand landCommand
        DitchCommand ditchCommand
        FpChange fpChange
        FpRequest fpRequest
        StatusMessage statusMessage
    
    ctypedef struct Command:
        CommandType_e commandType
        VelocityCommand velocityCommand
        P2PCommand p2PCommand
        SpeedChange speedChange
        AltChange altChange
        TakeoffCommand takeoffCommand
        LandCommand landCommand
        DitchCommand ditchCommand
        FpChange fpChange
        FpRequest fpRequest
        StatusMessage statusMessage

cdef extern from "Guidance.h":
    ctypedef enum GuidanceMode:
        FLIGHTPLAN
        VECTOR
        POINT2POINT
        TAKEOFF
        LAND
        SPEED_CHANGE
        ALT_CHANGE
        GUIDE_NOOP

    ctypedef struct GuidanceOutput_t:
        char activePlan[25]
        int guidanceMode
        int nextWP
        double distH2WP
        double distV2WP
        double xtrackDev
        double velCmd[3]
        double target[3]
        bint wpReached
        bint yawForward

cdef extern from "Interfaces.h":
    ctypedef packed struct bands_t:
        double time
        int numConflictTraffic
        int numBands
        int type[20]
        double min[20]
        double max[20]
        int recovery
        int currentConflictBand
        double timeToViolation[2]
        double timeToRecovery
        double minHDist
        double minVDist
        double resUp
        double resDown
        double resPreferred
    
    ctypedef enum tcp_e:
        TCP_NONE      = 0
        TCP_BOT       = 1
        TCP_EOT       = 2
        TCP_MOT       = 4
        TCP_EOTBOT    = 5
        TCP_NONEg     = 6
        TCP_BGS       = 7
        TCP_EGS       = 8
        TCP_EGSBGS    = 9
        TCP_NONEv     = 10
        TCP_BVS       = 11
        TCP_EVS       = 12
        TCP_EVSBVS    = 13

    ctypedef enum objectType_e:
        _TRAFFIC_SIM_   = 0
        _TRAFFIC_ADSB_  = 1
        _TRAFFIC_RADAR_ = 2
        _TRAFFIC_FLARM_ = 3
        _OBSTACLE_      = 4
    
    ctypedef packed struct waypoint_t:
        uint16_t  index
        double  time
        char    name[20]
        double  latitude
        double  longitude
        double  altitude
        tcp_e tcp[3]
        double  tcpValue[3]
        char info[100]
    
    ctypedef packed struct trajectoryMonitorData_t:
        bint fenceConflict
        bint trafficConflict
        bint lineOfSight2goal 
        uint8_t conflictFenceID
        char conflictCallSign[20]
        double timeToTrafficViolation
        double timeToFenceViolation
        double recoveryPosition[3]
        double offsets1[3]
        double offsets2[3]
        int nextWP
        int nextFeasibleWP
    
    ctypedef struct cognition_params_t:
        int resolutionType
        double DTHR
        double ZTHR
        double allowedXtrackDeviation
        double lookaheadTime
        double persistenceTime
        int return2NextWP
        bint active
    
    ctypedef packed struct geofenceConflict_t:
        uint8_t numConflicts
        uint16_t numFences
        uint8_t conflictFenceIDs[5]
        uint8_t conflictTypes[5]
        double timeToViolation[5]
        double recoveryPosition[3]

cdef extern from "TrafficMonitor.h":
    void* newDaidalusTrafficMonitor(char* callsign,char* filename)
    int TrafficMonitor_InputIntruderData(void * obj,int source, int id, char* calls, double *pos,double * vel, double time,double sumPos[6],double sumVel[6])
    int TrafficMonitor_InputOwnshipData(void * obj, double * position, double * velocity, double time,double sumPos[6],double sumVel[6]) 
    void TrafficMonitor_MonitorTraffic(void * obj,double *wind)
    bint TrafficMonitor_CheckPointFeasibility(void *obj,double *pos,double speed)
    void TrafficMonitor_GetTrackBands(void* obj,bands_t* bands)
    void TrafficMonitor_GetSpeedBands(void* obj,bands_t* bands)
    void TrafficMonitor_GetAltBands(void* obj,bands_t* bands)
    void TrafficMonitor_GetVerticalSpeedBands(void* obj,bands_t* bands)
    int TrafficMonitor_GetTrafficAlerts(void* obj,int,char* id,int* alert)

cdef extern from "TargetTracker.h":
    void* new_TargetTracker(char* callsign,char* configFile)
    void TargetTracker_ReadParamsFromFile(void* obj,char* configFile);
    void TargetTracker_SetHomePosition(void* obj,double position[3])
    void TargetTracker_SetModelUncertainty(void* obj,double sigmaP[6],double sigmaV[6]);
    void TargetTracker_SetGateThresholds(void* obj,double p,double v);
    void TargetTracker_InputMeasurement(void* obj,char* callsign,double time,double position[3],double velocity[3],double sigmaPos[6],double sigmaVel[6])
    void TargetTracker_InputCurrentState(void* obj,double time,double position[3],double velocity[3],double sigmaP[6],double sigmaV[6]);
    int TargetTracker_GetTotalIntruders(void* obj)
    void TargetTracker_GetIntruderData(void* obj,int i,char* callsign,double* time,double position[3],double velocity[3],double sigmaPos[6],double sigmaVel[6])
    void TargetTracker_UpdatePredictions(void* obj,double time);

cdef extern from "Cognition.h":
    void* CognitionInit(const char callsign[],const char config[])
    void ReadParamFromFile(void* obj,char config[]);
    void InputVehicleState(void *obj,const double pos[3],const double vel[3],const double heading)
    void InputFlightPlanData(void* obj,char planID[],waypoint_t wpts[],int totalWP,
                             double initHeading,bint kinematize,double repairTurnRate)
    void InputTrajectoryMonitorData(void* obj,const trajectoryMonitorData_t* tjMonData)
    void InputParameters(void *obj,const cognition_params_t *new_params)
    void InputDitchStatus(void *obj,const double ditch_pos[3],const double todAltitude,const bint ditch_requested)
    void InputMergeStatus(void *obj,const int merge_status)
    void InputTrafficAlert(void *obj,const char callsign[20],const int alertid)
    void InputTrackBands(void *obj,const bands_t *track_bands)
    void InputSpeedBands(void *obj,const bands_t *speed_bands)
    void InputAltBands(void *obj,const bands_t *alt_bands)
    void InputVSBands(void *obj,const bands_t *vs_bands)
    void InputGeofenceConflictData(void *obj,const geofenceConflict_t *gf_conflict)
    void ReachedWaypoint(void *obj, const char plan_id[], const int wp_reached_id)
    int GetCognitionOutput(void *obj,Command *command)
    void GetResolutionType(void *obj)
    void StartMission(void *obj,const int mission_start_value,const double delay)
    int RunCognition(void *obj,double time) 

cdef extern from "Guidance.h":
    ctypedef struct GuidanceParams_t:
        double defaultWpSpeed
        double captureRadiusScaling
        double guidanceRadiusScaling
        double turnRateGain
        double climbFpAngle
        double climbAngleVRange
        double climbAngleHRange
        double climbRateGain
        double maxClimbRate
        double minClimbRate
        double maxCap
        double minCap
        double maxSpeed
        double minSpeed
        bint yawForward

    ctypedef struct GuidanceOutput_t:
        char activePlan[25];
        int guidanceMode;
        int nextWP;
        double distH2WP;
        double distV2WP;
        double xtrackDev;
        double velCmd[3];
        double target[3];
        bint wpReached;

    void* InitGuidance(char config[])
    void guidReadParamFromFile(void* obj,char config[]);
    void guidSetParams(void* obj,GuidanceParams_t* params)
    void guidSetAircraftState(void* obj, double position[],double velocity[])
    void guidSetWindData(void* obj,double windFrom,double windSpeed)
    void guidInputFlightplanData(void* obj,char planID[],waypoint_t wpts[],int totalWP,double initHeading,bint kinematize,double repairTurnRate)
    void RunGuidance(void* obj,double time)
    void guidInputVelocityCmd(void* obj,double velcmd[])
    void SetGuidanceMode(void* obj,GuidanceMode mode,char* planID,int nextWP,bint eta)
    void ChangeWaypointSpeed(void* obj,char planID[],int wpID,double val,bint updateAll)
    void ChangeWaypointAlt(void* obj,char planID[],int wpID,double val,bint updateAll)
    void ChangeWaypointETA(void* obj,char planID[],int wpID,double val,bint updateAll)
    void guidGetOutput(void* obj,GuidanceOutput_t* output)
    int guidGetWaypoint(void* obj, char planID[],int id, waypoint_t *wp)

cdef extern from "DubinsParams.h":
    ctypedef struct DubinsParams_t:
        double wellClearDistH;
        double wellClearDistV;
        double turnRate;
        double gs;
        double vs;
        double maxGS;
        double minGS;
        double maxVS;
        double minVS;
        double hAccel;
        double hDaccel;
        double vAccel;
        double vDaccel;
        double climbgs;
        double zSections;
        double vertexBuffer; 
        double maxH;


cdef extern from "TrajManager.h":
    void* new_TrajManager(char callsign[],char config[]);
    void TrajManager_ReadParamFromFile(void* obj,char config[]);
    void TrajManager_UpdateDubinsPlannerParameters(void * obj,DubinsParams_t* params);
    void TrafficMonitor_UpdateParameters(void * obj, char * carg2,bint log);
    int TrajManager_FindPath(void * obj,char planID[], double fromPosition[],double toPosition[],double fromVelocity[],double toVelocity[]);
    int TrajManager_GetTotalWaypoints(void * obj, char * planid);
    int TrajManager_GetWaypoint(void * obj, char * planid, int id,  waypoint_t * wp);
    void TrajManager_ClearAllPlans(void * obj);
    void TrajManager_InputGeofenceData(void * obj,int type,int index, int totalVertices, double floor, double ceiling, double pos[][2]);
    int TrajManager_InputTraffic(void* obj,char* callsign, double *position, double *velocity,double time);
    void TrajManager_InputFlightPlan(void* obj, char planID[],waypoint_t wpts[],int totalwp,
                                     double initHeading,bint kinematize,double repairTurnRate);
    void TrajManager_CombinePlan(void* obj,char planID_A[],char planID_B[],int index);
    void TrajManager_PlanToString(void* obj,char planID[],char outputString[],bint tcpColumnsLocal,long int timeshift);
    void TrajManager_StringToPlan(void* obj,char planID[],char inputString[]);
    void TrajManager_SetPlanOffset(void*obj, char planID[],int n,double offset);
    void TrajManager_GetPlanPosition(void* obj, char planID[],double t,double pos[]);
    trajectoryMonitorData_t TrajManager_MonitorTrajectory(void* obj,double time,char planID[],double position[],double velocity[],int nextWP1,int nextWP2);

cdef extern from "Merger.h": 
    ctypedef packed struct mergingData_t:
        int8_t aircraftID
        int8_t intersectionID
        double earlyArrivalTime
        double currentArrivalTime
        double lateArrivalTime
        int32_t numSchedulesComputed
        uint8_t zoneStatus
    
    ctypedef packed struct dataLog_t:
        int32_t intersectionID
        uint8_t nodeRole
        uint32_t totalNodes
        mergingData_t log[10]


    void* MergerInit(char callsign[],char config[],int vehicleID);
    void MergerReadParamFromFile(void* obj,char config[]);
    void  MergerDeinit(void *obj);
    void  MergerSetAircraftState(void* obj, double pos[],double vel[]);
    void  MergerSetVehicleConstraints(void* obj, double minVel, double maxVel, double turnRadius);
    void  MergerSetFixParams(void *obj, double separationTime,double coordzone,double schzone,double entryzone,double corridorWidth);
    void  MergerSetIntersectionData(void *obj, int i,int id, double position[]);
    void  MergerSetNodeLog(void* obj,dataLog_t* mgData);
    unsigned char  MergerRun(void *obj,double time);
    void  MergerOutputVelocity(void* obj,double* trk,double* gs,double* vs);
    int   MergerOutputTrajectory(void* obj,int i, double wp[4]);
    bint MergerGetArrivalTimes(void* obj,mergingData_t* mgData);

cdef extern from "UtilFunctions.h":
    double ComputeDistance(double posA[],double posB[])

