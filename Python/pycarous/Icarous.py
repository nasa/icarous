from ctypes import byref
import numpy as np
import time
from AutonomyStack import AutonomyStack
from CustomTypes import V2Vdata, TcpType, WPoint
from ichelper import (ConvertTrkGsVsToVned,
                      ConvertVnedToTrkGsVs,
                      ConvertToLocalCoordinates,
                      distance,
                      Getfence,
                      gps_offset,
                      GetFlightplan,
                      ConvertVnedToTrkGsVs,
                      ComputeHeading,
                      LoadIcarousParams,
                      ReadFlightplanFile,
                      GetEUTLPlanFromFile,
                      GetPlanPositions,
                      ParseAccordParamFile,
                      ConstructWaypointsFromList)
import time
from IcarousInterface import IcarousInterface

class Icarous(IcarousInterface):
    """
    Interface to launch and control an instance of pycarous
    (core ICAROUS modules called from python)
    """
    def __init__(self, home_pos, callsign="SPEEDBIRD", vehicleID=0, verbose=1,
                 logRateHz=5, fasttime=True, simtype="UAS_ROTOR",
                 monitor="DAIDALUS", daaConfig="data/IcarousConfig.txt",icConfig="data/IcarousConfig.txt"):
        """
        Initialize pycarous
        :param fasttime: when fasttime is True, simulation will run in fasttime
        :param simtype: string defining vehicle model: UAS_ROTOR, UAM_VTOL, ...
        :param monitor: string defining DAA module: DAIDALUS, ACAS, ...
        :param daaConfig: DAA module configuration file
        Other parameters are defined in parent class, see IcarousInterface.py
        """
        super().__init__(home_pos, callsign, vehicleID, verbose)

        self.simType = "pycarous"
        self.fasttime = fasttime
        if self.fasttime:
            self.currTime = 0
        else:
            self.currTime = time.time()

        # Initialize vehicle sim
        if simtype == "UAM_VTOL":
            from vehiclesim import UamVtolSim
            self.ownship = UamVtolSim(self.vehicleID, home_pos)
        elif simtype == "UAS_ROTOR":
            from vehiclesim import QuadSim
            self.ownship = QuadSim(self.vehicleID, home_pos)
        elif simtype == "UAM_SPQ":
            from vehiclesim import SixPassengerQuadSim
            self.ownship = SixPassengerQuadSim(self.vehicleID, home_pos)

        # Merger data
        self.arrTime = None
        self.logLatency = 0
        self.prevLogUpdate = 0
        self.localMergeFixes = []

        # Flight plan data
        self.flightplan1 = []
        self.flightplan2 = []
        self.etaFP1      = False
        self.etaFP2      = False
        self.nextWP1 = 1
        self.nextWP2 = 1
        self.plans = []
        self.localPlans = []
        self.activePlan = "Plan0"
        self.numSecPlan = 0
        self.repair = False
        self.daaType = monitor

        # Geofence data
        self.fences = []
        self.localFences= []
        self.numFences = 0

        # Other aircraft data
        self.daaConfig = daaConfig
        self.icConfig = icConfig
        self.emergbreak = False
        self.daa_radius = 0
        self.turnRate = 0
        self.fphases = -1
        self.land    = False
        self.ditch   = False

        self.loopcount = 0
        self.windFrom = 0.0
        self.windSpeed = 0.0
        self.defaultWPSpeed = 1.0
        self.mergeLogs = {}
        self.intersectionID = -2
        self.arrData = None

        opts = {}
        opts['callsign'] = self.callsign
        opts['daaConfig'] = self.daaConfig
        opts['icConfig'] = self.icConfig
        opts['verbose'] = verbose
        opts['vehicleID'] = self.vehicleID

        self.params = ParseAccordParamFile(self.daaConfig)
        self.core = AutonomyStack(opts)

    def SetPosUncertainty(self, xx, yy, zz, xy, xz, yz, coeff=0.8):
        self.ownship.SetPosUncertainty(xx, yy, zz, xy, xz, yz, coeff)

    def SetVelUncertainty(self, xx, yy, zz, xy, xz, yz, coeff=0.8):
        self.ownship.SetVelUncertainty(xx, yy, zz, xy, xz, yz, coeff)

    def InputTraffic(self,source,callsign,position,velocity):
        if callsign != self.callsign and np.abs(np.sum(position)) > 0:
            trkgsvs = ConvertVnedToTrkGsVs(velocity[0],velocity[1],velocity[2])
            self.core.InputIntruderState(self.currTime,source,callsign,position,trkgsvs)
            localPos = self.ConvertToLocalCoordinates(position)
            self.RecordTraffic(callsign, position, velocity, localPos)

    def InputFlightplan(self,waypoints,eta=False,repair=False,setInitialConditions=True):
        
        self.etaFP1 = eta
        self.repair = repair
        self.localPlans.append(self.GetLocalFlightPlan(waypoints))
        self.core.InputMissionFlightPlan(waypoints,repair,eta)

        #if repair: 
        #    waypoints = self.Guidance.GetKinematicPlan("Plan0")

        self.plans.append(waypoints)
        self.flightplan1 = waypoints 
        self.flightplan2 = []

        # Set initial conditions of model based on flightplan waypoints
        dist  = distance(waypoints[1].latitude,waypoints[1].longitude,waypoints[0].latitude,waypoints[0].longitude)
        speed = dist/(waypoints[1].time-waypoints[0].time)
        hdg   = ComputeHeading([waypoints[0].latitude,waypoints[0].longitude,0],[waypoints[1].latitude,waypoints[1].longitude,0])
        vn,ve,vd = ConvertTrkGsVsToVned(hdg,speed,0)
        if setInitialConditions:
            self.ownship.SetInitialConditions(z=waypoints[0].altitude,vx=ve,vy=vn)

    def InputFlightplanFromFile(self,filename,eta=False,repair=False,startTimeShift=0):
        import re
        match = re.search('\.eutl',filename)
        waypoints = []
        if match is None:
            fp = GetFlightplan(filename,self.defaultWPSpeed,eta) 
            waypoints = ConstructWaypointsFromList(fp,eta) 
        else:
            wps,totalwps = GetEUTLPlanFromFile(filename,0,timeshift=startTimeShift)
            for i in range(totalwps):
                waypoints.append(wps[i])
            eta = True
        self.InputFlightplan(waypoints,eta,repair)

    def InputGeofence(self,filename):
        self.fenceList = Getfence(filename)
        self.core.InputGeofence(self.fenceList)
        for fence in self.fenceList:
            self.numFences += 1

            localFence = []
            gf = []
            for vertex in fence['vertices']:
                localFence.append(self.ConvertToLocalCoordinates([*vertex,0]))
                gf.append([*vertex,0])

            self.localFences.append(localFence)
            self.fences.append(gf)

    def InputMergeFixes(self,filename):
        wp, ind, _, _, _ = ReadFlightplanFile(filename)
        self.localMergeFixes = list(map(self.ConvertToLocalCoordinates, wp))
        self.mergeFixes = wp
        self.core.SetIntersectionData(list(zip(ind,wp)))

    def SetParameters(self,params):
        if len(params.items()) == 0:
            return
        import os
        self.params.update(params)
        paramstr=''
        for item in self.params.items():
            paramstr+= item[0]+'='+str(item[1])+'\n'
        paramFile ='.tempICparam_'+self.callsign+'.txt' 
        fp = open(paramFile,'w')
        fp.write(paramstr)
        fp.close()
        self.core.InputParams(paramFile)
        os.remove(paramFile)


    def RunOwnship(self):
        self.controlInput = self.core.GetOutput()
        self.ownship.InputCommand(*self.controlInput)
        self.ownship.Run(windFrom=self.windFrom,windSpeed=self.windSpeed)

        opos = self.ownship.GetOutputPositionNED()
        ovel = self.ownship.GetOutputVelocityNED()

        self.localPos = opos

        (ogx, ogy) = gps_offset(self.home_pos[0], self.home_pos[1], opos[1], opos[0])
       
        self.position = [ogx, ogy, opos[2]]
        self.velocity = ovel
        self.trkgsvs = ConvertVnedToTrkGsVs(ovel[0],ovel[1],ovel[2])
        self.RecordOwnship()
        sigmaPos = [self.ownship.sigma_pos[0][0],
                    self.ownship.sigma_pos[1][1],
                    self.ownship.sigma_pos[2][2],
                    self.ownship.sigma_pos[0][1],
                    self.ownship.sigma_pos[0][2],
                    self.ownship.sigma_pos[1][2]]

        sigmaVel = [self.ownship.sigma_vel[0][0],
                    self.ownship.sigma_vel[1][1],
                    self.ownship.sigma_vel[2][2],
                    self.ownship.sigma_vel[0][1],
                    self.ownship.sigma_vel[0][2],
                    self.ownship.sigma_vel[1][2]]
        self.core.InputOwnshipState(self.currTime,self.position,self.trkgsvs,sigmaPos,sigmaVel)
        
    def InputMergeData(self,data):
        if data['intersectionID'] == self.intersectionID:
            self.mergeLogs[data['aircraftID']] = data
            self.core.InputMergerLogs(self.mergeLogs)

    def StartMission(self):
        self.core.StartFlight()
        self.missionStarted = True

    def CheckMissionComplete(self):
        return self.core.IsMissionComplete()

    def TransmitMergingData(self):
        """ Transmit data to coordinate merges """
        # Do not broadcast if running SBN
        if self.transmitter is None:
            return
        if not self.missionStarted or self.missionComplete:
            return

        out,arrdata = self.core.GetArrivalTimes()
        if out:
            if self.intersectionID != arrdata['intersectionID']:
                self.mergeLogs = {}
            self.mergeLogs[arrdata['aircraftID']] = arrdata
            self.intersectionID = arrdata['intersectionID']
        if self.intersectionID > 0:
            msg = V2Vdata('MERGER',arrdata)
            self.transmitter.transmit(self.currTime, self.position, msg)


    def Run(self):
        time_now = time.time()
        if not self.fasttime:
            if time_now - self.currTime >= 0.05:
                self.ownship.dt = time_now - self.currTime
                self.currTime = time_now
            else:
                return False
        else:
            self.currTime += 0.05

        if self.CheckMissionComplete():
            return True

        self.loopcount += 1

        if not self.missionStarted:
            return True

        self.RunOwnship()

        self.core.Run(self.currTime)

        if self.DitchCriteria is not None:
            if self.DitchCriteria(self.currTime,self.position,self.trkgsvs) and not self.ditch:
                self.core.StartDitch(self.ditchSite,self.ditchTOD)
                self.ditch = True


        self.TransmitPosition()

        self.TransmitMergingData()

        self.trkband = self.core.GetBands('track')
        self.gsband = self.core.GetBands('gs')
        self.vsband = self.core.GetBands('vs')
        self.altband = self.core.GetBands('alt')
        self.planoffsets = self.core.GetPlanOffsets()
        
        return True

    def Terminate(self):
        self.missionComplete = True
        outfp = self.core.GetAllSecondaryPlans()
        fps = []
        for fp in outfp:
            wps = []
            for wp in fp:
                wps.append(WPoint(**wp))
            fps.append(wps)

        if self.repair:
            self.plans.extend(fps)
        else:
            self.plans.extend(fps[1:])
