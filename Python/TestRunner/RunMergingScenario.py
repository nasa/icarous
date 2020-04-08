import os
import sys
import yaml
import time
import datetime
import shutil
import subprocess
from pymavlink import mavutil

sys.path.append("../Batch")
import BatchGSModule as GS
from RunScenarios import SetApps


sim_home = os.getcwd()
icarous_home = os.path.abspath(os.path.join(sim_home, "../.."))
icarous_exe = os.path.join(icarous_home, "exe", "cpu1")

class Vehicle:
    def __init__(self, icarous_process, gs_process, mav_forwarding_process, scenario):
        self.icarous = icarous_process
        self.gs = gs_process
        self.mav_forwarding = mav_forwarding_process
        self.scenario = scenario
        self.started = False

def SetUpVehicle(scenario, verbose=True, out=14557, output_dir="", sitl=False):
    """ Set up a vehicle running icarous, but don't start mission """
    name = scenario["name"].replace(' ', '-')
    cpu_id = scenario.get("cpu_id", 1)
    spacecraft_id = cpu_id - 1

    # Clear message queue to avoid icarous problems
    messages = os.listdir("/dev/mqueue")
    for m in messages:
        os.remove(os.path.join("/dev/mqueue", m))

    # Set up mavlink connections
    gs_port = 14553 + (cpu_id - 1)*10
    # Use mavproxy to forward mavlink stream (for visualization)
    mav_forwarding = None
    if out:
        icarous_port = gs_port
        gs_port += 1
        logfile = os.path.join(output_dir, name) + ".tlog"
        mav_forwarding = subprocess.Popen(["mavproxy.py",
                                           "--master=127.0.0.1:"+str(icarous_port),
                                           "--out=127.0.0.1:"+str(gs_port),
                                           "--out=127.0.0.1:"+str(out),
                                           "--target-system=1",
                                           "--target-component=5",
                                           "--logfile="+logfile],
                                          stdout=subprocess.DEVNULL)
    # Open connection for virtual ground station
    master = mavutil.mavlink_connection("127.0.0.1:"+str(gs_port))

    # Start the ICAROUS process
    os.chdir(icarous_exe)
    fpic = open('icout.txt','w')
    ic = subprocess.Popen(["./core-cpu1",
                           "-I "+str(spacecraft_id),
                           "-C "+str(cpu_id)],
                          stdout=fpic)

    # Pause for a couple of seconds here so that ICAROUS can boot up
    if verbose:
        print("Waiting for heartbeat...")
    master.wait_heartbeat()
    gs = GS.BatchGSModule(master, 1, 0)

    # Launch SITL simulator
    if sitl:
        LaunchArducopter(scenario)

    # Upload the flight plan
    gs.loadWaypoint(os.path.join(icarous_home, scenario["waypoint_file"]))

    # Upload the geofence
    if scenario.get("geofence_file"):
        gs.loadGeofence(os.path.join(icarous_home, scenario["geofence_file"]))

    # Upload the icarous parameters
    if scenario.get("parameter_file"):
        gs.loadParams(os.path.join(icarous_home, scenario["parameter_file"]))
    if scenario.get("param_adjustments"):
        for param_id, param_value in scenario["param_adjustments"].items():
            gs.setParam(param_id, param_value)

    # Load traffic vehicles
    if scenario.get("traffic"):
        for traf in scenario["traffic"]:
            gs.load_traffic([0]+traf)

    # Wait for GPS fix before starting mission
    time.sleep(1)
    if verbose:
        print("Waiting for GPS fix...")
    master.recv_match(type="GLOBAL_POSITION_INT", blocking=True)

    vehicle = Vehicle(ic, gs, mav_forwarding, scenario)
    return vehicle


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Run Icarous test scenarios")
    parser.add_argument("scenario",
                        help="yaml file containing scenario(s) to run ")
    parser.add_argument("--save", action="store_true",
                        help="save the simulation results")
    parser.add_argument("--num", type=int, nargs="+",
                        help="If multiple scenarios in file, just run this one")
    parser.add_argument("--verbose", action="store_true",
                        help="print sim information")
    parser.add_argument("--sitl", action="store_true",
                        help="use arducopter SITL sim instead of rotorsim")
    parser.add_argument("--output_dir", default="merging_sim_output",
                        help="directory to save output (default: 'merging_sim_output')")
    args = parser.parse_args()

    # Load scenarios from file
    with open(args.scenario, 'r') as f:
        scenario_list = yaml.load(f, Loader=yaml.FullLoader)
    if args.num is not None:
        selected_scenarios = [scenario_list[i] for i in args.num]
        scenario_list = selected_scenarios

    # Set the apps that ICAROUS will run
    SetApps(sitl=args.sitl, merger=True)

    # Run the scenarios
    for scenario in scenario_list:
        # Set up a directory to save log files
        name = scenario["name"].replace(' ', '-')
        timestamp = datetime.datetime.now().strftime("%Y-%m-%dT%H_%M_%S")
        output_dir = os.path.join(sim_home, args.output_dir, timestamp+"_"+name)
        os.makedirs(output_dir)

        # Copy merge_fixes file to exe/ram
        if "merge_fixes" in scenario:
            merge_fixes = os.path.join(icarous_home, scenario["merge_fixes"])
            ram_directory = os.path.join(icarous_home, "exe", "ram")
            shutil.copy(merge_fixes, ram_directory)

        # Set up each vehicle
        vehicles = []
        for vehicle in scenario["vehicles"]:
            cpu_id = vehicle.get("cpu_id", 1)
            out = 14556 + cpu_id
            v = SetUpVehicle(vehicle, verbose=args.verbose, out=out,
                             output_dir=output_dir, sitl=args.sitl)
            vehicles.append(v)


        # Wait for the given time limit
        t0 = time.time()
        time_limit = scenario["time_limit"]
        duration = 0
        while duration < time_limit:
            # Start vehicles
            for v in vehicles:
                if not v.started and duration >= v.scenario.get("delay", 0):
                    v.gs.StartMission()
                    v.started = True
                    print("***Starting %s" % v.scenario["name"])
            if args.verbose:
                print("Sim Duration: %.1fs       " % duration, end="\r")
            time.sleep(0.1)
            duration = time.time() - t0
        print("Simulation time limit finished")

        # Terminate Icarous, mavproxy, and arducopter
        for v in vehicles:
            v.icarous.kill()
            if v.mav_forwarding:
                subprocess.call(["kill", "-9", str(v.mav_forwarding.pid)])
        if args.sitl:
            subprocess.call(["pkill", "-9", "arducopter"])
            subprocess.call(["pkill", "-9", "xterm"])

        # Collect log files
        for f in os.listdir(icarous_exe):
            if f.startswith("merger") or f.startswith("raft"):
                os.rename(os.path.join(icarous_exe, f), os.path.join(output_dir, f))

        # Generate plots
        script = os.path.join(icarous_home, "Python", "MergingAnalysis", "ProcMergerLogs.py")
        n_vehicles = str(len(scenario["vehicles"]))
        merge_id = str(1)
        subprocess.call(["python3", script, n_vehicles, merge_id, output_dir])
