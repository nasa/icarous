import numpy as np

from VehicleSim import VehicleSim
from Merger import LogData, MergerData, MAX_NODES


class SimEnvironment:
    """ Class to manage pyicarous fast time simulations """
    def __init__(self):
        # Vehicle instances
        self.icInstances = []
        self.icStartDelay = []
        self.icTimeLimit = []
        self.tfList = []

        # Simulation environment conditions
        self.wind = [(0, 0)]
        self.commDelay = 0
        self.mergeFixFile = None

        # Simulation status
        self.count = 0
        self.time = 0
        self.windFrom, self.windSpeed = self.wind[0]


    def AddIcarousInstance(self, ic, delay=0, time_limit=1000):
        """
        Add an Icarous instance to the simulation environment
        @param ic: An Icarous instance
        @param delay: Time to delay before starting mission (s)
        @param time_limit: Time limit to fly before shutting vehicle down (s)
        """
        self.icInstances.append(ic)
        self.icStartDelay.append(delay)
        self.icTimeLimit.append(time_limit)


    def AddTraffic(self, idx, home, rng, brng, alt, speed, heading, crate):
        """
        Add a simulated virtual traffic vehicle to the simulation environment
        @param idx: traffic vehicle id
        @param home: home position (lat [deg], lon [deg], alt [m])
        @param rng: starting distance from home position [m]
        @param brng: starting bearing from home position [deg], 0 is North
        @param alt: starting altitude [m]
        @param speed: traffic speed [m/s]
        @param heading: traffic heading [deg], 0 is North
        @param crate: traffic climbrate [m/s]
        """
        tx = rng*np.sin(brng*np.pi/180)
        ty = rng*np.cos(brng*np.pi/180)
        tz = alt
        tvx = speed*np.sin(heading*np.pi/180)
        tvy = speed*np.cos(heading*np.pi/180)
        tvz = crate
        traffic = VehicleSim(idx, tx, ty, tz, tvx, tvy, tvz)
        traffic.home_gps = np.array(home)
        self.tfList.append(traffic)
        return traffic


    def RunTraffic(self):
        """ Update all traffic vehicles """
        for tf in self.tfList:
            tf.run(self.windFrom, self.windSpeed)
            self.TransmitPositionData(tf)


    def AddWind(self, wind):
        """
        Add a wind vector to simulation environment
        @param wind: a list of tuples representing the wind vector at each
        simulation timestep.
        ex: [(wind source [deg, 0 = North], wind speed [m/s]), ...]
        """
        self.wind = wind


    def GetWind(self):
        """ Return wind tuple for current simulation timestep """
        i = min(len(self.wind) - 1, self.count)
        return self.wind[i]


    def SetPosUncertainty(self, xx, yy, zz, xy, yz, xz, coeff=0.8):
        """
        Set position uncertainty for all existing Icarous and traffic instances
        @param xx: x position variance [m^2] (East/West)
        @param yy: y position variance [m^2] (North/South)
        @param zz: z position variance [m^2] (Up/Down)
        @param xy: xy position covariance [m^2]
        @param yz: yz position covariance [m^2]
        @param xz: xz position covariance [m^2]
        @param coeff: smoothing factor used for uncertainty (default=0.8)
        """
        for vehicle in (self.icInstances + self.tfList):
            vehicle.setpos_uncertainty(xx, yy, zz, xy, yz, xz, coeff)


    def InputMergeFixes(self, filename):
        """ Input a file to read merge fixes from """
        self.mergeFixFile = filename


    def ExchangeArrivalTimes(self):
        """ Exchange V2V communications between the Icarous instances """
        arrTimes = []
        log = []
        for ic in self.icInstances:
            if ic.arrTime is not None:
                arrTimes.append(ic.arrTime)

        for arr in arrTimes:
            fid = arr.intersectionID
            if fid <= 0:
                continue
            avail = False
            for g in log:
                if fid == g[0].intersectionID:
                    avail = True
                    g.append(arr)
            if not avail:
                log.append([arr])

        for lg in log:
            if len(lg) == 0:
                continue

            datalog = LogData()
            datalog.intersectionID = lg[0].intersectionID
            datalog.nodeRole = 1
            datalog.totalNodes = len(lg)
            mg = MergerData*MAX_NODES
            datalog.log = mg(*lg)
            for ic in self.icInstances:
                ic.InputMergeLogs(datalog, self.commDelay)


    def TransmitPositionData(self, source_vehicle):
        """
        Transmit vehicle position data to all Icarous instances.
        @param source_vehicle: The vehicle transmitting its position data
        """
        v_id = source_vehicle.vehicleID
        pos = source_vehicle.pos_gps
        locpos = source_vehicle.pos
        vel = source_vehicle.vel

        for target_vehicle in self.icInstances:
            if target_vehicle.vehicleID == v_id:
                continue
            target_vehicle.InputTraffic(v_id, pos, vel, locpos)


    def RunSimulation(self):
        """ Run simulation until mission is complete or time limit is reached """
        simComplete = False
        prevTime = [ic.currTime for ic in self.icInstances]
        if self.mergeFixFile is not None:
            for ic in self.icInstances:
                ic.InputMergeFixes(self.mergeFixFile)

        while not simComplete:
            status = False
            self.windFrom, self.windSpeed = self.GetWind()

            for i, ic in enumerate(self.icInstances):
                ic.InputWind(self.windFrom, self.windSpeed)

                # Send mission start command
                if not ic.startSent:
                    if ic.currTime - prevTime[i] > self.icStartDelay[i]:
                        ic.Cog.InputStartMission(0, 0)
                        ic.startSent = True
                        prevTime[i] = ic.currTime
                        print("%s : Start command sent at %f" %
                              (ic.callsign, prevTime[i]))

                # Run Icarous and send position data to other Icarous instances
                if not ic.CheckMissionComplete():
                    status |= ic.Run()
                    self.TransmitPositionData(ic.ownship)
                else:
                    # Update log for analysis, even if mission is complete
                    ic.RecordOwnship()
                    self.TransmitPositionData(ic.ownship)

                # Check if time limit has been met
                if ic.startSent and not ic.missionComplete:
                    if ic.currTime - prevTime[i] > self.icTimeLimit[i]:
                        ic.missionComplete = True
                        print("%s : Time limit reached at %f" %
                              (ic.callsign, ic.currTime))

            # Run traffic vehicles and send position data to Icarous instances
            if status:
                self.RunTraffic()

            self.ExchangeArrivalTimes()
            self.count += 1
            simComplete = all(ic.missionComplete for ic in self.icInstances)

        for i, ic in enumerate(self.icInstances):
            to_local = self.icInstances[0].ConvertToLocalCoordinates
            posNED = list(map(to_local, ic.ownshipLog["position"]))
            ic.ownshipLog["positionNED"] = posNED
            for tfid in ic.trafficLog.keys():
                tfPosNED = list(map(to_local, ic.trafficLog[tfid]["position"]))
                ic.trafficLog[tfid]["positionNED"] = tfPosNED
            localFP = list(map(to_local, ic.flightplan1))
            ic.localPlans[0] = localFP

    def WriteLog(self):
        """ Write json logs for each icarous instance """
        # First, transfer all vehicles to a common reference frame
        for i, ic in enumerate(self.icInstances):
            ic.WriteLog()
