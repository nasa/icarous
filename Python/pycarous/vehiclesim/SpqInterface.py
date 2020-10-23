from ctypes import c_char, c_int, c_double, c_bool, Structure
from collections import namedtuple


# Define data types for input to C++ function
datafields = ['distance','VTAS','Altitude','ROCD','Psi','Phi','N_pos','E_pos']
SpqState = namedtuple('SpqState',field_names=datafields)

class rtString(Structure):
    _fields_ = [
        ("data",c_char*10),
        ("size",c_int*2),
    ]

class SpqCommand(Structure):
    _fields_ =[
        ("VTAS",c_double),
        ("Alt",c_double),
        ("ROCD",c_double),
        ("Psi",c_double),
        ("GS",c_double),
        ("GT",c_double),
        ("VelocityControlMode",rtString),
        ("HeadingControlMode",rtString),
        ("VerticalControlMode",rtString),
    ]

class SpqGains(Structure):
    _fields_ =[
        ("k_VTAS",c_double),
        ("gLimit_dVTAS",c_double),
        ("k_ROCD",c_double),
        ("ROCD_max",c_double),
        ("gLimit_dROCD",c_double),
        ("k_Alt",c_double),
        ("k_deltaPsi",c_double),
        ("dPsi_desired_max",c_double),
        ("Phi_desired_max",c_double),
        ("k_Phi",c_double),
        ("dPhi_max",c_double),
        ("P_MAX",c_double),
        ("usePowerLimitEnforcement",c_bool),
    ]
