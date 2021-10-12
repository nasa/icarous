from ctypes import *
import numpy as np
from matplotlib import pyplot as plt
import os

from icutils.ichelper import ConvertTrkGsVsToVned
from vehiclesim import VehicleSimInterface

icmodules = os.path.join(os.environ['ICAROUS_HOME'],'Modules','lib')
lib = CDLL(os.path.join(icmodules,"libquadsim.so"),winmode=0)

lib.PC_Quadcopter_Simulation_initialize.restype = None
lib.PC_Quadcopter_Simulation_initialize.argtype = None

lib.PC_Quadcopter_Simulation_step.restype = None
lib.PC_Quadcopter_Simulation_step.argtype = None

lib.PC_QuadCopter_Simulation_SetInput.argtype = [c_double, c_double, c_double]
lib.PC_QuadCopter_Simulation_SetInput.restype = None

lib.PC_Quadcopter_Simulation_GetPosition.argtype = [c_double*3]
lib.PC_Quadcopter_Simulation_GetPosition.argtype = None

lib.PC_Quadcopter_Simulation_GetVelocity.argtype = [c_double*3]
lib.PC_Quadcopter_Simulation_GetVelocity.argtype = None


class QuadSim(VehicleSimInterface):
    """ Simulation model for a Quadrotor UAS """
    def __init__(self, idx, home_gps, dt=0.05):
        """
        Initialize a Quadrotor UAS simulation.
        Input arguments are defined in VehicleSimInterface.
        The vehicle starts at the given home_gps location.
        """
        super().__init__(idx, home_gps, dt)
        lib.PC_Quadcopter_Simulation_initialize()
        self.old_x = 0
        self.old_y = 0
        self.old_z = 0

    def InputCommand(self, track, gs, climbrate):
        vn, ve, vd = ConvertTrkGsVsToVned(track, gs, climbrate)
        inp1 = c_double(ve)
        inp2 = c_double(vn)
        inp3 = c_double(vd)
        lib.PC_QuadCopter_Simulation_SetInput(inp1, inp2, inp3)

    def Run(self, windFrom=0, windSpeed=0):
        lib.PC_Quadcopter_Simulation_step()
        return True

    def GetOutputPositionNED(self):
        n = np.zeros((1, 3))
        if self.noise:
            n = np.random.multivariate_normal(mean=np.array([0.0, 0.0, 0.0]),
                                              cov = self.sigma_pos, size=1)

        pos = c_double*3
        _pos = pos(0.0, 0.0, 0.0)
        lib.PC_Quadcopter_Simulation_GetPosition(_pos)

        x = _pos[0]
        y = _pos[1]
        z = _pos[2]
        # Adding noise to the sim output along with a simple smoothing filter
        outx = self.pcoeff*self.old_x + (1 - self.pcoeff)*(x + n[0, 0])
        outy = self.pcoeff*self.old_y + (1 - self.pcoeff)*(y + n[0, 1])
        outz = self.pcoeff*self.old_z + (1 - self.pcoeff)*(z + n[0, 2])
        self.old_x = outx
        self.old_y = outy
        self.old_z = outz
        return (outy, outx, outz, y, x, -z)

    def GetOutputVelocityNED(self):
        vel = c_double*3
        _vel = vel(0.0, 0.0, 0.0)
        lib.PC_Quadcopter_Simulation_GetVelocity(_vel)
        return (_vel[1], _vel[0], _vel[2])
