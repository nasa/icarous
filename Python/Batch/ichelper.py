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

def ConvertVnedToTrkGsVs(ve,vn,vz):
    angle = 360 + np.arctan2(ve,vn) * 180/np.pi
    trk = np.fmod(angle,360)
    gs = np.sqrt(vn**2 + ve**2)
    vs = vz
    return (trk,gs,vs)

def ComputeHeading(start,end):
    relvec = [end[0]-start[0],end[1]-start[1],end[2]-start[2]]
    (trk,gs,vs) = ConvertVnedToTrkGsVs(relvec[0],relvec[1],relvec[2])
    return trk

def ConvertTrkGsVsToVned(trk,gs,vs):
    vy = gs * np.cos(trk*np.pi/180)
    vx = gs * np.sin(trk*np.pi/180)
    vz = vs
    return (vx,vy,vz)

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


