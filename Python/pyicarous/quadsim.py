from ctypes import *
import numpy as np
from matplotlib import pyplot as plt

lib = CDLL("libquadsim.so")


lib.PC_Quadcopter_Simulation_initialize.restype = None
lib.PC_Quadcopter_Simulation_initialize.argtype = None

lib.PC_Quadcopter_Simulation_step.restype = None
lib.PC_Quadcopter_Simulation_step.argtype = None

lib.PC_QuadCopter_Simulation_SetInput.argtype = [c_double,c_double,c_double]
lib.PC_QuadCopter_Simulation_SetInput.restype = None

lib.PC_Quadcopter_Simulation_GetPosition.argtype = [c_double*3]
lib.PC_Quadcopter_Simulation_GetPosition.argtype = None

lib.PC_Quadcopter_Simulation_GetVelocity.argtype = [c_double*3]
lib.PC_Quadcopter_Simulation_GetVelocity.argtype = None


class QuadSim():
    def __init__(self):
        """
        @param coeff: value in range (0,1) to determine smoothing
        """
        lib.PC_Quadcopter_Simulation_initialize()
        self.noise = False
        self.old_x = 0
        self.old_y = 0
        self.old_z = 0
        self.coeff = 0

    def setpos_uncertainty(self,xx,yy,zz,xy,yz,xz,coeff=0.8):
        self.noise = True
        self.coeff = coeff
        self.sigma_pos = np.array([[xx, xy, xz],
                                   [xy, yy, yz],
                                   [xz, yz, zz]])

    def step(self):
        lib.PC_Quadcopter_Simulation_step()

    def input(self, u1, u2, u3):
        inp1 = c_double(u1)
        inp2 = c_double(u2)
        inp3 = c_double(u3)
        lib.PC_QuadCopter_Simulation_SetInput(inp1,inp2,inp3)

    def getOutputPosition(self):
        n = np.zeros((1,3))
        if self.noise:
            n = np.random.multivariate_normal(mean=np.array([0.0,0.0,0.0]),cov = self.sigma_pos, size=1)

        pos = c_double*3
        _pos = pos(0.0,0.0,0.0)
        lib.PC_Quadcopter_Simulation_GetPosition(_pos)

        x = _pos[0]
        y = _pos[1]
        z = _pos[2]
        # Adding noise to the sim output along with a simple smoothing filter
        outx = self.coeff*self.old_x + (1 - self.coeff)*(x + n[0,0]) 
        outy = self.coeff*self.old_y + (1 - self.coeff)*(y + n[0,1]) 
        outz = self.coeff*self.old_z + (1 - self.coeff)*(z + n[0,2]) 
        self.old_x = outx
        self.old_y = outy
        self.old_z = outz
        return (outx,outy,outz,x,y,z) 

    def getOutputVelocity(self):
        vel = c_double*3
        _vel = vel(0.0,0.0,0.0)
        lib.PC_Quadcopter_Simulation_GetVelocity(_vel)
        return (_vel[0],_vel[1], _vel[2])


"""
sim = QuadSim()

#sim.setpos_uncertainty(0.01,0.01,0,0,0,0)

target = (20,20,0)

N = 500

data = np.zeros((N,3))

for i in range(N):
    (x, y, z, x0, y0, z0) = sim.getOutputPosition()

    if abs(x - target[0]) > 1e-3:
        ux = 0.5
    else:
        ux = 0

    if abs(y - target[0]) > 1e-3:
        uy = 0.5
    else:
        uy = 0


    sim.input(ux,uy,0)

    sim.step()

    data[i,0] = x
    data[i,1] = y
    data[i,2] = z


plt.figure(1)
plt.plot(data[:,0],data[:,1])
plt.show()
"""
