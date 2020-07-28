import sys
import json
from Icarous import VisualizeSimData

class playback():
    def __init__(self):
        self.ownshipLog = []
        self.trafficLog = []
        self.localPlans = []
        self.localFences = []
        self.daa_radius = []

logfile = sys.argv[1]

if logfile != "":
    fp = open(logfile,'r')
    data = json.load(fp)
    pb = playback()
    pb.ownshipLog = data['ownship']
    pb.trafficLog = data['traffic']
    pb.localPlans = pb.ownshipLog['localPlans']
    pb.localFences = pb.ownshipLog['localFences']
    pb.daa_radius = data['parameters']['DET_1_WCV_DTHR']/3
    VisualizeSimData([pb],allplans=False,xmin=-50,ymin=-50,xmax=100,ymax=100,interval=5,record=False,filename="anim.mp4")
else:
    print("Invalid log file")

    

