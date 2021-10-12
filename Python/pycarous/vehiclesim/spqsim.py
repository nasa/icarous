from ctypes import *
import numpy as np
import os

from icutils.ichelper import GetWindComponent
from vehiclesim import VehicleSimInterface
from vehiclesim.SpqInterface import SpqState, SpqGains, SpqCommand, rtString

# Initialize c library (libspq.so must be on the path)
icmodules = os.path.join(os.environ['ICAROUS_HOME'],'Modules','lib')
libspq = CDLL(os.path.join(icmodules,"libspq.so"),winmode=0)
libspq.advance_sim.argtypes = [POINTER(SpqGains), c_double, c_double*8,
                               POINTER(SpqCommand), c_double, c_double*2]

class SixPassengerQuadSim(VehicleSimInterface):
    """ Simulation model for a Six-passenger Quadrotor UAM vehicle """
    def __init__(self, idx, home_gps, dt=0.05):
        """
        Initialize a Six-passenger Quadrotor UAM simulation.
        Input arguments are defined in VehicleSimInterface.
        The vehicle starts at the given home_gps location.
        """
        super().__init__(idx, home_gps, dt)
        self.massLevel = 2

        # Define controller gains
        self.gains = SpqGains()
        self.gains.k_VTAS           = 1.5         # airspeed gain (1/s)
        self.gains.gLimit_dVTAS     = 0.5         # max horizontal accel (g's)
        self.gains.k_ROCD           = 2*1.25*0.4  # climbrate gain (2*zeta*wn)
        self.gains.ROCD_max         = 2000        # max climbrate (fpm)
        self.gains.gLimit_dROCD     = 0.15        # max vertical accel (g's)
        self.gains.k_Alt            = 0.4**2*60   # altitude gain (wn^2)
        self.gains.k_deltaPsi       = 0.3         # turn rate gain (1/s)
        self.gains.dPsi_desired_max = 10          # max turn rate command (deg/s)
        self.gains.Phi_desired_max  = 60          # max bank angle (deg)
        self.gains.k_Phi            = 2.0         # bank angle feedback gain
        self.gains.dPhi_max         = 30          # max roll rate (deg/s)
        self.gains.P_MAX            = 2079        # max battery draw (MJ/hr)
        self.gains.usePowerLimitEnforcement = True  # limit accel based on P_MAX

        # Define input command
        self.command = SpqCommand()
        # Velocity contol mode: "GS" or "VTAS"
        self.command.VelocityControlMode = rtString(b"GS", (c_int*2)(1,2))
        self.command.VTAS = 0   # commanded airspeed (kts)
        self.command.GS   = 0   # commanded groundspeed (kts)
        # Vertical contol mode: "Alt" or "ROCD"
        self.command.VerticalControlMode = rtString(b"ROCD", (c_int*2)(1,4))
        self.command.Alt  = 0   # commanded altitude (ft)
        self.command.ROCD = 0   # commanded climb rate (fpm)
        # Vertical contol mode: "Heading" or "GT"
        self.command.HeadingControlMode  = rtString(b"GT", (c_int*2)(1,2))
        self.command.Psi  = 0   # commanded heading (deg CW from North)
        self.command.GT   = 0   # commanded ground track (deg CW from North)

        # Define initial vehicle state
        self.state = SpqState(
            distance = 0,   # horizontal distance from starting point (nm)
            VTAS = 0,       # airspeed (kts)
            Altitude = 0,   # altitude (ft)
            ROCD = 0,       # climb rate (fpm)
            Psi = 0,        # heading (deg CW from North)
            Phi = 0,        # bank angle (deg)
            N_pos = 0,      # distance North from starting point (nm)
            E_pos = 0,      # distance East from starting point (nm)
        )
        self.groundtrack = 0    # current ground track (deg CW from North)

        self.wind_mps = [0, 0]  # wind speed [vn, ve] (m/s)

    def SetInitialConditions(self,x=0,y=0,z=0,heading=0,speed=0,vs=0):
        self.pos0 = np.array([x,y,z])
        self.trk,self.gs,self.vs = heading,speed,vs
        self.state = SpqState(
            distance = 0,   # horizontal distance from starting point (nm)
            VTAS = speed * 1.94,       # airspeed (kts)
            Altitude = z*3.28084,   # altitude (ft)
            ROCD = vs*196.95,       # climb rate (fpm)
            Psi = heading,        # heading (deg CW from North)
            Phi = 0,        # bank angle (deg)
            N_pos = y*0.000539957,      # distance North from starting point (nm)
            E_pos = x*0.000539957,      # distance East from starting point (nm)
        )

    def InputCommand(self, track, gs, climbrate):
        self.command.VelocityControlMode = rtString(b"GS", (c_int*2)(1,2))
        self.command.VTAS = 0
        self.command.GS   = gs*1.94384449   # m/s to kts
        self.command.VerticalControlMode = rtString(b"ROCD", (c_int*2)(1,4))
        self.command.Alt  = 0
        self.command.ROCD = climbrate*196.8503937  # m/s to fpm
        self.command.HeadingControlMode  = rtString(b"GT", (c_int*2)(1,2))
        self.command.Psi  = 0
        self.command.GT   = track

    def Run(self, windFrom=0, windSpeed=0):
        self.wind_mps = GetWindComponent(windFrom, windSpeed, NED=True)[0:2]
        wind_kts = self.wind_mps*1.94384449 # m/s to kts
        last_pos = [self.state.N_pos, self.state.E_pos]

        x = (c_double*8)(*self.state)
        libspq.advance_sim(byref(self.gains),
                           self.dt,
                           x,
                           byref(self.command),
                           self.massLevel,
                           (c_double*2)(*wind_kts))
        self.state = SpqState(*x)

        # Compute ground track
        dN = self.state.N_pos - last_pos[0]
        dE = self.state.E_pos - last_pos[1]
        self.groundtrack = np.degrees(np.arctan2(dE, dN)) % 360
        return True

    def GetOutputPositionNED(self):
        pos_n = self.state.N_pos*1852       # nm to m
        pos_e = self.state.E_pos*1852       # nm to m
        alt   = self.state.Altitude*0.3048  # ft to m
        return (pos_n, pos_e, -alt)

    def GetOutputVelocityNED(self):
        airspeed = self.state.VTAS*0.5144   # kts to m/s
        vn = airspeed*np.cos(np.radians(self.groundtrack)) + self.wind_mps[0]
        ve = airspeed*np.sin(np.radians(self.groundtrack)) + self.wind_mps[1]
        vd = -self.state.ROCD*0.00508  # fpm to m/s
        return (vn, ve, vd)
