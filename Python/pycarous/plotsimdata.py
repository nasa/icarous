#! /usr/bin/env python3

import json
import glob
import os
import numpy as np
from matplotlib import pyplot as plt


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
        log["callsign"] = os.path.basename(f).strip(".json")
        logs.append(log)
os.chdir(output_dir)


traffic = False
plt.figure()
for log in logs:
    pos = np.array(log["ownship"]["positionNED"])
    plt.plot(pos[:, 1], pos[:, 0], label=log["callsign"])
    if traffic:
        for tlog in log["traffic"].values():
            tpos = np.array(tlog["positionNED"])
            plt.plot(tpos[:, 1], tpos[:, 0], '.', label="traffic "+log["callsign"])
plt.legend()
plt.xlabel("X (m)"); plt.ylabel("Y (m)")
plt.axis('equal')
plt.savefig("simplot.png")

plt.figure()
for log in logs:
    pos = np.array(log["ownship"]["positionNED"])
    plt.plot(log["ownship"]["t"], pos[:, 2], label=log["callsign"])
    if traffic:
        for tlog in log["traffic"].values():
            tpos = np.array(tlog["positionNED"])
            plt.plot(tlog["t"], tpos[:, 2], '.', label="traffic "+log["callsign"])
plt.legend()
plt.xlabel("Time (s)"); plt.ylabel("Altitude (m)")
plt.savefig("alt.png")

plt.figure()
for log in logs:
    vel = np.array(log["ownship"]["velocityNED"])
    gs = [np.sqrt(vx**2 + vy**2) for vx,vy,vz in vel]
    plt.plot(log["ownship"]["t"], gs, label=log["callsign"])
plt.legend()
plt.xlabel("Time (s)"); plt.ylabel("Ground Speed (m/s)")
plt.savefig("speed.png")
plt.show()
