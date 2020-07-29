import json
import os
import glob
import numpy as np
from Icarous import VisualizeSimData

class playback():
    def __init__(self):
        self.ownshipLog = []
        self.trafficLog = []
        self.localPlans = []
        self.localFences = []
        self.localMergeFixes = []
        self.daa_radius = []
        self.params = {}

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Visualize Icarous log")
    parser.add_argument("logfile", help="Icarous json log file or directory containing multiple json logs")
    parser.add_argument("--record", action="store_true", help="record animation to file")
    parser.add_argument("--output", default="animation.mp4", help="video file name with .mp4 extension")
    parser.add_argument("--pad",type=float, default=25.0, help="extend the min/max values of the axes by the padding (in meters), default = 25.0 [m]")
    args = parser.parse_args()

    files = []
    pbs   = []
    if os.path.isfile(args.logfile):
        files.append(args.logfile)
    else:
        path =  args.logfile.rstrip('/')+'/*.json'
        files = glob.glob(path)

    xmin, ymin = 1e10, 1e10
    xmax, ymax = -1e10, -1e10 
    for file in files:
        fp = open(file,'r')
        data = json.load(fp)
        pb = playback()
        pb.ownshipLog = data['ownship']
        pb.trafficLog = data['traffic']
        pb.localPlans = pb.ownshipLog['localPlans']
        pb.localFences = pb.ownshipLog['localFences']
        pb.params = data['parameters']
        pb.daa_radius = pb.params['DET_1_WCV_DTHR']/3
        pb.localMergeFixes = data['mergefixes']
        pbs.append(pb)
        _xmin = np.min(np.array(pb.ownshipLog['positionNED'])[:,1])
        _xmax = np.max(np.array(pb.ownshipLog['positionNED'])[:,1])
        _ymin = np.min(np.array(pb.ownshipLog['positionNED'])[:,0])
        _ymax = np.max(np.array(pb.ownshipLog['positionNED'])[:,0])
        xmin = np.min([xmin,_xmin])
        ymin = np.min([ymin,_ymin])
        xmax = np.max([xmax,_xmax])
        ymax = np.max([ymax,_ymax])

    padding = args.pad
    xmin -= padding
    ymin -= padding
    xmax += padding
    ymax += padding
    VisualizeSimData(pbs,allplans=False,xmin=xmin,ymin=ymin,xmax=xmax,ymax=ymax,interval=5,record=args.record,filename=args.output)

    

