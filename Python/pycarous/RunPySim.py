#!/usr/bin/env python3

from SimEnvironment import SimEnvironment
from Icarous import Icarous
from ichelper import GetHomePosition,ReadTrafficInput
import argparse
import os

def checkDAAType(value):
    if value.upper() not in ['DAIDALUS','ACAS']:
        raise argparse.ArgumentTypeError("%s is an invalid DAA option" % value)
    return value.upper()

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
parser.add_argument("-l", "--tlimit", type=float, default=300,
                   help='set max sim time limit (in seconds). default 300 s')
parser.add_argument("-p", "--params", type=str, default='data/IcarousConfig.txt',
                   help='icarous parameter file. default: data/IcarousConfig.txt')
parser.add_argument("-g", "--geofence", type=str, default='',
                   help='geofence xml input. example: data/geofence2.xml')
parser.add_argument("-c", "--daaConfig", type=str, default='data/IcarousConfig.txt',
                   help='specify configuration file if one is required by the DAA module specified by -d/--daaType')
parser.add_argument("-v", "--verbosity", type=int, choices=[0,1,2], default=1,
                   help='Set print verbosity level')
parser.add_argument("--realtime", dest="fasttime", action="store_false",
                   help='Run sim in real time')
parser.add_argument("--simtype", choices=["UAM_VTOL","UAM_SPQ","UAS_ROTOR"], default="UAM_VTOL",
                   help='select vehicle simulator')
parser.add_argument("--fasttime", dest="fasttime", action="store_true",
                   help='Run sim in fast time (not available for cFS simulations)')
parser.add_argument("--cfs", action="store_true",
                   help='Run Icarous using cFS instead of pycarous')
parser.add_argument("-u", "--uncertainty", type=bool, default=False,
                   help='Enable uncertainty')
parser.add_argument("-r", "--repair", action="store_true",
                   help='Convert the given flightplan into a EUTL plan')
parser.add_argument("-e", "--eta", action="store_true",
                   help='Enable eta control for waypoint arrivals')
parser.add_argument("-w", "--wind", nargs=2, type=float, metavar=("SOURCE", "SPEED"), default=[0.0, 0.0],
                   help='Simulate constant wind: SOURCE (deg, 0=North), SPEED (m/s)')
parser.add_argument("--daalog",  action="store_true",
                   help='Enable daa logs')
args = parser.parse_args()
if args.cfs:
    args.fasttime = False

# Create log folder for module logs
try:
    os.mkdir(os.path.join(os.getcwd(),'log'))
except FileExistsError:
    pass
except:
    raise

# Initialize simulation environment
sim = SimEnvironment(fasttime=args.fasttime,verbose=args.verbosity)
sim.AddWind([args.wind])

# Set the home position for the simulation
HomePos = GetHomePosition(args.flightplan)

# Add traffic inputs
if args.traffic != '':
    tfinputs = ReadTrafficInput(args.traffic)
    for tf in tfinputs:
        sim.AddTraffic(tf[0], HomePos, *tf[1:-2],delay=tf[-2],transmitter=tf[-1])

# Initialize Icarous class
if args.cfs:
    from IcarousRunner import IcarousRunner
    ic = IcarousRunner(HomePos, verbose=args.verbosity)
else:
    ic = Icarous(HomePos,simtype=args.simtype,monitor=args.daaType,verbose=args.verbosity,
                 daaConfig=args.daaConfig, fasttime=args.fasttime)

if args.daalog:
    # Dirty hack to silently update the daa logging parameter from commandline
    os.system("sed -Ein -e \'s/(LOGDAADATA)(\\ *)([0-1])(\\.0*)/\\1\\21\\4/\' "+args.params)

# Read params from file and input params
ic.SetParametersFromFile(args.params)

# Input flightplan
ic.InputFlightplanFromFile(args.flightplan,eta=args.eta,repair=args.repair)

# Input geofences from file
if args.geofence != '':
    ic.InputGeofence(args.geofence)

# Add icarous instance to sim environment
sim.AddIcarousInstance(ic,time_limit=args.tlimit)

#from GroundSystem import GroundPlanner
# Add ground planner instance
#sim.AddGroundSystem(GroundPlanner(HomePos,2,"GSPlanner",filename='data/flightplan_gs.txt'))


# Set position uncertainty for vehicles in the simulation
if args.uncertainty:
    sim.SetPosUncertainty(0.1, 0.1, 0, 0, 0, 0)

# Run the Simulation
sim.RunSimulation()

# Save json log outputs
sim.WriteLog()
