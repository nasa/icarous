#!/usr/bin/env python3

import json
import os
import glob
import numpy as np
from icutils.ichelper import GetPlanPositions,GetEUTLPlanFromFile,ConvertToLocalCoordinates
from CustomTypes import TcpType

class playback():
    def __init__(self):
        self.callsign = ''
        self.ownshipLog = []
        self.trafficLog = []
        self.localPlans = []
        self.localFences = []
        self.localMergeFixes = []
        self.daa_radius = []
        self.params = {}

def VisualizeSimData(icList,allplans=False,showpaths=True,showtrace=True,showtraffic=True,xmin=-100,ymin=-100,xmax=100,ymax=100,playbkspeed=1,interval=30,record=False,filename="",network=[]):
    '''
    ic: icarous object
    allplans: True - plot all computed plans, False - plot only the mission plan
    xmin,ymin : plot axis min values
    xmax,ymax : plot axis max values
    interval  : Interval between frames
    '''
    if record:
        import matplotlib; matplotlib.use('Agg')
    from Animation import AgentAnimation
    anim= AgentAnimation(xmin,ymin, xmax,ymax,showtrace,playbkspeed,interval,record,filename)

    vehicleSize1 = np.abs(xmax - xmin)/100
    vehicleSize2 = np.abs(ymax - ymin)/100
    vehicleSize  = np.max([vehicleSize1,vehicleSize2])
    homePos = icList[0].home_pos
    getLocPos = lambda pos: np.array(ConvertToLocalCoordinates(homePos,pos))
    for j,ic in enumerate(icList):
        anim.AddAgent(ic.callsign,vehicleSize,'r',ic.ownshipLog,show_circle=True,circle_rad=ic.daa_radius)
        for i,pln in enumerate(ic.plans):
            planTime = pln[0][0]
            planPositions = np.array(GetPlanPositions(ic.plans[i],0.1))
            points = np.array(list(map(getLocPos,planPositions)))
            if i == 0:
                planWPs = np.array(pln)[:,1:] 
                labels = [[TcpType.getString(val[3]),TcpType.getString(val[4]),TcpType.getString(val[5])]\
                           for val in planWPs]
                if showpaths:
                    anim.AddPath(np.array(list(map(getLocPos,planWPs))),'k--',points,labels,time=planTime)

            if i > 0 and allplans:
                planWPs = np.array(pln)[:,1:] 
                if showpaths:
                    anim.AddPath(np.array(list(map(getLocPos,planWPs))),'k--',points,time=planTime)
        tfids = ic.trafficLog.keys()
        for key in tfids:
            if showtraffic or key[0:2] == 'tf':
                anim.AddAgent('traffic_'+str(key),vehicleSize,'b',ic.trafficLog[key])
        for fence in ic.localFences:
            fence.append(fence[0])
            anim.AddFence(np.array(fence),'c-.')
    for fix in icList[0].localMergeFixes:
        anim.AddZone(fix[::-1][1:3],float(icList[0].params['coordination_zone'].split(' ')[0]),'r')
        anim.AddZone(fix[::-1][1:3],float(icList[0].params['schedule_zone'].split(' ')[0]),'b')
        anim.AddZone(fix[::-1][1:3],float(icList[0].params['entry_zone'].split(' ')[0]),'g')

    for plan in network:
        planWPs = np.array(plan)[:,1:]
        anim.AddPath(np.array(list(map(getLocPos,planWPs))),'k--',color2='k')


    anim.run()

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Visualize Icarous log")
    parser.add_argument("logfile", help="Icarous json log file or directory containing multiple json logs")
    parser.add_argument("--allplans", action="store_true", help="plot all paths")
    parser.add_argument("--notraffic", action="store_true", help="don't consider icarous vehicles as traffic")
    parser.add_argument("--record", action="store_true", help="record animation to file")
    parser.add_argument("--nopaths", action="store_true", help="don't show flightplans")
    parser.add_argument("--notrace", action="store_true", help="don't show flight traces")
    parser.add_argument("--output", default="animation.mp4", help="video file name with .mp4 extension")
    parser.add_argument("--pad",type=float, default=25.0, help="extend the min/max values of the axes by the padding (in meters), default = 25.0 [m]")
    parser.add_argument("--speed",type=int, default=1.0, help="increase playback speed by given factor")
    parser.add_argument("--routes",default='',help="routes file")
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
    valid = False
    routes = []
    if args.routes != '':
        n = 1
        index = -1
        while n != 0:
            index = index + 1
            wps,n = GetEUTLPlanFromFile(args.routes,index)
            if n > 0:
                locplan = [[wps[i].time,wps[i].latitude,wps[i].longitude,wps[i].altitude] for i in range(n)]
                routes.append(locplan)

    for file in files:
        try:
            fp = open(file,'r')
            data = json.load(fp)
            valid = True
            pb = playback()
            pb.ownshipLog = data['state']
            pb.callsign = data['callsign']
            pb.trafficLog = data['traffic']
            pb.plans = data['flightplans']
            pb.home_pos = data['origin']
            pb.localPlans = data['flightplans_local']
            pb.localFences = [fence["vertices"] for fence in data['geofences_local']]
            pb.params = data['parameters']
            alerter = pb.params['alerters'].split(',')[0]
            dthr = float(pb.params[alerter+'_det_1_WCV_DTHR'].split(' ')[0])
            pb.daa_radius = dthr*0.3048
            pb.localMergeFixes = data['mergefixes_local']
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

    if valid:
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
         VisualizeSimData(pbs,allplans=args.allplans,showtraffic=not args.notraffic,showtrace=not args.notrace,showpaths=not args.nopaths,xmin=xmin,ymin=ymin,xmax=xmax,ymax=ymax,playbkspeed=args.speed,interval=5,record=args.record,filename=args.output,network=routes)