import os
import sys
import yaml
import json
import time
import datetime
import shutil
import subprocess

sys.path.append("../pyicarous")
import BatchGSModule as GS
from SimVehicle import SimVehicle
from Icarous import (Icarous, ReadFlightplanFile, LoadIcarousParams,
                    StartTraffic, RunSimulation)

sim_home = os.getcwd()
icarous_home = os.path.abspath(os.path.join(sim_home, "../.."))
icarous_exe = os.path.join(icarous_home, "exe", "cpu1")


def SetApps(sitl=False, merger=False):
    '''set the apps that ICAROUS will run'''
    if sitl:
        sim_app = "arducopter"
    else:
        sim_app = "rotorsim"

    app_list = ["Icarouslib","port_lib", "scheduler", sim_app, "gsInterface",
                "cognition", "guidance", "traffic", "trajectory", "geofence"]
    if merger:
        app_list += ["merger", "raft", "SBN", "udp"]

    approot = os.path.join(icarous_home, "apps")
    outputloc = os.path.join(icarous_exe, "cf")

    subprocess.call(["python3", "../cFS_Utils/ConfigureApps.py",
                     approot, outputloc, *app_list])


def RunScenario(scenario, sitl=False, verbose=False, out=14557,
                use_threads=True, output_dir="sim_output"):
    """ Run an ICAROUS scenario using cFS """
    # Copy merge_fixes file to exe/ram
    if "merge_fixes" in scenario:
        merge_fixes = os.path.join(icarous_home, scenario["merge_fixes"])
        ram_directory = os.path.join(icarous_home, "exe", "ram")
        shutil.copy(merge_fixes, ram_directory)

    # Clear existing log files
    for f in os.listdir(icarous_exe):
        if f.endswith(".txt") or f.endswith(".log"):
            os.remove(os.path.join(icarous_exe, f))

    # Set up each vehicle
    vehicles = []
    for vehicle_scenario in scenario["vehicles"]:
        out_port = out
        if out_port is not None:
            out_port = out + (vehicle_scenario.get("cpu_id", 1) - 1)*10
        v = SimVehicle(vehicle_scenario, verbose=verbose, out=out_port,
                       output_dir=output_dir, sitl=sitl)
        vehicles.append(v)

    # Run the simulation
    t0 = time.time()
    time_limit = scenario["time_limit"]
    duration = 0
    if use_threads:
        from threading import Thread
        threads = [Thread(target=v.run, args=[time_limit]) for v in vehicles]
        for t in threads:
            t.start()

    finished = False
    while not finished:
        if not use_threads:
            for v in vehicles:
                finished |= v.step(time_limit)
        else:
            finished = duration > time_limit
            time.sleep(0.1)
        if verbose:
            print("Sim Duration: %.1fs       " % duration, end="\r")
        duration = time.time() - t0
    for v in vehicles:
        v.terminate()
    if verbose:
        print("Simulation finished")

    # Shut down SITL if necessary
    if sitl:
        subprocess.call(["pkill", "-9", "arducopter"])
        subprocess.call(["pkill", "-9", "mavproxy"])

    # Collect log files
    for f in os.listdir(icarous_exe):
        if f.endswith(".txt") or f.endswith(".log"):
            os.rename(os.path.join(icarous_exe, f), os.path.join(output_dir, f))
    for f in os.listdir(output_dir):
        if f.endswith("tlog.raw"):
            os.remove(os.path.join(output_dir, f))


