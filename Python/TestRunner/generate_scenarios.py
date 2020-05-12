import pandas as pd
import os
import yaml
import argparse


# This script generates a yaml scenario file from a csv/spreadsheet.
# This makes it much easier to generate many scenarios by hand.

parser = argparse.ArgumentParser(description="Generate yaml scenarios from a table")
parser.add_argument("In", help="csv file defining a list of scenarios")
parser.add_argument("Out", nargs='?', help="yaml output file")
args = parser.parse_args()
if args.Out is None:
    args.Out = os.path.splitext(args.In)[0]+".yaml"

table = pd.read_csv(args.In)
scenario_list = []

for i, row in table.iterrows():
    # Read scenario info
    file_home = row.get("File Home", "")
    time_limit = row.get("Time Limit", 1000)
    scenario = {"name": row["Scenario Name"],
                "time_limit": time_limit,
                "merge_fixes": os.path.join(file_home, row.get("Merge Fixes", None))}

    # Read scenario info for each vehicle
    vehicles = []
    for i in range(int(row["Num Vehicles"])):
        name = "AC"+str(i)
        vehicle = {"name": name,
                   "cpu_id": i+1,
                   "waypoint_file": os.path.join(file_home, row[name+" Flightplan"]),
                   "parameter_file": os.path.join(file_home, row["Parameter File"]),
                   "param_adjustments": {},
                   "traffic": [],
                   "delay": row.get(name+" Delay", 0),
                   "time_limit": time_limit}
        if name+" Geofence" in row:
            vehicle["geofence_file"] = os.path.join(file_home, row[name+" Geofence"])

        # Read parameter adjustments
        for column in row.keys():
            if column.startswith(name+" Param:") or column.startswith("Param:"):
                param_id = column.split(":")[1]
                param_value = row[column]
                vehicle["param_adjustments"][param_id] = param_value

        vehicles.append(vehicle)

    scenario["vehicles"] = vehicles
    scenario_list.append(scenario)

# Write scenarios to a file
with open(args.Out, 'w') as f:
    yaml.dump(scenario_list, f)
    print("Scenarios written to %s" % args.Out)
