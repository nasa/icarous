from ctypes import *

class Cog(Structure):
    _fields_ = [ \
 ( "UTCtime",c_ulonglong ),\
 ( "nextPrimaryWP",c_int ),\
 ( "nextSecondaryWP",c_int ),\
 ( "nextFeasibleWP1",c_int ),\
 ( "nextFeasibleWP2",c_int ),\
 ( "nextWP",c_int ),\
 ( "currentPlanID",c_char*10 ),\
 ( "Plan0",c_bool ),\
 ( "Plan1",c_bool ),\
 ( "primaryFPReceived",c_bool ),\
 ( "scenarioTime",c_double ),\
 ( "num_waypoints",c_int ),\
 ( "num_waypoints2",c_int ),\
 ( "wpPrev1",c_double*3 ),\
 ( "wpPrev2",c_double*3),\
 ( "wpNext1",c_double*3 ),\
 ( "wpNext2",c_double*3 ),\
 ( "wpNextFb1",c_double*3 ),\
 ( "wpNextFb2",c_double*3 ),\
 ( "position",c_double*3 ),\
 ( "velocity",c_double*3 ),\
 ( "hdg",c_double ),\
 ( "speed",c_double ),\
 ( "resolutionSpeed",c_double ),\
 ( "refWPTime",c_double ),\
 ( "wpMetricTime",c_bool ),\
 ( "keepInConflict",c_bool ),\
 ( "keepOutConflict",c_bool ),\
 ( "recoveryPosition",c_double*3 ),\
 ( "trafficConflict",c_bool ),\
 ( "trafficTrackConflict",c_bool ),\
 ( "trafficSpeedConflict",c_bool ),\
 ( "trafficAltConflict",c_bool ),\
 ( "returnSafe",c_bool ),\
 ( "trafficResType",c_int ),\
 ( "preferredTrack",c_double ),\
 ( "preferredSpeed",c_double ),\
 ( "preferredAlt",c_double ),\
 ( "DTHR",c_double ),\
 ( "ZTHR",c_double ),\
 ( "vsBandsNum",c_int ),\
 ( "resVUp",c_double ),\
 ( "resVDown",c_double ),\
 ( "prevResSpeed",c_double ),\
 ( "prevResAlt",c_double ),\
 ( "prevResTrack",c_double ),\
 ( "prevResVspeed",c_double ),\
 ( "trkBandNum",c_int ),\
 ( "trkBandType",c_int*20 ),\
 ( "trkBandMin",c_double*20 ),\
 ( "trkBandMax",c_double*20 ),\
 ( "resolutionTypeCmd",c_int ),\
 ( "XtrackConflict1",c_bool ),\
 ( "XtrackConflict2",c_bool ),\
 ( "directPathToFeasibleWP1",c_bool ),\
 ( "directPathToFeasibleWP2",c_bool ),\
 ( "xtrackDeviation1",c_double ),\
 ( "xtrackDeviation2",c_double ),\
 ( "allowedXtrackDev1",c_double ),\
 ( "allowedXtrackDev2",c_double ),\
 ( "nextWPFeasibility1",c_bool ),\
 ( "nextWPFeasibility2",c_bool ),\
 ( "xtrkGain",c_double ),\
 ( "fp1ClosestPointFeasible",c_bool ),\
 ( "fp2ClosestPointFeasible",c_bool ),\
 ( "ditchsite",c_double*3 ),\
 ( "ditch",c_bool ),\
 ( "resetDitch",c_bool ),\
 ( "endDitch",c_bool ),\
 ( "ditchRouteFeasible",c_bool ),\
 ( "fpPhase",c_int ),\
 ( "takeoffState",c_int ),\
 ( "cruiseState",c_int ),\
 ( "emergencyDescentState",c_int ),\
 ( "takeoffComplete",c_int ),\
 ( "geofenceConflictState",c_int ),\
 ( "XtrackConflictState",c_int ),\
 ( "trafficConflictState",c_int ),\
 ( "return2NextWPState",c_int ),\
 ( "request",c_byte ),\
 ( "missionStart",c_int ),\
 ( "requestGuidance2NextWP",c_int ),\
 ( "p2pcomplete",c_bool ),\
 ( "fp1complete",c_bool ),\
 ( "fp2complete",c_bool ),\
 ( "topofdescent",c_bool ),\
 ( "mergingActive",c_ubyte ),\
 ( "guidanceCommand",c_ubyte ),\
 ( "cmdparams",c_double*10 ),\
 ( "statusBuf",c_char*250 ),\
 ( "statusSeverity",c_ubyte ),\
 ( "sendCommand",c_bool ),\
 ( "sendStatusTxt",c_bool ),\
 ( "sendStatusWPReached",c_bool ),\
 ( "pathRequest",c_bool ),\
 ( "searchType",c_ubyte ),\
 ( "startPosition",c_double*3 ),\
 ( "stopPosition",c_double*3 ),\
 ( "startVelocity",c_double*3 ),\
 ]


class Cognition():
    def __init__(self):
        self.mCognition = CDLL('libCognition.so')
        self.mCognition.InitializeCognition.argtypes = [POINTER(Cog)] 
        self.mCognition.FlightPhases.argtypes = [POINTER(Cog)]

    def InitializeCognition(self,cogd):
        self.mCognition.InitializeCognition(byref(cogd))

    def RunFlightPhases(self,cogd):
        self.mCognition.FlightPhases(byref(cogd))
