import subprocess
import os
import os.path
import sys
import time
import signal
from BatchGSModule import *


f1 = open('sim_output.txt', 'w')
f2 = open('sitl_output.txt','w')

# Start sim vehicle simulation script
#sim = subprocess.Popen(["sim_vehicle.py","-v","ArduCopter","-l","37.1021769,-76.3872069,5,0","-S","1"],stdout=f1)

# Start ICAROUS
#sitl = subprocess.Popen(["java","-cp","lib/icarous.jar:lib/jssc-2.8.0.jar","launch","-v",\
#                         "--sitl","localhost","14551",\
#                         "--com","localhost","14552","14553",\
#                         "--mode", "active"],stdout=f2)

# Open a mavlink UDP port
master = None
try:
    master = mavutil.mavlink_connection("udp:127.0.0.1:14553", dialect="icarous",source_system=1)
except Exception as msg:
    print "Error opening mavlink connection"

master.wait_heartbeat()
GS = BatchGSModule(master,1,0)
GS.loadWaypoint("../../Java/params/flightplan.txt")
GS.loadGeofence("../../Java/params/geofence.xml")
GS.StartMission()
#time.sleep(120)
#os.kill(sim.pid,signal.SIGTERM)
#subprocess.Popen(["pkill","xterm"])
#sitl.kill()
#gs.kill()
#os.kill(os.getpid(),signal.SIGTERM)