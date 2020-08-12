import random
import numpy as np

from communicationmodels import propagationmodels as pm


class ReceptionModel:
    def __init__(self, propagation_model=pm.NoLossPropagation()):
        self.propagation_model = propagation_model
        pm_name = self.propagation_model.model_name
        self.model_name = "Deterministic Reception (%s)" % pm_name
    """
    A model to determine probability of messages being received
    :param propagation_model: a model from propagationmodels to compute signal
    path loss
    """

    def p_reception(self, tx_power, freq, tx_pos_xyz,
                    rx_pos_xyz, rx_sensitivity):
        """
        Return probability of receiving a given message
        :param tx_power: transmitted power (W)
        :param freq: frequency of transmission (Hz)
        :param tx_pos_xyz: current position of transmitter [x, y, z] (m)
        :param rx_pos_xyz: current position of receiver [x, y, z] (m)
        :param rx_sensitivity: minimum received power threshold for reception
        """
        rx_power_w = self.propagation_model.received_power(tx_power,
                                                           freq,
                                                           tx_pos_xyz,
                                                           rx_pos_xyz)
        return rx_power_w > rx_sensitivity

    def received(self, tx_power, freq, tx_pos_xyz, rx_pos_xyz, rx_sensitivity):
        """
        Return whether a message was successfully received (True/False)
        :param tx_power: transmitted power (W)
        :param freq: frequency of transmission (Hz)
        :param tx_pos_xyz: current position of transmitter [x, y, z] (m)
        :param rx_pos_xyz: current position of receiver [x, y, z] (m)
        :param rx_sensitivity: minimum received power threshold for reception
        """
        p_rx = self.p_reception(tx_power,
                                freq,
                                tx_pos_xyz,
                                rx_pos_xyz,
                                rx_sensitivity)
        return random.random() < p_rx

    def communication_range(self, rx_sensitivity, tx_power, freq, h_t, h_r):
        """
        Return deterministic communication range (m)
        :param rx_sensitivity: min received power threshold for reception (W)
        :param tx_power: transmitted power (W)
        :param freq: frequency of transmission (Hz)
        :param h_t: height of transmitter above ground (m)
        :param h_r: height of receiver above ground (m)
        """
        return self.propagation_model.inverse(rx_sensitivity,
                                              tx_power,
                                              freq,
                                              h_t,
                                              h_r)

    def plot(self, rx_sensitivity, tx_power, freq, h_t=100, h_r=100,
             save=False, show=False):
        """
        Plot reception probability versus increasing separation range
        :param rx_sensitivity: min received power threshold for reception (W)
        :param tx_power: transmitted power (W)
        :param freq: frequency of transmission (Hz)
        :param h_t: height of transmitter above ground (m)
        :param h_r: height of receiver above ground (m)
        :param save: save the plot (bool)
        :param show: show the plot (bool)
        """
        from matplotlib import pyplot as plt
        tx_pos_xyz = [0, 0, h_t]
        CR = self.communication_range(rx_sensitivity, tx_power, freq, h_t, h_r)
        xmax = min(2*CR, 100000)
        xs = np.linspace(0, xmax, 10000)
        ys = [self.p_reception(tx_power,
                               freq,
                               tx_pos_xyz,
                               [0, x, h_r],
                               rx_sensitivity)
              for x in xs]
        plt.plot(xs, ys, label=self.model_name)
        plt.xlabel("Separation Distance (m)")
        plt.ylabel("Probability of Reception")
        plt.xlim((0, xmax))
        plt.ylim((0, 1.1))
        plt.legend(loc=3)
        plt.grid()
        if save:
            plt.savefig(self.model_name.replace(" ", ""))
        if show:
            plt.show()

    def heatmap(self, rx_sensitivity, tx_power, freq, h_t=100, h_r=100,
                save=False, show=False):
        """
        Plot a 2D map of reception probability
        :param rx_sensitivity: min received power threshold for reception (W)
        :param tx_power: transmitted power (W)
        :param freq: frequency of transmission (Hz)
        :param h_t: height of transmitter above ground (m)
        :param h_r: height of receiver above ground (m)
        :param save: save the plot (bool)
        :param show: show the plot (bool)
        """
        from matplotlib import pyplot as plt
        tx_pos_xyz = [0, 0, h_t]
        CR = self.communication_range(rx_sensitivity, tx_power, freq, h_t, h_r)
        range_max = min(2*CR, 100000)
        xs = np.linspace(-range_max, range_max, 100)
        ys = np.linspace(-range_max, range_max, 100)
        heatmap = np.empty((xs.size, ys.size))
        for i in range(len(xs)):
            for j in range(len(ys)):
                rx_pos_xyz = [xs[i], ys[j], h_r]
                heatmap[i, j] = self.p_reception(tx_power,
                                                 freq,
                                                 tx_pos_xyz,
                                                 rx_pos_xyz,
                                                 rx_sensitivity)
        extent = [xs[0], xs[-1], ys[0], ys[-1]]
        plt.imshow(heatmap, extent=extent)
        cb = plt.colorbar()
        cb.set_label("Probability of Reception")
        plt.title(self.model_name)
        plt.ylabel('y (m)')
        plt.xlabel('x (m)')
        if save:
            plt.savefig("%s_map.png" % self.model_name.replace(" ", ""))
        if show:
            plt.show()


