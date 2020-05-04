from Icarous import *
from matplotlib import pyplot as plt
from Animation import AgentAnimation

# Set the home position for the simulation
HomePos = [37.55290000,-122.27250000,0.000000]

# List to hold simulated traffic vehicles
tfList = []

# Start a single traffic vehicle from Home at specified
# range,brg,alt,track,speed,climb rate
# Call this function again to start multiple traffic vehicles
#StartTraffic(HomePos,15000,309,5,50,127,0,tfList)
StartTraffic(HomePos,14000,309,100,50,127,0,tfList)

# Set uncertainty and smoothing params for traffic
#for tf in tfList:
    #tf.setpos_uncertainty(0.01,0.01,0,0,0,0)

# Initialize Icarous class
ic = Icarous(HomePos,simtype = 'UAM_VTOL')

# Setup uncertainty and smoothing parameters
#ic.setpos_uncertainty(0.01,0.01,0,0,0,0)

# Read params from file and input params
params = LoadIcarousParams('icarous_default2.parm')
ic.daafile = "DaidalusQuadConfig2.txt"
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
icInstances = []
icInstances.append(ic)


RunSimulation(icInstances,tfList)

# Plot data for visualization    
plt.figure(1)
plt.plot(np.array(ic.positionLog)[:,0],np.array(ic.positionLog)[:,1],'r')
plt.plot(np.array(ic.localPlans[0])[:,1],np.array(ic.localPlans[0])[:,0],'g--')
plt.scatter(np.array(ic.localPlans[0])[:,1],np.array(ic.localPlans[0])[:,0])
for tf in tfList:
    plt.plot(np.array(tf.log['pos'])[:,0],np.array(tf.log['pos'])[:,1],'b')
plt.show()


anim= AgentAnimation(-15000,-2000, 100,15000,5)
for pln in ic.localPlans:
    anim.AddPath(np.array(pln),'--')
anim.AddAgent('ownship',100,'r',ic.ownshipLog,show_circle=True,circle_rad = 1222)
anim.AddAgent('traffic0',100,'b',ic.trafficLog[0])

anim.run()
