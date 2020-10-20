from ctypes import *
from enum import IntEnum
from collections import namedtuple

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
    _fields_ = [("name",c_char*25),
                ("fromPosition",c_double*3),
                ("toPosition",c_double*3),
                ("startVelocity",c_double*3),
                ("endVelocity",c_double*3)]

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

class TcpType(IntEnum):
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

class Command(Structure):
    _fields_ = [("commandType",c_int),
                ("commandU",CommandU)]


class Waypoint(Structure):
    _pack_ = 1
    _fields_ = [("index",c_uint16),
                ("time",c_double),
                ("name",c_char*20),
                ("latitude",c_double),
                ("longitude",c_double),
                ("altitude",c_double),
                ("tcp",c_int*3),
                ("tcpValue",c_double*3)]

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
    ("resPreferred",c_double) ]

class GeofenceConflict(Structure):
    _pack_ = 1
    _fields_ = [
        ("numConflicts",c_uint8),
        ("numFences",c_uint16),
        ("conflictFenceIDs",c_uint8*5),
        ("conflictTypes",c_uint8*5),
        ("timeToViolation",c_double*5),
        ("recoveryPosition",c_double*3)]

class TrajectoryMonitorData(Structure):
    _pack = 1
    _fields_ =[
        ("fenceConflict",c_bool),
        ("trafficConflict",c_bool),
        ("conflictFenceID",c_uint8),
        ("conflictCallSign",c_char*20),
        ("timeToFenceViolation",c_double),
        ("timeToTrafficViolation",c_double),
        ("recoveryPosition",c_double*3),
        ("offsets",c_double*3),
        ("nextWP",c_int),
        ("nextFeasibleWP",c_int)
    ]

# Datastructure used for V2V data exchange
datafields = ['type','payload']
V2Vdata = namedtuple('V2Vdata',field_names=datafields)

datafields = ['distance','VTAS','Altitude','ROCD','Psi','Phi','N_pos','E_pos']
SpqState = namedtuple('SpqState',field_names=datafields)

class SpqCommand(Structure):
    _pack = 1
    _fields_ =[
        ("VTAS",c_double),
        ("Alt",c_double),
        ("ROCD",c_double),
        ("Psi",c_double),
        ("GS",c_double),
        ("GT",c_double),
        ("VelocityControlMode",c_char*10),
        ("HeadingControlMode",c_char*10),
        ("VerticalControlMode",c_char*10),
    ]

class SpqGains(Structure):
    _pack = 1
    _fields_ =[
        ("k_VTAS",c_double),
        ("gLimit_dVTAS",c_double),
        ("k_ROCD",c_double),
        ("ROCD_max",c_double),
        ("gLimit_dROCD",c_double),
        ("k_Alt",c_double),
        ("k_deltaPsi",c_double),
        ("dPsi_desired_max",c_double),
        ("Phi_desired_max",c_double),
        ("k_Phi",c_double),
        ("dPhi_max",c_double),
        ("P_MAX",c_double),
        ("k_VTAS",c_bool),
        ("usePowerLimitEnforcement",c_double),
    ]

class SpqMassTable(Structure):
    _pack = 1
    _fields_ =[
        ("lo",c_double),
        ("nom",c_double),
        ("hi",c_double),
        ("W0_k_dVTAS",c_double),
        ("W0_k_dROCD",c_double),
        ("W0_k_bank",c_double),
        ("W0_k_ROCD",c_double),
        ("W0_dV_lim",c_double),
        ("W0_dR_lim",c_double),
    ]

class SpqMassLookup(Structure):
    _pack = 1
    _fields_ =[
        ("lo",c_double*13),
        ("nom",c_double*13),
        ("hi",c_double*13),
    ]

class SpqHover(Structure):
    _pack = 1
    _fields_ =[
        ("FL",c_double*13),
        ("fuel",SpqMassLookup),
        ("k_dVTAS",c_double),
        ("k_dROCD",c_double),
        ("k_bank",c_double),
        ("k_ROCD",c_double),
        ("dV_lim",c_double),
        ("dR_lim",c_double),
    ]

class SpqVertical(Structure):
    _pack = 1
    _fields_ =[
        ("FL",c_double*13),
        ("ROCD",c_double*13),
        ("fuel",SpqMassLookup),
        ("k_dVTAS",c_double),
        ("k_dROCD",c_double),
        ("k_bank",c_double),
        ("k_ROCD",c_double),
        ("dV_lim",c_double),
        ("dR_lim",c_double),
    ]

class SpqDescent(Structure):
    _pack = 1
    _fields_ =[
        ("FL",c_double*13),
        ("ROCD",c_double*13),
        ("TAS",c_double*13),
        ("fuel",SpqMassLookup),
        ("k_dVTAS",c_double),
        ("k_dROCD",c_double),
        ("k_bank",c_double),
        ("k_ROCD",c_double),
        ("dV_lim",c_double),
        ("dR_lim",c_double),
    ]

class SpqClimb(Structure):
    _pack = 1
    _fields_ =[
        ("FL",c_double*13),
        ("ROCD",SpqMassLookup),
        ("TAS",c_double*13),
        ("fuel",c_double*13),
        ("k_dVTAS",c_double),
        ("k_dROCD",c_double),
        ("k_bank",c_double),
        ("k_ROCD",c_double),
        ("dV_lim",c_double),
        ("dR_lim",c_double),
    ]

class SpqCruise(Structure):
    _pack = 1
    _fields_ =[
        ("FL",c_double*13),
        ("TAS",c_double*13),
        ("fuel",SpqMassLookup),
        ("k_dVTAS",c_double),
        ("k_dROCD",c_double),
        ("k_bank",c_double),
        ("k_ROCD",c_double),
        ("dV_lim",c_double),
        ("dR_lim",c_double),
    ]

class SpqMcpIrp(Structure):
    _pack = 1
    _fields_ =[
        ("FL",c_double*13),
        ("TAS",SpqMassLookup),
        ("k_dVTAS",c_double),
        ("k_dROCD",c_double),
        ("k_bank",c_double),
        ("k_ROCD",c_double),
        ("dV_lim",c_double),
        ("dR_lim",c_double),
    ]

class SpqModel(Structure):
    _pack = 1
    _fields_ =[
        ("Hover",SpqHover),
        ("VerticalClimb",SpqVertical),
        ("VerticalDescent",SpqVertical),
        ("LowDescent",SpqDescent),
        ("Descent",SpqDescent),
        ("Climb",SpqClimb),
        ("LowCruise",SpqCruise),
        ("NominalCruise",SpqCruise),
        ("HighCruise",SpqCruise),
        ("EndCruise",SpqCruise),
        ("TransitionCruise",SpqCruise),
        ("MCPCruise",SpqMcpIrp),
        ("IRPCruise",SpqMcpIrp),
        ("mass",SpqMassTable),
        ("batteryCapacity",c_double),
        ("MCP_Power_Limit",c_double),
        ("IRP_Power_Limit",c_double),
    ]
