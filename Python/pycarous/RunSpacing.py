from matplotlib import pyplot as plt

from SimEnvironment import SimEnvironment
from Icarous import *


# Initialize simulation environment
sim = SimEnvironment()

# Initialize Icarous class
HomePos1 = [37.102177, -76.387207, 0.000000]
HomePos2 = [37.102177, -76.387937, 0.000000]
ic1 = Icarous(HomePos1, simtype="UAM_VTOL", vehicleID=0, verbose=1, callsign="sc0")
ic2 = Icarous(HomePos2, simtype="UAM_VTOL", vehicleID=1, verbose=1, callsign="sc1")

# Read params from file and input params
params = LoadIcarousParams('data/spacing_default.parm')
ic1.SetParameters(params)
ic2.SetParameters(params)

# Input flight plans
flightplan1 = [ [37.102177, -76.387207, 5.000000, 5.0],
               [37.102177, -76.287207, 5.000000, 5.0]]
flightplan2 = [ [37.102177, -76.387207, 5.000000, 10.0],
               [37.102177, -76.287207, 5.000000, 10.0]]
ic1.InputFlightplan(flightplan1, 0)
ic2.InputFlightplan(flightplan2, 0)

sim.AddIcarousInstance(ic1, delay=5, time_limit=100)
sim.AddIcarousInstance(ic2, delay=5, time_limit=100)
sim.SetPosUncertainty(0.01, 0.01, 0, 0, 0, 0)

# Run the simulation
sim.RunSimulation()
sim.WriteLog()

# Plot data for visualization
#getSpeed = lambda vned: np.sqrt(vned[0]**2 + vned[1]**2 + vned[2]**2)
#speedic1 = [getSpeed(x) for x in ic1.ownshipLog['velocityNED']]
#speedic2 = [getSpeed(x) for x in ic2.ownshipLog['velocityNED']]
#plt.figure(1)
#plt.plot(ic1.ownshipLog['t'], speedic1, 'r', label='ownship1')
#plt.plot(ic2.ownshipLog['t'], speedic2, 'b', label='ownship2')
#plt.xlabel('t (s)')
#plt.ylabel('speed m/s')
#plt.savefig("spacing_example.png")

VisualizeSimData(sim.icInstances, allplans=False, xmin=-30, ymin=-30, xmax=100, ymax=100, interval=5, record=False, filename="anim.mp4")
