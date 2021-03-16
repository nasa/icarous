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
    (trk,gs,vs) = ConvertVnedToTrkGsVs(relvec[0],relvec[1],-relvec[2])
    return trk

def ComputeDistanceNED(start,end):
    relvec = [end[0]-start[0],end[1]-start[1],end[2]-start[2]]
    return np.linalg.norm(relvec)

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
    wp_tcp = []
    wp_tcpvalue = []
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
            wp_tcp.append([int(float(wplist[5])),0,0])
            wp_tcpvalue.append([float(wplist[6]),0,0])
            wp_speed.append(speed)

    return wp,wp_ind,wp_speed,wp_tcp,wp_tcpvalue

def GetFlightplan(filename, defaultWPSpeed=1, eta=False):
    '''
    Read a flight plan from a waypoint file
    :param filename: name of the waypoint file
    :param defaultWPSpeed: default speed if no speed commands given in the file
    :param scenarioTime: start time of the scenario (s)
    :param eta: when True, ICAROUS enforces wp arrival times, wp_metric (s)
                when False, ICAROUS sets speed to each wp, wp_metric (m/s)
    :return: a list of waypoints [lat, lon, alt, wp_metric, tcp, tcp_value]
    '''
    wp, time, speed, tcp, tcpValue = ReadFlightplanFile(filename)
    fp = []
    if eta:
        combine = time
    else:
        combine = speed

    count = 0
    for w, i in zip(wp, combine):
        if not eta and i < 0:
            i = defaultWPSpeed
        fp.append(w + [i]+[tcp[count]] + [tcpValue[count]])
        count+=1
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

def GetHomePosition(filename,id=0):
    import re
    match = re.search('\.eutl',filename)
    if match is not None:
        wps,totalwps = GetEUTLPlanFromFile(filename,id) 
        return [wps[0].latitude,wps[0].longitude,0]
    else:
        wp,_,_,_,_ = ReadFlightplanFile(filename)
        return [wp[0][0],wp[0][1],0]

def ReadTrafficInput(filename):
    import yaml
    try:
        f = open(filename,mode='r')
    except (IOError,TypeError):
        print("Failed to open file '%s'" % filename)
        return
    tfinput =  yaml.load(f,yaml.Loader)
    return tfinput

def ConvertToLocalCoordinates(home_pos,pos):
    try:
        from AccordUtil import ConvertLLA2NED
        return ConvertLLA2NED(home_pos,pos)
    except:
        dh = lambda x, y: abs(distance(home_pos[0],home_pos[1],x,y))
        if pos[1] > home_pos[1]:
            sgnX = 1
        else:
            sgnX = -1

        if pos[0] > home_pos[0]:
            sgnY = 1
        else:
            sgnY = -1
        dx = dh(home_pos[0],pos[1])
        dy = dh(pos[0],home_pos[1])
        return [dy*sgnY,dx*sgnX,pos[2]]

def Getfence(filename):
    import re,yaml
    match = re.match('.*\.ya?ml',filename)
    if match is None:
        '''add geofences from a file'''
        import xml.etree.ElementTree as ET
        tree = ET.parse(filename)
        root = tree.getroot()        
        fenceList = []    
        for child in root:
            id    = int(child.get('id'));
            type  = child.find('type').text;
            numV  = int(child.find('num_vertices').text);            
            floor = float(child.find('floor').text);
            roof  = float(child.find('roof').text);
            Vertices = [];
        
            if(len(child.findall('vertex')) >= numV):        
                for vertex in child.findall('vertex'):
                    coord = (float(vertex.find('lat').text),
                            float(vertex.find('lon').text))
                
                    Vertices.append(coord)

            # Check geofence niceness
            Geofence = {'id':id,'type': type,'numV':numV,'floor':floor,
                        'roof':roof,'vertices':Vertices[:numV]}
            fenceList.append(Geofence)
    else:
        fenceList = yaml.load(open(filename),yaml.Loader)
    return fenceList

