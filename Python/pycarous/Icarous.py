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
                 monitor="DAIDALUS", daaConfig="data/DaidalusQuadConfig.txt"):
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

        # Geofence data
        self.fences = []
        self.localFences= []
        self.numFences = 0

        # Other aircraft data
        self.daaConfig = daaConfig
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
        opts['daaLog'] = False
        opts['verbose'] = verbose
        opts['vehicleID'] = self.vehicleID
        self.core = AutonomyStack(opts)

    def SetPosUncertainty(self, xx, yy, zz, xy, xz, yz, coeff=0.8):
        self.ownship.SetPosUncertainty(xx, yy, zz, xy, xz, yz, coeff)

    def SetVelUncertainty(self, xx, yy, zz, xy, xz, yz, coeff=0.8):
        self.ownship.SetVelUncertainty(xx, yy, zz, xy, xz, yz, coeff)

    def InputTraffic(self,callsign,position,velocity):
        if callsign != self.callsign and np.abs(np.sum(position)) > 0:
            trkgsvs = ConvertVnedToTrkGsVs(velocity[0],velocity[1],velocity[2])
            self.core.InputIntruderState(self.currTime,callsign,position,trkgsvs)
            localPos = self.ConvertToLocalCoordinates(position)
            self.RecordTraffic(callsign, position, velocity, localPos)

    def InputFlightplan(self,waypoints,eta=False,repair=False):
        
        self.etaFP1 = eta
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
        if not eta:
            self.ownship.SetInitialConditions(z=waypoints[0].altitude,vx=ve,vy=vn)

    def InputFlightplanFromFile(self,filename,eta=False,repair=False,startTimeShift=0):
        import re
        match = re.search('\.eutl',filename)
        waypoints = []
        if match is None:
            fp = GetFlightplan(filename,self.defaultWPSpeed,eta) 
            waypoints = ConstructWaypointsFromList(fp,eta) 
        else:
            wps,totalwps = GetEUTLPlanFromFile(filename,self.vehicleID,timeshift=startTimeShift)
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
        self.params = params
        self.core.SetParams(params)

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

        self.RunOwnship()

        if not self.missionStarted:
            return True

        self.core.Run(self.currTime)

        self.TransmitPosition()

        self.TransmitMergingData()

        self.trkband = self.core.GetBands('track')
        self.gsband = self.core.GetBands('gs')
        self.vsband = self.core.GetBands('vs')
        self.altband = self.core.GetBands('alt')
        
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
            
        self.plans.extend(fps)


def VisualizeSimData(icList,allplans=False,showtraffic=True,xmin=-100,ymin=-100,xmax=100,ymax=100,playbkspeed=1,interval=30,record=False,filename=""):
    '''
    ic: icarous object
    allplans: True - plot all computed plans, False - plot only the mission plan
    xmin,ymin : plot axis min values
    xmax,ymax : plot axis max values
    interval  : Interval between frames
    '''
    if record:
        import matplotlib; matplotlib.use('Agg')
    from Animation import AgentAnimation
    anim= AgentAnimation(xmin,ymin, xmax,ymax,playbkspeed,interval,record,filename)

    vehicleSize1 = np.abs(xmax - xmin)/100
    vehicleSize2 = np.abs(ymax - ymin)/100
    vehicleSize  = np.max([vehicleSize1,vehicleSize2])
    homePos = icList[0].home_pos
    for j,ic in enumerate(icList):
        anim.AddAgent('ownship'+str(j),vehicleSize,'r',ic.ownshipLog,show_circle=True,circle_rad=ic.daa_radius)
        for i,pln in enumerate(ic.plans):
            planPositions = np.array(GetPlanPositions(ic.plans[i],0.1))
            getLocPos = lambda pos: np.array(ConvertToLocalCoordinates(homePos,pos))
            points = np.array(list(map(getLocPos,planPositions)))
            if i == 0:
                planWPs = np.array(pln)[:,1:] 
                labels = [[TcpType.getString(val[3]),TcpType.getString(val[4]),TcpType.getString(val[5])]\
                           for val in planWPs]
                anim.AddPath(np.array(list(map(getLocPos,planWPs))),'k--',points,labels)

            if i > 0 and allplans:
                planWPs = np.array(pln)[:,1:] 
                anim.AddPath(np.array(list(map(getLocPos,planWPs))),'k--',points)
        tfids = ic.trafficLog.keys()
        if showtraffic:
            for key in tfids:
                anim.AddAgent('traffic'+str(key),vehicleSize,'b',ic.trafficLog[key])
        for fence in ic.localFences:
            fence.append(fence[0])
            anim.AddFence(np.array(fence),'c-.')
    for fix in icList[0].localMergeFixes:
        anim.AddZone(fix[::-1][1:3],icList[0].params['COORD_ZONE'],'r')
        anim.AddZone(fix[::-1][1:3],icList[0].params['SCHEDULE_ZONE'],'b')
        anim.AddZone(fix[::-1][1:3],icList[0].params['ENTRY_RADIUS'],'g')

    anim.run()
