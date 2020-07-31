from SimEnvironment import SimEnvironment
from Icarous import *
from ichelper import GetHomePosition,ReadTrafficInput
import argparse

parser = argparse.ArgumentParser(description=\
" Run a fast time simulation of Icarous with the provided inputs.\n\
  - See available input flags below.\n\
  - Icarous output are written to log file: SPEEDBIRD.json\n\
  - Use VisualizeLog.py to animate/record simoutput.\n\
  - See VisualizeLog.py --help for more information.",
                    formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument("-d", "--daaType", type=checkDAAType, default='DAIDALUS', 
                   help='Specify DAA modules to be used. DAIDALUS,ACAS,...')
parser.add_argument("-f", "--flightplan", type=str, default='data/flightplan.txt', 
                   help='flightplan file. default: data/flightplan.txt')
parser.add_argument("-t", "--traffic", type=str, default='',
                   help='File containing traffic initial condition. See data/traffic.txt for example')
parser.add_argument("-p", "--params", type=str, default='data/icarous_default.parm',
                   help='icarous parameter file. default: data/icarous_default.parm')
parser.add_argument("-g", "--geofence", type=str, default='',
                   help='geofence xml input. example: data/geofence2.xml')
parser.add_argument("-c", "--daaConfig", type=str, default='data/DaidalusQuadConfig.txt',
                   help='specify configuration file if one is required by the DAA module specified by -d/--daaType')
parser.add_argument("-v", "--verbosity", type=int, choices=[1,2], default=1,
                   help='Set print verbosity level')
parser.add_argument("-u", "--uncertainty", type=bool, default=False,
                   help='Enable uncertainty')
args = parser.parse_args()

# Initialize simulation environment
sim = SimEnvironment()

# Set the home position for the simulation
HomePos = GetHomePosition(args.flightplan)

# Add traffic inputs
if args.traffic != '':
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