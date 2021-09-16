from icutils.AccordUtil import ConvertNED2LLA
import pandas as pd

from vehiclesim import VehicleSimInterface
from communicationmodels import get_transmitter
from CustomTypes import V2Vdata


class TrafficReplay(VehicleSimInterface):
    """ Vehicle sim to replay """
    def __init__(self, logfile, channel, delay=0, homePos = None, units = [1,1,1,1,1,1],filter=[]):
        """
        Initialize traffic log replay
        :param logfile: filename for csv traffic log containing columns for:
            time, vehicleID, lat, lon, alt, vN, vE, vD, [position uncertainty, velocity uncertainty]
            Properly formatted csv traffic logs can be generated from tlog, json, or daa
            log files using the read_traffic.py tool in SIRIUS
        :param channel: communication channel to replay traffic messages on
        :param delay: number of seconds to wait before starting log replay
        """
        self.delay = delay
        self.transmitter = get_transmitter("GroundTruth", "TrafficReplay", channel)

        # Read position updates from csv file
        self.position_updates = pd.read_csv(logfile, index_col="time")
        self.position_updates.sort_index(inplace=True)
        self.position_updates.index -= self.position_updates.index[0]
        self.homePos = homePos
        self.units = units
        self.filter = filter

        self.lastBcastTime = 0

    def InputCommand(self, track=0, gs=0, climbrate=0):
        """ Replayed log can't react to input commands """
        pass

    def Run(self, windFrom=0, windTo=0):
        """ Replaying log doesn't need to simulate vehicles """
        pass

    def GetOutputPositionNED(self):
        pass

    def GetOutputVelocityNED(self):
        pass

    def TransmitPosition(self, current_time):
        """ Transmit recorded traffic positions """
        if self.transmitter is None:
            return

        # Transmit all traffic messages up to current time
        traffic_messages = self.position_updates[self.lastBcastTime:current_time]
        for i, traffic_msg in traffic_messages.iterrows():
            if traffic_msg.vehicleID in self.filter:
                continue 
            if self.homePos is not None:
                posLLA = ConvertNED2LLA(self.homePos,[traffic_msg.sy * self.units[0],traffic_msg.sx*self.units[1],traffic_msg.sz*self.units[2]])
                velNED = [traffic_msg.vy*self.units[3], traffic_msg.vx*self.units[4], traffic_msg.vz*self.units[5]]
            else:
                posLLA = [traffic_msg.lat*self.units[0], traffic_msg.lon*self.units[1], traffic_msg.alt*self.units[2]]
                velNED = [traffic_msg.vn*self.units[3], traffic_msg.ve*self.units[4], traffic_msg.vd*self.units[5]]
            msg_data = {
                "source": self.transmitter.sensorType,
                "callsign": "replay_" + traffic_msg.vehicleID,
                "pos": posLLA,
                "vel": velNED,
            }
            msg = V2Vdata("INTRUDER", msg_data)
            self.transmitter.transmit(current_time, posLLA, msg)

            self.lastBcastTime = current_time
