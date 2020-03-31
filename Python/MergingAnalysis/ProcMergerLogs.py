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
        commandedSpeed = float(entries[11])
        mergeDev = float(entries[12].rstrip('\n'))
        mergingStatus = int(entries[13])
        data.append([time, intID, dist2int, speed, nodeRole,
                     earlyArrTime, currArrTime, lateArrTime,
                     zone, numSc, mergeSpeed, commandedSpeed, mergeDev, mergingStatus])
    return data

def EndTrim(inputData,index):
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
IntID      = int(sys.argv[2])
PathToLogs = sys.argv[3]

AllVehicles = []
Vehicle  = {}
for i in range(NumVehicles):
    filename = PathToLogs + '/merger_appdata_' + str(i) + '.txt'
    AllVehicles.append(ReadLogData(filename))

for i in range(NumVehicles):
    vehicle = np.array(AllVehicles[i])
    # Extract data only for required intersection
    vehicle = vehicle[ abs(vehicle[:,1] - IntID) < 1e-3 ]

    # Trim redundant data
    vehicle = EndTrim(vehicle,2)


    #FixTimes(Vehicle[:,0])
    Vehicle[i] = vehicle

plt.figure(1)
for i in range(NumVehicles):
  for j in range(Vehicle[i].shape[0]):
    if Vehicle[i][j,13] == 1:
      startval = Vehicle[i][j,0] 
      break;
  stopval =  Vehicle[i][-1,0]
  line, = plt.plot(Vehicle[i][:, 0], Vehicle[i][:, 2])
  col_line = line.get_color()
  plt.axvspan(startval,stopval,color=col_line,alpha=0.5)
  line.set_label('Vehicle'+str(i))
plt.xlabel('time (s)')
plt.ylabel('distance to merge fix (m)')
plt.legend()
plt.savefig(PathToLogs+'/time_distance.png')

plt.figure(2)
for i in range(NumVehicles):
  line1, = plt.plot(Vehicle[i][:, 0], Vehicle[i][:, 3], '')
  line2, = plt.plot(Vehicle[i][:, 0], Vehicle[i][:, 10], '--')
  line3, = plt.plot(Vehicle[i][:, 0], Vehicle[i][:, 11], '-.')
  line1.set_label('Vehicle'+str(i)+' actual')
  line2.set_label('Vehicle'+str(i)+' merging')
  line3.set_label('Vehicle'+str(i)+' commanded')
plt.xlabel('time (s)')
plt.ylabel('speed (m/s)')
plt.legend()
plt.savefig(PathToLogs+'/time_speed.png')

plt.figure(3)
for i in range(NumVehicles):
  line, = plt.plot(Vehicle[i][:,0],Vehicle[i][:,4])
  line.set_label('Vehicle'+str(i))
plt.yticks([0.0,1.0,2.0,3.0],['NEUTRAL','FOLLOWER','CANDIDATE','LEADER'],rotation=45)
plt.xlabel('time (s)')
plt.ylabel('Node states')
plt.legend()
plt.savefig(PathToLogs+'/time_states.png')
plt.show()

