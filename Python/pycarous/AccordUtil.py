from ctypes import *
from Interfaces import TcpType
import numpy as np

libUtils = CDLL('libUtils.so')

libUtils.ConvertLLA2END.argtypes = [c_double*3,c_double*3,c_double*3]
libUtils.ConvertEND2LLA.argtypes = [c_double*3,c_double*3,c_double*3]
libUtils.ComputeHeading.argtypes = [c_double*3,c_double*3]
libUtils.ComputeHeading.restype = c_double

libUtils.ComputeDistance.argtypes = [c_double*3,c_double*3]
libUtils.ComputeDistance.restype = c_double
pos = c_double*3

def ConvertLLA2NED(posA,posB):
    home  = pos(*posA)
    query = pos(*posB)
    output = pos(0,0,0)
    libUtils.ConvertLLA2END(home,query,output)
    return [output[1],output[0],output[2]]

def ConvertNED2LLA(posA,posB):
    home  = pos(*posA)
    query = pos(*posB)
    output = pos(0,0,0)
    libUtils.ConvertEND2LLA(home,query,output)
    return [output[1],output[0],output[2]]


def ComputeHeading(posA,posB):
    home  = pos(*posA)
    query = pos(*posB)
    return libUtils.ComputeHeading(home,query)

def ComputeDistance(posA,posB):
    home  = pos(*posA)
    query = pos(*posB)
    return libUtils.ComputeDistance(home,query)


def plotTcpPlan(flightplan,tcp,tcpValue,local=True):
    '''
    Generate a set of points representing the track of the flightplan
    flightplan : list of [lat,lon,alt,time]. Units: [deg,deg,m,s]
    tcp        : list of tuples of trajectory change point types 
    tcpValue   : list of tuples of tcp value
    local      : True if coordinates are in NED frame/False if GPS coordinates
    returns (N,E,D,ptN,ptE,ptD)
      N,E corresponds to North and East coordinates of track
      ptN,ptE corresponds to North and East coordinats of waypoints
    '''
    N = []
    E = []
    D = []
    ptN = []
    ptE = []
    ptD = []
    vs = 0.0
    gs = 0.0 
    heading = 0.0 
    haccel = 0.0
    vaccel = 0.0
    t = 0.0
    dt = 0.001

    if local:
        import ichelper
        getDistance = ichelper.ComputeDistanceNED
        getHeading  = ichelper.ComputeHeading
        getLocal    = lambda xa,xb: xb
    else:
        getDistance = ComputeDistance
        getHeading  = ComputeHeading
        getLocal    = ConvertLLA2NED

    dist = getDistance(flightplan[0][1:4],flightplan[1][1:4])
    distV = flightplan[1][3] - flightplan[0][3]
    if dist - distV > 1e-3:
        gs = dist/(flightplan[1][0] - flightplan[0][0])
    else:
        # if the first two waypoints indicate a direct climb, set gs = 0
        gs = 0

    heading = getHeading(flightplan[0][1:4],flightplan[1][1:4])*np.pi/180

    if distV > 0:
        vs = 2
    else:
        vs = 0

    pos = flightplan[0][1:4]
    turnRate,haccel,vaccel = 0.0,0.0,0.0
    center = np.array([0,0,0])
    radius = 0
    for i in range(len(flightplan)):
        if i == 0:
            posl = getLocal(pos,pos)
            ptN.append(posl[0])
            ptE.append(posl[1])
            ptD.append(posl[2])
            N.append(posl[0])
            E.append(posl[1])
            D.append(posl[2])
            continue

        posAg = flightplan[i-1][1:4]
        posBg = flightplan[i][1:4]
        posAl = getLocal(pos,posAg)
        posBl = getLocal(pos,posBg)
        tlimit = flightplan[i][0]

        
        ptN.append(posBl[0])
        ptE.append(posBl[1])
        ptD.append(posBl[2])
        timeA = flightplan[i-1][0]
        timeB = flightplan[i][0]

        if tcp[i-1][0] == TcpType.TCP_BOT or tcp[i-1][0] == TcpType.TCP_MOT or tcp[i-1][0] == TcpType.TCP_EOTBOT: 
            radius = tcpValue[i-1][0]
            turnRate = gs/radius
            perpHeading = np.pi/2 if radius > 0 else -np.pi/2
            centerHeading = heading + perpHeading
            center = np.array(posAl) + np.fabs(radius)*np.array([np.cos(centerHeading),np.sin(centerHeading),0])
        elif tcp[i-1][0] == TcpType.TCP_EOT:
            turnRate = 0.0

        if tcp[i-1][1] == TcpType.TCP_BGS or tcp[i-1][1] == TcpType.TCP_EGSBGS:
            haccel = tcpValue[i-1][1] 
        elif tcp[i-1][1] == TcpType.TCP_EGS:
            haccel = 0.0

        if tcp[i-1][2] == TcpType.TCP_BVS or tcp[i-1][1] == TcpType.TCP_EVSBVS:
            vaccel = tcpValue[i-1][2]
        elif tcp[i-1][2] == TcpType.TCP_EVS:
            vaccel = 0.0

        tstep = (timeB-timeA)/100
        tspan = np.arange(timeA,timeB+tstep,tstep)
        for t in tspan:
            dt = t - timeA
            if np.fabs(turnRate)>1e-8:
                newheading = heading + turnRate*dt
                newPos = center + np.fabs(radius)*np.array([np.cos(newheading-perpHeading),np.sin(newheading-perpHeading),0])
            else:
                newL   = gs*dt + 0.5*haccel*dt**2
                newPos = np.array(posAl) + newL*np.array([np.cos(heading),np.sin(heading),0.0])

            newPos[2] = newPos[2] + vs*dt + 0.5*vaccel*dt**2 
            N.append(newPos[0])
            E.append(newPos[1])
            D.append(newPos[2])
        heading = heading + turnRate*(timeB-timeA)
        gs = gs + haccel*(timeB-timeA)
        vs = vs + vaccel*(timeB-timeA)


        """ 
        while t <= tlimit:
            gs += haccel*dt
            vs += vaccel*dt
            heading += turnRate *  dt
            heading = np.fmod(heading,2*np.pi)
            N.append(N[-1] + gs*np.cos(heading) * dt)
            E.append(E[-1] + gs*np.sin(heading) * dt)
            D.append(D[-1] + vs*dt)
            t += dt
        """

        
    return (N,E,D,ptN,ptE,ptD)