class PerfectReception(ReceptionModel):
    def __init__(self):
        super().__init__()
        self.model_name = "Perfect Reception"
    """
    An unrealistic reception model that assumes all transmitted messages are
    received by all receivers.
    """

    def p_reception(self, tx_power, freq, tx_pos_xyz,
                    rx_pos_xyz, rx_sensitivity):
        return 1


class DeterministicReception(ReceptionModel):
    def __init__(self, propagation_model=pm.FreeSpacePropagation()):
        super().__init__(propagation_model=propagation_model)
    """
    A reception model that uses a given propagation model deterministically.
    For a given receiver sensitivity, all messages within range will be
    received and all messages from out of range will be missed.
    :param propagation_model: a model from propagationmodels to compute signal
    path loss
    """


class ConstantReception(ReceptionModel):
    def __init__(self, rx_rate=1):
        super().__init__()
        self.reception_rate = rx_rate
        self.model_name = "Constant Reception (P = %.2f)" % self.reception_rate
    """
    An unrealistic reception model where every message has an equal chance of
    being received, regardless of transmitter/receiver positions.
    :param rx_rate: rate of successful message reception (0 <= rx_rate <= 1)
    """

    def p_reception(self, tx_power, freq, tx_pos_xyz,
                    rx_pos_xyz, rx_sensitivity):
        return self.reception_rate


class RayleighReception(ReceptionModel):
    def __init__(self, propagation_model=pm.FreeSpacePropagation()):
        super().__init__(propagation_model=propagation_model)
        pm_name = self.propagation_model.model_name
        self.model_name = "Rayleigh Reception (%s)" % pm_name
    """
    A probabilistic reception model using a Rayleigh distribution. Each
    transmitted message has a chance of being received based on the position
    of the transmitter and receiver. This model reflects an environment with
    a large number of scattering objects, and is less appropriate when line of
    sight communication is possible.
    :param propagation_model: a model from propagationmodels to compute signal
    path loss
    """

    def rayleigh(self, d, CR):
        return np.exp(-(d/CR)**2)

    def p_reception(self, tx_power, freq, tx_pos_xyz,
                    rx_pos_xyz, rx_sensitivity):
        d = np.linalg.norm(np.array(tx_pos_xyz) - np.array(rx_pos_xyz))
        h_t = tx_pos_xyz[2]
        h_r = rx_pos_xyz[2]
        CR = self.communication_range(rx_sensitivity, tx_power, freq, h_t, h_r)
        p_rx = self.rayleigh(d, CR)
        return p_rx


class NakagamiReception(ReceptionModel):
    def __init__(self, m=3, propagation_model=pm.FreeSpacePropagation()):
        super().__init__(propagation_model=propagation_model)
        self.m = m          # Nakagami fading parameter
        pm_name = self.propagation_model.model_name
        self.model_name = "Nakagami Reception (%s, m = %d)" % (pm_name, self.m)
    """
    A probabilistic reception model using a Nakagami distribution. Each
    transmitted message has a chance of being received based on the position
    of the transmitter and receiver. The m parameter of the distribution
    accounts for different levels of multipath fading effects.
    :param propagation_model: a model from propagationmodels to compute signal
    path loss
    :param m: Nakagmi-m distribution parameter. m=1 matches Rayleigh
    distribution. Higher values of m correspond to less fading.
    """

    def nakagami(self, d, CR):
        A = np.exp(-self.m*(d/CR)**2)
        B = sum([(self.m*(d/CR)**2)**(i-1)/np.math.factorial(i - 1)
                 for i in range(1, self.m + 1)])
        return A*B

    def p_reception(self, tx_power, freq, tx_pos_xyz,
                    rx_pos_xyz, rx_sensitivity):
        d = np.linalg.norm(np.array(tx_pos_xyz) - np.array(rx_pos_xyz))
        h_t = tx_pos_xyz[2]
        h_r = rx_pos_xyz[2]
        CR = self.communication_range(rx_sensitivity, tx_power, freq, h_t, h_r)
        p_rx = self.nakagami(d, CR)
        return p_rx
