from ctypes import *


cdouble3 = c_double*3

MAX_NODES = 5

class MergerData(Structure):
    _pack_ = 1
    _fields_ = [
            ("aircraftID",c_byte),\
            ("intersectionID",c_byte),\
            ("earlyArrivalTime",c_double),\
            ("currentArrivalTime",c_double),\
            ("lateArrivalTime",c_double),\
            ("numSchedulesComputed",c_int),\
            ("zoneStatus",c_ubyte)
            ]

class LogData(Structure):
    _pack_ = 1
    _fields_ = [
            ("intersectionID",c_int),\
            ("nodeRole",c_ubyte),\
            ("totalNodes",c_uint),\
            ("log",MergerData*MAX_NODES)
            ]


class Merger():
    def __init__(self,callsign,vehicleID):

        # Define the type interfaces
        self.lib = CDLL('libMerger.so')
        self.lib.MergerInit.restype = c_void_p
        self.lib.MergerInit.argtypes = [c_char_p,c_int]
        self.lib.MergerDeinit.argtypes = [c_void_p]
        self.lib.MergerSetAircraftState.argtypes = [c_void_p,cdouble3,cdouble3]
        self.lib.MergerSetVehicleConstraints.argtypes = [c_void_p,c_double,c_double,c_double]
        self.lib.MergerSetFixParams.argtypes = [c_void_p,c_double,c_double,c_double,c_double,c_double]
        self.lib.MergerSetIntersectionData.argtypes = [c_void_p,c_int,c_int,cdouble3]
        self.lib.MergerSetNodeLog.argtypes = [c_void_p,POINTER(LogData)]
        self.lib.MergerRun.argtypes = [c_void_p,c_double]
        self.lib.MergerRun.restype = c_ubyte
        self.lib.MergerOutputVelocity.argtypes = [c_void_p,c_double*1,c_double*1,c_double*1]
        self.lib.MergerOutputTrajectory.argtypes = [c_void_p,c_int,c_double*4]
        self.lib.MergerGetArrivalTimes.argtypes = [c_void_p,POINTER(MergerData)]
        self.lib.MergerGetArrivalTimes.restype = c_bool

        # Initiate the C++ class
        self.obj = self.lib.MergerInit(c_char_p(callsign.encode('utf-8')),c_int(vehicleID))

    def Deinit(self):
        self.lib.MergerDeinit(self.obj)

    def SetAircraftState(self,position,velocity):
        pos = cdouble3(*position)
        vel = cdouble3(*velocity)
        self.lib.MergerSetAircraftState(self.obj,pos,vel)

    def SetVehicleConstraints(self,mings,maxgs,turnRadius):
        self.lib.MergerSetVehicleConstraints(self.obj,c_double(mings),c_double(maxgs),c_double(turnRadius))

    def SetFixParams(self,sepTime,czone,szone,ezone,cwidth):
        self.lib.MergerSetFixParams(self.obj,c_double(sepTime),c_double(czone),c_double(szone),c_double(ezone),c_double(cwidth))

    def SetIntersectionData(self,intersection):
        for i,f in enumerate(intersection):
            fid = int(f[0])
            fix = f[1]
            pos = cdouble3(fix[0],fix[1],fix[2])
            self.lib.MergerSetIntersectionData(self.obj,c_int(i),c_int(fid),pos)

    def SetMergerLog(self,log):
        self.lib.MergerSetNodeLog(self.obj,byref(log))

    def GetVelocityCmd(self):
        trk = c_double(0)
        gs = c_double(0)
        vs = c_double(0)
        self.lib.MergerOutputVelocity(self.obj,byref(trk),byref(gs),byref(vs))
        return (trk.value,gs.value,vs.value)

    def GetArrivalTimes(self):
        arrData = MergerData()
        out = self.lib.MergerGetArrivalTimes(self.obj,byref(arrData))
        return (out,arrData)

    def Run(self,time):
        return self.lib.MergerRun(self.obj,c_double(time))
