import sys
import numpy as np
from matplotlib import pyplot as plt

fenceFile = sys.argv[1]
waypointFile = sys.argv[2]

fence = np.loadtxt(fenceFile)
waypoints = np.loadtxt(waypointFile)

numFences = int(fence[fence.shape[0]-1,0])+1
Fences = []

plt.figure(1)
for i in range(numFences):
    vertices = []
    for F in fence:
        if int(F[0]) == i:
            vertices.append(F[1:])
    vertices.append(vertices[0])
    Fences.append(np.array(vertices))
    plt.plot(Fences[i][:,0],Fences[i][:,1])

plt.plot(waypoints[:,0],waypoints[:,1],'o-')
plt.xlabel("x (m)")
plt.ylabel("y (m)")
plt.show()
