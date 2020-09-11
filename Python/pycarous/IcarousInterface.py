import abc
import sys

sys.path.append("../pycarous")
from ichelper import LoadIcarousParams, ReadFlightplanFile, distance

class IcarousInterface(abc.ABC):
    """
    An abstract base class for interfacing with ICAROUS in python. Includes
    functions for inputting flight plan, geofence, and traffic information,
    and for recording log files. Subclasses must implement all of the
    functions marked @abc.abstractmethod.
    """
    def __init__(self, home_pos, callsign="SPEEDBIRD", vehicleID=0, verbose=1,
                 **kwargs):
        """
        Initialize Icarous interface
        :param home_pos: initial position / origin for local coordinates
        :param callsign: callsign for the vehicle
        :param vehicleID: unique integer ID for this Icarous instance
        :param verbose: control printout frequency (0: none, 1: some, 2+: more)
        Any additional keyword arguments are passed to Setup()
        """
        self.home_pos = home_pos
        self.callsign = callsign
        self.vehicleID = vehicleID
        self.verbose = verbose
        self.simType = ""

        self.defaultWPSpeed = 1
        self.windFrom = 0
        self.windSpeed = 0

        # Aircraft data
        self.params       = {}
        self.localMergeFixes = []
        self.plans        = []
        self.localPlans   = []
        self.fences       = []
        self.localFences  = []
        self.flightplan1  = []
        self.controlInput = [0.0, 0.0, 0.0]
        self.fenceList    = []
        self.mergeFixes = []

        # Aircraft state
        self.currTime = 0
        self.position = self.home_pos
        self.velocity = [0.0, 0.0, 0.0]
        self.trkgsvs  = [0.0, 0.0, 0.0]
        self.localPos = [0.0, 0.0, 0.0]
        self.terminated = False
        self.running = False
        self.missionStarted = False
        self.missionComplete = False

        # Vehicle logs
        self.ownshipLog = {"t": [],
                           "position": [],
                           "velocityNED": [],
                           "commandedVelocityNED": [],
                           "positionNED": [],
                           "trkbands": [],
                           "gsbands": [],
                           "altbands": [],
                           "vsbands": [],
                           "localPlans": [],
                           "localFences": []}
        self.trafficLog = {}

        # Call setup function
        self.Setup(**kwargs)

    @abc.abstractmethod
    def Setup(self):
        """ Conduct any initialization necessary to run ICAROUS """
        pass

    @abc.abstractmethod
    def SetPosUncertainty(self, xx, yy, zz, xy, yz, xz, coeff=0.8):
        """
        Set position uncertainty for vehicle model
        :param xx: x position variance [m^2] (East/West)
        :param yy: y position variance [m^2] (North/South)
        :param zz: z position variance [m^2] (Up/Down)
        :param xy: xy position covariance [m^2]
        :param yz: yz position covariance [m^2]
        :param xz: xz position covariance [m^2]
        :param coeff: smoothing factor used for uncertainty (default=0.8)
        """
        pass

    def InputWind(self, windFrom, windSpeed):
        """
        Set the current wind vector for vehicle simulation
        :param windFrom: wind source (deg CW, 0 is North)
        :param windSpeed: wind speed (m/s)
        """
        self.windFrom = windFrom
        self.windSpeed = windSpeed

    @abc.abstractmethod
    def InputTraffic(self, idx, position, velocity):
        """
        Input traffic surveillance data to ICAROUS
        :param idx: ID of the traffic vehicle
        :param position: traffic position [lat, lon, alt] (deg, deg, m)
        :param velocity: traffic velocity [vn, ve, vd] (m/s, m/s, m/s)
        """
        pass

    @abc.abstractmethod
    def InputFlightplan(self, fp, scenarioTime=0, eta=False):
        """
        Input a flight plan as a list of waypoints
        :param fp: a list of waypoints [lat, lon, alt, wp_metric]
        :param scenarioTime: start time of the scenario (s)
        :param eta: when True, ICAROUS enforces wp arrival times, wp_metric (s)
                    when False, ICAROUS sets speed to each wp, wp_metric (m/s)
        """
        pass

    @abc.abstractmethod
    def InputFlightplanFromFile(self, filename, scenarioTime=0, eta=False):
        """
        Input a flight plan from a MAVProxy formatted text file
        :param filename: path to the flight plan file
        :param scenarioTime: start time of the scenario (s)
        :param eta: when True, ICAROUS enforces wp arrival times, wp_metric (s)
                    when False, ICAROUS sets speed to each wp, wp_metric (m/s)
        """
        pass

    @abc.abstractmethod
    def InputGeofence(self, filename):
        """
        Input geofences from an xml file
        :param filename: path to xml geofence file
        """
        pass

    @abc.abstractmethod
    def InputMergeFixes(self, filename):
        """
        Input merge points from a MAVProxy formatted text file
        :param filename: path to the merge point file
        """
        pass

    @abc.abstractmethod
    def SetParameters(self, params):
        """
        Input ICAROUS parameters from a dictionary
        :param params: dict of params, PARAM_NAME -> PARAM_VALUE
        """
        pass

    def SetParametersFromFile(self, filename):
        """
        Input ICAROUS parameters from a .parm text file
        :param filename: path to .parm parameter file
        """
        params = LoadIcarousParams(filename)
        self.SetParameters(params)

    @abc.abstractmethod
    def InputMergeLogs(self, logs, delay):
        """
        Input V2V merge coordination data
        :param logs: merging log data
        :param delay: latency time to wait before processing logs (s)
        """
        pass

    @abc.abstractmethod
    def CheckMissionComplete(self):
        """ Return True if the mission is complete """
        pass

    def ConvertToLocalCoordinates(self, pos):
        """
        Convert a position to local coordinates relative to an origin
        :param pos: a position to convert [lat, lon, alt] (deg, deg, m)
        :return: local position, NEU (m, m, m)
        """
        dh = lambda x, y: abs(distance(self.home_pos[0], self.home_pos[1], x, y))
        if pos[1] > self.home_pos[1]:
            sgnX = 1
        else:
            sgnX = -1
        if pos[0] > self.home_pos[0]:
            sgnY = 1
        else:
            sgnY = -1
        dx = dh(self.home_pos[0], pos[1])
        dy = dh(pos[0], self.home_pos[1])
        return [dy*sgnY, dx*sgnX, pos[2]]

    def GetLocalFlightPlan(self, fp):
        local = []
        for wp in fp:
            local.append(self.ConvertToLocalCoordinates(wp))
        return local

    def RecordOwnship(self):
        if sum(abs(p) for p in self.position) < 1e-3:
            return
        self.ownshipLog["t"].append(self.currTime)
        self.ownshipLog["position"].append(self.position)
        self.ownshipLog["velocityNED"].append(self.velocity)
        self.ownshipLog["positionNED"].append(self.localPos)
        self.ownshipLog["commandedVelocityNED"].append(self.controlInput)

    def RecordTraffic(self, traffic_id, position, velocity, localPos):
        if traffic_id not in self.trafficLog:
            self.trafficLog[traffic_id] = {"t": [],
                                           "position": [],
                                           "velocityNED": [],
                                           "positionNED": []}
        self.trafficLog[traffic_id]["t"].append(self.currTime)
        self.trafficLog[traffic_id]["position"].append(list(position))
        self.trafficLog[traffic_id]["velocityNED"].append(velocity)
        self.trafficLog[traffic_id]["positionNED"].append(localPos)

    @abc.abstractmethod
    def Run(self):
        """ Run one timestep of the simulation """
        pass

    @abc.abstractmethod
    def StartMission(self):
        """ Send MissionStart command to ICAROUS """
        pass

    @abc.abstractmethod
    def Terminate(self):
        """ End any processes if necessary """
        pass

    def WriteLog(self, logname=""):
        """
        Save log data to a json file
        :param logname: name for the log file, default is callsign.json
        """
        self.ownshipLog['localPlans'] = self.localPlans
        self.ownshipLog['localFences'] = self.localFences
        if logname == "":
             logname = self.callsign + '.json'

        import json
        if self.verbose > 0:
            print("writing log: %s" % logname)
        log_data = {"ownship": self.ownshipLog,
                    "traffic": self.trafficLog,
                    "waypoints": self.flightplan1,
                    "geofences": self.fenceList,
                    "parameters": self.params,
                    "mergefixes": self.localMergeFixes,
                    "sim_type": self.simType}

        with open(logname, 'w') as f:
            json.dump(log_data, f)

