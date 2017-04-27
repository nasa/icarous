import xml.etree.ElementTree as ET
import matplotlib
import numpy as np
import os
import datetime
import sys
import pylab

from pymavlink import mavutil
from numpy import sin,cos
try:
    from polygon_contain import *
except:
    print "*******************************"
    print "PolyCARP not found.\n Download PolyCARP from https://github.com/nasa/PolyCARP.git and add the Python folder in this repository to the PYTHONPATH environment variable."
    print "*****************************"
    sys.exit(0)

from argparse import ArgumentParser

fenceList = []

def GetGeofence(filename):
    """
    Function to read geofence
    :param filename: .xml file containing geofence (ICAROUS geofence format)
    :return: adds fences to fenceList
    """
    tree = ET.parse(filename)
    root = tree.getroot()        
    
    for child in root:
        id    = int(child.get('id'))
        type  = int(child.find('type').text)
        numV  = int(child.find('num_vertices').text)
        floor = float(child.find('floor').text)
        roof  = float(child.find('roof').text)
        Vertices = []
        
        if(len(child.findall('vertex')) == numV):        
            for vertex in child.findall('vertex'):
                coord = (float(vertex.find('lat').text),
                         float(vertex.find('lon').text))                
                Vertices.append(coord)
                Geofence = {'id':id,'type': type,'numV':numV,'floor':floor,
                            'roof':roof,'Vertices':Vertices}
                fenceList.append(Geofence)

def LLA2NED(origin,position):
    """
    Convert from geodetic coordinates to NED coordinates
    :param origin:  origin of NED frame in geodetic coordinates
    :param position: position to be converted to NED
    :return: returns position in NED
    """
    R    = 6371000  # radius of earth
    oLat = origin[0]*np.pi/180
    oLon = origin[1]*np.pi/180

    if(len(origin) > 2):
        oAlt = origin[2]
    else:
        oAlt = 0

    pLat = position[0]*np.pi/180
    pLon = position[1]*np.pi/180

    if(len (origin) > 2):
        pAlt = position[2]
    else:
        pAlt = 0

    # convert given positions from geodetic coordinate frame to ECEF
    oX   = (R+oAlt)*cos(oLat)*cos(oLon)
    oY   = (R+oAlt)*cos(oLat)*sin(oLon)
    oZ   = (R+oAlt)*sin(oLat)

    Pref = np.array([[oX],[oY],[oZ]])

    pX   = (R+pAlt)*cos(pLat)*cos(pLon)
    pY   = (R+pAlt)*cos(pLat)*sin(pLon)
    pZ   = (R+pAlt)*sin(pLat)

    P    = np.array([[pX],[pY],[pZ]])

    # Convert from ECEF to NED
    Rne  = np.array([[-sin(oLat)*cos(oLon), -sin(oLat)*sin(oLon), cos(oLat)],
                     [-sin(oLon),                cos(oLon),          0     ],
                     [-cos(oLat)*cos(oLon), -cos(oLat)*sin(oLon),-sin(oLat)]])
    
    Pn   = np.dot(Rne,(P - Pref))

    if(len (origin) > 2):
        return [Pn[0,0], Pn[1,0], Pn[2,0]]
    else:
        return [Pn[0,0], Pn[1,0]]

def GetProximity(position,fence):
    """
    Function to calucate proximity to a geofence (returns proximity to closest edge)
    :param position: position in NED frame
    :param fence: list containing fence vertices in NED
    :return: proximity (scalar value), closest edge
    """
    prox = 1e15

    x3 = position[0]
    y3 = position[1]

    closest_edge = 0
    
    for i in range(len(fence)):

        """
        if(i == (len(fence) - 1)):
            j = 0
        else:
            j = i+1
        """
        j = (i+1)%len(fence)

        x1 = fence[i][0]
        y1 = fence[i][1]

        x2 = fence[j][0]
        y2 = fence[j][1]

        m  = (y2 - y1)/(x2 - x1)

        a  = m
        b  = -1
        c  = y1 - m*x1

        x0 = (b*(b*x3 - a*y3) - a*c)/(a**2+b**2)
        y0 = (a*(-b*x3 + a*y3) - b*c)/(a**2+b**2)

        insegment = False

        AB = np.array([[x2 - x1],[y2 - y1]])
        AC = np.array([[x0 - x1],[y0 - y1]])
        C  = np.array([[x0],[y0]])

        projAC  = np.dot(AC.T,AB)/(np.linalg.norm(AB)**2)

        if projAC >=0 and projAC <= 1:
            insegment = True

        if insegment:
            CD = np.array([[x3 - x0],[y3 - y0]])
            lenCD = np.linalg.norm(CD)

            if lenCD < prox:
                prox = lenCD
                closest_edge = i

    return (prox,closest_edge)

