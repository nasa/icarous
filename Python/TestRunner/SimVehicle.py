import time
import subprocess
import shutil
import os
import sys
import json

from pymavlink import mavutil, mavwp

sys.path.append("../Batch")
import BatchGSModule as GS
from ichelper import LoadIcarousParams, ReadFlightplanFile


sim_home = os.getcwd()
icarous_home = os.path.abspath(os.path.join(sim_home, "../.."))
icarous_exe = os.path.join(icarous_home, "exe", "cpu1")


def vehicle_log():
    return {"t": [], "position": [], "velocityNED": []}


class SimVehicle:
    def __init__(self, scenario, verbose=True, out=14557, output_dir="", sitl=False):
        self.scenario = scenario
        self.verbose = verbose
        self.out = out
        self.output_dir = output_dir
        self.sitl = sitl
        self.started = False
        if self.sitl:
            self.sim_type = "cFS SITL"
        else:
            self.sim_type = "cFS rotorsim"

        self.setup()


    def launch_arducopter(self):
        waypoints,_,_ = ReadFlightplanFile(os.path.join(icarous_home,
                                           self.scenario["waypoint_file"]))
        start_point = ','.join(str(x) for x in waypoints[0][0:3]+[0])
        icarous_params = LoadIcarousParams(os.path.join(icarous_home,
                                           self.scenario["parameter_file"]))
        sitl_params = LoadIcarousParams("sitl_defaults.parm")
        sitl_params["WPNAV_SPEED"] = icarous_params["DEF_WP_SPEED"]*100
        sitl_param_file = os.path.join(self.output_dir, self.name + "_sitl.parm")
        with open(sitl_param_file, 'w') as f:
            for param_id, param_value in sitl_params.items():
                f.write("%-16s %f\n" % (param_id, param_value))
        arguments = ["sim_vehicle.py", "-v", "ArduCopter",
                     "-l", str(start_point),
                     "--add-param-file", sitl_param_file,
                     "--use-dir", sitl_param_file,
                     "-I", str(self.spacecraft_id)]
        arguments += ["-m", "--logfile="+os.path.join(self.output_dir,
                                                      self.name+"_sitl.tlog")]
        subprocess.Popen(arguments, stdout=subprocess.DEVNULL)
        if self.verbose:
            print("Waiting several seconds to allow ArduCopter to start up")
        time.sleep(60)


    def setup(self):
        scenario = self.scenario
        self.name = scenario["name"]
        self.cpu_id = scenario.get("cpu_id", 1)
        self.spacecraft_id = self.cpu_id - 1

        # Set up mavlink connections
        icarous_port = 14553 + (self.cpu_id - 1)*10
        gs_port = icarous_port + 1
        # Use mavproxy to forward mavlink stream (for visualization)
        logfile = os.path.join(self.output_dir, self.name + ".tlog")
        self.mav_forwarding = subprocess.Popen(["mavproxy.py",
                                           "--master=127.0.0.1:"+str(icarous_port),
                                           "--out=127.0.0.1:"+str(gs_port),
                                           "--out=127.0.0.1:"+str(self.out),
                                           "--target-system=1",
                                           "--target-component=5",
                                           "--logfile="+logfile],
                                          stdout=subprocess.DEVNULL)
        # Open connection for virtual ground station
        master = mavutil.mavlink_connection("127.0.0.1:"+str(gs_port))

        # Start the ICAROUS process
        os.chdir(icarous_exe)
        fpic = open(self.name + "_icout.txt",'w')
        self.ic = subprocess.Popen(["./core-cpu1",
                                    "-I "+str(self.spacecraft_id),
                                    "-C "+str(self.cpu_id)],
                                   stdout=fpic)
        os.chdir(sim_home)

        # Pause for a couple of seconds here so that ICAROUS can boot up
        if self.verbose:
            print("Telemetry for %s is on 127.0.0.1:%d" % (self.name, self.out))
            print("Waiting for heartbeat...")
        master.wait_heartbeat()
        self.gs = GS.BatchGSModule(master, 1, 0)

        # Launch SITL simulator
        if self.sitl:
            self.launch_arducopter()

        # Set up the scenario (flight plan, geofence, parameters, traffic)
        self.gs.loadWaypoint(os.path.join(icarous_home, scenario["waypoint_file"]))
        shutil.copy(os.path.join(icarous_home, scenario["waypoint_file"]),
                    os.path.join(self.output_dir, self.name + ".waypoints"))
        if scenario.get("geofence_file"):
            self.gs.loadGeofence(os.path.join(icarous_home, scenario["geofence_file"]))
            shutil.copy(os.path.join(icarous_home, scenario["geofence_file"]),
                        os.path.join(self.output_dir, self.name + "_geofence.xml"))
        if scenario.get("parameter_file"):
            self.gs.loadParams(os.path.join(icarous_home, scenario["parameter_file"]))
        if scenario.get("param_adjustments"):
            for param_id, param_value in scenario["param_adjustments"].items():
                self.gs.setParam(param_id, param_value)
        if scenario.get("traffic"):
            for traf in scenario["traffic"]:
                self.gs.load_traffic([0]+traf)
        self.waypoints = self.get_waypoints()
        self.params = self.gs.getParams()
        self.geofences = self.gs.fenceList

        # Wait for GPS fix before starting mission
        if self.verbose:
            print("Waiting for GPS fix...")
        while True:
            m = master.recv_match(type="GLOBAL_POSITION_INT", blocking=False)
            if m is None:
                continue
            if m.lat > 1e-5:
                break


    def get_waypoints(self):
        '''extract waypoints'''
        WP = []
        wploader = self.gs.wploader
        for i in range(wploader.count()):
            if wploader.wp(i).command == 16:
                WP.append([wploader.wp(i).x, wploader.wp(i).y,
                           wploader.wp(i).z, wploader.wp(i).seq])
        return WP


    def write_log(self):
        # Construct the sim data for verification
        simdata = {"scenario": self.scenario,
                   "ownship": self.ownship_log,
                   "traffic": self.traffic_log,
                   "waypoints": self.waypoints,
                   "geofences": self.geofences,
                   "parameters": self.params,
                   "sim_type": self.sim_type}

        # Save the sim data
        dest = os.path.join(self.output_dir, self.name + "_simoutput.json")
        with open(dest, 'w') as f:
            json.dump(simdata, f)
        print("\nWrote " + self.name + " simulation data")


    def write_params(self):
        param_file = os.path.join(self.output_dir, self.name + ".parm")
        with open(param_file, 'w') as f:
            for param_id, param_value in self.params.items():
                f.write("%-16s %f\n" % (param_id, param_value))


    def run(self, time_limit):
        self.ownship_log = vehicle_log()
        self.traffic_log = {}
        delay = self.scenario.get("delay", 0)
        t0 = time.time()
        duration = 0
        while duration < time_limit:
            if not self.started and duration >= delay:
                self.gs.StartMission()
                self.started = True
                if self.verbose:
                    print("***Starting %s" % self.name)
            time.sleep(0.01)
            currentT = time.time()
            duration = currentT - t0

            self.gs.Update_traffic()

            msg = self.gs.master.recv_match(blocking=False, type=["GLOBAL_POSITION_INT"])

            if msg is None:
                continue

            # Store ownship position/velocity information
            self.ownship_log["t"].append(duration)
            self.ownship_log["position"].append([msg.lat/1E7, msg.lon/1E7,
                                                 msg.relative_alt/1E3])
            self.ownship_log["velocityNED"].append([msg.vx/1E2, msg.vy/1E2,
                                                    msg.vz/1E2])

            # Store traffic position/velocity information
            for i, traf in enumerate(self.gs.traffic_list):
                if i not in self.traffic_log.keys():
                    self.traffic_log[i] = vehicle_log()
                self.traffic_log[i]["t"].append(duration)
                self.traffic_log[i]["position"].append([traf.lat, traf.lon, traf.alt])
                self.traffic_log[i]["velocityNED"].append([traf.vx0, traf.vy0, traf.vz0])

        # Once simulation is finished, kill the icarous process
        self.ic.kill()
        subprocess.call(["kill", "-9", str(self.mav_forwarding.pid)])

        self.write_log()
        self.write_params()
