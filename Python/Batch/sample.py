from pyicarous import WellClearParams,IcarousSim
from matplotlib import pyplot as plt
from mpl_toolkits import mplot3d
import numpy as np


params = WellClearParams()
params.data["det_1_WCV_DTHR"] = "30.0 [ft]"
params.WriteParams()

# Create a simulator
ic = IcarousSim((37.102177,-76.387207,5),1,[37.1021769,-76.38495433,5.0])

# Setup uncertainty and smoothing parameters
ic.setpos_uncertainty_ownship(0.05,0.05,0,0,0,0)

# Input traffic and setup uncertainty for traffic
ic.InputTraffic(100,90,5,1,270,0)
ic.setpos_uncertainty_traffic(0.05,0.05,0,0,0,0)

# Run the simulation
ic.Run()


# Plot simulation results
time = [i*0.05 for i in range(len(ic.ownshipPosLog))]
vehspeed = [np.sqrt(i[0]**2 + i[1]**2 + i[2]**2) for i in ic.ownshipVelLog]


plt.figure(1)
plt.plot(np.array(ic.ownshipPosLog)[:,0],np.array(ic.ownshipPosLog)[:,1],'r')
plt.plot(np.array(ic.trafficPosLog[0])[:,0],np.array(ic.trafficPosLog[0])[:,1],'b')
plt.axis([-200,200,-200,200])

plt.figure(2)
plt.plot(np.array(time),np.array(ic.ownshipPosLog)[:,2])

plt.figure(3)
plt.plot(time,vehspeed)

plt.show()
