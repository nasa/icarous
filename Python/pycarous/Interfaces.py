from ctypes import *
from enum import IntEnum

class VelocityCommand(Structure):
    _fields_ = [("vn",c_double),
                ("ve",c_double),
                ("vu",c_double)]

class P2PCommand(Structure):
    _fields_ = [("point",c_double*3),
                ("speed",c_double)]

class SpeedChange(Structure):
    _fields_ = [("name",c_char*25),
                ("speed",c_double),
                ("hold",c_double)]

class AltChange(Structure):
    _fields_ = [("name",c_char*25),
                ("altitude",c_double),
                ("hold",c_double)]

class TakeoffCommand(Structure):
    _fields_ = []

class LandCommand(Structure):
    _fields_ = []

class DitchCommand(Structure):
    _fields_ = []

class FpChange(Structure):
    _fields_ = [("name",c_char*25),
                ("wpIndex",c_int),
                ("nextFeasibleWp",c_int)]

class FpRequest(Structure):
    _fields_ = [("searchType",c_int),
                ("name",c_char*25),
                ("fromPosition",c_double*3),
                ("toPosition",c_double*3),
                ("startVelocity",c_double*3)]

class StatusMessage(Structure):
    _fields_ = [("severity",c_int),
                ("buffer", c_char*250)]

class CommandU(Union):
    _fields_ = [("velocityCommand",VelocityCommand),
                ("p2pCommand",P2PCommand),
                ("speedChange",SpeedChange),
                ("altChange",AltChange),
                ("takeoffCommand",TakeoffCommand),
                ("landCommand",LandCommand),
                ("ditchCommand",DitchCommand),
                ("fpChange",FpChange),
                ("fpRequest",FpRequest),
                ("statusMessage",StatusMessage)]

class CommandTypes(IntEnum):
    VELOCITY = 0
    P2P = 1
    SPEED_CHANGE = 2
    ALT_CHANGE = 3
    TAKEOFF = 4
    LAND = 5
    DITCH = 6
    FP_CHANGE = 7
    FP_REQUEST = 8
    STATUS_MESSAGE = 9


class Command(Structure):
    _fields_ = [("commandType",c_int),
                ("commandU",CommandU)]


int20arr = c_int*20
double20arr = c_double*20
bool50arr = c_bool*50

class Bands(Structure): 
    _pack_ = 1
    _fields_ = [ 
    ("time", c_double),
    ("numConflictTraffic", c_int),
    ("numBands",c_int),
    ("type",int20arr),
    ("min",double20arr),
    ("max",double20arr),
    ("recovery",c_int),
    ("currentConflictBand",c_int),
    ("timeToViolation",c_double*2),
    ("timeToRecovery",c_double),
    ("minHDist",c_double),
    ("minVDist",c_double),
    ("resUp",c_double),
    ("resDown",c_double),
    ("resPreferred",c_double),
    ("wpFeasibility1",bool50arr),
    ("wpFeasibility2",bool50arr),
    ("fp1ClosestPointFeasible",c_bool),
    ("fp2ClosestPointFeasible",c_bool)]

class GeofenceConflict(Structure):
    _pack_ = 1
    _fields_ = [
        ("numConflicts",c_uint8),
        ("numFences",c_uint16),
        ("conflictFenceIDs",c_uint8*5),
        ("conflictTypes",c_uint8*5),
        ("timeToViolation",c_double*5),
        ("recoveryPosition",c_double*3),
        ("waypointConflict1",bool50arr),
        ("directPathToWaypoint1",bool50arr),
        ("waypointConflict2",bool50arr),
        ("directPathToWaypoint2",bool50arr)]

