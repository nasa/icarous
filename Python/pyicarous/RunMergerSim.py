from matplotlib import pyplot as plt

from SimEnvironment import SimEnvironment
from Icarous import *


# Initialize simulation environment
sim = SimEnvironment()
sim.InputMergeFixes('../TestRunner/tcl4+/merging_scenario/merge_fixes.txt')

# Initialize Icarous class
HomePos1 = [37.415756, -122.056634, 0]
HomePos2 = [37.416407, -122.056657, 0]
ic1 = Icarous(HomePos1, simtype="UAM_VTOL", vehicleID=0, verbose=1, callsign="sc0")
ic2 = Icarous(HomePos2, simtype="UAM_VTOL", vehicleID=1, verbose=1, callsign="sc1")

# Read params from file and input params
params = LoadIcarousParams('../TestRunner/tcl4+/merging_scenario/merging_default.parm')
ic1.SetParameters(params)
ic2.SetParameters(params)

# Input flight plans
ic1.InputFlightplanFromFile('../TestRunner/tcl4+/merging_scenario/vehicle0_waypoints.txt')
ic2.InputFlightplanFromFile('../TestRunner/tcl4+/merging_scenario/vehicle1_waypoints.txt')

sim.AddIcarousInstance(ic1, delay=5, time_limit=100)
sim.AddIcarousInstance(ic2, delay=5, time_limit=100)
sim.SetPosUncertainty(0.01, 0.01, 0, 0, 0, 0)

# Run the simulation
sim.RunSimulation()
sim.WriteLog()

# Plot data for visualization
#plt.figure(1)
#for ic in sim.icInstances:
    #plt.plot(np.array(ic.ownshipLog["positionNED"])[:, 1],
             #np.array(ic.ownshipLog["positionNED"])[:, 0],
             #label=ic.vehicleID)
#plt.legend()
#plt.savefig("merging_example.png")

VisualizeSimData(sim.icInstances, allplans=False, xmin=-30, ymin=-30, xmax=100, ymax=100, interval=5, record=False, filename="anim.mp4")
