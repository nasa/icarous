from SimEnvironment import SimEnvironment
from Icarous import *
from ichelper import GetHomePosition,ReadTrafficInput
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-d", "--daaType", type=checkDAAType, default='DAIDALUS')
parser.add_argument("-f", "--flightplan", type=str, default='data/flightplan.txt')
parser.add_argument("-t", "--traffic", type=str, default='data/traffic.txt')
parser.add_argument("-p", "--params", type=str, default='data/icarous_default.parm')
parser.add_argument("-g", "--geofence", type=str, default='')
parser.add_argument("-c", "--daaConfig", type=str, default='data/DaidalusQuadConfig.txt')
parser.add_argument("-v", "--verbosity", type=int, choices=[1,2], default=1)
parser.add_argument("-u", "--uncertainty", type=bool, default=False)
args = parser.parse_args()

# Initialize simulation environment
sim = SimEnvironment()

# Set the home position for the simulation
HomePos = GetHomePosition(args.flightplan)

# Add traffic inputs
tfinputs = ReadTrafficInput(args.traffic)
for tf in tfinputs:
    sim.AddTraffic(tf[0], HomePos, *tf[1:])

# Initialize Icarous class
ic = Icarous(HomePos,simtype="UAM_VTOL",monitor=args.daaType,verbose=args.verbosity,daaConfig=args.daaConfig)

# Read params from file and input params
ic.SetParametersFromFile(args.params)

# Input flightplan
ic.InputFlightplanFromFile(args.flightplan)

# Input geofences from file
if args.geofence != '':
    ic.InputGeofence("data/geofence2.xml")

# Add icarous instance to sim environment
sim.AddIcarousInstance(ic)

# Set position uncertainty for vehicles in the simulation
if args.uncertainty:
    sim.SetPosUncertainty(0.1, 0.1, 0, 0, 0, 0)

# Run the Simulation
sim.RunSimulation()

# Save json log outputs
sim.WriteLog()