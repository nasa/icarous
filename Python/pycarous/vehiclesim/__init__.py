from vehiclesim.VehicleSimInterface import VehicleSimInterface
from vehiclesim.trafficreplay import TrafficReplay
from vehiclesim.AccordReplay import AccordReplay
availableSimTypes = []
try:
    from vehiclesim.quadsim import QuadSim
    availableSimTypes.append('UAS_ROTOR')
except:
    pass

try:
    from vehiclesim.spqsim import SixPassengerQuadSim
    availableSimTypes.append('UAM_SPQ')
except:
    pass

from vehiclesim.uamsim import UamVtolSim
availableSimTypes.append('UAM_VTOL')
