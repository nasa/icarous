import math
import numpy as np
from matplotlib import pyplot as plt
from mpl_toolkits import mplot3d
from pyTrafficMonitor import TrafficMonitor
from quadsim import QuadSim

from ichelper import (ConvertTrkGsVsToVned,
                      gps_offset,
                      ConvertVnedToTrkGsVs,
                      ComputeHeading)


class VehicleSim():
    def __init__(self, dt, x, y, z, vx, vy, vz):
        self.dt = dt
        self.pos = np.array([x, y, z])
        self.vel = np.array([vx, vy, vz])

    def run(self, U):
        self.vel = self.vel + 0.2 * (U - self.vel)
        self.pos = self.pos + self.vel * self.dt


def ComputeControl(speed, currentPos, nextPos):
    ds = nextPos - currentPos
    ds = ds/np.linalg.norm(ds) * speed
    return ds


class IcarousSim():
    def __init__(self, initialPos, targetPos, vehicleSpeed):
        """
        @initialPos (lat,lon) tuple containing starting
                    latitude, longitude  of simulation
        @targetPos (rangeN,rangeE,rangeD) tuple containing
                    N,E,D range to target position
        """

        self.home_pos = [initialPos[0], initialPos[1]]
        self.traffic = []
        self.ownship = QuadSim()
        self.targetPos = np.array([targetPos[0], targetPos[1], targetPos[2]])
        self.simSpeed = vehicleSpeed

        self.currentOwnshipPos = (0, 0, 0)
        self.currentOwnshipVel = (0, 0, 0)
        self.ownshipPosLog = []
        self.ownshipVelLog = []

        self.trafficPosLog = []
        self.trafficVelLog = []

        self.tfMonitor = TrafficMonitor(0)

        self.preferredTrack = []
        self.preferredSpeed = []
        self.preferredAlt = []

        self.dist = 200
        self.count = 0
        self.ptrack = 0
        self.pspeed = 0
        self.palt = 0
        self.pvs = 0
        self.conflict = 0
        self.heading2target = 0
        self.trackResolution = False
        self.speedResolution = False
        self.altResolution = False
        self.resObtained = False
        self.resType = 0

        self.relpos = []
        self.relvel = []

    def InputTraffic(self, rng, brng, alt, speed, heading, crate):
        tx = rng*np.sin(brng*np.pi/180)
        ty = rng*np.cos(brng*np.pi/180)
        tz = alt
        tvx = speed*np.sin(heading*np.pi/180)
        tvy = speed*np.cos(heading*np.pi/180)
        tvz = crate
        self.traffic.append(VehicleSim(0.5, tx, ty, tz, tvx, tvy, tvz))

    def Run(self):

        while(self.dist > 10):
            self.count += 1
            if (self.count > 2000) and (self.dist >= 200):
                break

            self.dist = np.linalg.norm(self.targetPos - self.currentOwnshipPos)
            if self.conflict == 0:
                U = ComputeControl(1, self.currentOwnshipPos, self.targetPos)
            else:
                if self.trackResolution:
                    (ve, vn, vd) = ConvertTrkGsVsToVned(self.ptrack, 1, 0)
                elif self.speedResolution:
                    (ve, vn, vd) = ConvertTrkGsVsToVned(self.heading2target,
                                                        self.pspeed, 0)
                elif self.altResolution:
                    (ve, vn, vd) = ConvertTrkGsVsToVned(self.heading2target,
                                                        1, self.pvs)
                U = np.array([ve, vn, vd])

            self.ownship.step(U[0], U[1], U[2])

            opos = self.ownship.getOutputPosition()
            ovel = self.ownship.getOutputVelocity()
            self.currentOwnshipPos = (opos[0], opos[1], opos[2])
            self.currentOwnshipVel = (ovel[0], ovel[1], ovel[2])

            self.ownshipPosLog.append(self.currentOwnshipPos)

            home_pos = self.home_pos
            targetPos = self.targetPos
            (ogx, ogy) = gps_offset(home_pos[0], home_pos[1],
                                    self.currentOwnshipPos[0],
                                    self.currentOwnshipPos[1])
            (wgx, wgy) = gps_offset(home_pos[0], home_pos[1],
                                    targetPos[0], targetPos[1])

            ownship_pos = [ogx, ogy, opos[2]]
            ownship_vel = [ovel[0], ovel[1], ovel[2]]
            ovelTrkGsVs = ConvertVnedToTrkGsVs(*ownship_vel)
            self.heading2target = ComputeHeading(ownship_pos, targetPos)

            for i in range(len(self.traffic)):
                traffic = self.traffic[i]
                traffic.run(self.traffic[i].vel)
                self.trafficPosLog[i].append((traffic.pos[0],
                                              traffic.pos[1],
                                              traffic.pos[2]))

                self.trafficVelLog[i].append((traffic.vel[0],
                                              traffic.vel[1],
                                              traffic.vel[2]))

                (tgx, tgy) = gps_offset(home_pos[0], home_pos[1],
                                        traffic.pos[0], traffic.pos[1])

                traffic_pos = [tgx, tgy, traffic.pos[2]]
                traffic_vel = [traffic.vel[0], traffic.vel[1], traffic.vel[2]]

                self.tfMonitor.input_traffic(i, traffic_pos,
                                             traffic_vel, self.count)

            self.tfMonitor.monitor_traffic(ownship_pos, ovelTrkGsVs,
                                           self.count)

            (conflict1, self.ptrack) = self.tfMonitor.GetTrackBands()
            (conflict2, self.pspeed) = self.tfMonitor.GetGSBands()
            (conflict3, pdown, pup, self.palt) = self.tfMonitor.GetAltBands()

            self.conflict = conflict1 or conflict2 or conflict3

            if (self.conflict is True) and not self.resObtained:
                self.resObtained = True
                # Place holder for to use functions to obtain resolutions

            if self.resObtained:

                if self.trackResolution:
                    if not math.isfinite(self.ptrack) or self.ptrack == -1:
                        if len(self.preferredTrack) > 0:
                            self.ptrack = self.preferredTrack[-1]
                        else:
                            self.ptrack = -1
                    currentHeading = ovelTrkGsVs[0]
                    safe2Turn = self.tfMonitor.check_safe_to_turn(ownship_pos,ovelTrkGsVs,currentHeading,self.heading2target)
                    wpFeasibility = self.tfMonitor.monitor_wp_feasibility(ownship_pos,ovelTrkGsVs,[wgx,wgy,targetPos[2]])
                    newOvelTrkGsVs = ovelTrkGsVs
                    newOvelTrkGsVs = (self.heading2target,ovelTrkGsVs[1],ovelTrkGsVs[2])
                    wpFeasibility |= self.tfMonitor.monitor_wp_feasibility(ownship_pos,newOvelTrkGsVs,[wgx,wgy,targetPos[2]])

                    if safe2Turn == 0 and not wpFeasibility:
                        self.conflict = 1
                        self.ptrack = self.preferredTrack[-1]

                    self.preferredTrack.append(self.ptrack)

                elif self.speedResolution:
                    if not math.isfinite(self.pspeed):
                        if len(self.preferredSpeed) > 0:
                            self.pspeed = self.preferredSpeed[-1]
                        else:
                            self.pspeed = -100

                    self.vehicleSpeed.append(ovelTrkGsVs[1])
                    ovelTrkGsVs = (ovelTrkGsVs[0],self.simSpeed,ovelTrkGsVs[2])
                    wpFeasibility = self.tfMonitor.monitor_wp_feasibility(ownship_pos,ovelTrkGsVs,[wgx,wgy,targetPos[2]])
                    if not wpFeasibility:
                        self.conflict = 1
                        if len(self.preferredSpeed) > 0:
                            self.pSpeed = self.preferredSpeed[-1]
                        else:
                            self.pSpeed = -100

                    self.preferredSpeed.append(self.pspeed)

                    if self.pspeed == -100:
                        self.pspeed = self.simSpeed

                elif self.altResolution:
                    self.palt = pup + 2
                    if not math.isfinite(self.palt):
                        if len(self.preferredAlt) > 0:
                            self.palt = self.preferredAlt[-1]
                        else:
                            self.palt = -1000

                    wpFeasibility = self.tfMonitor.monitor_wp_feasibility(ownship_pos,ovelTrkGsVs,[wgx,wgy,targetPos[2]])
                    if not wpFeasibility: 
                        self.conflict = 1

                    if self.conflict:
                        diffAlt = self.palt - ownship_pos[2]
                        self.pvs = 0.1 * diffAlt

                    self.preferredAlt.append(self.palt)

                    if self.palt == -1000:
                        self.palt = 5


"""
ic = Icarous(None,[200.0,0.0,5.0],None)
ic.Run()



plt.figure(2)
plt.plot(ic.posOwnX,ic.posOwnY,'r')
plt.plot(ic.posTrafficX,ic.posTrafficY,'b')
sqre = plt.Polygon([[80,-20],[120,-20],[120,20],[80,20]],fill=None,edgecolor='r')
plt.scatter(100,0,c='green')
plt.gca().add_line(sqre)
plt.axis([-200,200,-200,200])


plt.show()
"""
