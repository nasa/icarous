from Trajectory import *
from AccordUtil import *
from ichelper import *
from matplotlib import pyplot as plt
from matplotlib import patches
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import time
# initial params for rrt
params = DubinsParams()
params.turnRate = 15 
params.zSections = 1
params.vertexBuffer = 2
params.hAccel = 1
params.vAccel = 1
params.maxGS = 15
params.minGS = 1
params.maxVS = 1.5
params.minVS = -1.5
params.hAccel = 1.5
params.vAccel = 1
params.hDaccel = -1
params.vDaccel = -1
params.climbgs = 1 
params.gs = 1
params.vs = 1.5 
params.maxH = 500
params.wellClearDistH = 10
params.wellClearDistV = 10


Home = [37.102335, -76.38696725, 5.000000]
goalxy = gps_offset(Home[0],Home[1],100,0) 
#goalxy = gps_offset(Home[0],Home[1],-100,250) 
Goal = [*goalxy,5.0]
Goal = [37.10211,-76.386481,5.0]
Vel  = [89.99890039,1.8952*0.51444,0]
Velf  = [223.83467332,1.8952*0.51444,0]

traj = Trajectory("plan")

traj.UpdateDubinsPlannerParams(params)

fenceList = []
fenceList = Getfence('data/geofence2.xml')
localFenceList = []
for fence in fenceList:
    #if fence['id'] == 0:
    #    continue
    traj.InputGeofenceData(fence)
    localFence = []
    for vertex in fence['Vertices']:
        vertex3d = [*vertex,Home[2]]
        localFence.append(ConvertLLA2NED(Home,vertex3d))
    localFence.append(localFence[0])
    localFenceList.append(localFence)

#traj.InputTrafficData(1,[37.10233259,-76.38832752,5.0],[270,2.04*0.5144,0])
#traj.InputTrafficData(2,[37.10206582,-76.38683474,5.0],[90,2.04*0.5144,0])

t1 = time.time()
traj.FindPath("plan0",Home,Goal,Vel,Velf)
t2 = time.time()

print("Computation time:",t2-t1)

waypoints = traj.GetFlightPlan("plan0",)

if len(waypoints) <= 1:
    print("No path computed")

fp = [[wp.time,wp.latitude,wp.longitude,wp.altitude] for wp in waypoints]
tcp = [[*wp.tcp] for wp in waypoints]
tcpValues = [[*wp.tcpValue] for wp in waypoints]
n,e,d,ptn,pte,ptd = plotTcpPlan(fp,tcp,tcpValues,local=False)

minc1 = min(n)
minc2 = min(e)
minc = np.min([minc1,minc2]) - 5
maxc1 = max(n)
maxc2 = max(e)
maxc = np.max([maxc1,maxc2]) + 5
if len(localFenceList) > 0:
    minc = np.min([minc,np.min(np.array(localFenceList))]) - 5
    maxc = np.max([maxc,np.max(np.array(localFenceList))]) + 5

plt.plot(e,n)
plt.scatter(pte,ptn)
for fence in localFenceList:
    plt.plot(np.array(fence)[:,1],np.array(fence)[:,0])
plt.xlim([minc,maxc])
plt.ylim([minc,maxc])

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot(e,n,d)



ax.set_xlim([minc,maxc])
ax.set_ylim([minc,maxc])
ax.set_xlabel("E [m]")
ax.set_ylabel("N [m]")
ax.set_zlabel("D [m]")
plt.show()