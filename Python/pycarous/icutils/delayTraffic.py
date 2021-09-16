import numpy as np
import sys


def main(range,bearing,alt,speed,heading,vs,delay):
    bearing = 90 - bearing
    heading = 90 - heading

    # %%
    origVector = np.array([range*np.cos(bearing*np.pi/180),
                           range*np.sin(bearing*np.pi/180),
                           alt])

    delaylenXY = speed * delay 
    delaylenZ  = -vs * delay 
    delayVector = np.array([delaylenXY*np.cos(heading*np.pi/180 + np.pi),
                            delaylenXY*np.sin(heading*np.pi/180 + np.pi),
                            delaylenZ])

    delayPos = origVector + delayVector

    newRange = np.linalg.norm(delayPos)
    newBearing = np.fmod(2*np.pi + np.pi/2 - np.arctan2(delayPos[1],delayPos[0]),2*np.pi) * 180/np.pi
    newAlt = delayPos[2]
    print("For a delay of %f, range = %f, bearing = %f, alt = %f\n"%(delay,newRange,newBearing,newAlt))
    # %%

if __name__ == "__main__":
    range   = float(sys.argv[1])
    bearing = float(sys.argv[2])
    alt     = float(sys.argv[3])
    speed   = float(sys.argv[4])
    heading = float(sys.argv[5])
    vs      = float(sys.argv[6])
    delay   = float(sys.argv[7])
    main(range,bearing,alt,speed,heading,vs,delay)
