from ctypes import *

class GeofenceMonitor():
    def __init__(self,params):
        self.lib = CDLL("libGeofenceMonitor.so")
        self.lib.new_GeofenceMonitor.restype = c_void_p
        self.lib.new_GeofenceMonitor.argtypes = [POINTER(c_double)]
        self.lib.GeofenceMonitor_SetGeofenceParameters.argtypes = [c_void_p,POINTER(c_double)]
        self.lib.GeofenceMonitor_InputGeofenceData.argtypes = [c_void_p,c_int,c_int,c_int,c_double,c_double,POINTER(c_double*2)]
        self.lib.GeofenceMonitor_CheckViolation.argtypes = [c_void_p,POINTER(c_double),c_double,c_double,c_double]
        self.lib.GeofenceMonitor_CheckViolation.restype = c_bool 
        self.lib.GeofenceMonitor_GetNumConflicts.argtypes = [c_void_p]
        self.lib.GeofenceMonitor_GetNumConflicts.restype = c_int
        self.lib.GeofenceMonitor_GetConflict.argtypes = [c_void_p,c_int,POINTER(c_int),POINTER(c_bool),POINTER(c_bool),POINTER(c_double),POINTER(c_double),POINTER(c_int)]
        self.lib.GeofenceMonitor_GetCloesetRecoveryPoint = [c_void_p,c_double*3,c_double*3]
        self.lib.GeofenceMonitor_CheckWPFeasibility.argtypes = [c_void_p,c_double*3,c_double*3]
        self.lib.GeofenceMonitor_CheckWPFeasibility.restype = c_bool
        _params = c_double*5
        self.module = self.lib.new_GeofenceMonitor(_params(*params))

    def SetParameters(self,params):
        _params = c_double*5
        self.lib.GeofenceMonitor_SetGeofenceParameters(self.module,_params(*params))

    def InputData(self,gf):
        numV = len(gf['vertices'])
        Vert = (c_double*2)*numV
        vert = Vert()
        for i in range(numV):
            vert[i][0] = gf['vertices'][i][0]
            vert[i][1] = gf['vertices'][i][1]

        self.lib.GeofenceMonitor_InputGeofenceData(self.module,
                                              c_int(0 if gf['type'] == 'KEEPIN' else 1),
                                              c_int(gf['id']),
                                              numV,
                                              c_double(gf['floor']),
                                              c_double(gf['roof']),
                                              vert)

    def CheckViolation(self,pos,trk,gs,vs):
        Pos = c_double*3
        return self.lib.GeofenceMonitor_CheckViolation(self.module,
                                           Pos(*pos),
                                           c_double(trk),
                                           c_double(gs),
                                           c_double(vs))

    def GetNumConflicts(self):
        numConf = self.lib.GeofenceMonitor_GetNumConflicts(self.module)
        return numConf

    def GetConflict(self,count):
        Pos = c_double*3
        fid = c_int()
        conflict = c_bool()
        violation = c_bool()
        recPoint = Pos()
        time = c_double()
        ftype = c_int()
        self.lib.GeofenceMonitor_GetConflict(self.module,c_int(count),byref(fid),
                                        byref(conflict),
                                        byref(violation),
                                        recPoint,
                                        byref(time),
                                        byref(ftype))
        return (fid.value,conflict.value,violation.value,recPoint,time.value,ftype.value)

    def CheckWPFeasibility(self,pos,wp):
        Pos = c_double*3
        pos1 = Pos(*pos)
        pos2 = Pos(*wp)
        return self.lib.GeofenceMonitor_CheckWPFeasibility(self.module,pos1,pos2)

