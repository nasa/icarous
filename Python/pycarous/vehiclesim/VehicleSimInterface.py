import abc
import numpy as np

from ichelper import gps_offset

class VehicleSimInterface(abc.ABC):
    """
    An abstract base class for simulating vehicle dynamics.
    Subclasses must implement all of the functions marked @abc.abstractmethod.
    """
    def __init__(self, idx, home_gps, dt=0.05):
        """
        Initialize vehicle sim interface
        :param idx: unique id of this vehicle
        :param home_gps: home gps position [lat (deg), lon (deg), alt (m)]
        :param dt: timestep for each simulation step (s)
        """
        # Vehicle data
        self.vehicleID = idx
        self.home_gps = home_gps
        self.dt = dt

        # Uncertainty settings
        self.noise = False
        self.coeff = 0.8
        self.sigma_pos = 0

        # Current wind speed
        self.vw = np.array([0, 0, 0])

    def SetInitialConditions(self,x=0,y=0,z=0,vx=0,vy=0,vz=0):
        """
        Set initial conditions
        :param x: x position [m] East
        :param y: y position [m] North
        :param z: z position [m] Up
        :param vx: velocity [m/s] east component
        :param vy: velocity [m/s] north component
        :param vz: velocity [m/s] down component
        """
        pass

    def SetPosUncertainty(self, xx, yy, zz, xy, yz, xz, coeff=0.8):
        """
        Set position uncertainty
        :param xx: x position variance [m^2] (East/West)
        :param yy: y position variance [m^2] (North/South)
        :param zz: z position variance [m^2] (Up/Down)
        :param xy: xy position covariance [m^2]
        :param yz: yz position covariance [m^2]
        :param xz: xz position covariance [m^2]
        :param coeff: smoothing factor used for uncertainty (default=0.8)
        """
        self.noise = True
        self.coeff = coeff
        self.sigma_pos = np.array([[xx, xy, xz],
                                   [xy, yy, yz],
                                   [xz, yz, zz]])

    @abc.abstractmethod
    def InputCommand(self, track, gs, climbrate):
        """
        Input control commands to the simulated vehicle
        :param track: desired ground track (degrees CW from North)
        :param gs: desired ground speed (m/s)
        :param climbrate: desired climb rate (m/s)
        """
        pass

    @abc.abstractmethod
    def Run(self, windFrom=0, windSpeed=0):
        """
        Run one timestep of the vehicle model simulation and update
        the current position
        :param windFrom: current wind source (degrees CW from North)
        :param windSpeed: current wind speed (m/s)
        """
        pass

    @abc.abstractmethod
    def GetOutputPositionNED(self):
        """
        Return the current position in NED coordinates relative to home_gps
        :return: [pos_n (m), pos_e (m), pos_d (m)]
        """
        pass

    def GetOutputPositionLLA(self):
        """
        Return the current position in GPS coordinates
        :return: [lat (deg), lon (deg), alt (m)]
        """
        pos_ned = self.GetOutputPositionNED()
        pos_gps = gps_offset(self.home_gps[0], self.home_gps[1],
                             pos_ned[1], pos_ned[0])
        return pos_gps + (pos_ned[2],)

    @abc.abstractmethod
    def GetOutputVelocityNED(self):
        """
        Return the current NED velocity in m/s
        :return: [vn (m/s), vn (m/s), vn (m/s)]
        """
        pass
