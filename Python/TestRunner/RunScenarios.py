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


def RunScenario(scenario, verbose=0, fasttime=True, eta=False, python=True,
                out=None, use_sbn=False, output_dir="sim_output"):
    """ Run an ICAROUS scenario """
    if python:
        log_home = os.path.join(icarous_home, "Python", "pycarous", "log")
    else:
        log_home = os.path.join(icarous_exe, "log")
    ClearLogs(log_home)

    # Create simulation environment
    sim = SimEnvironment(verbose=verbose, fasttime=fasttime)
    if "merge_fixes" in scenario:
        sim.InputMergeFixes(os.path.join(icarous_home, scenario["merge_fixes"]))
    sim.AddWind(scenario.get("wind", [(0, 0)]))

    # Add Icarous instances to simulation environment
    num_vehicles = len(scenario["vehicles"])
    sim_time_limit = scenario.get("time_limit", 1000)

    # If any vehicles are using pycarous, do not run SBN
    if any(v.get("python", python) for v in scenario["vehicles"]):
        use_sbn = False

    for v in scenario["vehicles"]:
        cpu_id = v.get("cpu_id", len(sim.icInstances) + 1)
        spacecraft_id = cpu_id - 1
        callsign = v.get("name", "vehicle%d" % spacecraft_id)
        fp_file = os.path.join(icarous_home, v["waypoint_file"])
        HomePos = GetHomePosition(fp_file)
        use_python = v.get("python", python)

        # Initialize Icarous class
        if use_python:
            os.chdir("../pycarous")
            ic = Icarous(HomePos, simtype="UAM_VTOL", vehicleID=spacecraft_id,
                         callsign=callsign, verbose=verbose, fasttime=fasttime)
        else:
            apps = GetApps(sitl=args.sitl, merger=args.merger, sbn=use_sbn)
            ic = IcarousRunner(HomePos, vehicleID=spacecraft_id,
                               callsign=callsign, verbose=verbose,
                               apps=apps, out=out)

        # Set parameters
        param_file = os.path.join(icarous_home, v["parameter_file"])
        params = LoadIcarousParams(param_file)
        if v.get("param_adjustments"):
            params.update(v["param_adjustments"])
        params["RESSPEED"] = params["DEF_WP_SPEED"]
        ic.SetParameters(params)

        # Input flight plan
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

    # Collect the log files
    os.chdir(output_dir)
    sim.WriteLog()
    os.chdir(sim_home)
    CollectLogs(log_home, output_dir)


def ClearLogs(source):
    if not os.path.isdir(source):
        return
    for f in os.listdir(source):
        if f.endswith(".log"):
            os.remove(os.path.join(source, f))


def CollectLogs(source, output_dir):
    dest = os.path.join(output_dir, "log")
    for f in os.listdir(source):
        if f.endswith(".log"):
            oldname = f
            # Rename the log to match json log naming convention
            try:
                # Rename the log to match json log naming convention
                log_type, vehicle_name, timestamp = oldname.split('-')
                cpu_id = int(vehicle_name.strip("aircraft")) + 1
                callsign = next((v["name"] for v in scenario["vehicles"]
                                if v.get("cpu_id", 0) == cpu_id), "0")
                newname = "-".join([log_type, callsign, timestamp])
            except:
                newname = oldname
            shutil.copy(os.path.join(source, f), os.path.join(dest, newname))


def RunValidation(out_dir):
    if args.merger:
        import ValidateMerge as VM
        VM.run_validation(out_dir, 1, args.test, args.plot, args.save)
    else:
        import ValidateSim as VS
        VS.run_validation(out_dir, args.test, args.plot, args.save)


def GetApps(apps="default", sitl=False, merger=False, s2d=False, sbn=False):
    """
    Set the apps that ICAROUS will run
    :param apps: Starting list of the apps, or "default" to use default apps
    :param sitl: When true, run arducopter instead of rotorsim
    :param merger: When true, add the merger apps (merger and raft)
    :param s2d: When true, add the safe2ditch app
    :param sbn: When true, add the software bus network apps (SBN, SBN_UDP)
    """
    if apps == "default":
        app_list = ["Icarouslib","port_lib", "scheduler",
                    "gsInterface", "cognition", "guidance",
                    "traffic", "trajectory", "geofence"]
    else:
        app_list = apps

    if sitl:
        app_list.append("arducopter")
    else:
        app_list.append("rotorsim")

    if merger:
        app_list += ["merger", "raft"]

    if sbn:
        app_list += ["SBN", "SBN_UDP"]

    if s2d:
        app_list.append("safe2ditch")

    return app_list


def set_up_output_dir(scenario, base_dir="sim_output"):
    """ Set up a directory to save log files for a scenario """
    name = scenario["name"].replace(' ', '-')
    timestamp = datetime.datetime.now().strftime("%Y-%m-%dT%H_%M_%S")
    output_dir = os.path.join(sim_home, base_dir, timestamp+"_"+name)
    output_logdir = os.path.join(output_dir,'log')
    os.makedirs(output_dir)
    os.makedirs(output_logdir)
    return output_dir


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Run Icarous test scenarios")
    parser.add_argument("scenario",
                        help="yaml file containing scenario(s) to run")
    parser.add_argument("--num", type=int, nargs="+",
                        help="index of scenario to run")
    parser.add_argument("--sitl", action="store_true",
                        help="use SITL simulator (only for cFS simulations)")
    parser.add_argument("--eta", action="store_true",
                        help="Enforce waypoint etas")
    parser.add_argument("--merger", action="store_true",
                        help="run merging apps")
    parser.add_argument("--s2d", action="store_true",
                        help="run safe2ditch app")
    parser.add_argument("--sbn", action="store_true",
                        help="run the software bus network (cFS)")
    parser.add_argument("--python", action="store_true",
                        help="use pycarous instead of cFS")
    parser.add_argument("--realtime", dest="fasttime", action="store_false",
                        help='Run sim in real time')
    parser.add_argument("--fasttime", dest="fasttime", action="store_true",
                        help='Run sim in fast time (only for pycarous)')

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
        default_params = scenario_data.get("sim_params", {})
        parser.set_defaults(**default_params)
        args = parser.parse_args()
    if args.num is not None:
        selected_scenarios = [scenario_list[i] for i in args.num]
        scenario_list = selected_scenarios

    if not args.python:
        args.fasttime = False

    # Run the scenarios
    for scenario in scenario_list:
        use_python = scenario.get("python", args.python)
        out_dir = set_up_output_dir(scenario, args.output_dir)
        if args.verbose > 0:
            print("~~~~ Running scenario: %s ~~~~" % scenario["name"])

        # Run the simulation
        RunScenario(scenario, verbose=args.verbose, fasttime=args.fasttime,
                    eta=args.eta, python=use_python, out=args.out,
                    use_sbn=args.sbn, output_dir=out_dir)

        # Perform validation
        if args.validate:
            time.sleep(2)
            RunValidation(out_dir)
