import sys
import time
import os
from pymavlink import mavwp
import importlib

sys.path.append("../Batch")
from Icarous import *


sim_home = os.getcwd()
icarous_home = os.path.abspath(os.path.join(sim_home, "../.."))
icarous_exe = os.path.join(icarous_home, "exe", "cpu1")


def SetupPyIcarous(scenario):
    """ Set up a python Icarous to run a given scenario """

    # Read flight plan
    wploader = mavwp.MAVWPLoader()
    wploader.load(os.path.join(icarous_home, scenario["waypoint_file"]))
    waypoints = [(wp.x, wp.y, wp.z, 1) for wp in wploader.wpoints if wp.command == 16]
    waypoints[0] = (waypoints[0][0], waypoints[0][1], waypoints[0][2], 0)
    HomePos = waypoints[0][:3]

    # Start Icarous
    ic = Icarous(HomePos)
    ic.setpos_uncertainty(0.01, 0.01, 0, 0, 0, 0)

    # Load flight plan
    ic.InputFlightplan(waypoints, 0)

    # Load geofence
    if scenario.get("geofence_file"):
        ic.InputGeofence(os.path.join(icarous_home, scenario["geofence_file"]))

    # Load parameters
    params = LoadIcarousParams(os.path.join(icarous_home, scenario["parameter_file"]))
    if scenario.get("param_adjustments"):
        params.update(scenario["param_adjustments"])
    ic.SetParameters(params)

    # Load traffic
    tfList = []
    for tf in scenario["traffic"]:
        StartTraffic(HomePos, tf[0], tf[1], tf[2], tf[3], tf[4], tf[5], tfList)
    for tf in tfList:
        tf.setpos_uncertainty(0.01, 0.01, 0, 0, 0, 0)

    return ic, tfList

def RunPyIcarous(ic, tfList):
    while not ic.CheckMissionComplete():
        status = ic.Run()
        if not status:
            continue

        RunTraffic(tfList)
        for i, tf in enumerate(tfList):
            ic.InputTraffic(i, tf.pos_gps, tf.vel)


if __name__ == "__main__":
    import argparse
    import yaml
    from matplotlib import pyplot as plt
    parser = argparse.ArgumentParser(description="Run Icarous test scenarios")
    parser.add_argument("scenario", help="yaml file containing scenario(s) to run ")
    parser.add_argument("--num", default="", help="If multiple scenarios in file, just run this one")
    args = parser.parse_args()

    # Load scenarios
    if os.path.isfile(args.scenario):
        f = open(args.scenario, 'r')
        scenario_list = yaml.load(f, Loader=yaml.FullLoader)
        f.close()
    else:
        scenario_list = [json.loads(args.scenario)]

    if args.num:
        scenario_list = [scenario_list[int(args.num)]]

    for i, scenario in enumerate(scenario_list):
        print("****"+scenario["name"]+"****")
        ic, tfList = SetupPyIcarous(scenario)
        RunPyIcarous(ic, tfList)

        plt.figure(1)
        plt.plot(np.array(ic.positionLog)[:, 0], np.array(ic.positionLog)[:, 1], 'r')
        for tf in tfList:
            plt.plot(np.array(tf.log['pos'])[:, 0], np.array(tf.log['pos'])[:, 1], 'b')
        plt.show()
