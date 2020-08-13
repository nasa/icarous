import numpy as np

from VehicleSim import VehicleSim
from ichelper import distance
from Merger import LogData, MergerData, MAX_NODES
from communicationmodels import channelmodels as cm
from communicationmodels import sensormodels as sm


class SimEnvironment:
    """ Class to manage pycarous fast time simulations """
    def __init__(self):
        # Vehicle instances
        self.icInstances = []
        self.icStartDelay = []
        self.icTimeLimit = []
        self.tfList = []

        # Communication channel
        pm = cm.propagationmodels.TwoRayGroundPropagation()
        rm = cm.receptionmodels.RayleighReception()
        self.comm_channel = cm.ChannelModel(propagation_model=pm,
                                            reception_model=rm)

        # Simulation environment conditions
        self.wind = [(0, 0)]
        self.commDelay = 0
        self.mergeFixFile = None
        self.home_gps = [0, 0, 0]

        # Simulation status
        self.count = 0
        self.current_time = 0
        self.windFrom, self.windSpeed = self.wind[0]


    def AddIcarousInstance(self, ic, delay=0, time_limit=1000,
                           transmitter="GroundTruth", receiver="GroundTruth"):
        """
        Add an Icarous instance to the simulation environment
        @param ic: An Icarous instance
        @param delay: Time to delay before starting mission (s)
        @param time_limit: Time limit to fly before shutting vehicle down (s)
        @param transmitter: A Transmitter to send V2V position data
        @param receiver: A Receiver to send V2V position data
        """
        self.icInstances.append(ic)
        self.icStartDelay.append(delay)
        self.icTimeLimit.append(time_limit)

        # Create a transmitter/receiver for V2V communications
        ic.transmitter = sm.get_transmitter(transmitter, self.comm_channel)
        ic.receiver = sm.get_receiver(receiver, self.comm_channel)

        # Set simulation home position
        if self.home_gps == [0, 0, 0]:
            self.home_gps = ic.home_pos


    def AddTraffic(self, idx, home, rng, brng, alt, speed, heading, crate,
                   transmitter="GroundTruth"):
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
        @param transmitter: A Transmitter to send V2V position data
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

        # Create a transmitter for V2V communications
        traffic.transmitter = sm.get_transmitter(transmitter, self.comm_channel)

        return traffic


    def RunTraffic(self):
        """ Update all traffic vehicles """
        for tf in self.tfList:
            tf.run(self.windFrom, self.windSpeed)
            data = {"pos": tf.pos_gps, "vel": tf.getOutputVelocityNED()}
            tf.transmitter.transmit(self.current_time, tf.vehicleID, tf.pos_gps, data)


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
                if ic.arrTime is None:
                    continue
                if ic.arrTime.intersectionID == datalog.intersectionID:
                    ic.InputMergeLogs(datalog, self.commDelay)


    def RunSimulation(self):
        """ Run simulation until mission is complete or time limit is reached """
        simComplete = False
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
                    if self.current_time >= self.icStartDelay[i]:
                        ic.Cog.InputStartMission(0, 0)
                        ic.startSent = True
                        print("%s : Start command sent at %f" %
                              (ic.callsign, self.current_time))

                # Run Icarous and send position data to other Icarous instances
                status |= ic.Run()

                # Transmit V2V position data
                data = {"pos": ic.position, "vel": ic.velocity}
                ic.transmitter.transmit(self.current_time, ic.vehicleID, ic.position, data)

                # Check if time limit has been met
                if ic.startSent and not ic.missionComplete:
                    if self.current_time >= self.icTimeLimit[i]:
                        ic.missionComplete = True
                        print("%s : Time limit reached at %f" %
                              (ic.callsign, self.current_time))

            # Run traffic vehicles
            self.RunTraffic()

            # Receive V2V position data
            for ic in self.icInstances:
                received_msgs = ic.receiver.receive(self.current_time, ic.position)
                for msg in received_msgs:
                    if msg.sender_id != ic.vehicleID:
                        ic.InputTraffic(msg.sender_id, msg.data["pos"], msg.data["vel"])
            self.comm_channel.flush()

            self.ExchangeArrivalTimes()
            self.count += 1
            self.current_time = self.count*0.05
            simComplete = all(ic.missionComplete for ic in self.icInstances)
        self.ConvertLogsToLocalCoordinates()


    def ConvertLogsToLocalCoordinates(self):
        for i, ic in enumerate(self.icInstances):
            to_local = self.ConvertToLocalCoordinates
            posNED = list(map(to_local, ic.ownshipLog["position"]))
            ic.ownshipLog["positionNED"] = posNED
            for tfid in ic.trafficLog.keys():
                tfPosNED = list(map(to_local, ic.trafficLog[tfid]["position"]))
                ic.trafficLog[tfid]["positionNED"] = tfPosNED
            ic.localPlans = []
            ic.localFences = []
            ic.localMergeFixes = []
            for plan in ic.plans:
                localFP = list(map(to_local, plan))
                ic.localPlans.append(localFP)
            for fence in ic.fences:
                localFence = list(map(to_local, fence))
                ic.localFences.append(localFence)
            ic.localMergeFixes = list(map(to_local, ic.mergeFixes))


    def ConvertToLocalCoordinates(self, pos_gps):
        def dh(x, y):
            return abs(distance(self.home_gps[0], self.home_gps[1], x, y))
        if pos_gps[1] > self.home_gps[1]:
            sgnX = 1
        else:
            sgnX = -1
        if pos_gps[0] > self.home_gps[0]:
            sgnY = 1
        else:
            sgnY = -1
        dx = dh(self.home_gps[0], pos_gps[1])
        dy = dh(pos_gps[0], self.home_gps[1])
        return [dy*sgnY, dx*sgnX, pos_gps[2]]


    def WriteLog(self):
        """ Write json logs for each icarous instance """
        for i, ic in enumerate(self.icInstances):
            ic.WriteLog()
