import numpy as np

data = np.loadtxt('boxes2Towers.txt',dtype='string')

numFences = data.shape[0]



FenceList = []

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
        
            
        
