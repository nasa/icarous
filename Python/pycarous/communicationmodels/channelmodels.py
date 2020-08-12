from collections import namedtuple
import numpy as np

from communicationmodels import propagationmodels
from communicationmodels import receptionmodels


# Namedtuple to represent a transmitted message
field_names = 'freq tx_power sent_time sender_id tx_pos_xyz data'
Message = namedtuple('Message', field_names=field_names)


class ChannelModel:
    def __init__(self,
                 propagation_model=propagationmodels.NoLossPropagation(),
                 reception_model=receptionmodels.PerfectReception(),
                 ):
        self.propagation_model = propagation_model
        self.reception_model = reception_model
        self.messages = []
    """
    Class to represent communications channel
    :param propagation_model: a model from propagationmodels to compute
    signal path loss
    :param reception_model: a model from receptionmodels to compute
    reception rates
    """

    def transmit(self, msg):
        """
        Transmit a message onto the channel
        :param msg: a Message being transmitted
        """
        self.messages.append(msg)

    def receive(self, rx_pos_xyz, rx_sensitivity):
        """
        Return list of messages successfully received by a receiver
        :param rx_pos_xyz: current position of this receiver [x, y, z] (m)
        :param rx_sensitivity: min received power threshold for reception (W)
        """
        received_messages = []
        for msg in self.messages:
            if self.reception_model.received(msg.tx_power,
                                             msg.freq,
                                             msg.tx_pos_xyz,
                                             rx_pos_xyz,
                                             rx_sensitivity):
                received_messages.append(msg)
        return received_messages

    def flush(self):
        """ Clear all messages from the channel """
        self.messages = []
