# distutils: language=c++

from AutonomyStackDef cimport *
from libc.string cimport strcpy
from libcpp.string cimport string

cdef class Planner:
    cdef void* TrajManager
    
    def __cinit__(self,opts):
        self.TrajManager = new_TrajManager(b'speedbird.txt',b'IcarousConfig.txt')

    def FindPath(self,startpos,startvel,endpos,endvel):
        cdef double startPosition[3]
        cdef double endPosition[3]
        cdef double startVel[3]
        cdef double endVel[3]
        cdef int numWP

        for i in range(3):
            startPosition[i] = startpos[i]
            endPosition[i] = endpos[i]
            startVel[i] = startvel[i]
            endVel[i] = endvel[i]
        
        # Find the new path
        numWP = TrajManager_FindPath(self.TrajManager,
                                     b'test', 
                                     startPosition,
                                     endPosition,
                                     startVel,
                                     endVel)
        return numWP

    def GetPlan(self):
        cdef int n
        cdef waypoint_t wp[500]
        cdef list fp = []
        cdef list wps = []
        cdef double timeA
        cdef double timeB
        cdef double planPos[3]
        cdef double t
        n = TrajManager_GetTotalWaypoints(self.TrajManager,b'test')
        for i in range(n):
            TrajManager_GetWaypoint(self.TrajManager,b'test',i,&wp[i])
            wps.append([wp[i].latitude,wp[i].longitude,wp[i].altitude,wp[i].time])
        timeA = wps[0][-1]
        timeB = wps[-1][-1]
        t = timeA
        while t <= timeB:
            TrajManager_GetPlanPosition(self.TrajManager,b'test',t,planPos)
            t = t + 0.1 
            fp.append([planPos[0],planPos[1],planPos[2]])
            
        return fp,wps
