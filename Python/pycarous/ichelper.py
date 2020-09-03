import math
import random
import numpy as np

radius_of_earth = 6378100.0

def distance(lat1, lon1, lat2, lon2):
    '''return distance between two points in meters,
    coordinates are in degrees
    thanks to http://www.movable-type.co.uk/scripts/latlong.html'''
    lat1 = math.radians(lat1)
    lat2 = math.radians(lat2)
    lon1 = math.radians(lon1)
    lon2 = math.radians(lon2)
    dLat = lat2 - lat1
    dLon = lon2 - lon1

    a = math.sin(0.5*dLat)**2 + math.sin(0.5*dLon)**2 * math.cos(lat1) * math.cos(lat2)
    c = 2.0 * math.atan2(math.sqrt(a), math.sqrt(1.0-a))
    return radius_of_earth * c


def wrap_valid_longitude(lon):
    ''' wrap a longitude value around to always have a value in the range
        [-180, +180) i.e 0 => 0, 1 => 1, -1 => -1, 181 => -179, -181 => 179
    '''
    return (((lon + 180.0) % 360.0) - 180.0)

def gps_newpos(lat, lon, bearing, distance):
    '''extrapolate latitude/longitude given a heading and distance
    thanks to http://www.movable-type.co.uk/scripts/latlong.html
    '''
    lat1 = math.radians(lat)
    lon1 = math.radians(lon)
    brng = math.radians(bearing)
    dr = distance/radius_of_earth

    lat2 = math.asin(math.sin(lat1)*math.cos(dr) +
                     math.cos(lat1)*math.sin(dr)*math.cos(brng))
    lon2 = lon1 + math.atan2(math.sin(brng)*math.sin(dr)*math.cos(lat1),
                             math.cos(dr)-math.sin(lat1)*math.sin(lat2))
    return (math.degrees(lat2), wrap_valid_longitude(math.degrees(lon2)))

def gps_offset(lat, lon, east, north):
    '''return new lat/lon after moving east/north
    by the given number of meters'''
    bearing = math.degrees(math.atan2(east, north))
    distance = math.sqrt(east**2 + north**2)
    return gps_newpos(lat, lon, bearing, distance)

def ConvertVnedToTrkGsVs(vn,ve,vz):
    angle = 360 + np.arctan2(ve,vn) * 180/np.pi
    trk = np.fmod(angle,360)
    gs = np.sqrt(vn**2 + ve**2)
    vs = -vz
    return (trk,gs,vs)

def ComputeHeading(start,end):
    relvec = [end[0]-start[0],end[1]-start[1],end[2]-start[2]]
    (trk,gs,vs) = ConvertVnedToTrkGsVs(relvec[1],relvec[0],-relvec[2])
    return trk

def ConvertTrkGsVsToVned(trk,gs,vs):
    vy = gs * np.cos(trk*np.pi/180)
    vx = gs * np.sin(trk*np.pi/180)
    vz = -vs
    return (vy,vx,vz)

def GetInitialConditions():
    theta = random.randint(90,270)
    intercept = random.randint(0,20)
    return GetInitialConditions2(theta,intercept)

def GetInitialConditions2(theta,intercept):
    point = (100, intercept)
    thetaR = np.mod(360 + theta + 180,360)

    pointS = (point[0] + 150 * np.sin(thetaR*np.pi/180),point[1] + 150 * np.cos(thetaR*np.pi/180))
    _rangeO = 100
    timeO = _rangeO/1
    _rangeA = np.linalg.norm(pointS)
    _bearingA = np.mod(2*np.pi + np.arctan2(pointS[0], pointS[1]), 2*np.pi) * 180/np.pi
    _altA = 5
    _headingA = theta
    _speedA = 150/timeO
    _climbRateA = 0
    return (_rangeA, _bearingA, _altA, _speedA, _headingA, _climbRateA)

def LoadIcarousParams(filename):
    '''load parameters from a file'''
    try:
        f = open(filename, mode='r')
    except (IOError, TypeError):
        print("Failed to open file '%s'" % filename)
        return
    param = {}
    for line in f:
        line = line.replace('=',' ')
        line = line.strip()
        if not line or line[0] == "#":
            continue
        a = line.split()
        if len(a) < 1:
            print("Invalid line: %s" % line)
            continue
        param[a[0]] = float(a[1])

    return param

def ReadFlightplanFile(filename):
    '''
    Read data from a waypoint file
    :param filename: name of the waypoint file
    :return: (wp, wp_ind, wp_speed), wp is list of waypoints, wp_ind is list
    of param4 values for each wp, wp_speed is list of speed for each wp
    '''
    try:
        f = open(filename,mode='r')
    except (IOError,TypeError):
        print("Failed to open file '%s'" % filename)
        return

    wp = []
    wp_ind = []
    wp_speed = []
    speed = -1
    line='#'
    while line != '':
        line = f.readline()
        lc = line.replace(' ','\t').split('\t')
        if len(lc) < 10:
            continue

        wplist = [elem for elem in lc if elem != '']

        if int(wplist[3]) == 178:
            speed = float(wplist[5])

        if int(wplist[3]) == 16:
            wp.append([float(wplist[8]),float(wplist[9]),float(wplist[10])])
            wp_ind.append(float(wplist[7]))
            wp_speed.append(speed)

    return wp,wp_ind,wp_speed

def GetFlightplanFile(filename, defaultWPSpeed=1, scenarioTime=0, eta=False):
    '''
    Read a flight plan from a waypoint file
    :param filename: name of the waypoint file
    :param defaultWPSpeed: default speed if no speed commands given in the file
    :param scenarioTime: start time of the scenario (s)
    :param eta: when True, ICAROUS enforces wp arrival times, wp_metric (s)
                when False, ICAROUS sets speed to each wp, wp_metric (m/s)
    :return: a list of waypoints [lat, lon, alt, wp_metric]
    '''
    wp, time, speed = ReadFlightplanFile(filename)
    fp = []
    if eta:
        combine = time
    else:
        combine = speed
    for w, i in zip(wp, combine):
        if not eta and i < 0:
            i = defaultWPSpeed
        fp.append(w + [i])
    return fp

def GetWindComponent(windFrom,windSpeed,NED=True):
    windTo = np.mod(360 + windFrom + 180,360) # Wind towards heading with respect to true north
    vw_y   = np.cos(windTo * np.pi/180) * windSpeed
    vw_x   = np.sin(windTo * np.pi/180) * windSpeed
    if NED:
        vw     = np.array([vw_y,vw_x,0])
    else:
        vw     = np.array([vw_x,vw_y,0])
    return vw

def GetHomePosition(filename):
    wp,_,_ = ReadFlightplanFile(filename)
    return [wp[0][0],wp[0][1],0]

def ReadTrafficInput(filename):
    try:
        f = open(filename,mode='r')
    except (IOError,TypeError):
        print("Failed to open file '%s'" % filename)
        return
    
    tfinput = []
    # Read the comment line
    line = f.readline()
    while line != '':
        line = f.readline()
        if line != '':
            lc = line.replace(' ','').split(',')
            tfinput.append([int(lc[0])] + [float(i) for i in lc[1:]])

    return tfinput
