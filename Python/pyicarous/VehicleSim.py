import numpy as np
from ichelper import gps_offset,GetWindComponent

class VehicleSim():
    def __init__(self, idx,x, y, z, vx, vy, vz, dt=0.05):
        self.vehicleID = idx
        self.dt = dt
        self.pos0 = np.array([x, y, z])
        self.vel0 = np.array([vx, vy, vz])
        self.noise = False
        self.coeff = 0
        self.sigma_pos = 0
        self.old_x = 0
        self.old_y = 0
        self.old_z = 0
        self.pos = np.array([x, y, z])
        self.vel = np.array([vx, vy, vz])
        self.U   = np.array([0.0, 0.0, 0.0])
        self.pos_gps = np.array([0.0,0.0,0.0])
        self.home_gps = np.array([0.0,0.0,0.0])
        self.log = {}
        self.log['pos'] = []
        self.log['vel'] = []
        self.log['gps'] = []
        self.vw  = np.array([vx, vy, vz])

    def setpos_uncertainty(self,xx,yy,zz,xy,yz,xz,coeff=0.8):
        self.noise = True
        self.coeff = coeff
        self.sigma_pos = np.array([[xx, xy, xz],
                                   [xy, yy, yz],
                                   [xz, yz, zz]])

    def input(self,U1,U2,U3):
        self.U[0] = U1
        self.U[1] = U2
        self.U[2] = -U3

    def step(self,windFrom=0,windSpeed=0):
        vw = GetWindComponent(windFrom,windSpeed,NED=False)
        speed = np.linalg.norm(self.vel0 + vw)
        if speed <= 0:
            vw     = np.array([0.0,0.0,0.0])
        self.vw   = vw
        self.vel0 = self.vel0 + self.dt * (self.U - self.vel0)
        self.pos0 = self.pos0 + (self.vel0 + vw) * self.dt
        n = np.zeros((1,3))
        if self.noise:
            n = np.random.multivariate_normal(mean=np.array([0.0,0.0,0.0]),cov = self.sigma_pos, size=1)

        self.pos[0] = self.coeff*self.pos[0] + (1 - self.coeff)*(self.pos0[0] + n[0,0]) 
        self.pos[1] = self.coeff*self.pos[1] + (1 - self.coeff)*(self.pos0[1] + n[0,1]) 
        self.pos[2] = self.coeff*self.pos[2] + (1 - self.coeff)*(self.pos0[2] + n[0,2]) 

    def run(self,windFrom=0,windSpeed=0):
        oldvel = self.getOutputVelocity() - self.vw
        self.input(oldvel[0],oldvel[1],oldvel[2])
        self.step(windFrom,windSpeed)
        (tgx, tgy) = gps_offset(self.home_gps[0], self.home_gps[1],
                                self.pos[0], self.pos[1])
        self.pos_gps[0] = tgx
        self.pos_gps[1] = tgy
        self.pos_gps[2] = self.pos[2]
        self.log['gps'].append(self.pos_gps.tolist())
        self.log['pos'].append(self.pos.tolist())
        self.log['vel'].append(self.vel.tolist())

    def getOutputPosition(self):
        return (self.pos[0],self.pos[1],self.pos[2])

    def getOutputVelocity(self):
        return (self.vel0[0] + self.vw[0],self.vel0[1] + self.vw[1],self.vel0[2] + self.vw[2])


def StartTraffic(idx, home, rng, brng, alt, speed, heading, crate, tflist=[]):
        tx = rng*np.sin(brng*np.pi/180)
        ty = rng*np.cos(brng*np.pi/180)
        tz = alt
        tvx = speed*np.sin(heading*np.pi/180)
        tvy = speed*np.cos(heading*np.pi/180)
        tvz = crate
        sim = VehicleSim(idx,tx, ty, tz, tvx, tvy, tvz)
        sim.home_gps = np.array(home)
        tflist.append(sim)


def RunTraffic(tflist,windFrom=0,windSpeed=0):
    for tf in tflist:
        tf.run(windFrom, windSpeed)
