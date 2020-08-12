from ctypes import *
import xml.etree.ElementTree as ET



def Getfence(filename):
    '''add geofences from a file'''
    tree = ET.parse(filename)
    root = tree.getroot()        
    fenceList = []    
    for child in root:
        id    = int(child.get('id'));
        type  = int(child.find('type').text);
        numV  = int(child.find('num_vertices').text);            
        floor = float(child.find('floor').text);
        roof  = float(child.find('roof').text);
        Vertices = [];
    
        if(len(child.findall('vertex')) >= numV):        
            for vertex in child.findall('vertex'):
                coord = (float(vertex.find('lat').text),
                         float(vertex.find('lon').text))
            
                Vertices.append(coord)

        # Check geofence niceness
        Geofence = {'id':id,'type': type,'numV':numV,'floor':floor,
                    'roof':roof,'Vertices':Vertices[:numV]}
        fenceList.append(Geofence)
    return fenceList

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
        self.lib.GeofenceMonitor_GetConflict.argtypes = [c_void_p,c_int,POINTER(c_int),POINTER(c_bool),POINTER(c_bool),POINTER(c_double),POINTER(c_int)]
        self.lib.GeofenceMonitor_GetCloesetRecoveryPoint = [c_void_p,c_double*3,c_double*3]
        self.lib.GeofenceMonitor_CheckWPFeasibility.argtypes = [c_void_p,c_double*3,c_double*3]
        self.lib.GeofenceMonitor_CheckWPFeasibility.restype = c_bool
        _params = c_double*5
        self.module = self.lib.new_GeofenceMonitor(_params(*params))

    def SetParameters(self,params):
        _params = c_double*5
        self.lib.GeofenceMonitor_SetGeofenceParameters(self.module,_params(*params))

    def InputData(self,gf):
        Vert = (c_double*2)*gf['numV']
        vert = Vert()
        for i in range(gf['numV']):
            vert[i][0] = gf['Vertices'][i][0]
            vert[i][1] = gf['Vertices'][i][1]

        self.lib.GeofenceMonitor_InputGeofenceData(self.module,
                                              c_int(gf['type']),
                                              c_int(gf['id']),
                                              c_int(gf['numV']),
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
        ftype = c_int()
        self.lib.GeofenceMonitor_GetConflict(self.module,c_int(count),byref(fid),
                                        byref(conflict),
                                        byref(violation),
                                        recPoint,
                                        byref(ftype))
        return (fid.value,conflict.value,violation.value,recPoint,ftype.value)

    def CheckWPFeasibility(self,pos,wp):
        Pos = c_double*3
        pos1 = Pos(*pos)
        pos2 = Pos(*wp)
        return self.lib.GeofenceMonitor_CheckWPFeasibility(self.module,pos1,pos2)