def ConstructWaypointsFromList(fp,eta=False):
    from CustomTypes import Waypoint,TcpType
    speeds = []
    times  = []
    waypoints = []
    tcp = False
    if eta:
        times = [wp[3] for wp in fp]
    else:
        speeds = [wp[3] for wp in fp]

    for i in range(len(fp)):
        if not eta:
            if i == 0:
                times.append(0)
            else:
                distH = distance(fp[i-1][0],fp[i-1][1],fp[i][0],fp[i][1])
                distV = np.fabs(fp[i][2] - fp[i-1][2])
                if distH > 1e-3:
                    times.append(times[-1] + distH/speeds[i]) 
                else:
                    times.append(times[-1] + distV/2.0)
        
        wp = Waypoint()
        wp.index = i
        wp.time = times[i] 
        wp.latitude = fp[i][0]
        wp.longitude = fp[i][1]
        wp.altitude = fp[i][2]
        if len(fp[i]) > 4:
            tcp = True
        if tcp:
            wp.tcp[0] = fp[i][4][0]
            wp.tcp[1] = fp[i][4][1]
            wp.tcp[2] = fp[i][4][2]
            wp.tcpValue[0] = fp[i][5][0]
            wp.tcpValue[1] = fp[i][5][1]
            wp.tcpValue[2] = fp[i][5][2]
        else:
            wp.tcp[0] = TcpType.TCP_NONE 
            wp.tcp[1] = TcpType.TCP_NONEg
            wp.tcp[2] = TcpType.TCP_NONEv
        
        waypoints.append(wp)
    return waypoints

def ParseDaidalusConfiguration(filename):
    from ctypes import Structure, CDLL, c_char_p, c_int, c_char, c_double
    lib = CDLL('libUtils.so')
    class ParsedParam(Structure):
        _fields_=[
            ("key",c_char*50),
            ("valueString",c_char*50),
            ("unitString",c_char*10),
            ("value",c_double)
        ]

    lib.ParseParameterFile.argtypes = [c_char_p,ParsedParam*500]
    lib.ParseParameterFile.restype = c_int

    ParamsArray = ParsedParam * 500
    params = ParamsArray()
    n = lib.ParseParameterFile(c_char_p(filename.encode('utf-8')),params)
    DaidalusParam = {}
    for param in params:
        key = param.key.decode('utf-8')
        if key != "":
            DaidalusParam[key] =[param.value, param.valueString.decode('utf-8')]

    return DaidalusParam

def GetEUTLPlanFromFile(filename,index,linearize=False):
    from ctypes import CDLL,c_char_p,c_int,c_bool
    from CustomTypes import Waypoint,TcpType
    lib = CDLL('libUtils.so')
    wpArray = Waypoint*100
    wps = wpArray()
    lib.GetEUTLPlanFromFile.argtypes = [c_char_p,c_int,wpArray,c_bool]
    lib.GetEUTLPlanFromFile.restype  = c_int
    n = lib.GetEUTLPlanFromFile(c_char_p(filename.encode('utf-8')),index,wps,c_bool(linearize))
    return wps,n

def GetPlanPositions(waypoints,timeDelta):
    from ctypes import CDLL,c_char_p,c_int,c_bool,c_double,c_void_p
    from CustomTypes import Waypoint,TcpType
    lib = CDLL('libUtils.so')
    wpArray = Waypoint*100
    pos = (c_double*3)()
    wps = wpArray()
    timeStart = waypoints[0][0]
    timeEnd   = waypoints[-1][0]
    lenWPs = len(waypoints)
    lib.GetPlanPosition.argtypes = [c_void_p,wpArray,c_int,c_double,c_double*3]
    lib.GetPlanPosition.restype = c_void_p
    
    for i,wp in enumerate(waypoints):
        wps[i].time = wp[0]
        wps[i].latitude = wp[1]
        wps[i].longitude = wp[2]
        wps[i].altitude = wp[3]
        wps[i].tcp[0] = wp[4]
        wps[i].tcp[1] = wp[5]
        wps[i].tcp[2] = wp[6]
        wps[i].tcpValue[0] = wp[7]
        wps[i].tcpValue[1] = wp[8]
        wps[i].tcpValue[2] = wp[9]
    
    positions = []
    
    t = timeStart
    plan = c_void_p()
    while t < timeEnd:
       plan = lib.GetPlanPosition(plan,wps,c_int(lenWPs),c_double(t),pos)
       positions.append([pos[0],pos[1],pos[2]])
       t = t + 0.25

    return positions

    
    
  



