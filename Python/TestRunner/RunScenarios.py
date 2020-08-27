import os
import sys
import yaml
import json
import time
import datetime
import shutil
import subprocess

sys.path.append("../pycarous")
import BatchGSModule as GS
from SimVehicle import SimVehicle
from Icarous import (Icarous, ReadFlightplanFile, LoadIcarousParams)
from SimEnvironment import SimEnvironment

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
        app_list += ["merger", "raft", "SBN", "SBN_UDP"]

    app_list.append("safe2ditch")

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
    for f in os.listdir(os.path.join(icarous_exe, "log")):
        os.remove(os.path.join(icarous_exe, "log", f))

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
    threads = []
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
    source = os.path.join(icarous_exe, "log")
    dest = os.path.join(output_dir, "log")
    for f in os.listdir(source):
        if not f.startswith("."):
            shutil.copy(os.path.join(source, f), dest)


def RunScenarioPy(scenario, verbose=False, eta=False, output_dir="sim_output"):
    """ Run an ICAROUS scenario using pyIcarous """
    os.chdir(output_dir)

    # Create fasttime simulation environment
    sim = SimEnvironment()
    if "merge_fixes" in scenario:
        sim.InputMergeFixes(os.path.join(icarous_home, scenario["merge_fixes"]))
    sim.AddWind(scenario.get("wind", [(0, 0)]))

    # Add Icarous instances to simulation environment
    num_vehicles = len(scenario["vehicles"])
    time_limit = scenario.get("time_limit", 1000)
    for v_scenario in scenario["vehicles"]:
        cpu_id = v_scenario.get("cpu_id", len(sim.icInstances) + 1)
        spacecraft_id = cpu_id - 1
        callsign = v_scenario.get("name", "vehicle%d" % spacecraft_id)
        waypoints,_,_ = ReadFlightplanFile(os.path.join(icarous_home,
                                           v_scenario["waypoint_file"]))
        HomePos = waypoints[0][0:3]

        ic = Icarous(HomePos, simtype="UAM_VTOL", vehicleID=spacecraft_id,
                     callsign=callsign, verbose=1)

        # Set parameters
        param_file = os.path.join(icarous_home, v_scenario["parameter_file"])
        params = LoadIcarousParams(param_file)
        if v_scenario.get("param_adjustments"):
            params.update(v_scenario["param_adjustments"])
        params["RESSPEED"] = params["DEF_WP_SPEED"]
        ic.SetParameters(params)

        # Input flight plan
        fp_file = os.path.join(icarous_home, v_scenario["waypoint_file"])
        ic.InputFlightplanFromFile(fp_file, eta=eta)

        # Input geofences
        if v_scenario.get("geofence_file"):
            gf_file = os.path.join(icarous_home, v_scenario["geofence_file"])
            ic.InputGeofence(gf_file)

        # Input simulated traffic
        for tf in v_scenario["traffic"]:
            traf_id = num_vehicles + len(sim.tfList)
            sim.AddTraffic(traf_id, HomePos, *tf)

        delay = v_scenario.get("delay", 0)
        sim.AddIcarousInstance(ic, delay, time_limit)

    sim.RunSimulation()
    sim.WriteLog()
    os.chdir(sim_home)


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
    from collections import ChainMap
    parser = argparse.ArgumentParser(description="Run Icarous test scenarios")
    parser.add_argument("scenario", help="yaml file containing scenario(s) to run ")
    parser.add_argument("--num", type=int, nargs="+", help="index of scenario to run")
    parser.add_argument("--verbose", action="store_true", help="print output")
    parser.add_argument("--sitl", action="store_true", help="use SITL simulator")
    parser.add_argument("--eta", action="store_true", help="Enforce wp etas")
    parser.add_argument("--merger", action="store_true", help="run merging apps")
    parser.add_argument("--python", action="store_true", help="use pyIcarous")
    parser.add_argument("--out", type=int, default=None, help="port to fwd mavlink stream")
    parser.add_argument("--validate", action="store_true", help="check test conditions")
    parser.add_argument("--test", action="store_true", help="assert test conditions")
    parser.add_argument("--plot", action="store_true", help="generate plots")
    parser.add_argument("--save", action="store_true", help="save plots")
    parser.add_argument("--threads", action="store_true", help="use threads for simulations")
    parser.add_argument("--output_dir", default="sim_output", help="directory for output")
    args = parser.parse_args()

    # Load scenarios from file
    with open(args.scenario, 'r') as f:
        scenario_data = yaml.load(f, Loader=yaml.FullLoader)
        scenario_list = scenario_data.get("scenarios", [])
        input_sim_params = scenario_data.get("sim_params", {})
        sim_params = ChainMap(input_sim_params, vars(args))
    if args.num is not None:
        selected_scenarios = [scenario_list[i] for i in args.num]
        scenario_list = selected_scenarios

    if not sim_params["python"]:
        # Set the apps that ICAROUS will run
        SetApps(sitl=sim_params["sitl"], merger=sim_params["merger"])

    # Run the scenarios
    for scenario in scenario_list:
        if args.verbose:
            print("Running scenario: %s" % scenario["name"])
        out_dir = set_up_output_dir(scenario, args.output_dir)

        # Run the simulation
        if sim_params["python"]:
            RunScenarioPy(scenario, args.verbose, sim_params["eta"], out_dir)
        else:
            RunScenario(scenario, sim_params["sitl"], args.verbose, output_dir=out_dir,
                        out=args.out, use_threads=args.threads)

        # Perform validation
        time.sleep(2) 
        if args.validate:
            if sim_params["merger"]:
                import ValidateMerge as VM
                VM.run_validation(out_dir, 1, args.test, args.plot, args.save)
            else:
                import ValidateSim as VS
                VS.run_validation(out_dir, args.test, args.plot, args.save)
