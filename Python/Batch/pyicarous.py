import math
import numpy as np
from pyTrafficMonitor import TrafficMonitor
from quadsim import QuadSim

from ichelper import (ConvertTrkGsVsToVned,
                      distance,
                      gps_offset,
                      ConvertVnedToTrkGsVs,
                      ComputeHeading)


class WellClearParams():
    def __init__(self):
        self.data = {}
        self.data["lookahead_time"] = "60.000000 [s]"
        self.data["left_trk"] = "180.000000 [deg]"
        self.data["right_trk"] = "180.000000 [deg]"
        self.data["min_gs"] = "0.200000 [m/s]"
        self.data["max_gs"] = "5.000000 [m/s]"
        self.data["min_vs"] = "-400.000000 [fpm]"
        self.data["max_vs"] = "400.000000 [fpm]"
        self.data["min_alt"] = "0.000000 [ft]"
        self.data["max_alt"] = "500.000000 [ft]"
        # Kinematic Parameters
        self.data["trk_step"] = "1.000000 [deg]"
        self.data["gs_step"] = "0.100 [m/s]"
        self.data["vs_step"] = "5.000000 [fpm]"
        self.data["alt_step"] = "1.000000 [m]"
        self.data["horizontal_accel"] = "1.000000 [m/s^2]"
        self.data["vertical_accel"] = "1.000000 [m/s^2]"
        self.data["turn_rate"] = "0.000000 [deg/s]"
        self.data["bank_angle"] = "0.000000 [deg]"
        self.data["vertical_rate"] = "0.000000 [m/s]"
        # Recovery Bands Parameters
        self.data["recovery_stability_time"] = "0.000000 [s]"
        self.data["min_horizontal_recovery"] = "2.000000 [m]"
        self.data["min_vertical_recovery"] = "1.000000 [m]"
        self.data["recovery_trk"] = "true"
        self.data["recovery_gs"] = "false"
        self.data["recovery_vs"] = "false"
        self.data["recovery_alt"] = "false"
        # Collision Avoidance Bands Parameters
        self.data["ca_bands"] = "true"
        self.data["ca_factor"] = "0.200000"
        self.data["horizontal_nmac"] = "1.000000 [m]"
        self.data["vertical_nmac"] = "0.500000 [m]"
        # Implicit Coordination Parameters
        self.data["conflict_crit"] = "false"
        self.data["recovery_crit"] = "false"
        # Horizontal Contour Threshold
        self.data["contour_thr"] = "180.000000 [deg]"
        # Alert Levels
        self.data["alert_1_alerting_time"] = "10.000000 [s]"
        self.data["alert_1_detector"] = "det_1"
        self.data["alert_1_early_alerting_time"] = "15.000000 [s]"
        self.data["alert_1_region"] = "NEAR"
        self.data["alert_1_spread_alt"] = "0.000000 [m]"
        self.data["alert_1_spread_gs"] = "0.000000 [m/s]"
        self.data["alert_1_spread_trk"] = "0.000000 [deg]"
        self.data["alert_1_spread_vs"] = "0.000000 [fpm]"
        self.data["conflict_level"] = "1"
        self.data["det_1_WCV_DTHR"] = "20.000000 [m]"
        self.data["det_1_WCV_TCOA"] = "0.000000 [s]"
        self.data["det_1_WCV_TTHR"] = "0.000000 [s]"
        self.data["det_1_WCV_ZTHR"] = "30.000000 [m]"
        self.data["load_core_detection_det_1"] = "gov.nasa.larcfm.ACCoRD.WCV_TAUMOD"

    def WriteParams(self):
        fp = open('DaidalusQuadConfig.txt','w')
        for key in self.data.keys():
            fp.write(key + "=" + self.data[key] + "\n")

        fp.close()

class VehicleSim():
    def __init__(self, dt, x, y, z, vx, vy, vz):
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

    def setpos_uncertainty(self,xx,yy,zz,xy,yz,xz,coeff):
        self.noise = True
        self.coeff = coeff
        self.sigma_pos = np.array([[xx, xy, xz],
                                   [xy, yy, yz],
                                   [xz, yz, zz]])

    def input(self,U1,U2,U3):
        self.U[0] = U1
        self.U[1] = U2
        self.U[2] = -U3

    def step(self):
        self.vel0 = self.vel0 + 0.01 * (self.U - self.vel0)
        self.pos0 = self.pos0 + self.vel0 * self.dt
        n = np.zeros((1,3))
        if self.noise:
            n = np.random.multivariate_normal(mean=np.array([0.0,0.0,0.0]),cov = self.sigma_pos, size=1)

        self.pos[0] = self.coeff*self.pos[0] + (1 - self.coeff)*(self.pos0[0] + n[0,0]) 
        self.pos[1] = self.coeff*self.pos[1] + (1 - self.coeff)*(self.pos0[1] + n[0,1]) 
        self.pos[2] = self.coeff*self.pos[2] + (1 - self.coeff)*(self.pos0[2] + n[0,2]) 

    def getOutputPosition(self):
        return (self.pos[0],self.pos[1],self.pos[2])

    def getOutputVelocity(self):
        return (self.vel0[0],self.vel0[1],self.vel0[2])


