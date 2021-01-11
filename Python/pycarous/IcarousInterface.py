import abc
import sys
import numpy as np

from ichelper import LoadIcarousParams, ReadFlightplanFile, distance, ConvertToLocalCoordinates

class IcarousInterface(abc.ABC):
    """
    An abstract base class for interfacing with ICAROUS in python. Includes
    functions for inputting flight plan, geofence, and traffic information,
    and for recording log files. Subclasses must implement all of the
    functions marked @abc.abstractmethod.
    """
    def __init__(self, home_pos, callsign="SPEEDBIRD", vehicleID=0,
                 verbose=1, logRateHz=5):
        """
        Initialize Icarous interface
        :param home_pos: initial position / origin for local coordinates
        :param callsign: callsign for the vehicle
        :param vehicleID: unique integer ID for this Icarous instance
        :param verbose: control printout frequency (0: none, 1: some, 2+: more)
        :param logRateHz: number of data points to log per second
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
        self.apps         = []
        self.params       = {}
        self.localMergeFixes = []
        self.plans        = []
        self.localPlans   = []
        self.fences       = []
        self.localFences  = []
        self.flightplan1  = []
        self.controlInput = [0.0, 0.0, 0.0]
        self.fenceList    = []
        self.mergeFixes   = []
        self.arrTime      = None
        self.trkband      = None
        self.gsband       = None
        self.altband      = None
        self.vsband       = None

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
        self.land = False

        # Vehicle logs
        self.ownshipLog = {"time": [],
                           "position": [],
                           "velocityNED": [],
                           "commandedVelocityNED": [],
                           "positionNED": [],
                           "planOffsets":[],
                           "trkbands": [],
                           "gsbands": [],
                           "altbands": [],
                           "vsbands": []}
        self.trafficLog = {}
        self.logRateHz = logRateHz
        self.minLogInterval = 1/self.logRateHz - 0.01
        self.planoffsets = [0,0,0,0,0,0]

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

    def InputV2VData(self, data):
        """
        Input V2V data to ICAROUS
        :param data: the V2V data to input
        """
        for d in data:
            if d.type == "INTRUDER":
                self.InputTraffic(d.payload["id"], d.payload["pos"], d.payload["vel"])
            elif d.type == "MERGER":
                if self.arrTime is None:
                    return
                elif self.arrTime.intersectionID == d.payload.intersectionID:
                    self.InputMergeLogs(d.payload, 0.0)

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
    def InputFlightplan(self, fp, eta=False, repair=False):
        """
        Input a flight plan as a list of waypoints
        :param fp: a list of waypoints [lat, lon, alt, wp_metric]
        :param eta: when True, ICAROUS enforces wp arrival times, wp_metric (s)
                    when False, ICAROUS sets speed to each wp, wp_metric (m/s)
        :param repair: when True, repair linear flight plan to EUTL format
        """
        pass

    @abc.abstractmethod
    def InputFlightplanFromFile(self, filename, eta=False, repair=False):
        """
        Input a flight plan from a MAVProxy formatted text file
        :param filename: path to the flight plan file
        :param scenarioTime: start time of the scenario (s)
        :param eta: when True, ICAROUS enforces wp arrival times, wp_metric (s)
                    when False, ICAROUS sets speed to each wp, wp_metric (m/s)
        :param repair: when True, repair linear flight plan to EUTL format
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

    def ConvertToLocalCoordinates(self,pos):
        return ConvertToLocalCoordinates(self.home_pos,pos)

    def GetLocalFlightPlan(self, fp):
        local = []
        for wp in fp:
            if type(wp) == list:
                pt = wp
            else:
                pt = [wp.latitude,wp.longitude,wp.altitude]
            
            tcps = [wp.tcp[0],wp.tcp[1],wp.tcp[2]]
            tcpValues = [wp.tcpValue[0],wp.tcpValue[1],wp.tcpValue[2]]
            local.append([wp.time,*self.ConvertToLocalCoordinates(pt),*tcps,*tcpValues])
        return local

    def RecordOwnship(self):
        if self.ownshipLog["time"]:
            last_time = self.ownshipLog["time"][-1]
        else:
            last_time = -1
        if self.currTime - last_time < self.minLogInterval:
            return
        if sum(abs(p) for p in self.position) < 1e-3:
            return

        self.ownshipLog["time"].append(self.currTime)
        self.ownshipLog["position"].append(self.position)
        self.ownshipLog["velocityNED"].append(self.velocity)
        self.ownshipLog["positionNED"].append(self.localPos)
        self.ownshipLog["commandedVelocityNED"].append(self.controlInput)
        self.ownshipLog["planOffsets"].append(self.planoffsets)

        filterinfnan = lambda x: "nan" if np.isnan(x)  else "inf" if np.isinf(x) else x
        getbandlog = lambda bands: {} if bands is None else {
                                                             #"conflict": bands.currentConflictBand,
                                                             #"resup": filterinfnan(bands.resUp),
                                                             #"resdown": filterinfnan(bands.resDown),
                                                             "numBands": bands.numBands,
                                                             "bandTypes": [bands.type[i] for i in range(20)],
                                                             "low": [bands.min[i] for i in range(20)],
                                                             "high": [bands.max[i] for i in range(20)]}
        self.ownshipLog["trkbands"].append(getbandlog(self.trkband))
        #self.ownshipLog["gsbands"].append(getbandlog(self.gsband))
        #self.ownshipLog["altbands"].append(getbandlog(self.altband))
        #self.ownshipLog["vsbands"].append(getbandlog(self.vsband))

    def RecordTraffic(self, traffic_id, position, velocity, localPos):
        if traffic_id not in self.trafficLog:
            self.trafficLog[traffic_id] = {"time": [],
                                           "position": [],
                                           "velocityNED": [],
                                           "positionNED": []}
            last_time = -1
        else:
            last_time = self.trafficLog[traffic_id]["time"][-1]
        if self.currTime - last_time < self.minLogInterval:
            return
        self.trafficLog[traffic_id]["time"].append(self.currTime)
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
        :param logname: name for the log file, default is simlog-[callsign].json
        """
        if logname == "":
            logname = "log/simlog-%s.json" % self.callsign
        if self.verbose > 0:
            print("writing log: %s" % logname)

        waypoints = [[wp.time,wp.latitude,wp.longitude,wp.altitude,*wp.tcp,*wp.tcpValue]\
                      for wp in self.flightplan1]
        geofences_local = [fence.copy() for fence in self.fenceList]
        for fence, vertices in zip(geofences_local, self.localFences):
            fence["vertices"] = vertices

        log_data = {"state": self.ownshipLog,
                    "callsign": self.callsign,
                    "traffic": self.trafficLog,
                    "origin": self.home_pos,
                    "flightplans": [waypoints],
                    "flightplans_local": self.localPlans,
                    "geofences": self.fenceList,
                    "geofences_local": geofences_local,
                    "mergefixes_local": self.localMergeFixes,
                    "parameters": self.params,
                    "sim_type": self.simType}

        import json
        with open(logname, 'w') as f:
            json.dump(log_data, f)
