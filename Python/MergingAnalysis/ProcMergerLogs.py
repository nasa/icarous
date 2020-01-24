import numpy as np
from matplotlib import pyplot as plt
import sys

def ReadLogData(filename):
    fp = open(filename, 'r')
    fp.readline()
    data_string = fp.readlines()
    data = []
    for line in data_string:
        entries = line.split(',')
        time = float(entries[0])
        intID = int(entries[1])
        dist2int = float(entries[2])
        speed = float(entries[3])
        nodeRole = int(entries[4])
        earlyArrTime = int(entries[5].lstrip().lstrip('('))
        currArrTime = int(entries[6])
        lateArrTime = int(entries[7].lstrip().rstrip(')'))
        zone = int(entries[8])
        numSc = int(entries[9])
        mergeSpeed = float(entries[10])
        mergeDev = float(entries[11].rstrip('\n'))
        data.append([time, intID, dist2int, speed, nodeRole,
                     earlyArrTime, currArrTime, lateArrTime,
                     zone, numSc, mergeSpeed, mergeDev])
    return data

def FixTimes(inputTimes):
    numRows = inputTimes.shape[0]
    i = 0
    val1 = inputTimes[i]
    # Go through all the entries
    #import pdb; pdb.set_trace()
    while i < numRows:
        # Initialize count and start
        count = 1
        start = i
        val2 = val1
        while val1 == val2:
            i += 1
            if i >= numRows:
                break
            val2 = val1
            val1 = inputTimes[i]
            if val1 == val2:
                count += 1
        for j in range(count):
            if count > 1: 
                inputTimes[start + j] = val2 + (1.0/float(count)) * (j + 1)
            else:
                inputTimes[start + j] = val2 + 0.5

    return inputTimes

def EndTrim(inputData,index):
    #import pdb; pdb.set_trace()
    i = inputData.shape[0] - 1
    val1 = inputData[i,2]
    val2 = inputData[i-1,2]
    while abs(val1 - val2) < 1e-3:
        i -= 1
        val1 = val2
        val2 = inputData[i,2]

    j= i + 2
    inputData = inputData[:j,:]
    return inputData



NumVehicles = int(sys.argv[1])
PathToLogs = sys.argv[2]

Vehicles = []
for i in range(NumVehicles):
    filename = PathToLogs + '/merger_appdata_' + str(i) + '.txt'
    Vehicles.append(ReadLogData(filename))

Vehicles1 = np.array(Vehicles[0])
Vehicles2 = np.array(Vehicles[1])
Vehicles3 = np.array(Vehicles[2])


# Extract data only for intersction 1
Vehicles1 = Vehicles1[ abs(Vehicles1[:,1] - 1) < 1e-3 ]
Vehicles2 = Vehicles2[ abs(Vehicles2[:,1] - 1) < 1e-3 ]
Vehicles3 = Vehicles3[ abs(Vehicles3[:,1] - 1) < 1e-3 ]

Vehicles1 = EndTrim(Vehicles1,2)
Vehicles2 = EndTrim(Vehicles2,2)
Vehicles3 = EndTrim(Vehicles3,2)

FixTimes(Vehicles1[:,0])
FixTimes(Vehicles2[:,0])
FixTimes(Vehicles3[:,0])

plt.figure(1)
line1, = plt.plot(Vehicles1[:, 0], Vehicles1[:, 2], 'r',)
line2, = plt.plot(Vehicles2[:, 0], Vehicles2[:, 2], 'g',)
line3, = plt.plot(Vehicles3[:, 0], Vehicles3[:, 2], 'b',)
line1.set_label('Vehicle1')
line2.set_label('Vehicle2')
line3.set_label('Vehicle3')
plt.xlabel('time (s)')
plt.ylabel('distance to merge fix (m)')
plt.legend()

plt.figure(2)
plt.plot(Vehicles1[:, 0], Vehicles1[:, 3], 'r')
plt.plot(Vehicles1[:, 0], Vehicles1[:, 10], 'r--')
plt.plot(Vehicles2[:, 0], Vehicles2[:, 3], 'g')
plt.plot(Vehicles2[:, 0], Vehicles2[:, 10], 'g--')
plt.plot(Vehicles3[:, 0], Vehicles3[:, 3], 'b')
plt.plot(Vehicles3[:, 0], Vehicles3[:, 10], 'b--')
plt.xlabel('time (s)')
plt.ylabel('speed (m/s)')
plt.legend(['Vehicle1 actual','Vehicle1 command','Vehicle2 actual','Vehicle2 command','Vehicle3 actual','Vehicle3 command'])

plt.figure(3)
plt.plot(Vehicles1[:,0],Vehicles1[:,4],'r')
plt.plot(Vehicles2[:,0],Vehicles2[:,4],'g')
plt.plot(Vehicles3[:,0],Vehicles3[:,4],'b')
plt.yticks([0.0,1.0,2.0,3.0],['NEUTRAL','FOLLOWER','CANDIDATE','LEADER'],rotation=45)
plt.xlabel('time (s)')
plt.ylabel('Node states')
plt.show()

