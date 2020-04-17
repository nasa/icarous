from ctypes import *
from ichelper import distance

lib = CDLL('libPathPlanner.so')
lib._wrap_new_PathPlanner.restype = c_void_p
lib._wrap_new_PathPlanner.argtypes = [c_double,c_double]
lib._wrap_PathPlanner_UpdateAstarParameters.argtypes = [c_void_p,c_bool,c_double,c_double,c_double,c_char_p]
lib._wrap_PathPlanner_UpdateRRTParameters.argtypes = [c_void_p,c_double,c_int,c_double,c_int,c_double,c_char_p]
lib._wrap_PathPlanner_UpdateDAAParameters.argtypes = [c_void_p,c_char_p]
lib._wrap_PathPlanner_FindPath.argtypes = [c_void_p,c_int,c_char_p,c_double*3,c_double*3,c_double*3]
lib._wrap_PathPlanner_FindPath.restype = c_int
lib._wrap_PathPlanner_GetWaypoint.argtypes = [c_void_p,c_char_p,c_int,c_double*4]
lib._wrap_PathPlanner_InputGeofenceData.argtypes = [c_void_p,c_int,c_int,c_int,c_double,c_double,POINTER(c_double*2)]
lib._wrap_PathPlanner_InputTraffic.argtypes = [c_void_p,c_int,c_double*3,c_double*3]
lib._wrap_PathPlanner_InputTraffic.restype = c_int
lib._wrap_PathPlanner_InputFlightPlan.argtypes = [c_void_p,c_char_p,c_int,c_double*3,c_double]
Pos = c_double*3

class Trajectory():
    def __init__(self,obsbuffer,ceiling):
        self.module = lib._wrap_new_PathPlanner(c_double(obsbuffer),c_double(ceiling))

    def UpdateAstarParams(self,enable3d,gridSize,resSpeed,lookahed,daaconfig):
        lib._wrap_PathPlanner_UpdateAstarParameters(self.module,c_bool(enable3d),
                c_double(gridSize),
                c_double(resSpeed),
                c_double(lookahed),
                c_char_p(daaconfig.encode('utf-8')))


    def UpdateRRTParams(self,resSpeed,NStep,dt,Dt,capR,daaconfig):
        lib._wrap_PathPlanner_UpdateRRTParameters(self.module,c_double(resSpeed),
                 c_int(NStep),
                 c_double(dt),
                 c_int(Dt),
                 c_double(capR),
                 c_char_p(daaconfig.encode('utf-8')))


    def UpdateDAAParams(self,param):
        lib._wrap_PathPlanner_UpdateDAAParameters(self.module,c_char_p(param.encode('utf-8')))


    def FindPath(self,serachType,planID,fromPos,toPos,fromVel):
        return lib._wrap_PathPlanner_FindPath(self.module,c_int(serachType),c_char_p(planID.encode('utf-8')),
                Pos(*fromPos),Pos(*toPos),Pos(*fromVel))


    def GetWaypoint(self,planID,wpID):
        WP = c_double*4
        wp = WP()
        lib._wrap_PathPlanner_GetWaypoint(self.module,c_char_p(planID.encode('utf-8')),c_int(wpID),wp)
        return wp

    
    def InputFlightplan(self,planID,fp,eta=False):

        wpTime = 0
        for i in range(len(fp)):
            if not eta:
                if i > 0:
                    speed = fp[i][3]
                    prevWP  = fp[i-1][:3]
                    nextWP  = fp[i][:3]
                    dist = distance(prevWP[0],prevWP[1],nextWP[0],nextWP[1])
                    wpTime += dist/speed
            else:
                wpTime = fp[i][3]
            wp = Pos(fp[i][0],fp[i][1],fp[i][2]) 
            lib._wrap_PathPlanner_InputFlightPlan(self.module,c_char_p(planID.encode('utf-8')),c_int(i),wp,c_double(wpTime))


    def InputGeofenceData(self,gf):
        Vert = (c_double*2)*gf['numV']
        vert = Vert()
        for i in range(gf['numV']):
            vert[i][0] = gf['Vertices'][i][0]
            vert[i][1] = gf['Vertices'][i][1]

        lib._wrap_PathPlanner_InputGeofenceData(self.module,
                                              c_int(gf['type']),
                                              c_int(gf['id']),
                                              c_int(gf['numV']),
                                              c_double(gf['floor']),
                                              c_double(gf['roof']),
                                              vert)


    def InputTrafficData(self,index,position,velocity):
        cpos = c_double*3
        cvel = c_double*3
        _pos = cpos(position[0],position[1],position[2])
        _vel = cvel(velocity[0],velocity[1],velocity[2])
        _index = c_int(index)
        return lib._wrap_PathPlanner_InputTraffic(self.module,_index,_pos,_vel)