def GetPerformanceData(num,args):
    # work out msg types we are interested in
    t   = []
    lat = []
    lon = []
    alt = []
    x   = []
    y   = []
    z   = []
    vx  = []
    vy  = []
    vz  = []

    logfilename = args.logfolder
    #logfilename = args.logfolder + "/test"+str(num+1)+"/mav.tlog"
    geofilename = args.logfolder + "/test"+str(num+1)+"/geofence.xml"

    print "processing test : " + str(num+1)

    try:
        if not os.path.exists(logfilename):
            print "Log file doesn't exist: " + logfilename
            sys.exit(0)
        '''
        if not os.path.exists(geofilename):
            print "Geofence file doesn't exist: " + geofilename
            sys.exit(0)
        '''
    except NameError:
        print "file not provided"
        sys.exit(0)

    # Get geofences
    #print "Reading geofence"
    #GetGeofence(geofilename)

    #fence = fenceList[0]
    roof = args.roof # fence['roof']

    from matplotlib import pyplot as plt

    # Get flight data
    print "Reading log file to extract require data"
    mlog = mavutil.mavlink_connection(logfilename, notimestamps=args.notimestamps, zero_time_base=args.zero_time_base,
                                      dialect=args.dialect)

    timeshift = args.timeshift
    while True:
        msg = mlog.recv_match(blocking=False, type=["GLOBAL_POSITION_INT"])

        if mlog.percent >= 100:
            print "Completed log file playback successfully"
            break

        if msg is None: break
        try:
            tdays = matplotlib.dates.date2num(datetime.datetime.fromtimestamp(msg._timestamp + timeshift))
        except ValueError:
            # this can happen if the log is corrupt
            # ValueError: year is out of range
            break

        t.append(msg.time_boot_ms)
        lat.append(msg.lat/1E7)
        lon.append(msg.lon/1E7)
        alt.append(msg.relative_alt/1E3)
        vx.append(msg.vx/1E2)
        vy.append(msg.vy/1E2)
        vz.append(msg.vz/1E2)


    proximity = [roof - val for val in alt] # proximity

    plt.plot(t,proximity)
    plt.show()

    approach = True
    count = 0
    minVal = 100
    minProx = []
    record = True
    for i,val in enumerate(proximity):
        if i < len(proximity) -1:
            vi = proximity[i]
            vj = proximity[i+1]

            if vj > vi and record:
                minProx.append(vi)
                record = False
            elif vj < vi:
                record = True

    plt.figure(2)
    plt.plot(minProx)
    plt.show()

    minProx = [val for val in minProx if val < args.cap]

    pmean = np.mean(minProx)
    pstd  = np.std(minProx)

    print pmean,pstd

    return [pmean,pstd]


parser = ArgumentParser(description=__doc__)
parser.add_argument("--logfolder", help="mavlink log folder with NUM subfolders containing log files for each test")
parser.add_argument("--zero-time-base", action='store_true', help="use Z time base for DF logs")
parser.add_argument("--dialect", default="ardupilotmega", help="MAVLink dialect")
parser.add_argument("--no-timestamps", dest="notimestamps", action='store_true', help="Log doesn't have timestamps")
parser.add_argument("--timeshift", type=float, default=0, help="shift time on first graph in seconds")
parser.add_argument("--reverse", action='store_true',default=False, help ="reverse geofence vertices order if not in CCW")
parser.add_argument("--num", type=int, default=0, help = "num datapoints on X axis - starts from 1m/s upto NUM m/s")
parser.add_argument("--show", action="store_true", help="show plots")
parser.add_argument("--cap", type=float, default=5,help="maximum proximity value")
parser.add_argument("--virtual", action="store_false",help="use virtual edges for minimum proximity detection")
parser.add_argument("--roof",type=float, default=10, help="geofence roof")
parser.add_argument("-o","--output", help="output file name")
args = parser.parse_args()

data = []
for num in range(args.num):
    data.append(GetPerformanceData(num,args))

dataArray = np.array(data)
np.savetxt(args.output,dataArray)
