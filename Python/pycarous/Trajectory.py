from ctypes import *
from ichelper import distance
from Interfaces import Waypoint,TrajectoryMonitorData

class DubinsParams(Structure):
    _fields_=[
        ("wellClearDistH",c_double),
        ("wellClearDistV",c_double),
        ("turnRate",c_double),
        ("gs",c_double),
        ("vs",c_double),
        ("maxGS",c_double),
        ("minGS",c_double),
        ("maxVS",c_double),
        ("minVS",c_double),
        ("hAccel",c_double),
        ("hDaccel",c_double),
        ("vAccel",c_double),
        ("vDaccel",c_double),
        ("climbgs",c_double),
        ("zSections",c_double),
        ("vertexBuffer",c_double),
        ("maxH",c_double)
    ]

Pos = c_double*3

class Trajectory():
    def __init__(self,callsign):
        self.lib = CDLL('libTrajectoryManager.so')
        self.lib.new_TrajManager.restype = c_void_p
        self.lib.TrajManager_UpdateDubinsPlannerParameters.argtypes = [c_void_p,POINTER(DubinsParams)]
        self.lib.TrajManager_FindPath.argtypes = [c_void_p,c_char_p,c_double*3,c_double*3,c_double*3,c_double*3]
        self.lib.TrajManager_FindPath.restype = c_int
        self.lib.TrajManager_GetWaypoint.argtypes = [c_void_p,c_char_p,c_int,POINTER(Waypoint)]
        self.lib.TrajManager_GetWaypoint.restype = c_int
        self.lib.TrajManager_SetPlanOffset.argtypes = [c_void_p,c_char_p,c_int,c_double]
        self.lib.TrajManager_InputGeofenceData.argtypes = [c_void_p,c_int,c_int,c_int,c_double,c_double,POINTER(c_double*2)]
        self.lib.TrajManager_InputTraffic.argtypes = [c_void_p,c_int,c_double*3,c_double*3,c_double]
        self.lib.TrajManager_InputTraffic.restype = c_int
        self.lib.TrajManager_InputFlightPlan.argtypes = [c_void_p,c_char_p,Waypoint*50,c_int,c_double,c_bool]
        self.lib.TrajManager_GetTotalWaypoints.argtypes = [c_void_p,c_char_p]
        self.lib.TrajManager_GetTotalWaypoints.restype = c_int
        self.lib.TrajManager_CombinePlan.argtypes = [c_void_p,c_char_p,c_char_p,c_int]
        self.lib.TrajManager_MonitorTrajectory.argtypes = [c_void_p,c_double,c_char_p,c_double*3,c_double*3,c_int,c_int]
        self.lib.TrajManager_MonitorTrajectory.restype = TrajectoryMonitorData

        self.mUtils = CDLL('libUtils.so')
        self.mUtils.ComputeXtrackDistance.argtypes = [c_double*3,c_double*3,c_double*3,c_double*3]

        self.module = self.lib.new_TrajManager(c_char_p(callsign.encode('utf-8')))



    def UpdateDubinsPlannerParams(self,params):
        self.lib.TrajManager_UpdateDubinsPlannerParameters(self.module,byref(params))

    def CombinePlan(self,planA,planB,wp=-1):
        self.lib.TrajManager_CombinePlan(self.module,c_char_p(planA.encode('utf-8')),c_char_p(planB.encode('utf-8')),c_int(wp))

    def FindPath(self,planID,fromPos,toPos,fromVel,toVel):
        return self.lib.TrajManager_FindPath(self.module,c_char_p(planID.encode('utf-8')),
                Pos(*fromPos),Pos(*toPos),Pos(*fromVel),Pos(*toVel))


    def GetTotalWaypoints(self,planID):
        return self.lib.TrajManager_GetTotalWaypoints(self.module,c_char_p(planID.encode('utf-8')))

    def GetWaypoint(self,planID,wpID):
        wp = Waypoint()
        total = self.lib.TrajManager_GetWaypoint(self.module,c_char_p(planID.encode('utf-8')),c_int(wpID),byref(wp))
        return total,wp

    def SetPlanOffset(self,planID,wpID,offset):
        self.lib.TrajManager_SetPlanOffset(self.module,c_char_p(planID.encode('utf-8')),c_int(wpID),c_double(offset))

    def GetFlightPlan(self,planID):
        waypoints = []
        total = 1
        i = 0
        while i < total:
            total,wp = self.GetWaypoint(planID,i)
            i += 1
            if total > 0:
                waypoints.append(wp)
 
        return waypoints

    def InputFlightplanData(self,planID,fp,repair=False):
          n = len(fp)
          wparray = Waypoint*50
          wpts = wparray()
          for i,wp in enumerate(fp):
               wpts[i] = wp
          self.lib.TrajManager_InputFlightPlan(self.module,c_char_p(planID.encode('utf-8')),
                                          wpts,c_int(n),c_double(0),c_bool(repair))

    def InputGeofenceData(self,gf):
        Vert = (c_double*2)*gf['numV']
        vert = Vert()
        for i in range(gf['numV']):
            vert[i][0] = gf['Vertices'][i][0]
            vert[i][1] = gf['Vertices'][i][1]

        self.lib.TrajManager_InputGeofenceData(self.module,
                                              c_int(gf['type']),
                                              c_int(gf['id']),
                                              c_int(gf['numV']),
                                              c_double(gf['floor']),
                                              c_double(gf['roof']),
                                              vert)


    def InputTrafficData(self,index,position,velocity,time):
        cpos = c_double*3
        cvel = c_double*3
        _pos = cpos(position[0],position[1],position[2])
        _vel = cvel(velocity[0],velocity[1],velocity[2])
        _index = c_int(index)
        return self.lib.TrajManager_InputTraffic(self.module,_index,_pos,_vel,c_double(time))

    def ComputeXtrackDistance(self,wpA,wpB,position):
        cpos = c_double*3
        offset = cpos(0,0,0)
        self.mUtils.ComputeXtrackDistance(cpos(*wpA[:3]),cpos(*wpB[:3]),cpos(*position),offset)
        return offset

    def MonitorTrajectory(self,time,planID,pos,vel,nextWP1,nextWP2):
        cpos = c_double*3
        posIn = cpos(*pos)
        velIn = cpos(*vel)
        planid = c_char_p(planID.encode('utf-8'))
        return self.lib.TrajManager_MonitorTrajectory(self.module,c_double(time),planid,posIn,velIn,c_int(nextWP1),c_int(nextWP2))

    def ComputeClosesetPoint(self,wpA,wpB,position):
        offset = self.ComputeXtrackDistance(wpA,wpB,position)
        dist = distance(*wpA[:2],*wpB[:2])
        if dist<1e-3:
            return wpB
        else:
            n = 1 - (dist - offset[1])/dist
            C = [0.0,0.0,0.0]
            for i in range(3):
                C[i] = wpA[i] + n*(wpB[i] - wpA[i])

            return C

