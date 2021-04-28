import abc
import numpy as np

from icutils.ichelper import gps_offset
from CustomTypes import V2Vdata

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

        self.transmitter = None

        # Uncertainty settings
        self.noise = False
        self.pcoeff = 0
        self.vcoeff = 0
        self.sigma_pos = np.zeros((3,3)) 
        self.sigma_vel = np.zeros((3,3)) 
        self.delay = 0
        self.lastBroadcastTime = 0.0
        self.broadcastInterval = 1.0

        # Current wind speed
        self.vw = np.array([0, 0, 0])

    def SetInitialConditions(self,x=0,y=0,z=0,heading=0,speed=0,vs=0):
        """
        Set initial conditions
        :param x: x position [m] East
        :param y: y position [m] North
        :param z: z position [m] Up
        :param heading: heading [degree]
        :param speed: ground speed [m/s] 
        :param vs: vertical speed [m/s]
        """
        pass

    def SetPosUncertainty(self, xx, yy, zz, xy, xz, yz, coeff=0.8):
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
        self.pcoeff = coeff
        self.sigma_pos = np.array([[xx, xy, xz],
                                   [xy, yy, yz],
                                   [xz, yz, zz]])

    def SetVelUncertainty(self, xx, yy, zz, xy, xz, yz, coeff=0.8):
        """
        Set velocity uncertainty
        :param xx: x velocity variance [m^2] (East/West)
        :param yy: y velocity variance [m^2] (North/South)
        :param zz: z velocity variance [m^2] (Up/Down)
        :param xy: xy velocity covariance [m^2]
        :param yz: yz velocity covariance [m^2]
        :param xz: xz velocity covariance [m^2]
        :param coeff: smoothing factor used for uncertainty (default=0.8)
        """
        self.noise = True
        self.vcoeff = coeff
        self.sigma_vel = np.array([[xx, xy, xz],
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

    def GetCovariances(self):
        """
        Return the 6 covariance elements
        """
        sigmaPos = [self.sigma_pos[0][0],
                    self.sigma_pos[1][1],
                    self.sigma_pos[2][2],
                    self.sigma_pos[0][1],
                    self.sigma_pos[0][2],
                    self.sigma_pos[1][2]]

        sigmaVel = [self.sigma_vel[0][0],
                    self.sigma_vel[1][1],
                    self.sigma_vel[2][2],
                    self.sigma_vel[0][1],
                    self.sigma_vel[0][2],
                    self.sigma_vel[1][2]]
        return sigmaPos,sigmaVel

    def TransmitPosition(self, current_time):
        """ Transmit current position """
        if self.transmitter is None:
            return
        gps_position = self.GetOutputPositionLLA()
        velocity = self.GetOutputVelocityNED()
        sigmaP,sigmaV = self.GetCovariances()
        msg_data = {
            "source": self.transmitter.sensorType,
            "callsign": "tf" + str(self.vehicleID),
            "pos": gps_position,
            "vel": velocity,
            "sigmaP": sigmaP,
            "sigmaV": sigmaV
        }
        msg = V2Vdata("INTRUDER", msg_data)
        if current_time - self.lastBroadcastTime > self.broadcastInterval:
            self.lastBroadcastTime = current_time
            self.transmitter.transmit(current_time, gps_position, msg)
