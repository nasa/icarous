import pandas as pd

from vehiclesim import VehicleSimInterface
from communicationmodels import get_transmitter
from CustomTypes import V2Vdata


class TrafficReplay(VehicleSimInterface):
    """ Vehicle sim to replay """
    def __init__(self, logfile, channel, delay=0):
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

        self.current_time = None

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

        if self.current_time is None:
            self.position_updates.index += current_time
            self.current_time = current_time

        # Transmit all traffic messages up to current time
        traffic_messages = self.position_updates[self.current_time:current_time]
        for i, traffic_msg in traffic_messages.iterrows():
            posLLA = [traffic_msg.lat, traffic_msg.lon, traffic_msg.alt]
            velNED = [traffic_msg.vn, traffic_msg.ve, traffic_msg.vd]
            msg_data = {
                "callsign": "replay_" + traffic_msg.vehicleID,
                "pos": posLLA,
                "vel": velNED,
            }
            msg = V2Vdata("INTRUDER", msg_data)
            self.transmitter.transmit(current_time, posLLA, msg)

        self.current_time = current_time
