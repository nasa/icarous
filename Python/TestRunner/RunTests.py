import sys
import subprocess
import time
import json
import numpy as np
import BatchGSModule as GS
from polygon_contain import *
from pymavlink import mavutil,mavwp

# Data structure to collect simulation data
class vehicle(object):
    def __init__(self):
        self.position = []
        self.velocity = []

def GetPolygons(origin,fenceList):
    """
    Constructs a list of Polygon (for use with PolyCARP) given a list of fences
    @param fenceList list of fence dictionaries
    @return list of polygons (a polygon here is a list of Vectors)
    """
    Polygons = []
    for fence in fenceList:
        vertices = fence["Vertices"]
        vertices_ned = [list(reversed(GS.LLA2NED(origin,position))) for position in vertices]
        polygon = [(vertex[0],vertex[1]) for vertex in vertices_ned]
        Polygons.append(polygon)
    return Polygons


def VerifySimOutput(data):
    ownship = data["ownship_pos"]
    traffic = data["traffic_pos"]
    waypoints = data["waypoints"]
    geofences = data["geofences"]
    numWaypoints = len(waypoints)

    origin  = [waypoints[0][0],waypoints[0][1],0.0]
    lla2ned = lambda x: GS.LLA2NED(origin,x)
    ownship_poslocal = map(lla2ned,ownship)
    traffic_poslocal = map(lla2ned,traffic)
    waypoints_poslocal = map(lla2ned,waypoints)

    progress = {}
    keepInViolation = False
    keepOutViolation = False
    trafficViolation = False

    for i,elem in enumerate(waypoints):
        progress["wp"+str(i)] = False

    # Check for progress to waypoints. Let progress be a list of booleans (one
    # for each waypoint, True if its reached)
    for j,pos in enumerate(ownship_poslocal):
        for i,wp in enumerate(waypoints_poslocal):
            dist = np.sqrt( (pos[0] - wp[0])**2 + (pos[1] - wp[1])**2 )
            val = True if dist<6 else False
            progress["wp"+str(i)] = progress["wp"+str(i)] or val

        if len(geofences) == 0:
            continue


        # Check for keep in fence violations
        keep_in_fence = [Vector(*vertex) for vertex in geofences[0]]
        s = Vector(pos[1],pos[0])
        if not definitely_inside(keep_in_fence,s,0.01):
            keepInViolation = True

        # Check for keep out fence violations
        for fence in geofences[1:]:
           fencePoly = [Vector(*vertex) for vertex in fence]
           if definitely_inside(fencePoly,s,0.01):
                keepOutViolation = True

        # Check for traffic violations
        if(len(traffic_poslocal) > 0):
            tfpos = traffic_poslocal[j]
            dist  = np.sqrt( (pos[0] - tfpos[0])**2 + (pos[1] - tfpos[1])**2 )
            if dist < 10:
                trafficViolation = True

    return (progress,keepInViolation,keepOutViolation,trafficViolation)

if(sys.argv[1] == str(0)):
    trafficAvailable = False
else:
    trafficAvailable = True

simTimeLimit     = 250

ownship = vehicle()
traffic = vehicle()

try:
    master = mavutil.mavlink_connection("127.0.0.1:14553")
except Exception as msg:
    print("Error opening mavlink connection")

# Start the ICAROUS process
ic = subprocess.Popen(["core-cpu1","-I 0","-C 1"])

time.sleep(20)

# Pause for a couple of seconds here so that ICAROUS can boot up
master.wait_heartbeat();
gs = GS.BatchGSModule(master,1,0)

# Upload the test flight plan
gs.loadWaypoint("../../../Examples/InputData/flightplan.txt")

if not trafficAvailable:
    # Upload the test geofence
    gs.loadGeofence("../../../Examples/InputData/geofence2.xml")
else:
    gs.load_traffic([0,116,81,5,1,270,0])

gs.StartMission()

# Run simulation for specified duration
startT   = time.time()
duration = 0
while(duration < simTimeLimit):
    currentT = time.time()
    duration = currentT - startT

    if trafficAvailable:
        gs.Update_traffic()

    msg = None
    msg = master.recv_match(blocking=False, type=["GLOBAL_POSITION_INT"])

    if msg:
        # Storing ownship position/velocity information
        ownship.position.append([msg.lat/1E7,msg.lon/1E7,msg.relative_alt/1E3])
        ownship.velocity.append([msg.vx/1E2,msg.vy/1E2,msg.vz/1E2])

        if trafficAvailable:
            # Storing traffic position/velocity information
            traffic.position.append([gs.traffic_list[0].lat,gs.traffic_list[0].lon,gs.traffic_list[0].alt])
            traffic.velocity.append([gs.traffic_list[0].vx0,gs.traffic_list[0].vy0,gs.traffic_list[0].vz0])

# Once simulation is finished, kill the icarous process
ic.kill()

# Get the waypoints
WP = []
wpcount = gs.wploader.count()
for i in range(wpcount):
    WP.append([gs.wploader.wp(i).x,gs.wploader.wp(i).y,gs.wploader.wp(i).z])

origin  = [WP[0][0],WP[0][1]]

# Get the geofences
GF = GetPolygons(origin,gs.fenceList)

# Construct the sim data to for verification
simdata = {"ownship_pos":ownship.position,
           "traffic_pos":traffic.position,
           "geofences"  :GF,
           "waypoints"  :WP}

json.dump(simdata,open("simoutput.json","w"))

# Verify the sim output
val= VerifySimOutput(simdata)

print(val)

if not trafficAvailable:
    wpSeqResult = {'wp0':True, 'wp1':False, 'wp2':True, \
                         'wp3':True, 'wp4':False, 'wp5':True, \
                         'wp6':False, 'wp7':True}
else:
    wpSeqResult = {'wp0':True, 'wp1':False, 'wp2':True, \
                         'wp3':True, 'wp4':True, 'wp5':True, \
                         'wp6':True, 'wp7':True}


assert (val[0]==wpSeqResult), "waypoint test failed"
assert (val[1]==False), "Keep in fence violation"
assert (val[2]==False), "Keep out fence violation"
assert (val[3]==False), "Traffic conflict"

"""
lla2ned = lambda x: GS.LLA2NED(origin,x)
ownship_poslocal = map(lla2ned,simdata["ownship_pos"])
traffic_poslocal = map(lla2ned,simdata["traffic_pos"])
ownpos_x  = [val[0] for val in ownship_poslocal]
ownpos_y  = [val[1] for val in ownship_poslocal]
traffic_x = [val[0] for val in traffic_poslocal]
traffic_y = [val[1] for val in traffic_poslocal]

geopos0_x = [val[0] for val in simdata["geofences"][0]]
geopos0_y = [val[1] for val in simdata["geofences"][0]]
geopos1_x = [val[0] for val in simdata["geofences"][1]]
geopos1_y = [val[1] for val in simdata["geofences"][1]]


geopos0_x.append(geopos0_x[0])
geopos0_y.append(geopos0_y[0])
geopos1_x.append(geopos1_x[0])
geopos1_y.append(geopos1_y[0])

from matplotlib import pyplot as plt
plt.plot(ownpos_y,ownpos_x)
plt.plot(geopos0_x,geopos0_y)
plt.plot(geopos1_x,geopos1_y)
plt.show()
"""
