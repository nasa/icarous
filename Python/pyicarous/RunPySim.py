from Icarous import *
from matplotlib import pyplot as plt

# Set the home position for the simulation
HomePos = [37.102177,-76.387207,0.000000]

# List to hold simulated traffic vehicles
tfList = []

# Start a single traffic vehicle from Home at specified
# range,brg,alt,track,speed,climb rate
# Call this function again to start multiple traffic vehicles
StartTraffic(1,HomePos,100,80,5,1,270,0,tfList)

# Set uncertainty and smoothing params for traffic
#for tf in tfList:
#    tf.setpos_uncertainty(0.01,0.01,0,0,0,0)

# Initialize Icarous class
ic = Icarous(HomePos,simtype="UAM_VTOL",verbose=1)

# Setup uncertainty and smoothing parameters
#ic.setpos_uncertainty(0.01,0.01,0,0,0,0)

# Read params from file and input params
params = LoadIcarousParams('data/icarous_default.parm')
ic.SetParameters(params)

# Input flightplan
flightplan = [ [37.102177,-76.387207,5.000000,0.0],
               [37.102335,-76.387195,5.000000,1.0],
               [37.102335,-76.386206,5.000000,1.0],
               [37.102110,-76.386481,5.000000,1.0],
               [37.101796,-76.386859,5.000000,1.0],
               [37.102177,-76.387207,5.000000,1.0]]

ic.InputFlightplan(flightplan,0)

# Input geofences from file
#ic.InputGeofence("data/geofence2.xml")

icInstances = []
icInstances.append(ic)

icDelay = []
tLimit = []
simWind = [(90,0.1)] # (wind from, speed)

RunSimulation(icInstances,tfList,startDelay = icDelay,timeLimit = tLimit,wind=simWind)

# Plot data for visualization    
#plt.figure(1)
#plt.plot(np.array(ic.positionLog)[:,0],np.array(ic.positionLog)[:,1],'r')
#for tf in tfList:
#    plt.plot(np.array(tf.log['pos'])[:,0],np.array(tf.log['pos'])[:,1],'b')
#
#plt.figure(2)
#plt.plot([i for i in range(len(ic.positionLog))],np.array(ic.positionLog)[:,2])
#plt.show()

VisualizeSimData(icInstances,allplans=False,xmin=-30,ymin=-30,xmax=100,ymax=100,interval=5,record=False,filename="anim.mp4")
