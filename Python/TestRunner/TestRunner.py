import os
import sys
import yaml
import time
import datetime
import shutil
import subprocess

sys.path.append("../Batch")
import BatchGSModule as GS
from SimVehicle import SimVehicle

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


def RunScenario(scenario, sitl=False, verbose=False, output_dir="sim_output"):
    from threading import Thread
    # Set up a directory to save log files
    name = scenario["name"].replace(' ', '-')
    if verbose:
        print("Running scenario: %s" % name)
    timestamp = datetime.datetime.now().strftime("%Y-%m-%dT%H_%M_%S")
    output_dir = os.path.join(sim_home, output_dir, timestamp+"_"+name)
    os.makedirs(output_dir)

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
        out = 14557 + (vehicle_scenario.get("cpu_id", 1) - 1)*10
        v = SimVehicle(vehicle_scenario, verbose=verbose, out=out,
                       output_dir=output_dir, sitl=sitl)
        vehicles.append(v)

    # Run the simulation
    t0 = time.time()
    time_limit = scenario["time_limit"]
    duration = 0
    threads = [Thread(target=v.run, args=[time_limit]) for v in vehicles]
    for t in threads:
        t.start()
    while duration < time_limit:
        if verbose:
            print("Sim Duration: %.1fs       " % duration, end="\r")
        time.sleep(0.1)
        duration = time.time() - t0
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

    return output_dir


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Run Icarous test scenarios")
    parser.add_argument("scenario", help="yaml file containing scenario(s) to run ")
    parser.add_argument("--num", type=int, nargs="+", help="index of scenario to run")
    parser.add_argument("--verbose", action="store_true", help="print output")
    parser.add_argument("--sitl", action="store_true", help="use SITL simulator")
    parser.add_argument("--merger", action="store_true", help="run merging apps")
    parser.add_argument("--python", action="store_true", help="use pyIcarous")
    parser.add_argument("--output_dir", default="sim_output", help="directory for output")
    args = parser.parse_args()

    # Load scenarios from file
    with open(args.scenario, 'r') as f:
        scenario_list = yaml.load(f, Loader=yaml.FullLoader)
    if args.num is not None:
        selected_scenarios = [scenario_list[i] for i in args.num]
        scenario_list = selected_scenarios

    # Set the apps that ICAROUS will run
    SetApps(sitl=args.sitl, merger=args.merger)

    # Run the scenarios
    for scenario in scenario_list:
        output_dir = RunScenario(scenario, args.sitl, args.verbose, args.output_dir)

        if args.merger:
            subprocess.call(["python3", "ValidateMerge.py", output_dir, "--plot", "--save"])
        else:
            subprocess.call(["python3", "ValidateSim.py", output_dir, "--plot", "--save"])
