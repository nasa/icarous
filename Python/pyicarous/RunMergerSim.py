from Icarous import *
from matplotlib import pyplot as plt
from Animation import AgentAnimation

# Set the home position for the simulation
HomePos1 = [37.415756,-122.056634,0]
HomePos2 = [37.416407,-122.056657,0]

icInstances = []

# Initialize Icarous class
ic1 = Icarous(HomePos1,simtype="UAM_VTOL",vehicleID=0,verbose=1,logName="sc0")
ic2 = Icarous(HomePos2,simtype="UAM_VTOL",vehicleID=1,verbose=1,logName="sc1")

icInstances.append(ic1)
icInstances.append(ic2)


# Setup uncertainty and smoothing parameters
#ic.setpos_uncertainty(0.01,0.01,0,0,0,0)

# Read params from file and input params
params = LoadIcarousParams('../TestRunner/tcl4+/merging_scenario/merging_default.parm')

ic1.SetParameters(params)
ic2.SetParameters(params)

ic1.InputFlightplanFromFile('../TestRunner/tcl4+/merging_scenario/vehicle0_waypoints.txt')
ic2.InputFlightplanFromFile('../TestRunner/tcl4+/merging_scenario/vehicle1_waypoints.txt')

ic1.InputMergeFixes('../TestRunner/tcl4+/merging_scenario/merge_fixes.txt')
ic2.InputMergeFixes('../TestRunner/tcl4+/merging_scenario/merge_fixes.txt')

tfList =[]

icDelay = [5.0,0.0]
tlimit = [15.0,15.0]
RunSimulation(icInstances,tfList,startDelay=icDelay,timeLimit=tlimit)

# Plot data for visualization    
anim= AgentAnimation(-50,-50, 100,30,30)
for i,ic in enumerate(icInstances):
    name = 'ownship' + str(i)
    anim.AddPath(np.array(ic.localPlans[0]),'k--')
    anim.AddAgent(name,2,'r',ic.ownshipLog,show_circle=True,circle_rad=10)

anim.run()
