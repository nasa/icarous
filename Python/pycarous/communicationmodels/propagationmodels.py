import numpy as np

from communicationmodels import util


class PropagationModel:
    def __init__(self, L=1):
        self.L = L  # Path loss factor
        self.model_name = "Constant Propagation (L = %.2f)" % self.L
    """
    A model to determine path loss for a transmitted signal.
    :param L: constant factor to reduce transmit power by
    (default is 1 = no loss)
    """

    def received_power(self, tx_power, freq, tx_pos_gps, rx_pos_gps):
        """
        Return received power (W) of a transmission
        :param tx_power: transmitted power (W)
        :param freq: frequency of transmission (Hz)
        :param tx_pos_gps: current position of transmitter [x, y, z] (m)
        :param rx_pos_gps: current position of receiver [x, y, z] (m)
        """
        return tx_power/self.L

    def inverse(self, rx_sensitivity, tx_power, freq, h_t=100, h_r=100):
        """
        Return max distance (m) to receive rx_sensitivity power (W)
        :param rx_sensitivity: min received power threshold for reception (W)
        :param tx_power: transmitted power (W)
        :param freq: frequency of transmission (Hz)
        :param h_t: height of transmitter above ground (m)
        :param h_r: height of receiver above ground (m)
        """
        return np.inf

    def plot(self, rx_sensitivity, tx_power, freq, h_t=100, h_r=100,
             save=False, show=False):
        """
        Plot received power versus increasing separation range
        :param rx_sensitivity: min received power threshold for reception (W)
        :param tx_power: transmitted power (W)
        :param freq: frequency of transmission (Hz)
        :param h_t: height of transmitter above ground (m)
        :param h_r: height of receiver above ground (m)
        :param save: save the plot (bool)
        :param show: show the plot (bool)
        """
        from matplotlib import pyplot as plt
        tx_pos = [0, 0, h_t]
        CR = self.inverse(rx_sensitivity, tx_power, freq, h_t, h_r)
        xmax = min(2*CR, 100000)
        xs = np.linspace(0, xmax, 10000)
        rx_pos = [util.gps_offset(tx_pos, 0, x) + (h_r,) for x in xs]
        ys = [self.received_power(tx_power, freq, tx_pos, rx_pos_i)
              for rx_pos_i in rx_pos]
        plt.plot(xs, ys, label=self.model_name)
        plt.xlabel("Separation Distance (m)")
        plt.ylabel("Received Power (W)")
        plt.xlim((0, xmax))
        rx_pos = util.gps_offset(tx_pos, 0, xmax/2) + (h_r,)
        rx_power_at_CR = self.received_power(tx_power, freq, tx_pos, rx_pos)
        plt.ylim((0, 3*rx_power_at_CR))
        plt.legend(loc=3)
        plt.grid()
        if save:
            plt.savefig(self.model_name.replace(" ", ""))
        if show:
            plt.show()


class NoLossPropagation(PropagationModel):
    def __init__(self):
        super().__init__(L=1)
        self.model_name = "Lossless Propagation"
    """
    An completely unrealistic and simplistic model for path loss that assumes
    all transmitted power reaches any receiver antenna (UNREALISTIC)
    """


class FreeSpacePropagation(PropagationModel):
    def __init__(self, L=1):
        super().__init__(L=L)
        self.model_name = "Free Space Propagation"
    """
    A model to determine path loss for a transmitted signal using Free Space
    Path Loss equation. Assumes free, open space and isotropic antennae.
    :param L: constant factor to reduce transmit power by
    (default is 1 = no loss)
    """

    def received_power(self, tx_power, freq, tx_pos_gps, rx_pos_gps):
        w = 3e8/freq                # Wavelength in meters
        C = w**2/((4*np.pi)**2*self.L)
        d = util.distance(tx_pos_gps, rx_pos_gps)
        if d < w:
            return tx_power
        return tx_power*C/(d**2)

    def inverse(self, rx_sensitivity, tx_power, freq, h_t=100, h_r=100):
        if rx_sensitivity == 0:
            return np.inf
        w = 3e8/freq                # Wavelength in meters
        C = w**2/((4*np.pi)**2*self.L)
        d = np.sqrt(tx_power*C/rx_sensitivity)
        return d


class TwoRayGroundPropagation(PropagationModel):
    def __init__(self, L=1):
        super().__init__(L=L)
        self.model_name = "Two Ray Ground Propagation"
    """
    A model to determine path loss for a transmitted signal using Two Ray
    Ground equation. Assumes free, open space and isotropic antennae. Accounts
    for ground effects - uses antenna heights as inputs.
    :param L: constant factor to reduce transmit power by
    (default is 1 = no loss)
    """

    def received_power(self, tx_power, freq, tx_pos_gps, rx_pos_gps):
        w = 3e8/freq                # Wavelength in meters
        C = w**2/((4*np.pi)**2*self.L)
        d = util.distance(tx_pos_gps, rx_pos_gps)
        if d < w:
            return tx_power
        h_t = tx_pos_gps[2]
        h_r = rx_pos_gps[2]
        return tx_power*C*h_t**2*h_r**2/(d**4)

    def inverse(self, rx_sensitivity, tx_power, freq, h_t=100, h_r=100):
        if rx_sensitivity == 0:
            return np.inf
        w = 3e8/freq                # Wavelength in meters
        C = w**2/((4*np.pi)**2*self.L)
        d = (tx_power*C*h_t**2*h_r**2/rx_sensitivity)**(1/4)
        return d
