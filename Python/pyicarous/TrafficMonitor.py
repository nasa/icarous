from ctypes import *

from Interfaces import Bands

class TrafficMonitor():


    def __init__(self,callsign,configs,log):
        self.lib = CDLL('libTrafficMonitor.so')

        self.lib.newDaidalusTrafficMonitor.restype = c_void_p
        self.lib.newDaidalusTrafficMonitor.argtypes = [c_char_p,c_char_p,c_bool]
        self.lib.TrafficMonitor_UpdateParameters.argtypes = [c_void_p,c_char_p,c_bool]
        self.lib.TrafficMonitor_InputIntruderData.argtypes = [c_void_p,c_int,c_char_p,c_double*3,c_double*3,c_double]
        self.lib.TrafficMonitor_InputOwnshipData.argtypes  = [c_void_p,c_double*3,c_double*3,c_double]
        self.lib.TrafficMonitor_MonitorTraffic.argtypes = [c_void_p]
        self.lib.TrafficMonitor_CheckPointFeasibility.argtypes = [c_void_p,c_double*3,c_double]
        self.lib.TrafficMonitor_CheckPointFeasibility.restype = c_bool
        self.lib.TrafficMonitor_GetTrackBands.argtypes = [c_void_p,POINTER(Bands)]
        self.lib.TrafficMonitor_GetSpeedBands.argtypes = [c_void_p,POINTER(Bands)]
        self.lib.TrafficMonitor_GetAltBands.argtypes = [c_void_p,POINTER(Bands)]
        self.lib.TrafficMonitor_GetVerticalSpeedBands.argtypes = [c_void_p,POINTER(Bands)]
        daidalusFile = configs[0]
        self.obj = self.lib.newDaidalusTrafficMonitor(c_char_p(callsign.encode('utf-8')),c_char_p(daidalusFile.encode('utf-8')),c_bool(log))

    def SetParameters(self,params,log):
        self.lib.TrafficMonitor_UpdateParameters(self.obj,c_char_p(params.encode('utf-8')),c_bool(log))

    def input_traffic(self,index,position,velocity,time):
        cpos = c_double*3
        cvel = c_double*3
        _pos = cpos(position[0],position[1],position[2])
        _vel = cvel(velocity[0],velocity[1],velocity[2])
        _time = c_double(time)
        _index = c_int(index)
        ret = c_int(0)
        callsign = "traffic"+str(index)
        ret.value = self.lib.TrafficMonitor_InputIntruderData(self.obj,_index,c_char_p(callsign.encode('utf-8')),_pos,_vel,_time)
        
    def monitor_traffic(self,position,velocity,time):
        cpos = c_double*3
        _pos = cpos(position[0],position[1],position[2])
        _vel = cpos(velocity[0],velocity[1],velocity[2])
        self.lib.TrafficMonitor_InputOwnshipData(self.obj,_pos,_vel,c_double(time))
        self.lib.TrafficMonitor_MonitorTraffic(self.obj)

    def monitor_wp_feasibility(self,wp,speed):
        cpos = c_double*3
        _wp = cpos(wp[0],wp[1],wp[2])
        val = c_bool(0)
        val.value = self.lib.TrafficMonitor_CheckPointFeasibility(self.obj,_wp,c_double(speed))
        return val.value

    def GetTrackBands(self):
        bands = Bands()
        self.lib.TrafficMonitor_GetTrackBands(self.obj,byref(bands))
        return bands

    def GetGSBands(self):
        bands = Bands()
        self.lib.TrafficMonitor_GetSpeedBands(self.obj,byref(bands))
        return bands

    def GetAltBands(self):
        bands = Bands()
        self.lib.TrafficMonitor_GetAltBands(self.obj,byref(bands))
        return bands

    def GetVSBands(self):
        bands = Bands()
        self.lib.TrafficMonitor_GetVerticalSpeedBands(self.obj,byref(bands))
        return bands