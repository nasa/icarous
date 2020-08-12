import numpy as np

from communicationmodels.channelmodels import Message


class Transmitter:
    def __init__(self,
                 channel,
                 sensor_type="Truth",
                 tx_power=40,
                 freq=978e6,
                 update_interval=0):
        self.channel = channel
        self.sensorType = sensor_type
        self.transmitPower = tx_power
        self.frequencyHz = freq
        self.updateInterval = update_interval
        self.timeLastTransmit = 0
    """
    Class to represent a transmitter
    :param channel: communications channel to transmit on
    :param sensor_type: name of the sensor
    :param tx_power: transmit power (W)
    :param freq: transmit frequency (Hz)
    :param update_interval: time between transmissions (s), 0 to always send
    """

    def transmit(self, current_time, callsign, tx_pos_xyz, data):
        """
        Transmit data onto the communication channel
        :param current_time: current time (s) of the simulation
        :param callsign: callsign of this transmitter
        :param tx_pos_xyz: current position of this transmitter [x, y, z] (m)
        :param data: the data to send
        """
        if current_time - self.timeLastTransmit < self.updateInterval:
            return False
        self.timeLastTransmit = current_time
        msg = Message(self.frequencyHz, self.transmitPower, current_time,
                      callsign, tx_pos_xyz, data)
        self.channel.transmit(msg)
        return msg


class Receiver:
    def __init__(self,
                 channel,
                 sensor_type="Truth",
                 sensitivity=0,
                 latency=0):
        self.channel = channel
        self.sensorType = sensor_type
        self.sensitivity = sensitivity
        self.latency = latency
        self.messages = []
    """
    Class to represent a receiver
    :param channel: communications channel to receive from
    :param sensor_type: name of the sensor
    :param sensitivity: minimum received power threshold for reception (W)
    :param latency: time to wait before receiving a message (s)
    """

    def receive(self, current_time, rx_pos_xyz):
        """
        Return list of messages that are successfully received
        from the communication channel
        :param current_time: current time (s) of the simulation
        :param rx_pos_xyz: current position of this receiver [x, y, z] (m)
        """
        received_msgs = self.channel.receive(rx_pos_xyz, self.sensitivity)
        self.messages += received_msgs
        msgs = [m for m in self.messages
                if m.sent_time + self.latency >= current_time]
        self.messages = [m for m in self.messages
                         if m.sent_time + self.latency < current_time]
        return msgs


class DummyTransmitter(Transmitter):
    def __init__(self, channel):
        super().__init__(channel)

    def transmit(self, current_time, callsign, tx_pos_xyz, data):
        pass


class DummyReceiver(Receiver):
    def __init__(self, channel):
        super().__init__(channel)

    def receive(self, current_time, rx_pos_xyz):
        return []


class ADSBTransmitter(Transmitter):
    def __init__(self, channel, tx_power=10, update_interval=1):
        super().__init__(channel,
                         sensor_type="ADS-B",
                         tx_power=tx_power,
                         freq=978e6,
                         update_interval=update_interval)


class ADSBReceiver(Receiver):
    def __init__(self, channel, sensitivity=1e-10):
        super().__init__(channel,
                         sensor_type="ADS-B",
                         sensitivity=sensitivity,
                         latency=0.5)


class FLARMTransmitter(Transmitter):
    def __init__(self, channel):
        super().__init__(channel,
                         sensor_type="FLARM",
                         freq=928e6,
                         update_interval=1)


class FLARMReceiver(Receiver):
    def __init__(self, channel, sensitivity=1e-10):
        super().__init__(channel,
                         sensor_type="FLARM",
                         sensitivity=sensitivity,
                         latency=0.5)
