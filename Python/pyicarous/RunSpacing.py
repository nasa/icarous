from Icarous import *
from matplotlib import pyplot as plt
from Animation import AgentAnimation

# Set the home position for the simulation
HomePos1 = [37.102177,-76.387207,0.000000]
HomePos2 = [37.102177,-76.387937,0.000000]

icInstances = []

# Initialize Icarous class
ic1 = Icarous(HomePos1,simtype="UAM_VTOL",vehicleID=0,verbose=1,logName="sc0")
ic2 = Icarous(HomePos2,simtype="UAM_VTOL",vehicleID=1,verbose=1,logName="sc1")

icInstances.append(ic1)
icInstances.append(ic2)


# Setup uncertainty and smoothing parameters
#ic.setpos_uncertainty(0.01,0.01,0,0,0,0)

# Read params from file and input params
params = LoadIcarousParams('data/spacing_default.parm')

ic1.SetParameters(params)
ic2.SetParameters(params)

flightplan1 = [ [37.102177,-76.387207,5.000000,5.0],
               [37.102177,-76.287207,5.000000,5.0]]
flightplan2 = [ [37.102177,-76.387207,5.000000,10.0],
               [37.102177,-76.287207,5.000000,10.0]]



ic1.InputFlightplan(flightplan1,0)
ic2.InputFlightplan(flightplan2,0)


tfList =[]

icDelay = [0.0,0.0]
tlimit = [100.0,100.0]
RunSimulation(icInstances,tfList,startDelay=icDelay,timeLimit=tlimit)

getSpeed = lambda vned: np.sqrt(vned[0]**2 + vned[1]**2 + vned[2]**2)

speedic1 = [getSpeed(x) for x in ic1.ownshipLog['velocityNED']]
speedic2 = [getSpeed(x) for x in ic2.ownshipLog['velocityNED']]

plt.figure(1)
plt.plot(ic1.ownshipLog['t'],speedic1,'r',label='ownship1')
plt.plot(ic2.ownshipLog['t'],speedic2,'b',label='ownship2')
plt.xlabel('t (s)')
plt.ylabel('speed m/s')

# Plot data for visualization    
anim= AgentAnimation(-50,-50, 100,30,30)
for i,ic in enumerate(icInstances):
    name = 'ownship' + str(i)
    anim.AddPath(np.array(ic.localPlans[0]),'k--')
    anim.AddAgent(name,2,'r',ic.ownshipLog,show_circle=True,circle_rad=10)

anim.run()
