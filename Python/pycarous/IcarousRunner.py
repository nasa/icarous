import sys
import os
import time
import shutil
import signal
import subprocess
import numpy as np
from pymavlink import mavutil, mavwp

from ichelper import (ParseAccordParamFile,
                      ReadFlightplanFile,
                      GetFlightplan,
                      ConstructWaypointsFromList,
                      Getfence,
                      distance)
from IcarousInterface import IcarousInterface
import BatchGSModule as GS

sim_home = os.getcwd()
file_location = os.path.dirname(os.path.abspath(__file__))
icarous_home = os.path.abspath(os.path.join(file_location, "../.."))
icarous_exe = os.path.join(icarous_home, "exe", "cpu1")

class IcarousRunner(IcarousInterface):
    """
    Interface to launch and control a cFS instance of ICAROUS from python
    """
    def __init__(self, home_pos, callsign="SPEEDBIRD", vehicleID=0,
                 verbose=1, logRateHz=5, apps="default", sim_app="rotorsim", out=None):
        """
        Initialize an instance of ICAROUS running in cFS
        :param apps: List of the apps to run, or "default" to use default apps
        :param sim_app: Icarous app to use for vehicle simulation (default is "rotorsim")
        :param out: port number to forward MAVLink data for visualization
                    (use out=None to turn off MAVLink output)
        Other parameters are defined in parent class, see IcarousInterface.py
        """
        super().__init__(home_pos, callsign, vehicleID, verbose)

        self.sim_app = sim_app
        self.sim_process = None
        self.SetApps(apps=apps)
        print("%s sim app: %s" % (self.callsign, sim_app))
        self.icConfig = os.path.join(icarous_home, "exe", "ram", "IcarousConfig.txt")
        self.out = out
        self.cpu_id = self.vehicleID + 1
        self.spacecraft_id = self.vehicleID
        self.simType = "cFS/%s" % sim_app

        # Set up mavlink connections
        icarous_port = 14553 + 10*self.spacecraft_id
        gs_port = icarous_port
        if self.out is not None:
            # Use mavproxy to forward mavlink stream (for visualization)
            gs_port = icarous_port + 1
            logname = "%s-%f" % (self.callsign, time.time())
            self.mav_forwarding = subprocess.Popen(["mavproxy.py",
                                            "--master=127.0.0.1:" + str(icarous_port),
                                            "--out=127.0.0.1:" + str(gs_port),
                                            "--out=127.0.0.1:" + str(self.out),
                                            "--target-system=1",
                                            "--target-component=5",
                                            "--logfile=" + logname],
                                            stdout=subprocess.DEVNULL)
            if self.verbose:
                print("%s : Telemetry for %s is on 127.0.0.1:%d" %
                      (self.callsign, self.callsign, self.out))

        # Open connection for virtual ground station
        master = mavutil.mavlink_connection("127.0.0.1:"+str(gs_port))

        # Start the ICAROUS process
        os.chdir(icarous_exe)
        fpic = open("icout-%s-%f.txt" % (self.callsign, time.time()), 'w')
        self.ic = subprocess.Popen(["./core-cpu1",
                                    "-I "+str(self.spacecraft_id),
                                    "-C "+str(self.cpu_id)],
                                    stdout=fpic)
        os.chdir(sim_home)

        # Pause for a couple of seconds here so that ICAROUS can boot up
        if self.verbose:
            print("%s : Waiting for heartbeat..." % self.callsign)
        master.wait_heartbeat()
        self.gs = GS.BatchGSModule(master, target_system=1, target_component=0)
        self.params = ParseAccordParamFile(self.icConfig)
        # Launch SITL if necessary
        if sim_app == "arducopter":
            self.launch_arducopter()
        elif sim_app == "px4":
            self.launch_px4()

    def launch_arducopter(self):
        start_point = ','.join(str(x) for x in self.home_pos + [0])
        logfile = "sitl-%s-%f.tlog" % (self.callsign, time.time())
        arguments = [
            "sim_vehicle.py",
            "-v", "ArduCopter",
            "-l", str(start_point),
            "--use-dir", "sitl_files",
            "-I", str(self.spacecraft_id),
            "-m", "--logfile="+logfile,
        ]
        self.sim_process = subprocess.Popen(arguments, stdout=subprocess.DEVNULL)

        if self.verbose:
            print("Waiting several seconds to allow ArduCopter to start up")
        time.sleep(60)

    def launch_px4(self):
        print("launching px4")

    def SetPosUncertainty(self, xx, yy, zz, xy, yz, xz, coeff=0.8):
        # Setting position uncertainty isn't supported for cFS simulations
        pass

    def SetVelUncertainty(self, xx, yy, zz, xy, yz, xz, coeff=0.8):
        # Setting velocity uncertainty isn't supported for cFS simulations
        pass

    def InputTraffic(self, callsign, position, velocity):
        self.gs.Send_traffic(callsign, position, velocity)
        positionNED = self.ConvertToLocalCoordinates(position)
        self.RecordTraffic(callsign, position, velocity, positionNED)

    def InputMergeData(self, logs, delay):
        pass

    def InputFlightplan(self, fp, eta=False, repair=False,setInitialConditions=True):

        waypoints = ConstructWaypointsFromList(fp,eta) 
        self.flightplan1 = waypoints 
        self.plans.append(waypoints)
        self.localPlans.append(self.GetLocalFlightPlan(waypoints))
        self.gs.wploader.clear()
        for wp in fp:
            lat, lon, alt, wp_metric = wp
            self.gs.wploader.add_latlonalt(lat, lon, alt)
            self.gs.wploader.wp(-1).param4 = wp_metric
        self.gs.send_all_waypoints()
        time.sleep(1)

    def InputFlightplanFromFile(self, filename, eta=False, repair=False,startTimeShift=0):
        self.gs.loadWaypoint(filename)
        time.sleep(1)
        fp = GetFlightplan(filename, self.defaultWPSpeed, eta)
        waypoints = ConstructWaypointsFromList(fp,eta) 
        self.flightplan1 = waypoints 
        self.plans.append(waypoints)
        self.localPlans.append(self.GetLocalFlightPlan(waypoints))

    def InputGeofence(self, filename):
        self.gs.loadGeofence(filename)
        self.fenceList = Getfence(filename)
        for fence in self.fenceList:
            localFence = []
            gf = []
            for vertex in fence['vertices']:
                localFence.append(self.ConvertToLocalCoordinates([*vertex,0]))
                gf.append([*vertex,0])
            self.localFences.append(localFence)
            self.fences.append(gf)

    def InputMergeFixes(self, filename):
        wp, _, _, _, _ = ReadFlightplanFile(filename)
        self.localMergeFixes = list(map(self.ConvertToLocalCoordinates, wp))
        self.mergeFixes = wp
        dest = os.path.join(icarous_home, "exe", "ram", "merge_fixes.txt")
        shutil.copy(filename, dest)

    def SetParameters(self, params):
        
        self.params.update(params)
        paramstr=''
        for item in self.params.items():
            paramstr+= item[0]+'='+str(item[1])+'\n'
        paramFile = self.icConfig
        fp2 = open(paramFile,'w')
        fp2.write(paramstr)
        fp2.close()

    def InputMergeLogs(self, logs, delay):
        # When using cFS, merge data is exchanged over SBN
        pass

    def StartMission(self):
        self.gs.StartMission()
        self.missionStarted = True

    def CheckMissionComplete(self):
        if self.missionComplete:
            return True
        if self.missionStarted and self.land:
            self.missionComplete = True
            print("%s : Landing" % self.callsign)
        return self.missionComplete

    def Run(self):
        if self.terminated:
            return True
        if not self.running:
            self.t0 = time.time()
            self.running = True

        time.sleep(0.01)
        self.currTime = time.time()

        # Record position data to logs
        msg_types = ["GLOBAL_POSITION_INT", "ADSB_VEHICLE", "STATUSTEXT"]
        msg = self.gs.master.recv_match(blocking=False, type=msg_types)
        if msg is None:
            return False
        if msg.get_type() == "GLOBAL_POSITION_INT":
            # Store ownship position/velocity information
            self.position = [msg.lat*1e-7, msg.lon*1e-7, msg.alt*1e-3]
            self.velocity = [msg.vx*1e-2, msg.vy*1e-2, msg.vz*1e-2]
            self.localPos = self.ConvertToLocalCoordinates(self.position)
            self.RecordOwnship()
        elif msg.get_type() == "ADSB_VEHICLE":
            # Store traffic position/velocity information
            if msg.emitter_type == 255 or msg.ICAO_address == self.vehicleID:
                 return False
            position = [msg.lat*1e-7, msg.lon*1e-7, msg.altitude*1e-3]
            if 0 in position:
                return False
            positionNED = self.ConvertToLocalCoordinates(position)
            heading = msg.heading*1e-2
            vy = msg.hor_velocity*1e-2*np.sin(np.radians(heading))
            vx = msg.hor_velocity*1e-2*np.cos(np.radians(heading))
            velocityNED = [vx, vy, msg.ver_velocity*1e-2]
            callsign = msg.callsign
            self.RecordTraffic(callsign, position, velocityNED, positionNED)
        elif msg.get_type() == "STATUSTEXT":
            print("%s : %s" % (self.callsign, msg.text))
            if "Landing" in msg.text:
                self.land = True

        self.TransmitPosition()

        return False

    def Terminate(self):
        # Once simulation is finished, kill the icarous process
        if self.terminated:
            return
        self.terminated = True
        self.ic.kill()
        if self.out is not None:
            self.mav_forwarding.send_signal(signal.SIGINT)
        if self.sim_process is not None:
            self.sim_process.send_signal(signal.SIGINT)

    def SetApps(self, apps="default"):
        """
        Set the apps that ICAROUS will run
        :param apps: List of the apps to run, or "default" to use default apps
        """
        if apps == "default":
            apps = ["Icarouslib","port_lib", "scheduler", "gsInterface",
                    "cognition", "guidance", "traffic", "trajectory",
                    "geofence", "rotorsim"]

        # Set app used for vehicle simulation (remove other sim apps if present)
        if "rotorsim" in apps: apps.remove("rotorsim")
        if "arducopter" in apps: apps.remove("arducopter")
        if "px4" in apps: apps.remove("px4")
        apps.append(self.sim_app)

        approot = os.path.join(icarous_home, "apps")
        outputloc = os.path.join(icarous_exe, "cf")
        script = os.path.join(icarous_home, "Python/cFS_Utils/ConfigureApps.py")

        subprocess.call(["python3", script, approot, outputloc, *apps])
        self.apps = apps
