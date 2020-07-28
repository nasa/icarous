import json
from Icarous import VisualizeSimData

class playback():
    def __init__(self):
        self.ownshipLog = []
        self.trafficLog = []
        self.localPlans = []
        self.localFences = []
        self.daa_radius = []

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Visualize Icarous log")
    parser.add_argument("logfile", help="Icarous json log file")
    parser.add_argument("--record", action="store_true", help="record animation to file")
    parser.add_argument("--output", default="animation.mp4", help="video file name with .mp4 extension")
    args = parser.parse_args()

    fp = open(args.logfile,'r')
    data = json.load(fp)
    pb = playback()
    pb.ownshipLog = data['ownship']
    pb.trafficLog = data['traffic']
    pb.localPlans = pb.ownshipLog['localPlans']
    pb.localFences = pb.ownshipLog['localFences']
    pb.daa_radius = data['parameters']['DET_1_WCV_DTHR']/3
    VisualizeSimData([pb],allplans=False,xmin=-50,ymin=-50,xmax=100,ymax=100,interval=5,record=args.record,filename=args.output)

    

