from SimEnvironment import SimEnvironment
from Icarous import *

from matplotlib import pyplot as plt
import argparse

def checkDAAType(value):
    if value.upper() not in ['DAIDALUS','ACAS']:
        raise argparse.ArgumentTypeError("%s is an invalid DAA option" % value)
    return value.upper()

parser = argparse.ArgumentParser()
parser.add_argument("-t", "--daaType", type=checkDAAType, default='DAIDALUS')
parser.add_argument("-f", "--daaConfig", type=str, default='data/DaidalusQuadConfig.txt')
parser.add_argument("-x", "--daaConfigExtra", type=str, default=None, help="append this parameter string to the configuration file contents")
parser.add_argument("-v", "--verbosity", type=int, choices=[1,2], default=1)
parser.add_argument("-u", "--uncertainty", type=bool, default=False)
args = parser.parse_args()

# Initialize simulation environment
sim = SimEnvironment()
sim.AddWind([(90, 0.1)])    # (wind from, speed)

# Set the home position for the simulation
HomePos = [37.102177, -76.387207, 0.000000]

# Start a single traffic vehicle from Home at specified
# range, brg, alt, track, speed, climb rate
# Call this function again to start multiple traffic vehicles
sim.AddTraffic(1, HomePos, 100, 80, 5, 1, 270, 0)

# Initialize Icarous class
ic = Icarous(HomePos,simtype="UAM_VTOL",monitor=args.daaType,verbose=args.verbosity,daaConfig=args.daaConfig)

# Read params from file and input params
params = LoadIcarousParams('data/icarous_default.parm')
ic.SetParameters(params)
# Input flightplan
flightplan = [ [37.102177, -76.387207, 5.000000, 0.0],
               [37.102335, -76.387195, 5.000000, 1.0],
               [37.102335, -76.386206, 5.000000, 1.0],
               [37.102110, -76.386481, 5.000000, 1.0],
               [37.101796, -76.386859, 5.000000, 1.0],
               [37.102177, -76.387207, 5.000000, 1.0]]
ic.InputFlightplan(flightplan, 0)
# Input geofences from file
#ic.InputGeofence("data/geofence2.xml")
sim.AddIcarousInstance(ic)

# Set position uncertainty for vehicles in the simulation
if args.uncertainty:
    sim.SetPosUncertainty(0.1, 0.1, 0, 0, 0, 0)

# Run the Simulation
sim.RunSimulation()
sim.WriteLog()

# Plot data for visualization
#plt.figure(1)
#for ic in sim.icInstances:
    #plt.plot(np.array(ic.ownshipLog["positionNED"])[:, 1],
             #np.array(ic.ownshipLog["positionNED"])[:, 0],
             #label=ic.vehicleID)
#for tf in sim.tfList:
    #plt.plot(np.array(tf.log['pos'])[:, 0],
             #np.array(tf.log['pos'])[:, 1],
             #label=tf.vehicleID)
#plt.legend()
#plt.savefig("sim_example.png")

VisualizeSimData(sim.icInstances, allplans=False, xmin=-30, ymin=-30, xmax=100, ymax=100, interval=5, record=False, filename="anim.mp4")
