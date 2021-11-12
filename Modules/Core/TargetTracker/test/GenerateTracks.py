from numpy.core.defchararray import equal
from vehiclesim import UamVtolSim
from matplotlib import pyplot as plt
import vehiclesim
import numpy as np
import random
import sys

callsign     = sys.argv[1]
x0           = float(sys.argv[2])
y0           = float(sys.argv[3])
cmdHeading   = float(sys.argv[4])
cmdSpeed     = float(sys.argv[5])
cmdClimbRate = 0
span         = 1000

data = np.zeros((span,3))
time = np.zeros(span)
covariances = np.zeros(6)

# Define random covariance in the range [5,25]
sxx = 5 + 20*random.random()
syy = 5 + 20*random.random()
szz = 5 
sxy = 0.8*np.sqrt(sxx*syy)
syz = 0
sxz = 0
covariances[:] = [sxx,syy,szz,sxy,syz,sxz]

# Sample random input heading
# Using vehicle model to run simulation
vehicle = UamVtolSim(0,[0.0,0.0,0.0],x=x0,y=y0,z=10)
vehicle.InputCommand(cmdHeading,cmdSpeed,cmdClimbRate)
vehicle.SetPosUncertainty(sxx,syy,szz,sxy,sxz,syz,coeff=0.5)
    
dt = 0.05 
for j in range(span):
    time[j] = j*dt
    vehicle.Run(0,0)
    pos = vehicle.GetOutputPositionNED()
    data[j,:] = pos

# data is N,E,D, flip N and E to plot on X,Y
plt.plot(data[:,1],data[:,0])

fp = open(callsign+'.txt','w')
for j in range(span):
   n = data[j,0]
   e = data[j,1]
   d = data[j,2]
   sxx = covariances[0]
   syy = covariances[1]
   szz = covariances[2]
   sxy = covariances[3]
   syz = 0
   sxz = 0
   vxx = 0.1
   vyy = 0.1
   vzz = 0.1
   vxy = 0
   vyz = 0
   vxz = 0
   lat = 0
   lon = 0
   alt = 0
   fp.write('%f,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n'%
           (time[j],callsign,n,e,d,lat,lon,alt,cmdHeading,cmdSpeed,cmdClimbRate,sxx,syy,szz,sxy,syz,sxz,vxx,vyy,vzz,vxy,vyz,vxz))

fp.close()

plt.axis('equal')
plt.show()
