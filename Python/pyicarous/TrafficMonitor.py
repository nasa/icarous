from ctypes import *

from Interfaces import Bands

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
        self.obj = self.lib._wrap_new_TrafficMonitor(c_bool(log),c_char_p("data/DaidalusQuadConfig.txt".encode('utf-8')))

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
        bands = Bands()
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
        self.lib._wrap_TrafficMonitor_GetTrackBands(self.obj,byref(numBands),bands.type,bands.min,bands.max,
                                                    byref(recovery),
                                                    byref(currentConflict),
             				            byref(tviolation),
         				            byref(trecovery),
                                                    byref(minhdist),
         				            byref(minvdist),
                				    byref(resup),
                                                    byref(resdown),
                                                    byref(respref))
        bands.numBands = numBands.value
        bands.recovery = recovery.value
        bands.currentConflictBand = currentConflict.value
        bands.timeToViolation = tviolation.value
        bands.timeToRecovery = trecovery.value
        bands.minHdist = minhdist.value
        bands.minVdist = minvdist.value
        bands.resUp = resup.value 
        bands.resDown = resdown.value
        bands.resPreferred = respref.value
        return bands

    def GetGSBands(self):
            bands = Bands()
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
            self.lib._wrap_TrafficMonitor_GetGSBands(self.obj,byref(numBands),bands.type,bands.min,bands.max,
                                                        byref(recovery),
                                                        byref(currentConflict),
                                                        byref(tviolation),
                                                        byref(trecovery),
                                                        byref(minhdist),
                                                        byref(minvdist),
                                                        byref(resup),
                                                        byref(resdown),
                                                        byref(respref))
            bands.numBands = numBands.value
            bands.recovery = recovery.value
            bands.currentConflictBand = currentConflict.value
            bands.timeToViolation = tviolation.value
            bands.timeToRecovery = trecovery.value
            bands.minHdist = minhdist.value
            bands.minVdist = minvdist.value
            bands.resUp = resup.value 
            bands.resDown = resdown.value
            bands.resPreferred = respref.value
            return bands

    def GetAltBands(self):
            bands = Bands()
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
            self.lib._wrap_TrafficMonitor_GetAltBands(self.obj,byref(numBands),bands.type,bands.min,bands.max,
                                                        byref(recovery),
                                                        byref(currentConflict),
                                                        byref(tviolation),
                                                        byref(trecovery),
                                                        byref(minhdist),
                                                        byref(minvdist),
                                                        byref(resup),
                                                        byref(resdown),
                                                        byref(respref))
            bands.numBands = numBands.value
            bands.recovery = recovery.value
            bands.currentConflictBand = currentConflict.value
            bands.timeToViolation = tviolation.value
            bands.timeToRecovery = trecovery.value
            bands.minHdist = minhdist.value
            bands.minVdist = minvdist.value
            bands.resUp = resup.value 
            bands.resDown = resdown.value
            bands.resPreferred = respref.value
            return bands


    def GetVSBands(self):
            bands = Bands()
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
            self.lib._wrap_TrafficMonitor_GetVSBands(self.obj,byref(numBands),bands.type,bands.min,bands.max,
                                                        byref(recovery),
                                                        byref(currentConflict),
                                                        byref(tviolation),
                                                        byref(trecovery),
                                                        byref(minhdist),
                                                        byref(minvdist),
                                                        byref(resup),
                                                        byref(resdown),
                                                        byref(respref))
            bands.numBands = numBands.value
            bands.recovery = recovery.value
            bands.currentConflictBand = currentConflict.value
            bands.timeToViolation = tviolation.value
            bands.timeToRecovery = trecovery.value
            bands.minHdist = minhdist.value
            bands.minVdist = minvdist.value
            bands.resUp = resup.value 
            bands.resDown = resdown.value
            bands.resPreferred = respref.value
            return bands


