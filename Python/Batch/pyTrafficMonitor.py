from ctypes import *

lib = CDLL('../../Modules/lib/libTrafficMonitor.dylib')

lib._wrap_new_TrafficMonitor.restype = c_void_p
lib._wrap_TrafficMonitor_InputTraffic.argtypes = [c_void_p,c_int,c_double*3,c_double*3,c_double]
lib._wrap_TrafficMonitor_MonitorTraffic.argtypes = [c_void_p,c_double*3,c_double*3,c_double]
lib._wrap_TrafficMonitor_MonitorWPFeasibility.argtypes = [c_void_p,c_double*3,c_double*3,c_double*3]
lib._wrap_TrafficMonitor_CheckSafeToTurn.argtypes =[c_void_p,c_double*3,c_double*3,c_double,c_double] 
lib._wrap_TrafficMonitor_GetTrackBands.argtypes = [c_void_p,c_int*1,c_int*5,c_double*5,c_double*5,c_int*1,c_int*1,c_double*1,c_double*1,c_double*1,c_double*1,c_double*1,c_double*1,c_double*1]
lib._wrap_TrafficMonitor_GetGSBands.argtypes = [c_void_p,c_int*1,c_int*5,c_double*5,c_double*5,c_int*1,c_int*1,c_double*1,c_double*1,c_double*1,c_double*1,c_double*1,c_double*1,c_double*1]
lib._wrap_TrafficMonitor_GetAltBands.argtypes = [c_void_p,c_int*1,c_int*5,c_double*5,c_double*5,c_int*1,c_int*1,c_double*1,c_double*1,c_double*1,c_double*1,c_double*1,c_double*1,c_double*1]

class TrafficMonitor():
    def __init__(self,log):
        self.obj = lib._wrap_new_TrafficMonitor(c_bool(log),c_char_p("DaidalusQuadConfig.txt".encode('utf-8')))
  
    def input_traffic(self,index,position,velocity,time):
        cpos = c_double*3
        cvel = c_double*3
        _pos = cpos(position[0],position[1],position[2])
        _vel = cvel(velocity[0],velocity[1],velocity[2])
        _time = c_double(time)
        _index = c_int(index)
        ret = c_int(0)
        ret.value = lib._wrap_TrafficMonitor_InputTraffic(self.obj,_index,_pos,_vel,_time)
        
    def monitor_traffic(self,position,velocity,time):
        cpos = c_double*3
        _pos = cpos(position[0],position[1],position[2])
        _vel = cpos(velocity[0],velocity[1],velocity[2])
        lib._wrap_TrafficMonitor_MonitorTraffic(self.obj,_pos,_vel,c_double(time))

    def monitor_wp_feasibility(self,position,velocity,wp):
        cpos = c_double*3
        _pos = cpos(position[0],position[1],position[2])
        _vel = cpos(velocity[0],velocity[1],velocity[2])
        _wp = cpos(wp[0],wp[1],wp[2])
        val = c_bool(0)
        val.value = lib._wrap_TrafficMonitor_MonitorWPFeasibility(self.obj,_pos,_vel,_wp)
        return val.value

    def check_safe_to_turn(self,position,velocity,fromHeading,toHeading):
        cpos = c_double*3
        _pos = cpos(position[0],position[1],position[2])
        _vel = cpos(velocity[0],velocity[1],velocity[2])
        ret = c_int(0)
        ret.value = lib._wrap_TrafficMonitor_CheckSafeToTurn(self.obj,_pos,_vel,c_double(fromHeading),c_double(toHeading))
        return ret.value
      
    def GetTrackBands(self):
       numBands = c_int(0)
       intArr10 = c_int * 5
       doubleArr10 = c_double * 5
       recovery = c_int(0)
       currentConflict = c_int(0)
       tviolation = c_double(0)
       trecovery = c_double(0)
       minhdist = c_double(0)
       minvdist = c_double(0)
       resup = c_double(0)
       resdown = c_double(0)
       respref = c_double(0)
       bandTypes = intArr10(0,0,0,0,0)
       low = doubleArr10(0,0,0,0,0)
       high = doubleArr10(0,0,0,0,0)
       lib._wrap_TrafficMonitor_GetTrackBands(self.obj,byref(numBands),bandTypes,low,
                                               high,
                                               byref(recovery),
                                               byref(currentConflict),
            				       byref(tviolation),
 					       byref(trecovery),
                                               byref(minhdist),
 					       byref(minvdist),
               				       byref(resup),
                                               byref(resdown),
                                               byref(respref))

       return (currentConflict.value,respref.value)

    def GetGSBands(self):
       numBands = c_int(0)
       intArr10 = c_int * 5
       doubleArr10 = c_double * 5
       recovery = c_int(0)
       currentConflict = c_int(0)
       tviolation = c_double(0)
       trecovery = c_double(0)
       minhdist = c_double(0)
       minvdist = c_double(0)
       resup = c_double(0)
       resdown = c_double(0)
       respref = c_double(0)
       bandTypes = intArr10(0,0,0,0,0)
       low = doubleArr10(0,0,0,0,0)
       high = doubleArr10(0,0,0,0,0)
       lib._wrap_TrafficMonitor_GetGSBands(self.obj,byref(numBands),bandTypes,low,
                                               high,
                                               byref(recovery),
                                               byref(currentConflict),
            				       byref(tviolation),
 					       byref(trecovery),
                                               byref(minhdist),
 					       byref(minvdist),
               				       byref(resup),
                                               byref(resdown),
                                               byref(respref))

       return (currentConflict.value,respref.value)

    def GetAltBands(self):
       numBands = c_int(0)
       intArr10 = c_int * 5
       doubleArr10 = c_double * 5
       recovery = c_int(0)
       currentConflict = c_int(0)
       tviolation = c_double(0)
       trecovery = c_double(0)
       minhdist = c_double(0)
       minvdist = c_double(0)
       resup = c_double(0)
       resdown = c_double(0)
       respref = c_double(0)
       bandTypes = intArr10(0,0,0,0,0)
       low = doubleArr10(0,0,0,0,0)
       high = doubleArr10(0,0,0,0,0)
       lib._wrap_TrafficMonitor_GetAltBands(self.obj,byref(numBands),bandTypes,low,
                                               high,
                                               byref(recovery),
                                               byref(currentConflict),
            				       byref(tviolation),
 					       byref(trecovery),
                                               byref(minhdist),
 					       byref(minvdist),
               				       byref(resup),
                                               byref(resdown),
                                               byref(respref))

       return (currentConflict.value,resdown.value,resup.value,respref.value)

