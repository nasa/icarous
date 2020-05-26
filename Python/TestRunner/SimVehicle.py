import time
import subprocess
import shutil
import os
import sys
import json

from pymavlink import mavutil, mavwp

sys.path.append("../pyicarous")
import BatchGSModule as GS
from ichelper import LoadIcarousParams, ReadFlightplanFile


sim_home = os.getcwd()
icarous_home = os.path.abspath(os.path.join(sim_home, "../.."))
icarous_exe = os.path.join(icarous_home, "exe", "cpu1")
sitl_default = os.path.join("Python", "TestRunner", "sitl_files", "sitl_defaults.parm")


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
        self.running = False
        self.terminated = False
        if self.sitl:
            self.sim_type = "cFS SITL"
        else:
            self.sim_type = "cFS rotorsim"

        self.setup()


    def launch_arducopter(self):
        waypoints,_,_ = ReadFlightplanFile(os.path.join(icarous_home,
                                           self.scenario["waypoint_file"]))
        start_point = ','.join(str(x) for x in waypoints[0][0:3]+[0])

        # Set up parameters for SITL
        sitl_param_file = self.scenario.get("sitl_parameter_file", sitl_default)
        sitl_params = LoadIcarousParams(os.path.join(icarous_home, sitl_param_file))

        icarous_param_file = self.scenario["parameter_file"]
        icarous_params = LoadIcarousParams(os.path.join(icarous_home, icarous_param_file))
        icarous_params.update(self.scenario["param_adjustments"])

        sitl_params["WPNAV_SPEED"] = icarous_params["DEF_WP_SPEED"]*100
        sitl_param_file = os.path.join(self.output_dir, self.name + "_sitl.parm")
        with open(sitl_param_file, 'w') as f:
            for param_id, param_value in sitl_params.items():
                f.write("%-16s %f\n" % (param_id, param_value))

        # Launch SITL
        arguments = ["sim_vehicle.py", "-v", "ArduCopter",
                     "-l", str(start_point),
                     "--add-param-file", sitl_param_file,
                     "--use-dir", "sitl_files",
                     "-I", str(self.spacecraft_id)]
        logfile = os.path.join(self.output_dir, self.name+"_sitl.tlog")
        arguments += ["-m", "--logfile="+logfile]
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
        gs_port = icarous_port
        if self.out is not None:
            # Use mavproxy to forward mavlink stream (for visualization)
            gs_port = icarous_port + 1
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
        if self.verbose and self.out is not None:
            print("Telemetry for %s is on 127.0.0.1:%d" % (self.name, self.out))
        if self.verbose:
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
        self.gs.setParam("RESSPEED", self.params["DEF_WP_SPEED"])
        self.params = self.gs.getParams()
        self.geofences = self.gs.fenceList

        self.ownship_log = vehicle_log()
        self.traffic_log = {}
        self.delay = self.scenario.get("delay", 0)
        self.duration = 0

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
        dest = os.path.join(self.output_dir, self.name + ".json")
        with open(dest, 'w') as f:
            json.dump(simdata, f)
        print("\nWrote " + self.name + " simulation data")


    def write_params(self):
        param_file = os.path.join(self.output_dir, self.name + ".parm")
        with open(param_file, 'w') as f:
            for param_id, param_value in self.params.items():
                f.write("%-16s %f\n" % (param_id, param_value))

    def step(self, time_limit):
        if self.terminated:
            return True
        if not self.running:
            self.t0 = time.time()
            self.running = True

        if self.duration < time_limit:
            if self.duration >= self.delay:
                if not self.started:
                    self.gs.StartMission()
                    self.started = True
                    if self.verbose:
                        print("***Starting %s" % self.name)
            time.sleep(0.01)
            currentT = time.time()
            self.duration = currentT - self.t0

            self.gs.Update_traffic()

            msg = self.gs.master.recv_match(blocking=False, type=["GLOBAL_POSITION_INT"])

            if msg is None:
                return False

            # Store ownship position/velocity information
            self.ownship_log["t"].append(self.duration)
            self.ownship_log["position"].append([msg.lat/1E7, msg.lon/1E7,
                                                 msg.relative_alt/1E3])
            self.ownship_log["velocityNED"].append([msg.vx/1E2, msg.vy/1E2,
                                                    msg.vz/1E2])

            # Store traffic position/velocity information
            for i, traf in enumerate(self.gs.traffic_list):
                if i not in self.traffic_log.keys():
                    self.traffic_log[i] = vehicle_log()
                self.traffic_log[i]["t"].append(self.duration)
                self.traffic_log[i]["position"].append([traf.lat, traf.lon, traf.alt])
                self.traffic_log[i]["velocityNED"].append([traf.vx0, traf.vy0, traf.vz0])

            return False
        else:
            self.terminate()
            return True

    def run(self, time_limit):
        finished = False
        while not finished:
            finished = self.step(time_limit)

    def terminate(self):
        # Once simulation is finished, kill the icarous process
        self.ic.kill()
        if self.out is not None:
            subprocess.call(["kill", "-9", str(self.mav_forwarding.pid)])
        self.write_log()
        self.write_params()
