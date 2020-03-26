from ctypes import *

class cog(Structure):
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
 ( "mergingActive",c_bool ),\
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


mCognition = CDLL('libCognition.so')
gCog = cog.in_dll(mCognition,'cog');

def initCognition():
    gCog.returnSafe = True;
    gCog.nextPrimaryWP = 1;
    gCog.resolutionTypeCmd = -1;
    gCog.request = 0;
    gCog.fpPhase = 0;
    gCog.missionStart = -1;
    gCog.keepInConflict = False;
    gCog.keepOutConflict = False;
    gCog.p2pcomplete = False;
    gCog.takeoffComplete = -1;

    gCog.trafficConflictState = 0;
    gCog.geofenceConflictState = 0;
    gCog.trafficTrackConflict = False;
    gCog.trafficSpeedConflict = False;
    gCog.trafficAltConflict = False;
    gCog.XtrackConflictState = 0;
    gCog.resolutionTypeCmd = 2;
    gCog.requestGuidance2NextWP = -1;
    gCog.searchType = 1;
    gCog.topofdescent = False;
    gCog.ditch = False;
    gCog.endDitch = False;
    gCog.resetDitch = False;
    gCog.primaryFPReceived = False;
    gCog.nextWPFeasibility1 = True;
    gCog.nextWPFeasibility2 = True;

initCognition()
