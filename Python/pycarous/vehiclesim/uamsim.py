import numpy as np

from icutils.ichelper import gps_offset, GetWindComponent, ConvertTrkGsVsToVned, ConvertVnedToTrkGsVs
from vehiclesim import VehicleSimInterface

class UamVtolSim(VehicleSimInterface):
    """ Simulation model for a UAM VTOL vehicle """
    def __init__(self, idx, home_gps, x=0.0, y=0.0, z=0.0,
                 vx=0.0, vy=0.0, vz=0.0, dt=0.05):
        """
        Initialize a UAM VTOL vehicle simulation
        :param x: initial x position (meters east from home_gps)
        :param y: initial y position (meters north from home_gps)
        :param z: initial altitude (meters)
        :param vx: initial east velocity (m/s)
        :param vy: initial north velocity (m/s)
        :param vz: initial upward velocity (m/s)
        Other arguments are defined in VehicleSimInterface
        """
        super().__init__(idx, home_gps, dt)
        g = 9.8
        self.U   = np.array([0.0, 0.0, 0.0])
        self.pos0 = np.array([x, y, z])
        self.vel0 = np.array([vx, vy, vz])
        self.pos = np.array([x, y, z])
        self.vel = np.array([vx, vy, vz])
        self.turnRate = 20
        self.accel = 0.5*g
        self.daccel = -0.5*g
        self.vaccel = 0.15*g
        self.trk,self.gs,self.vs = ConvertVnedToTrkGsVs(vy,vx,vz)

    def InputCommand(self, track, gs, climbrate):
        #vn, ve, vd = ConvertTrkGsVsToVned(track, gs, climbrate)
        self.U[0] = track
        self.U[1] = gs
        self.U[2] = climbrate

    def SetInitialConditions(self,x=0,y=0,z=0,heading=0,speed=0,vs=0):
        self.pos0 = np.array([x,y,z])
        self.trk,self.gs,self.vs = heading,speed,vs
        

    def Run(self, windFrom=0, windSpeed=0):
        vw = GetWindComponent(windFrom, windSpeed, NED=False)
        speed = np.linalg.norm(self.vel0 + vw)
        if speed <= 0:
            vw     = np.array([0.0, 0.0, 0.0])
        self.vw   = vw

        if np.linalg.norm(self.vel0[:-1]) < 1e-2:
            self.trk = self.U[0]

        turnRate = 0 
        if np.fabs(self.trk - self.U[0]) > 1e-1:
            # if current track is different from
            # target, use turn rate to determine new track
            vec1 = np.array([np.sin(self.trk*np.pi/180),np.cos(self.trk*np.pi/180)])
            vec2 = np.array([np.sin(self.U[0]*np.pi/180),np.cos(self.U[0]*np.pi/180)])
            det = vec1[0]*vec2[1] - vec1[1]*vec2[0]
            if det > 0:
                turnRate = -self.turnRate 
            else:
                turnRate = self.turnRate

        accel = 0
        if np.fabs(self.U[1] - self.gs) > 1e-2:
            if (self.U[1] >= self.gs):
                accel = self.accel
            else:
                accel = self.daccel

        vaccel = 0
        if np.fabs(self.U[2] - self.vs) > 1e-3:
            if (self.U[2] >= self.vs):
                vaccel = self.vaccel
            else:
                vaccel = - self.vaccel

        self.trk = self.trk + turnRate*self.dt
        self.gs  = self.gs  + accel*self.dt
        self.vs  = self.vs  + vaccel*self.dt

        self.pos0    = self.pos0 + (self.vel0 + vw) * self.dt
        self.vel0[0] = self.gs*np.sin(self.trk * np.pi/180)
        self.vel0[1] = self.gs*np.cos(self.trk * np.pi/180)
        self.vel0[2] = self.vs
        n = np.zeros((1, 3))
        if self.noise:
            n = np.random.multivariate_normal(mean=np.array([0.0, 0.0, 0.0]), cov = self.sigma_vel, size=1)
            self.vel[0] = self.vcoeff*self.vel[0] + (1 - self.vcoeff)*(self.vel[0] + n[0, 0])
            self.vel[1] = self.vcoeff*self.vel[1] + (1 - self.vcoeff)*(self.vel[1] + n[0, 1])
            self.vel[2] = self.vcoeff*self.vel[2] + (1 - self.vcoeff)*(self.vel[2] + n[0, 2])

        if self.noise:
            n = np.random.multivariate_normal(mean=np.array([0.0, 0.0, 0.0]), cov = self.sigma_pos, size=1)
        self.pos[0] = self.pcoeff*self.pos[0] + (1 - self.pcoeff)*(self.pos0[0] + n[0, 0])
        self.pos[1] = self.pcoeff*self.pos[1] + (1 - self.pcoeff)*(self.pos0[1] + n[0, 1])
        self.pos[2] = self.pcoeff*self.pos[2] + (1 - self.pcoeff)*(self.pos0[2] + n[0, 2])
        return True

    def GetOutputPositionNED(self):
        return (self.pos[1], self.pos[0], -self.pos[2])

    def GetOutputVelocityNED(self):
        return (self.vel0[1] + self.vw[1], self.vel0[0] + self.vw[0], -self.vel0[2] + self.vw[2])
