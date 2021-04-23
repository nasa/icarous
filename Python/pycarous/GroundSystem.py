import abc

from communicationmodels import get_transmitter, get_receiver
from ichelper import ConvertToLocalCoordinates,ConvertToGPS
import numpy as np

class GroundSystem(abc.ABC):
    """
    An abstract base class for a ground system to interact with simulated
    vehicles in the air. Subclasses must implement all of the functions marked
    @abc.abstractmethod.
    """
    def __init__(self, position, system_id, callsign="GroundSystem1", verbose=1):
        """
        Initialize ground system interface
        :param position: position of the ground system transmitter
                         [lat (deg), lon (deg), alt (m)]
        :param system_id: unique integer ID for this ground system
        :param callsign: callsign for the ground system
        :param verbose: control printout frequency (0: none, 1: some, 2+: more)
        """
        self.position = position
        self.id = system_id
        self.name = callsign
        self.verbose = verbose
        self.current_time = 0

        self.transmitter = None
        self.receiver = None

    @abc.abstractmethod
    def Run(self):
        """ Action for ground system to take at each timestep of simulation"""
        pass

    def ReceiveV2VData(self):
        """
        Receive any V2V messages available to receiver model and input to ground system
        """
        if self.receiver is None:
            return
        received_msgs = self.receiver.receive(self.current_time, self.position)
        for msg in received_msgs:
            self.InputV2VData(msg.sent_time,msg.data)

    @abc.abstractmethod
    def InputV2VData(self,time, data):
        """
        Input V2V data to ground system. This could be ADS-B traffic data or other
        messages from aircraft or other ground systems.
        :param data: V2VData to input
        """
        pass


class AdsbRebroadcast(GroundSystem):
    
    def __init__(self, position, system_id, callsign="GroundSystem1", interval=1,verbose=1):
        super().__init__(position, system_id, callsign, verbose)
        self.log = []
        self.lastBroadCast = 0
        self.interval = interval

    def Run(self, current_time):
        if self.verbose > 0:
            print("Running %s" % self.name)
        self.current_time = current_time
        if self.current_time - self.lastBroadCast > self.interval:
            self.lastBroadCast = self.current_time
            while len(self.log):
                (t,data) = self.log.pop()
                data.payload["callsign"] = "REBROADCAST_" + data.payload["callsign"]
                anchor = self.position
                anchor[2] = 0.0
                localPos = ConvertToLocalCoordinates(anchor,data.payload["pos"])
                noise = np.random.randn(2)
                localPos[0] += noise[0]
                localPos[1] += noise[1]
                pos = ConvertToGPS(anchor,localPos)
                data.payload["pos"] = pos
                if self.transmitter is not None:
                    self.transmitter.transmit(current_time, self.position, data)

    def InputV2VData(self,time, data):
        if data.type == "INTRUDER" and not data.payload["callsign"].startswith("REBROADCAST"):
            print("GS received:", data.payload["callsign"], data.payload["pos"], data.payload["vel"])
            self.log.append(data)

class GroundPlanner(GroundSystem):
    def __init__(self,position,system_id,callsign,verbose=1,filename=None):
        super().__init__(position,system_id,callsign,verbose)
        import ichelper as ich
        self.fp = ich.GetFlightplan(filename) 
        self.waypoints = ich.ConstructWaypointsFromList(self.fp) 
        self.sent = False

    def Run(self,current_time):
        if not self.sent:
            if current_time > 10:
                if self.transmitter is not None:
                    import CustomTypes as ictypes
                    # shift waypoints to current time
                    for wp in self.waypoints:
                        wp.time += current_time
                    plan = ictypes.FPlan(False,False,self.waypoints)
                    data = ictypes.V2Vdata('FLIGHTPLAN',plan)
                    self.transmitter.transmit(current_time,self.position,data)
                    self.sent = True
    
    def InputV2VData(self, data):
        return 
