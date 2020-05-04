from ctypes import *

class Bands():
    def __init__(self):
        self.currentConflict = 0
        self.resup = 0
        self.resdown = 0
        self.resdown = 0
        self.numBands = 0
        self.bandTypes = []
        self.low = []
        self.high = []

class TrafficMonitor():


    def __init__(self,log):
        self.lib = CDLL('libTrafficMonitor.so')

        self.lib._wrap_new_TrafficMonitor.restype = c_void_p
        self.lib._wrap_TrafficMonitor_InputTraffic.argtypes = [c_void_p,c_int,c_char_p,c_double*3,c_double*3,c_double]
        self.lib._wrap_TrafficMonitor_MonitorTraffic.argtypes = [c_void_p,c_double*3,c_double*3,c_double]
        self.lib._wrap_TrafficMonitor_MonitorWPFeasibility.argtypes = [c_void_p,c_double*3,c_double*3,c_double*3]
        self.lib._wrap_TrafficMonitor_CheckSafeToTurn.argtypes =[c_void_p,c_double*3,c_double*3,c_double,c_double] 
        self.lib._wrap_TrafficMonitor_UpdateDAAParameters.argtypes = [c_void_p,c_char_p,c_bool]
        bandargtype = [c_void_p,c_int*1,c_int*20,c_double*20,c_double*20,\
                       c_int*1,c_int*1,c_double*1,c_double*1,c_double*1,\
                       c_double*1,c_double*1,c_double*1,c_double*1]
        self.lib._wrap_TrafficMonitor_GetTrackBands.argtypes = bandargtype
        self.lib._wrap_TrafficMonitor_GetGSBands.argtypes = bandargtype
        self.lib._wrap_TrafficMonitor_GetAltBands.argtypes = bandargtype
        self.lib._wrap_TrafficMonitor_GetVSBands.argtypes = bandargtype
        self.obj = self.lib._wrap_new_TrafficMonitor(c_bool(log),c_char_p("DaidalusQuadConfig.txt".encode('utf-8')))

    def deleteobj(self):
        self.lib._wrap_delete_TrafficMonitor(self.obj)

    def SetParameters(self,params,log):
        self.lib._wrap_TrafficMonitor_UpdateDAAParameters(self.obj,c_char_p(params.encode('utf-8')),c_bool(log))

    def input_traffic(self,index,position,velocity,time):
        cpos = c_double*3
        cvel = c_double*3
        _pos = cpos(position[0],position[1],position[2])
        _vel = cvel(velocity[0],velocity[1],velocity[2])
        _time = c_double(time)
        _index = c_int(index)
        ret = c_int(0)
        callsign = "traffic"+str(index)
        ret.value = self.lib._wrap_TrafficMonitor_InputTraffic(self.obj,_index,c_char_p(callsign.encode('utf-8')),_pos,_vel,_time)
        
    def monitor_traffic(self,position,velocity,time):
        cpos = c_double*3
        _pos = cpos(position[0],position[1],position[2])
        _vel = cpos(velocity[0],velocity[1],velocity[2])
        self.lib._wrap_TrafficMonitor_MonitorTraffic(self.obj,_pos,_vel,c_double(time))

    def monitor_wp_feasibility(self,position,velocity,wp):
        cpos = c_double*3
        _pos = cpos(position[0],position[1],position[2])
        _vel = cpos(velocity[0],velocity[1],velocity[2])
        _wp = cpos(wp[0],wp[1],wp[2])
        val = c_bool(0)
        val.value = self.lib._wrap_TrafficMonitor_MonitorWPFeasibility(self.obj,_pos,_vel,_wp)
        return val.value

    def check_safe_to_turn(self,position,velocity,fromHeading,toHeading):
        cpos = c_double*3
        _pos = cpos(position[0],position[1],position[2])
        _vel = cpos(velocity[0],velocity[1],velocity[2])
        ret = c_int(0)
        ret.value = self.lib._wrap_TrafficMonitor_CheckSafeToTurn(self.obj,_pos,_vel,c_double(fromHeading),c_double(toHeading))
        return ret.value
      
    def GetTrackBands(self):
       numBands = c_int(0)
       intArr20 = c_int * 20
       doubleArr20 = c_double * 20
       recovery = c_int(0)
       currentConflict = c_int(0)
       tviolation = c_double(0)
       trecovery = c_double(0)
       minhdist = c_double(0)
       minvdist = c_double(0)
       resup = c_double(0)
       resdown = c_double(0)
       respref = c_double(0)
       bandTypes = intArr20()
       low = doubleArr20()
       high = doubleArr20()
       self.lib._wrap_TrafficMonitor_GetTrackBands(self.obj,byref(numBands),bandTypes,low,
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

       trkband = Bands()
       trkband.currentConflict = currentConflict.value
       trkband.resup = resup.value
       trkband.resdown = resdown.value
       trkband.respref = respref.value
       trkband.numBands = numBands.value
       trkband.bandTypes = bandTypes
       trkband.low = low
       trkband.high = high
       return trkband

    def GetGSBands(self):
       numBands = c_int(0)
       intArr20 = c_int * 20
       doubleArr20 = c_double * 20
       recovery = c_int(0)
       currentConflict = c_int(0)
       tviolation = c_double(0)
       trecovery = c_double(0)
       minhdist = c_double(0)
       minvdist = c_double(0)
       resup = c_double(0)
       resdown = c_double(0)
       respref = c_double(0)
       bandTypes = intArr20(0,0,0,0,0)
       low = doubleArr20(0,0,0,0,0)
       high = doubleArr20(0,0,0,0,0)
       self.lib._wrap_TrafficMonitor_GetGSBands(self.obj,byref(numBands),bandTypes,low,
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

       gsband = Bands()
       gsband.currentConflict = currentConflict.value
       gsband.resup = resup.value
       gsband.resdown = resdown.value
       gsband.respref = respref.value
       gsband.numBands = numBands.value
       gsband.bandTypes = bandTypes
       gsband.low = low
       gsband.high = high
       return gsband

    def GetAltBands(self):
       numBands = c_int(0)
       intArr20 = c_int * 20
       doubleArr20 = c_double * 20
       recovery = c_int(0)
       currentConflict = c_int(0)
       tviolation = c_double(0)
       trecovery = c_double(0)
       minhdist = c_double(0)
       minvdist = c_double(0)
       resup = c_double(0)
       resdown = c_double(0)
       respref = c_double(0)
       bandTypes = intArr20(0,0,0,0,0)
       low = doubleArr20(0,0,0,0,0)
       high = doubleArr20(0,0,0,0,0)
       self.lib._wrap_TrafficMonitor_GetAltBands(self.obj,byref(numBands),bandTypes,low,
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

       altband = Bands()
       altband.currentConflict = currentConflict.value
       altband.resup = resup.value
       altband.resdown = resdown.value
       altband.respref = respref.value
       altband.numBands = numBands.value
       altband.bandTypes = bandTypes
       altband.low = low
       altband.high = high

       return altband 


    def GetVSBands(self):
       numBands = c_int(0)
       intArr10= c_int * 20
       doubleArr10 = c_double * 20
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
       self.lib._wrap_TrafficMonitor_GetVSBands(self.obj,byref(numBands),bandTypes,low,
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

       vsband = Bands()
       vsband.currentConflict = currentConflict.value
       vsband.resup = resup.value
       vsband.resdown = resdown.value
       vsband.respref = respref.value
       vsband.numBands = numBands.value
       vsband.bandTypes = bandTypes
       vsband.low = low
       vsband.high = high

       return vsband


