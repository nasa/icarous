import numpy as np
from numpy import sin,cos
import xml.etree.ElementTree as ET

def save_geofence(filename):

    top = ET.Element('Geofence')

    for fence in geofenceList:
        #print "constructing"
        child1   = ET.SubElement(top,'fence',id=str(fence['id']))
        child1_1 = ET.SubElement(child1,'type')
        child1_1.text = str(fence['type'])
        child1_2 = ET.SubElement(child1,'num_vertices')
        child1_2.text = str(fence['numV'])
        child1_3 = ET.SubElement(child1,'floor')
        child1_3.text = str(fence['floor'])
        child1_4= ET.SubElement(child1,'roof')
        child1_4.text = str(fence['roof'])
        for vertex in range(len(fence['Vertices'])):
            child1_5 = ET.SubElement(child1,'vertex',id=str(vertex))
            child1_5_1 = ET.SubElement(child1_5,'lat')
            child1_5_1.text = str(fence['Vertices'][vertex][0])
            child1_5_2 = ET.SubElement(child1_5,'lon')
            child1_5_2.text = str(fence['Vertices'][vertex][1])


    #print "writing"
    ET.ElementTree(top).write(filename)

def MakeGeofence(id,type,numV,floor,roof,vertices):

    Vertices = []
    for i in range(4):
        Vertices.append((vertices[i,0],vertices[i,1]))

    Geofence = {'id': id, 'type': type, 'numV': numV, 'floor': floor,
    'roof': roof, 'Vertices': Vertices}

    geofenceList.append(Geofence)

def ECEF2LLA(origin,vertices):

    R = 6371000  # radius of earth
    oLat = origin[0]*np.pi/180
    oLon = origin[1]*np.pi/180
    oAlt = 0

    # convert given positions from geodetic coordinate frame to ECEF
    oX = (R + oAlt) * cos(oLat) * cos(oLon)
    oY = (R + oAlt) * cos(oLat) * sin(oLon)
    oZ = (R + oAlt) * sin(oLat)

    verticesLLA = np.zeros((4,2))

    for i in range(4):
        pX = oX + vertices[i,0]
        pY = oY + vertices[i,1]

        pLon = np.arctan2(pY,pX)
        pLat = np.arccos(pX/(R*cos(pLon)))

        pLon = pLon*180/np.pi
        pLat = pLat*180/np.pi

        verticesLLA[i,0] = pLat
        verticesLLA[i,1] = pLon

    return verticesLLA

data = np.loadtxt('boxesKlondike.txt',dtype='string')
numFences = data.shape[0]

FenceList = []
geofenceList = []

for i in range(numFences):
    
    plate1 = []
    plate2 = []
    bz     = 0

    fence = np.zeros((8,3))
    for j in range(8):
        Vertices = data[i][j]
        Vertices = Vertices.split(',')
        x = float(Vertices[0])
        y = float(Vertices[1])
        z = float(Vertices[2])

        fence[j,0] = x
        fence[j,1] = y
        fence[j,2] = z

    #import pdb; pdb.set_trace()
    
    order = np.argsort(fence[:,2])
    fenceS = fence[order]

    fence1 = fenceS[0:4,:]
    fence2 = fenceS[4:8,:]
    
    order = np.argsort(fence1[:,1])
    fence1s = fence1[order]
    
    order = np.argsort(fence2[:,1])
    fence2s = fence2[order]

    fenceO = np.zeros((8,3))
    fenceO[0:4,:] = fence1s
    fenceO[4:8,:] = fence2s

    FenceList.append(fenceO)

    origin = [37.1021769,-76.3872069]

    fenceCCW = np.zeros((4,3))
    fenceCCW[0:2,:] = fence1s[0:2,:]
    fenceCCW[2,:]   = fence1s[3,:]
    fenceCCW[3, :] = fence1s[2, :]

    verticesLLA = ECEF2LLA(origin,fenceCCW)

    MakeGeofence(i,1,4,0,100,verticesLLA)

save_geofence('gfdataset.xml')