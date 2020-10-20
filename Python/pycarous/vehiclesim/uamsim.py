import numpy as np

from ichelper import gps_offset, GetWindComponent, ConvertTrkGsVsToVned
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
        self.U   = np.array([0.0, 0.0, 0.0])
        self.pos0 = np.array([x, y, z])
        self.vel0 = np.array([vx, vy, vz])
        self.pos = np.array([x, y, z])
        self.vel = np.array([vx, vy, vz])

    def InputCommand(self, track, gs, climbrate):
        vn, ve, vd = ConvertTrkGsVsToVned(track, gs, climbrate)
        self.U[0] = ve
        self.U[1] = vn
        self.U[2] = -vd

    def Run(self, windFrom=0, windSpeed=0):
        vw = GetWindComponent(windFrom, windSpeed, NED=False)
        speed = np.linalg.norm(self.vel0 + vw)
        if speed <= 0:
            vw     = np.array([0.0, 0.0, 0.0])
        self.vw   = vw
        self.vel0 = self.vel0 + self.dt * (self.U - self.vel0 - vw)
        self.pos0 = self.pos0 + (self.vel0 + vw) * self.dt

        n = np.zeros((1, 3))
        if self.noise:
            n = np.random.multivariate_normal(mean=np.array([0.0, 0.0, 0.0]), cov = self.sigma_pos, size=1)
        self.pos[0] = self.coeff*self.pos[0] + (1 - self.coeff)*(self.pos0[0] + n[0, 0])
        self.pos[1] = self.coeff*self.pos[1] + (1 - self.coeff)*(self.pos0[1] + n[0, 1])
        self.pos[2] = self.coeff*self.pos[2] + (1 - self.coeff)*(self.pos0[2] + n[0, 2])

    def GetOutputPositionNED(self):
        return (self.pos[1], self.pos[0], self.pos[2])

    def GetOutputVelocityNED(self):
        return (self.vel0[1] + self.vw[1], self.vel0[0] + self.vw[0], -self.vel0[2] + self.vw[2])