def RunScenarioPy(scenario, verbose=False, output_dir="sim_output"):
    """ Run an ICAROUS scenario using pyIcarous """
    os.chdir(output_dir)

    num_vehicles = len(scenario["vehicles"])
    icInstances = []
    tfList = []
    icDelay = []
    for v_scenario in scenario["vehicles"]:
        cpu_id = v_scenario.get("cpu_id", 1)
        spacecraft_id = cpu_id - 1
        waypoints,_,_ = ReadFlightplanFile(os.path.join(icarous_home,
                                           v_scenario["waypoint_file"]))
        HomePos = waypoints[0][0:3]
        name=v_scenario["name"]

        ic = Icarous(HomePos, simtype="UAM_VTOL", vehicleID=spacecraft_id,
                     verbose=1, logName=name)

        # Set up the scenario
        if "merge_fixes" in scenario:
            ic.InputMergeFixes(os.path.join(icarous_home, scenario["merge_fixes"]))

        params = LoadIcarousParams(os.path.join(icarous_home, v_scenario["parameter_file"]))
        if v_scenario.get("param_adjustments"):
            params.update(v_scenario["param_adjustments"])
            params["RESSPEED"] = params["DEF_WP_SPEED"]
        ic.SetParameters(params)

        ic.InputFlightplanFromFile(os.path.join(icarous_home, v_scenario["waypoint_file"]))

        if v_scenario.get("geofence_file"):
            ic.InputGeofence(os.path.join(icarous_home, v_scenario["geofence_file"]))

        for tf in v_scenario["traffic"]:
            traf_id = num_vehicles + len(tfList)
            StartTraffic(traf_id, HomePos, tf[0], tf[1], tf[2], tf[3], tf[4], tf[5], tfList)

        delay = v_scenario.get("delay", 0)
        icDelay.append(delay)

        icInstances.append(ic)

    for tf in tfList:
        tf.setpos_uncertainty(0.01, 0.01, 0, 0, 0, 0)

    tlimit = [scenario.get("time_limit", 1000)]*len(icInstances)
    RunSimulation(icInstances, tfList, startDelay=icDelay, timeLimit=tlimit)
    os.chdir(sim_home)

    # Collect log files
    for ic in icInstances:
        #ic.WriteLog(logname=os.path.join(output_dir, ic.logName+".json"))
        logname = os.path.join(output_dir, ic.logName+".json")
        print("writing log: %s" % logname)
        log_data = {"scenario": scenario,
                    "ownship_id": ic.vehicleID,
                    "ownship": ic.ownshipLog,
                    "traffic": ic.trafficLog,
                    "waypoints": ic.flightplan1,
                    "geofences": ic.fenceList,
                    "parameters": ic.params,
                    "sim_type": "pyIcarous"}
        with open(logname, 'w') as f:
            json.dump(log_data, f)


def set_up_output_dir(scenario, base_dir="sim_output"):
    """ Set up a directory to save log files for a scenario """
    name = scenario["name"].replace(' ', '-')
    timestamp = datetime.datetime.now().strftime("%Y-%m-%dT%H_%M_%S")
    output_dir = os.path.join(sim_home, base_dir, timestamp+"_"+name)
    os.makedirs(output_dir)
    return output_dir


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Run Icarous test scenarios")
    parser.add_argument("scenario", help="yaml file containing scenario(s) to run ")
    parser.add_argument("--num", type=int, nargs="+", help="index of scenario to run")
    parser.add_argument("--verbose", action="store_true", help="print output")
    parser.add_argument("--sitl", action="store_true", help="use SITL simulator")
    parser.add_argument("--out", type=int, default=None, help="port to fwd mavlink stream")
    parser.add_argument("--merger", action="store_true", help="run merging apps")
    parser.add_argument("--python", action="store_true", help="use pyIcarous")
    parser.add_argument("--validate", action="store_true", help="check test conditions")
    parser.add_argument("--test", action="store_true", help="assert test conditions")
    parser.add_argument("--plot", action="store_true", help="generate plots")
    parser.add_argument("--save", action="store_true", help="save plots")
    parser.add_argument("--output_dir", default="sim_output", help="directory for output")
    args = parser.parse_args()

    # Load scenarios from file
    with open(args.scenario, 'r') as f:
        scenario_list = yaml.load(f, Loader=yaml.FullLoader)
    if args.num is not None:
        selected_scenarios = [scenario_list[i] for i in args.num]
        scenario_list = selected_scenarios

    if not args.python:
        # Set the apps that ICAROUS will run
        SetApps(sitl=args.sitl, merger=args.merger)

    # Run the scenarios
    for scenario in scenario_list:
        if args.verbose:
            print("Running scenario: %s" % scenario["name"])
        out_dir = set_up_output_dir(scenario, args.output_dir)

        # Run the simulation
        if args.python:
            RunScenarioPy(scenario, args.verbose, out_dir)
        else:
            RunScenario(scenario, args.sitl, args.verbose, output_dir=out_dir,
                        out=args.out, use_threads=True)

        # Perform validation
        if args.validate:
            if args.merger:
                import ValidateMerge as VM
                VM.run_validation(out_dir, 1, args.test, args.plot, args.save)
            else:
                import ValidateSim as VS
                VS.run_validation(out_dir, args.test, args.plot, args.save)
