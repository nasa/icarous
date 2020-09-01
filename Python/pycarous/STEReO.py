from SimEnvironment import SimEnvironment
from Icarous import Icarous
from ichelper import GetHomePosition, ReadTrafficInput

import os
home_dir = os.path.abspath(".")
output_dir = "STEReO_data"

manned_fp = "STEReO_data/STEReO_manned.txt"
manned_params = "STEReO_data/STEReO_manned.parm"
uav_fp = "STEReO_data/STEReO_uav.txt"
uav_params = "STEReO_data/STEReO_uav.parm"

# Initialize simulation environment
sim = SimEnvironment()

# Set the home position for the simulation
manned_home = GetHomePosition(manned_fp)
uav_home = GetHomePosition(uav_fp)

# Initialize Icarous class
manned = Icarous(manned_home,callsign="MANNED",simtype="UAM_VTOL",verbose=1)
manned.SetParametersFromFile(manned_params)
manned.InputFlightplanFromFile(manned_fp)
uav = Icarous(uav_home,callsign="UAV",simtype="UAM_VTOL",verbose=1, vehicleID=1)
uav.SetParametersFromFile(uav_params)
uav.InputFlightplanFromFile(uav_fp)

# Add icarous instance to sim environment
sim.AddIcarousInstance(manned,time_limit=180)
sim.AddIcarousInstance(uav,time_limit=180, delay=30)

# Set position uncertainty for vehicles in the simulation
sim.SetPosUncertainty(0.0, 0.0, 0, 0, 0, 0)

# Run the Simulation
sim.RunSimulation()

# Save json log outputs
os.chdir(output_dir)
sim.WriteLog()
os.chdir(home_dir)