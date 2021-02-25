from ctypes import *


cdouble3 = c_double*3

MAX_NODES = 10

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

    def toJson(self):
        out = {}
        for name,type in self._fields_:
            out[name] = self.__getattribute__(name)
        return out

    def fromJson(self,obj):
        keys = obj.keys()
        for key in keys:
            self.__setattr__(key,obj[key])


class LogData(Structure):
    _pack_ = 1
    _fields_ = [
            ("intersectionID",c_int),\
            ("nodeRole",c_ubyte),\
            ("totalNodes",c_uint),\
            ("log",MergerData*MAX_NODES)
            ]

    def toJson(self):
        out = {}
        for name,type in self._fields_:
            if name == 'log':
                out[name] = []
                for i in range(MAX_NODES):
                    out[name] = self.__getattribute__(name)[i].toJson()
            else:
                out[name] = self.__getattribute__(name)
        return out
    
    def fromJson(self,obj):
        keys = obj.keys()
        for key in keys:
            if key == 'log':
                logs = (MergerData*MAX_NODES)()
                for i in range(MAX_NODES):
                    logs[i] = obj[key][i]
                self.__setattr__(key,logs)
            else:
                self.__setattr__(key,obj[key])

class Merger():
    def __init__(self,callsign,vehicleID):

        # Define the type interfaces
        self.lib1 = CDLL('libMerger.so')
        self.lib1.MergerInit.restype = c_void_p
        self.lib1.MergerInit.argtypes = [c_char_p,c_int]
        self.lib1.MergerDeinit.argtypes = [c_void_p]
        self.lib1.MergerSetAircraftState.argtypes = [c_void_p,cdouble3,cdouble3]
        self.lib1.MergerSetVehicleConstraints.argtypes = [c_void_p,c_double,c_double,c_double]
        self.lib1.MergerSetFixParams.argtypes = [c_void_p,c_double,c_double,c_double,c_double,c_double]
        self.lib1.MergerSetIntersectionData.argtypes = [c_void_p,c_int,c_int,cdouble3]
        self.lib1.MergerSetNodeLog.argtypes = [c_void_p,POINTER(LogData)]
        self.lib1.MergerRun.argtypes = [c_void_p,c_double]
        self.lib1.MergerRun.restype = c_ubyte
        self.lib1.MergerOutputVelocity.argtypes = [c_void_p,c_double*1,c_double*1,c_double*1]
        self.lib1.MergerOutputTrajectory.argtypes = [c_void_p,c_int,c_double*4]
        self.lib1.MergerGetArrivalTimes.argtypes = [c_void_p,POINTER(MergerData)]
        self.lib1.MergerGetArrivalTimes.restype = c_bool

        # Initiate the C++ class
        self.obj1 = self.lib1.MergerInit(c_char_p(callsign.encode('utf-8')),c_int(vehicleID))

    def Deinit(self):
        self.lib1.MergerDeinit(self.obj1)

    def SetAircraftState(self,position,velocity):
        pos = cdouble3(*position)
        vel = cdouble3(*velocity)
        self.lib1.MergerSetAircraftState(self.obj1,pos,vel)

    def SetVehicleConstraints(self,mings,maxgs,turnRadius):
        self.lib1.MergerSetVehicleConstraints(self.obj1,c_double(mings),c_double(maxgs),c_double(turnRadius))

    def SetFixParams(self,sepTime,czone,szone,ezone,cwidth):
        self.lib1.MergerSetFixParams(self.obj1,c_double(sepTime),c_double(czone),c_double(szone),c_double(ezone),c_double(cwidth))

    def SetIntersectionData(self,intersection):
        for i,f in enumerate(intersection):
            fid = int(f[0])
            fix = f[1]
            pos = cdouble3(fix[0],fix[1],fix[2])
            self.lib1.MergerSetIntersectionData(self.obj1,c_int(i),c_int(fid),pos)

    def SetMergerLog(self,log):
        self.lib1.MergerSetNodeLog(self.obj1,byref(log))

    def GetVelocityCmd(self):
        trk = c_double(0)
        gs = c_double(0)
        vs = c_double(0)
        self.lib1.MergerOutputVelocity(self.obj1,byref(trk),byref(gs),byref(vs))
        return (trk.value,gs.value,vs.value)

    def GetArrivalTimes(self):
        arrData = MergerData()
        out = self.lib1.MergerGetArrivalTimes(self.obj1,byref(arrData))
        return (out,arrData)

    def Run(self,time):
        active = self.lib1.MergerRun(self.obj1,c_double(time))
        return active