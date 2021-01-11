#!/usr/bin/env python3

import json
import glob
import os
import numpy as np
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


import argparse
parser = argparse.ArgumentParser(description="Plot Icarous log")
parser.add_argument("logfile", help="Icarous json log file or directory containing multiple json logs")
args = parser.parse_args()


files = []
output_dir = "."
if os.path.isfile(args.logfile):
    files.append(args.logfile)
elif os.path.isdir(args.logfile):
    path =  args.logfile.rstrip('/')+'/*.json'
    files = glob.glob(path)
    output_dir = args.logfile
logs = []
for f in files:
    with open(f) as fx:
        log = json.load(fx)
    logs.append(log)
os.chdir(output_dir)


traffic = False
plt.figure()
for log in logs:
    pos = np.array(log["state"]["positionNED"])
    plan = np.array(log["flightplans_local"][0])
    plt.plot(pos[:, 1], pos[:, 0], label=log["callsign"])
    plt.plot(plan[:, 2], plan[:, 1], '--o')
    if traffic:
        for tlog in log["traffic"].values():
            tpos = np.array(tlog["positionNED"])
            plt.plot(tpos[:, 1], tpos[:, 0], '.', label="traffic "+log["callsign"])
plt.legend()
plt.xlabel("X (m)"); plt.ylabel("Y (m)")
plt.axis('equal')
plt.savefig("simplot.png")

fig = plt.figure()
ax = fig.add_subplot(111,projection='3d')
for log in logs:
    pos = np.array(log["state"]["positionNED"])
    plan = np.array(log["flightplans_local"][0])
    tcps = plan[:,4:7]
    tcpValues = plan[:,7:10]
    if np.sum(tcps) > 0:
        from AccordUtil import plotTcpPlan
        n,e,d,ptn,pte,ptd = plotTcpPlan(plan,tcps,tcpValues)
        ax.plot(e, n, d,'r')
    ax.plot(pos[:, 1], pos[:, 0], pos[:,2],'b')
plt.legend()
ax.set_xlabel("X (m)")
ax.set_ylabel("Y (m)")
ax.set_zlabel("Z (m)")

plt.figure()
for log in logs:
    pos = np.array(log["state"]["positionNED"])
    plt.plot(log["state"]["time"], pos[:, 2], label=log["callsign"])
    if traffic:
        for tlog in log["traffic"].values():
            tpos = np.array(tlog["positionNED"])
            plt.plot(tlog["time"], tpos[:, 2], '.', label="traffic "+log["callsign"])
plt.legend()
plt.xlabel("Time (s)"); plt.ylabel("Altitude (m)")
plt.savefig("alt.png")

plt.figure()
for log in logs:
    vel = np.array(log["state"]["velocityNED"])
    gs = [np.sqrt(vx**2 + vy**2) for vx,vy,vz in vel]
    plt.plot(log["state"]["time"], gs, label=log["callsign"])
plt.legend()
plt.xlabel("Time (s)"); plt.ylabel("Ground Speed (m/s)")
plt.savefig("speed.png")

plt.figure()
for log in logs:
    offsets = np.array(log["state"]["planOffsets"])
    plt.plot(log["state"]["time"], offsets[:,0], label=log["callsign"])
plt.legend()
plt.xlabel("Time (s)"); plt.ylabel("offset (m)")
plt.savefig("offset.png")
plt.show()