def ComputeControl(speed, currentPos, nextPos):
    ds = nextPos - currentPos
    ds = ds/np.linalg.norm(ds) * speed
    ds[2] = -ds[2] * 30
    return ds


class IcarousSim():
    def __init__(self, initialPos, vehicleSpeed, targetPosLLA = None, targetPosNED = None, simtype="UAS_ROTOR"):
        """
        @initialPos (lat,lon) tuple containing starting
                    latitude, longitude  of simulation
        @targetPos (rangeN,rangeE,rangeD) tuple containing
                    N,E,D range to target position
        """

        self.home_pos = [initialPos[0], initialPos[1], initialPos[2]]
        self.traffic = []
        if simtype == "UAM_VTOL":
            self.ownship = VehicleSim(0.05,0.0,0.0,0.0,0.0,0.0,0.0)
        else:
            self.ownship = QuadSim()

        if targetPosNED is not None:
            self.targetPos = np.array([targetPosNED[1], targetPosNED[0], targetPosNED[2]])
        else:
            targetN = distance(self.home_pos[0],self.home_pos[1],targetPosLLA[0],self.home_pos[1])
            targetE = distance(self.home_pos[0],self.home_pos[1],self.home_pos[0],targetPosLLA[1])
            if(targetPosLLA[0] > self.home_pos[0]):
                signN = 1
            else:
                signN = -1

            if(targetPosLLA[1] > self.home_pos[1]):
                signE = 1
            else:
                signE = -1

            self.targetPos = np.array([signE*targetE,signN*targetN,targetPosLLA[2]])
        self.simSpeed = vehicleSpeed

        self.currentOwnshipPos = (0, 0, initialPos[2])
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
        self.cleared = False
        self.heading2target = 0
        self.trackResolution = True
        self.speedResolution = False
        self.altResolution = False
        self.resObtained = False
        self.resType = 0

        self.relpos = []
        self.relvel = []
        self.confcount = 0

    def setpos_uncertainty_ownship(self,xx,yy,zz,xy,yz,xz,coeff=0.8):
        self.ownship.setpos_uncertainty(xx,yy,zz,xy,yz,xz,coeff)

    def setpos_uncertainty_traffic(self,xx,yy,zz,xy,yz,xz,coeff=0.8):
        for tf in self.traffic:
            tf.setpos_uncertainty(xx,yy,zz,xy,yz,xz,coeff)

    def InputTraffic(self, rng, brng, alt, speed, heading, crate):
        tx = rng*np.sin(brng*np.pi/180)
        ty = rng*np.cos(brng*np.pi/180)
        tz = alt
        tvx = speed*np.sin(heading*np.pi/180)
        tvy = speed*np.cos(heading*np.pi/180)
        tvz = crate
        self.traffic.append(VehicleSim(0.05, tx, ty, tz, tvx, tvy, tvz))
        self.trafficPosLog.append([])
        self.trafficVelLog.append([])

    def Cleanup(self):
        self.tfMonitor.deleteobj()

    def Run(self):

        while(self.dist > 10):
            self.count += 1
            if (self.count > 2000) and (self.dist >= 1000):
                print("terminating simulation")
                break

            U = (0, 0, 0)

            self.dist = np.linalg.norm(self.targetPos - self.currentOwnshipPos)
            if self.conflict == 0:
                U = ComputeControl(self.simSpeed, self.currentOwnshipPos, self.targetPos)
                self.resObtained = False
            else:
                (ve,vn,vd) = (0,0,0)
                if self.trackResolution:
                    #print("executing track resolution")
                    (ve, vn, vd) = ConvertTrkGsVsToVned(self.ptrack, self.simSpeed, 0)
                elif self.speedResolution:
                    #print("executing speed resolution")
                    (ve, vn, vd) = ConvertTrkGsVsToVned(self.heading2target,
                                                        self.pspeed, 0)
                elif self.altResolution:
                    #print("executing alt resolution")
                    (ve, vn, vd) = ConvertTrkGsVsToVned(self.heading2target,
                                                        self.simSpeed, self.pvs)
                U = np.array([ve, vn, vd])



            self.ownship.input(U[0],U[1],U[2])
            self.ownship.step()

            opos = self.ownship.getOutputPosition()
            ovel = self.ownship.getOutputVelocity()

            self.currentOwnshipPos = (opos[0], opos[1], opos[2] + self.home_pos[2])
            self.currentOwnshipVel = (ovel[0], ovel[1], ovel[2])
            self.ownshipPosLog.append(self.currentOwnshipPos)
            self.ownshipVelLog.append(self.currentOwnshipVel)

            home_pos = self.home_pos
            targetPos = self.targetPos
            (ogx, ogy) = gps_offset(home_pos[0], home_pos[1],
                                    self.currentOwnshipPos[0],
                                    self.currentOwnshipPos[1])
            (wgx, wgy) = gps_offset(home_pos[0], home_pos[1],
                                    targetPos[0], targetPos[1])

            ownship_pos_gx = [ogx, ogy, self.currentOwnshipPos[2]]
            ownship_vel = [self.currentOwnshipVel[0], 
                           self.currentOwnshipVel[1], 
                           self.currentOwnshipVel[2] ]
            ovelTrkGsVs = ConvertVnedToTrkGsVs(*ownship_vel)
            self.heading2target = ComputeHeading(ownship_pos_gx, targetPos)


            traffic_pos_gx = []
            traffic_vel = []
            for i in range(len(self.traffic)):
                traffic = self.traffic[i]
                oldvel = traffic.getOutputVelocity()
                traffic.input(oldvel[0],oldvel[1],oldvel[2])
                traffic.step()
                newpos = traffic.getOutputPosition()
                newvel = traffic.getOutputVelocity()
                self.trafficPosLog[i].append(newpos)
                self.trafficVelLog[i].append(newvel)

                (tgx, tgy) = gps_offset(home_pos[0], home_pos[1],
                                        traffic.pos[0], traffic.pos[1])



                traffic_pos_gx = [tgx, tgy, traffic.pos[2]]
                traffic_vel = [traffic.vel[0], traffic.vel[1], traffic.vel[2]]

                self.tfMonitor.input_traffic(i, traffic_pos_gx,
                                             traffic_vel, self.count*0.05)

            self.tfMonitor.monitor_traffic(ownship_pos_gx, ovelTrkGsVs,
                                           self.count*0.05)

            (conflict1, self.ptrack) = self.tfMonitor.GetTrackBands()
            (conflict2, self.pspeed) = self.tfMonitor.GetGSBands()
            (conflict3, pdown, pup, self.palt) = self.tfMonitor.GetAltBands()

            
            self.conflict = (conflict1 == 1) or (conflict2 == 1) or (conflict3 == 1)
            if (self.conflict is True) and not self.resObtained:
                self.confcount = 1
                print("conflict detected")
                self.resObtained = True
                # Place holder for to use functions to obtain resolutions

            wpFeasibility = False
            if self.resObtained:
                if self.trackResolution:
                    if not math.isfinite(self.ptrack) or self.ptrack == -1:
                        if len(self.preferredTrack) > 0:
                            self.ptrack = self.preferredTrack[-1]
                        else:
                            self.ptrack = -1
                    currentHeading = ovelTrkGsVs[0]
                    safe2Turn = self.tfMonitor.check_safe_to_turn(ownship_pos_gx,ovelTrkGsVs,currentHeading,self.heading2target)
                    newOvelTrkGsVs = ovelTrkGsVs
                    newOvelTrkGsVs = (self.heading2target,ovelTrkGsVs[1],ovelTrkGsVs[2])
                    wpFeasibility = self.tfMonitor.monitor_wp_feasibility(ownship_pos_gx,ovelTrkGsVs,[wgx,wgy,targetPos[2]])

                    if not wpFeasibility:
                        self.conflict = 1
                        if self.ptrack == -1:
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
                    wpFeasibility = self.tfMonitor.monitor_wp_feasibility(ownship_pos_gx,ovelTrkGsVs,[wgx,wgy,targetPos[2]])
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
                    wpFeasibility = self.tfMonitor.monitor_wp_feasibility(ownship_pos_gx,ovelTrkGsVs,[wgx,wgy,targetPos[2]])
                    if not wpFeasibility: 
                        self.conflict = 1

                    if self.conflict:
                        diffAlt = self.palt - ownship_pos_gx[2]
                        self.pvs = 0.1 * diffAlt

                    self.preferredAlt.append(self.palt)

                    if self.palt == -1000:
                        self.palt = 5
            
