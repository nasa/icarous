import Planner
import numpy as np
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from icutils.AccordUtil import *

home = [0.0,0.0,0.0]
posA = [0.0,0.0,-10.0]
posB = [1000.0,1000.0,-50.0]
velA = [0.0, 10.0,0,0]
velB = [20.0,10.0,0.0]

lla2ned = lambda pos: ConvertLLA2NED(home,pos[:3])
ned2lla = lambda pos: ConvertNED2LLA(home,pos[:3])

fp = Planner.Planner(None)
posA = ned2lla(posA)
posB = ned2lla(posB)
numWP = fp.FindPath(posA,velA,posB,velB)



traj,wps = fp.GetPlan()
traj = list(map(lla2ned,traj))
wps = list(map(lla2ned,wps))
traj = np.array(traj)
wps = np.array(wps)

plt.figure(1)
plt.plot(traj[:,1],traj[:,0])
plt.scatter(wps[:,1],wps[:,0])
plt.axis('equal')

fig2 = plt.figure(2)
fig2.add_subplot(111,projection='3d')
plt.plot(traj[:,1],traj[:,0],traj[:,2])
plt.show()

