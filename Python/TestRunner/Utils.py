import sys
import subprocess
import time
import numpy as np
import BatchGSModule as GS
from polygon_contain import *
from pymavlink import mavutil,mavwp

# Data structure to collect simulation data
class vehicle(object):
    def __init__(self):
        self.position = []
        self.velocity = []

def GetPolygons(fenceList):
    """
    Constructs a list of Polygon (for use with PolyCARP) given a list of fences
    @param fenceList list of fence dictionaries
    @return list of polygons (a polygon here is a list of Vectors)
    """
    Polygons = []
    for fence in fenceList:
        vertices = fence["Vertices"]
        vertices_ned = [list(reversed(LLA2NED(origin,position))) for position in vertices]
        polygon = [Vector(vertex[0],vertex[1]) for vertex in vertices_ned]
        Polygons.append(polygon)
    return Polygons


def VerifySimOutput(data):
    ownship = data["ownship_pos"]
    traffic = data["traffic_pos"]
    waypoints = data["waypoints"]
    geofences = data["geofences"]
    numWaypoints = len(waypoints)

    origin  = [waypoints[0].x,waypoints[0].y,waypoints[0].z]
    lla2ned = lambda x: GS.LLA2NED(origin,x)
    ownship_poslocal = map(lla2ned,ownship.position)
    traffic_poslocal = map(lla2ned,traffic.position)
    waypoints_poslocal = map(lla2ned,waypoints)

    progress = {}
    keepInViolation = False
    keepOutViolation = False
    trafficViolation = False

    # Check for progress to waypoints. Let progress be a list of booleans (one
    # for each waypoint, True if its reached)
    for j,pos in enumerate(ownship_poslocal):
        for i,wp in enumerate(waypoints_poslocal):
            dist = np.sqrt( (pos[0] - wp[0])**2 + (pos[1] - wp[1])**2 )
            val = True if dist<6 else False
            progress["wp"+str(i)] = progress["wp"+str(i)] or val

        # Check for keep in fence violations
        keep_in_fence = geofences[0]
        s = Vector(pos[0],pos[1])
        if definitely_outside(keep_in_fence,s,0.0001):
            keepInViolation = True

        # Check for keep out fence violations
        for fence in geofences[1:]:
           if definitely_inside(fence,s,0.0001):
                keepOutViolation = True

        # Check for traffic violations
        tfpos = traffic_poslocal[j]
        dist  = np.sqrt( (pos[0] - tfpos[0])**2 + (pos[1] - tfpos[1])**2 )
        if dist < 10:
            trafficViolation = True

    return (progress,keepInViolation,keepOutViolation,trafficViolation)

trafficAvailable = False
simTimeLimit     = 100

try:
    master = mavutil.mavlink_connection("127.0.0.1:14553")
except Exception as msg:
    print "Error opening mavlink connection"

# Start the ICAROUS process
f1 = open('icarous_out.txt','w')
ic = subprocess.Popen(["core-cpu1","-I 0","-C 1"])#,stdout=f1)

# Pause for a couple of seconds here so that ICAROUS can boot up
time.sleep(10)

master.wait_heartbeat();
gs = GS.BatchGSModule(master,1,0)

# Upload the test flight plan
gs.loadWaypoint("../../../Examples/InputData/FlightPlan.txt")

# Upload the test geofence
gs.loadGeofence("../../../Examples/InputData/geofence2.xml")


ownship = vehicle()
traffic = vehicle()

gs.StartMission()

# Run simulation for specified duration
startT   = time.time()
duration = 0
while(duration < simTimeLimit):
    currentT = time.time()
    duration = currentT - startT

    if trafficAvailable:
        gs.UpdateTraffic()

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

"""
# Get the waypoints
WP = []
wpcount = GS.wploader.count()
for i in wpcount:
    WP.append([GS.wploader.wp(i).x,GS.wploader.wp(i).y,GS.wploader.wp(i).z])

# Get the geofences
GF = GetPolygons(GS.fenceList)

# Construct the sim data to for verification
simdata = {"ownship_pos":ownship.position,
           "traffic_pos":traffic.position,
           "geofences"  :GF,
           "waypoints"  :WP}

# Verify the sim output
VerifySimOutput(simdata)
"""
