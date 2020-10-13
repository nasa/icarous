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
    parser.add_argument("--allplans", action="store_true", help="plot all planned paths")
    parser.add_argument("--notraffic", action="store_true", help="only show icarous vehicles")
    parser.add_argument("--record", action="store_true", help="record animation to file")
    parser.add_argument("--output", default="animation.mp4", help="video file name with .mp4 extension")
    parser.add_argument("--pad",type=float, default=25.0, help="extend the min/max values of the axes by the padding (in meters), default = 25.0 [m]")
    parser.add_argument("--speed",type=float, default=25.0, help="increase playback speed by given factor")
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
        try:
            fp = open(file,'r')
            data = json.load(fp)
            pb = playback()
            pb.ownshipLog = data['ownship']
            pb.trafficLog = data['traffic']
            pb.localPlans = pb.ownshipLog['localPlans']
            pb.localFences = pb.ownshipLog['localFences']
            pb.params = data['parameters']
            pb.daa_radius = pb.params['DET_1_WCV_DTHR']*0.3048
            pb.localMergeFixes = data['mergefixes']
            pbs.append(pb)
            _xmin = np.min(np.array(pb.ownshipLog['positionNED'])[:,1])
            _xmax = np.max(np.array(pb.ownshipLog['positionNED'])[:,1])
            _ymin = np.min(np.array(pb.ownshipLog['positionNED'])[:,0])
            _ymax = np.max(np.array(pb.ownshipLog['positionNED'])[:,0])
            _xminfp = np.min(np.array(pb.localPlans[0])[:,2])
            _xmaxfp = np.max(np.array(pb.localPlans[0])[:,2])
            _yminfp = np.min(np.array(pb.localPlans[0])[:,1])
            _ymaxfp = np.max(np.array(pb.localPlans[0])[:,1])
            _xmin = np.min([_xmin,_xminfp])
            _xmax = np.max([_xmax,_xmaxfp])
            _ymin = np.min([_ymin,_yminfp])
            _ymax = np.max([_ymax,_ymaxfp])
            xmin = np.min([xmin,_xmin])
            ymin = np.min([ymin,_ymin])
            xmax = np.max([xmax,_xmax])
            ymax = np.max([ymax,_ymax])
        except:
            continue

    if (xmax-xmin) > (ymax-ymin):
        ymin = ymin + (ymax - ymin)/2 - (xmax-xmin)/2
        ymax = ymin + (xmax - xmin)
    elif (ymax-ymin) > (xmax-xmin):
        xmin = xmin + (xmax - xmin)/2 - (ymax-ymin)/2
        xmax = xmin + (ymax - ymin)

    padding = args.pad
    xmin -= padding
    ymin -= padding
    xmax += padding
    ymax += padding
    VisualizeSimData(pbs,allplans=args.allplans,showtraffic=not args.notraffic,xmin=xmin,ymin=ymin,xmax=xmax,ymax=ymax,playbkspeed=args.speed,interval=5,record=args.record,filename=args.output)

    

