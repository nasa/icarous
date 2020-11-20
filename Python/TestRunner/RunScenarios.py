#! python3

import os
import sys
import yaml
import time
import shutil
import datetime
import subprocess

sys.path.append("../pycarous")
from Icarous import Icarous
from IcarousRunner import IcarousRunner
from SimEnvironment import SimEnvironment
from ichelper import (ReadFlightplanFile, LoadIcarousParams, GetHomePosition)


sim_home = os.getcwd()
icarous_home = os.path.abspath(os.path.join(sim_home, "../.."))
icarous_exe = os.path.join(icarous_home, "exe", "cpu1")
pycarous_home = os.path.join(icarous_home, "Python", "pycarous")
pycarous_log_dir = os.path.join(pycarous_home, "log")
cFS_log_dir = os.path.join(icarous_exe, "log")
default_apps =  ["Icarouslib","port_lib", "scheduler", "gsInterface",
                 "cognition", "guidance", "traffic", "trajectory",
                 "geofence", "rotorsim"]
default_daa_file = os.path.join(pycarous_home, "data", "DaidalusQuadConfig.txt")


def RunScenario(scenario, verbose=0, fasttime=True, use_python=False,
                out=None, output_dir="sim_output"):
    """
    Run an ICAROUS scenario
    :param scenario: dictionary defining scenario inputs
    :param verbose: control printouts (0: none, 1: some, 2+: more)
    :param fasttime: run simulation in fasttime (pycarous only)
    :param use_python: run simulation with pycarous (override scenario inputs)
    :param out: port number to forward MAVLink data for visualization
                (use out=None to turn off MAVLink output)
    :param output_dir: location to save log files
    """
    # Clear out existing logs
    ClearLogs(pycarous_log_dir)
    ClearLogs(cFS_log_dir)

    # Set python option for each vehicle
    for v in scenario["vehicles"]:
        if use_python:
            v["python"] = True
        if not v.get("python", scenario.get("python", False)):
            fasttime = False

    # Create simulation environment
    sim_time_limit = scenario.get("time_limit", 1000)
    sim = SimEnvironment(verbose=verbose, fasttime=fasttime,
                         time_limit=sim_time_limit)
    if "merge_fixes" in scenario:
        sim.InputMergeFixes(os.path.join(icarous_home, scenario["merge_fixes"]))
    sim.AddWind(scenario.get("wind", [(0, 0)]))
    sitl_running = False

    # Add Icarous instances to simulation environment
    num_vehicles = len(scenario["vehicles"])
    for v in scenario["vehicles"]:
        v = dict(list(scenario.items()) + list(v.items()))
        cpu_id = v.get("cpu_id", len(sim.icInstances) + 1)
        spacecraft_id = cpu_id - 1
        callsign = v.get("name", "vehicle%d" % spacecraft_id)
        fp_file = os.path.join(icarous_home, v["waypoint_file"])
        HomePos = GetHomePosition(fp_file)
        daa_file = os.path.join(icarous_home,v.get("daa_file", default_daa_file))

        # Initialize Icarous class
        python = v.get("python", False)
        if python:
            os.chdir("../pycarous")
            ic = Icarous(HomePos, simtype="UAM_VTOL", vehicleID=spacecraft_id,
                         callsign=callsign, verbose=verbose, fasttime=fasttime,
                         daaConfig=daa_file)
        else:
            sim.fasttime = False
            try:
                shutil.copyfile(daa_file,icarous_exe+"/../ram/DaidalusQuadConfig.txt")
            except shutil.SameFileError:
                pass
            apps = v.get("apps", default_apps)
            sitl = v.get("sitl", False)
            if sitl:
                if "arducopter" not in apps:
                    apps.append("arducopter")
                if "rotorsim" in apps:
                    apps.remove("rotorsim")
            sitl_running |= sitl
            ic = IcarousRunner(HomePos, vehicleID=spacecraft_id,
                               callsign=callsign, verbose=verbose,
                               apps=apps, sitl=sitl, out=out)

        # Set parameters
        param_file = os.path.join(icarous_home, v["parameter_file"])
        params = LoadIcarousParams(param_file)
        if v.get("param_adjustments"):
            params.update(v["param_adjustments"])
        params["RESSPEED"] = params["DEF_WP_SPEED"]
        ic.SetParameters(params)

        # Input flight plan
        eta = v.get("eta", False)
        ic.InputFlightplanFromFile(fp_file, eta=eta)

        # Input geofences
        if v.get("geofence_file"):
            gf_file = os.path.join(icarous_home, v["geofence_file"])
            ic.InputGeofence(gf_file)

        # Input simulated traffic
        for tf in v["traffic"]:
            traf_id = num_vehicles + len(sim.tfList)
            sim.AddTraffic(traf_id, HomePos, *tf)

        delay = v.get("delay", 0)
        time_limit = min(sim_time_limit, v.get("time_limit", sim_time_limit))
        sim.AddIcarousInstance(ic, delay, time_limit)

    # Run the simulation
    sim.RunSimulation()

    # Shut down SITL if necessary
    if sitl_running:
        subprocess.call(["pkill", "-9", "arducopter"])
        subprocess.call(["pkill", "-9", "mavproxy"])

    # Collect the log files
    os.chdir(output_dir)
    sim.WriteLog()
    os.chdir(sim_home)
    CollectLogs(pycarous_log_dir, output_dir)
    CollectLogs(cFS_log_dir, output_dir)


