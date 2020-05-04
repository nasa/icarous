from ctypes import *
from enum import IntEnum

class GuidanceCommands(IntEnum):
    PRIMARY_FLIGHTPLAN = 0
    SECONDARY_FLIGHTPLAN = 1
    VECTOR = 2
    POINT2POINT = 3
    ORBIT = 4
    HELIX = 5
    TAKEOFF = 6
    LAND = 7
    SPEED_CHANGE = 8
    NOOP = 9


class GuidanceInput(Structure):
     _fields_ = [ 
     ("position",c_double*3), \
     ("velocity",c_double*3), \
     ("prev_waypoint",c_double*5),\
     ("curr_waypoint",c_double*5),\
     ("num_waypoints",c_int),\
     ("nextWP",c_int),\
     ("reachedStatusUpdated",c_bool),\
     ("speed",c_double),\
     ("velCmd",c_double*3)
     ]


class GuidanceOutput(Structure):
     _fields_ = [ ("velCmd",c_double*3), \
                 ("newNextWP",c_int), \
                 ("reachedStatusUpdated",c_bool)]


class GuidanceTable(Structure):
     _fields_ = [ \
                 ( "defaultWpSpeed",c_double ), \
                 ( "captureRadiusScaling",c_double ), \
                 ( "guidanceRadiusScaling",c_double ), \
                 ( "xtrkDev",c_double ), \
                 ( "climbFpAngle",c_double ), \
                 ( "climbAngleVRange",c_double ), \
                 ( "climbAngleHRange",c_double ), \
                 ( "climbRateGain",c_double ), \
                 ( "maxClimbRate",c_double ), \
                 ( "minClimbRate",c_double ), \
                 ( "maxCap",c_double ), \
                 ( "minCap",c_double ), \
                 ( "maxSpeed",c_double ), \
                 ( "minSpeed",c_double ), \
                 ( "yawForward",c_bool ) \
                ]


mGuidance = CDLL('libGuidance.so')

mGuidance.ComputeFlightplanGuidanceInput.argtypes = [POINTER(GuidanceInput),POINTER(GuidanceOutput),POINTER(GuidanceTable)]
mGuidance.ComputeFlightplanGuidanceInput.restype = c_int

#inp = GuidanceInput()
#out = GuidanceOutput()
#tbl = GuidanceTable()
#
#inp.num_waypoints = 15
#mGuidance.ComputeFlightplanGuidanceInput(byref(inp),byref(out),byref(tbl))
