from matplotlib import pyplot as plt

from SimEnvironment import SimEnvironment
from Icarous import *


# Initialize simulation environment
sim = SimEnvironment()

# Set the home position for the simulation
HomePos = [37.55290000,-122.27250000,0.000000]

# Start a single traffic vehicle from Home at specified
# range,brg,alt,track,speed,climb rate
# Call this function again to start multiple traffic vehicles
#StartTraffic(1,HomePos,15000,309,5,50,127,0,tfList)
sim.AddTraffic(1,HomePos,26000,309,1000,50,127,0)

# Initialize Icarous class
ic = Icarous(HomePos, simtype = 'UAM_VTOL')

# Read params from file and input params
params = LoadIcarousParams('data/icarous_default2.parm')
ic.daafile = "data/DaidalusQuadConfig.txt"
ic.SetParameters(params)

# Input flightplan
flightplan = [[37.55290000,-122.27250000,0.00000000, 0],
              [37.55569196,-122.27680163,403.34964813, 10],
              [37.56363069,-122.28903606,787.79770992, 10],
              [37.56790032,-122.29561791,989.20925446, 10],
              [37.56812906,-122.29597056,997.30231350, 50],
              [37.56836248,-122.29633044,999.99999956, 50] ,
              [37.57193913,-122.30184516,1000.00000000, 50],
              [37.63472832,-122.39880873,1000.00000000, 50],
              [37.63509610,-122.39937753,997.04321204, 50],
              [37.63545875,-122.39993840,988.17284609, 50],
              [37.63890105,-122.40526271,851.70136857, 50]]
ic.InputFlightplan(flightplan,0)

sim.AddIcarousInstance(ic)
sim.SetPosUncertainty(0.01, 0.01, 0, 0, 0, 0)

# Run the simulation
sim.RunSimulation()
sim.WriteLog()

# Plot data for visualization    
#plt.figure(1)
#plt.plot(np.array(ic.positionLog)[:,0],np.array(ic.positionLog)[:,1],'r')
#plt.plot(np.array(ic.localPlans[0])[:,1],np.array(ic.localPlans[0])[:,0],'g--')
#plt.scatter(np.array(ic.localPlans[0])[:,1],np.array(ic.localPlans[0])[:,0])
#for tf in tfList:
    #plt.plot(np.array(tf.log['pos'])[:,0],np.array(tf.log['pos'])[:,1],'b')
#plt.figure(2)
#plt.plot([i for i in range(len(ic.positionLog))],np.array(ic.positionLog)[:,2])
#plt.show()

VisualizeSimData(sim.icInstances, allplans=False, xmin=-30, ymin=-30, xmax=100, ymax=100, interval=5, record=False, filename="anim.mp4")
