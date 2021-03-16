from ctypes import *
from enum import IntEnum
from collections import namedtuple

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

    def getString(val):
        if val == TcpType.TCP_NONE:
            return "NONE"
        elif val == TcpType.TCP_BOT:
            return "BOT"
        elif val == TcpType.TCP_EOT:
            return "EOT"
        elif val == TcpType.TCP_MOT:
            return "MOT"
        elif val == TcpType.TCP_EOTBOT:
            return "EOTBOT"
        elif val == TcpType.TCP_NONEg:
            return "NONE"
        elif val == TcpType.TCP_BGS:
            return "BGS"
        elif val == TcpType.TCP_EGS:
            return "EGS"
        elif val == TcpType.TCP_EGSBGS:
            return "EGSBGS"
        elif val == TcpType.TCP_NONEv:
            return "NONE"
        elif val == TcpType.TCP_BVS:
            return "BVS"
        elif val == TcpType.TCP_EVS:
            return "EVS"
        elif val == TcpType.TCP_EVSBVS:
            return "EVSBVS"
        
class Waypoint(Structure):
    _pack_ = 1
    _fields_ = [("index",c_uint16),
                ("time",c_double),
                ("name",c_char*20),
                ("latitude",c_double),
                ("longitude",c_double),
                ("altitude",c_double),
                ("tcp",c_int*3),
                ("tcpValue",c_double*3),
                ("info",c_char*100)]

# Datastructure used for V2V data exchange
datafields = ['type','payload']
V2Vdata = namedtuple('V2Vdata',field_names=datafields)