def ClearLogs(source):
    if not os.path.isdir(source):
        return
    for f in os.listdir(source):
        if f.endswith(".log"):
            os.remove(os.path.join(source, f))


def CollectLogs(source, output_dir):
    for f in os.listdir(source):
        if f.endswith(".log"):
            oldname = f
            try:
                # Rename the log to match json log naming convention
                log_type, vehicle_name, timestamp = oldname.split('-')
                cpu_id = int(vehicle_name.strip("aircraft")) + 1
                callsign = next((v["name"] for v in scenario["vehicles"]
                                if v.get("cpu_id", 0) == cpu_id), "0")
                newname = "-".join([log_type, callsign, timestamp])
            except:
                newname = oldname
            shutil.copy(os.path.join(source, f), os.path.join(output_dir, newname))


def RunValidation(out_dir, merge=False):
    """ Run validation script on the log files in out_dir """
    from validation_scripts import ValidateSim as VS
    VS.run_validation(out_dir, args.test, args.plot, args.save)
    if merge:
        from validation_scripts import ValidateMerge as VM
        VM.run_validation(out_dir, 1, args.test, args.plot, args.save)


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
    parser.add_argument("scenario",
                        help="yaml file containing scenario(s) to run")
    parser.add_argument("--num", type=int, nargs="+",
                        help="index of scenario to run")
    parser.add_argument("--realtime", dest="fasttime", action="store_false",
                        help='Run sim in real time')
    parser.add_argument("--python", action="store_true",
                        help="use pycarous instead of cFS")

    output = parser.add_argument_group("output arguments")
    output.add_argument("--verbose", nargs='?', type=int, default=0, const=1,
                        help="control print output")
    output.add_argument("--out", type=int, default=None,
                        help="port to fwd mavlink stream")
    output.add_argument("--output_dir", default="sim_output",
                        help="directory for output")

    validation = parser.add_argument_group("validation arguments")
    validation.add_argument("--validate", action="store_true",
                        help="check test conditions")
    validation.add_argument("--test", action="store_true",
                        help="assert test conditions")
    validation.add_argument("--plot", action="store_true",
                        help="generate validation plots")
    validation.add_argument("--save", action="store_true",
                        help="save validation plots")
    args = parser.parse_args()

    # Load scenarios from file
    with open(args.scenario, 'r') as f:
        scenario_data = yaml.load(f, Loader=yaml.FullLoader)
        scenario_list = scenario_data.get("scenarios", [])
        defaults = scenario_data.get("defaults", {})
    if args.num is not None:
        selected_scenarios = [scenario_list[i] for i in args.num]
        scenario_list = selected_scenarios

    # Run the scenarios
    for scenario in scenario_list:
        out_dir = set_up_output_dir(scenario, args.output_dir)
        scenario = dict(list(defaults.items()) + list(scenario.items()))
        if args.verbose > 0:
            print("~~~~ Running scenario: %s ~~~~" % scenario["name"])

        # Run the simulation
        RunScenario(scenario, verbose=args.verbose, fasttime=args.fasttime,
                    use_python=args.python, out=args.out, output_dir=out_dir)

        # Perform validation
        args.validate = args.validate or args.test
        merge = ("merge_fixes" in scenario)
        if args.validate:
            time.sleep(2)
            RunValidation(out_dir, merge=merge)

