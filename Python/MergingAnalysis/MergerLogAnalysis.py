import numpy as np
import math
import json
import os
from matplotlib import pyplot as plt
from scipy.interpolate import interp1d
import itertools
import pandas as pd


RADIUS_OF_EARTH = 6378100.0


class MergerData:
    def __init__(self, v_id, group="test"):
        self.id = v_id
        self.output_dir = ""
        self.group = group
        self.t = []
        self.state = {"t": [],
                      "intID": [],
                      "dist2int": [],
                      "speed": [],
                      "nodeRole": [],
                      "earlyArrTime": [],
                      "currArrTime": [],
                      "lateArrTime": [],
                      "zone": [],
                      "numSch": [],
                      "mergeSpeed": [],
                      "commandedSpeed": [],
                      "mergeDev": [],
                      "mergingStatus": [],
                      "lat": [],
                      "lon": [],
                      "alt": []}
        self.current_role = None
        self.role_changes = []
        self.metrics = {}

    def get(self, x, t=None, merge_id=None, default="extrapolate"):
        """Return the value of state[x] at time, t, or during merge, merge_id"""
        if t is None and merge_id is None:
            # Return the value of x for all time available
            return self.state[x]
        elif t is not None:
            # Return the value of x at time, t
            return interp1d(self.state["t"], self.state[x], axis=0,
                            bounds_error=False, fill_value=default)(t)
        elif merge_id is not None:
            # Return the value of x during merge, merge_id
            if merge_id == "spacing":
                merge_id = -1
            if merge_id == "all":
                return self.state[x]
            else:
                return [self.state[x][i] for i in range(len(self.state[x]))
                        if self.state["intID"][i] == merge_id]


def ReadMergerAppData(filename, vehicle_id, group="test"):
    with open(filename, 'r') as fp:
        fp.readline()
        data_string = fp.readlines()

    data = MergerData(vehicle_id, group=group)
    data.output_dir = os.path.dirname(filename)
    for line in data_string:
        line = line.rstrip('\n')
        entries = line.split(',')
        if len(entries) != 17 or not entries[-1].strip():
            continue
        intID = int(entries[1])
        lon = float(entries[14])

        # Wait for reasonable lat/lon
        if abs(lon) < 1:
            continue

        t = float(entries[0])
        data.t.append(t)
        data.state["t"].append(t)
        data.state["intID"].append(int(entries[1]))
        data.state["dist2int"].append(float(entries[2]))
        data.state["speed"].append(float(entries[3]))
        data.state["nodeRole"].append(int(entries[4]))
        data.state["earlyArrTime"].append(float(entries[5].lstrip().lstrip('(')))
        data.state["currArrTime"].append(float(entries[6]))
        data.state["lateArrTime"].append(float(entries[7].lstrip().rstrip(')')))
        data.state["zone"].append(int(entries[8]))
        data.state["numSch"].append(int(entries[9]))
        data.state["mergeSpeed"].append(float(entries[10]))
        data.state["commandedSpeed"].append(float(entries[11]))
        data.state["mergeDev"].append(float(entries[12]))
        data.state["mergingStatus"].append(int(entries[13]))
        data.state["lat"].append(float(entries[14]))
        data.state["lon"].append(float(entries[15]))
        data.state["alt"].append(float(entries[16]))

        role = int(entries[4])
        if data.current_role is not None:
            data.role_changes[-1][2] = t
        if role != data.current_role:
            data.role_changes.append([role, t, None])
        data.current_role = role

    return data


def read_params(data_location):
    simlog = next(f for f in os.listdir(data_location) if f.endswith(".json"))
    with open(os.path.join(data_location, simlog)) as f:
        data = json.load(f)
    params = data["parameters"]
    return params


def compute_metrics(vehicles, merge_id=1):
    for v in vehicles:
        v.metrics["group"] = v.group
        v.metrics["merge_id"] = merge_id
        v.metrics["vehicle_id"] = v.id
    compute_election_times(vehicles)
    for v in vehicles:
        t = v.get("t", merge_id=merge_id)
        zone = v.get("zone", merge_id=merge_id)
        status = v.get("mergingStatus", merge_id=merge_id)
        numSch = v.get("numSch", merge_id=merge_id)
        dist2int = v.get("dist2int", merge_id=merge_id)
        coord = next((t[i] for i in range(len(t)) if zone[i] == 1), None)
        v.metrics["coord_time"] = coord
        sched = next((t[i] for i in range(len(t)) if zone[i] == 2 and t[i] > coord), None)
        v.metrics["sched_time"] = sched
        entry = next((t[i] for i in range(len(t)) if zone[i] == 3 and t[i] > sched), None)
        v.metrics["entry_time"] = entry
        v.metrics["initial_speed"] = v.get("speed", v.metrics["sched_time"])
        v.metrics["computed_schedule"] = (max(numSch) > 0)
        v.metrics["reached_merge_point"] = (min(dist2int) < 10)
        v.metrics["sched_arr_time"] = v.get("currArrTime", merge_id=merge_id)[-1]

        if v.metrics["reached_merge_point"]:
            _, v.metrics["actual_arr_time"] = min(zip(dist2int, t))
        else:
            v.metrics["actual_arr_time"] = None

        if v.metrics["computed_schedule"]:
            consensus_times = compute_consensus_times(v, merge_id)
            v.metrics["mean_consensus_time"] = np.mean(consensus_times)
            v.metrics["merge_speed"] = v.get("mergeSpeed", v.metrics["entry_time"])
        else:
            v.metrics["mean_consensus_time"] = None
            v.metrics["merge_speed"] = None

        speed = average_speed(v, v.metrics["entry_time"], v.metrics["actual_arr_time"])
        v.metrics["actual_speed_to_merge"] = speed

        min_sep = compute_min_separation(v, vehicles, merge_id=merge_id)
        v.metrics["min_sep_during_merge"] = min_sep
        min_sep = compute_min_separation(v, vehicles, merge_id="all")
        v.metrics["min_sep_during_flight"] = min_sep

        spacing_speed = v.get("speed", merge_id="spacing")
        v.metrics["mean_non-merging_speed"] = np.mean(spacing_speed)


def write_metrics(vehicles, merge_id=1, output_file="MergingMetrics.csv"):
    """ Add vehicle metrics to a csv table """
    if len(vehicles) == 0:
        return
    if os.path.isfile(output_file):
        table = pd.read_csv(output_file, index_col=0)
    else:
        table = pd.DataFrame({})
    for v in vehicles:
        index = v.group+"_"+str(merge_id)+"_"+str(v.id)
        metrics = pd.DataFrame(v.metrics, index=[index])
        table = metrics.combine_first(table)
    table = table[v.metrics.keys()]
    table.to_csv(output_file)


def get_leader(vehicles, t):
    if type(t) is not list:
        t = [t]
    for v in vehicles:
        v.roles = v.get("nodeRole", t)
    leader = [next((v.id for v in vehicles if v.roles[i] == 3), None)
              for i in range(len(t))]
    return leader


def compute_election_times(vehicles, merge_id=1):
    """ Compute the time it took for each vehicle to be elected leader """
    all_time = []
    v_dict = {}
    for v in vehicles:
        all_time += v.get("t", merge_id=merge_id)
        v_dict[v.id] = v
        v.metrics["time_to_become_leader"] = None
    all_time.sort()
    leader = get_leader(vehicles, all_time)

    i = 0
    while True:
        time_no_leader = next((t for t, l in zip(all_time[i:], leader[i:])
                               if l is None), None)
        if time_no_leader is None: break
        time_leader, leader_id = next(((t, l) for t, l in zip(all_time[i:], leader[i:])
                                       if l is not None and t > time_no_leader),
                                       (None, None))
        if time_leader is None: break
        election_time = time_leader - time_no_leader
        v_dict[leader_id].metrics["time_to_become_leader"] = election_time
        i = all_time.index(time_leader) + 1


def compute_consensus_times(vehicle, merge_id=1):
    """ Compute the time it took to reach consensus """
    A = vehicle.get("t", merge_id=merge_id)
    B = vehicle.get("numSch", merge_id=merge_id)
    times = []
    start = -1
    stop = 0
    old = 0
    new = 0
    collect = True
    for i in range(len(A)):
        if B[i] < 1e-3:
           continue
        if collect is True:
            if start < 0:
                start = A[i]
            new = B[i]
            if new == old:
                stop = A[i]
                times.append(stop - start)
                collect = False
                start = -1
            old = new
        else:
            new = B[i]
            if new != old:
                start = A[i]
                collect = True
            old = new
    return times


def average_speed(vehicle, t1, t2):
    if t1 is None or t2 is None:
        return None
    dX = vehicle.get("dist2int", t1) - vehicle.get("dist2int", t2)
    dT = t2 - t1
    return abs(dX/dT)


def compute_min_separation(ownship, traffic, merge_id=1):
    traffic = set(traffic)
    traffic.remove(ownship)
    dist = []
    for traf in traffic:
        t, d = compute_separation(ownship, traf, merge_id=merge_id)
        dist += d
    return min(dist)


def compute_separation(v1, v2, merge_id=1):
    time_range = v1.get("t", merge_id=merge_id)
    lat1 = v1.get("lat", time_range)
    lon1 = v1.get("lon", time_range)
    lat2 = v2.get("lat", time_range)
    lon2 = v2.get("lon", time_range)
    dist = [gps_distance(la1, lo1, la2, lo2) for la1,lo1,la2,lo2 in
            zip(lat1,lon1,lat2,lon2)]
    return time_range, dist


def gps_distance(lat1, lon1, lat2, lon2):
    '''return distance between two points in meters,
    coordinates are in degrees
    thanks to http://www.movable-type.co.uk/scripts/latlong.html'''
    lat1 = math.radians(lat1)
    lat2 = math.radians(lat2)
    lon1 = math.radians(lon1)
    lon2 = math.radians(lon2)
    dLat = lat2 - lat1
    dLon = lon2 - lon1

    a = math.sin(0.5*dLat)**2 + math.sin(0.5*dLon)**2 * math.cos(lat1) * math.cos(lat2)
    c = 2.0 * math.atan2(math.sqrt(a), math.sqrt(1.0-a))
    return RADIUS_OF_EARTH * c


def plot(vehicles, field, merge_id="all", save=False, fmt=""):
    plt.figure()
    for v in vehicles:
        time_range = v.get("t", merge_id=merge_id)
        plt.plot(time_range, v.get(field, time_range), fmt, label="vehicle"+str(v.id))
    plt.title(field + " vs time")
    plt.xlabel("time (s)")
    plt.ylabel(field)
    plt.legend()
    plt.grid()
    if save:
        plt.savefig(os.path.join(v.output_dir, field))


def plot_summary(vehicles, merge_id=1, save=False):
    plt.figure()
    for v in vehicles:
        t = v.get("t", merge_id=merge_id)
        line, = plt.plot(t, v.get("dist2int", merge_id=merge_id), label="vehicle"+str(v.id))
        v.color = line.get_color()
    for v in vehicles:
        if v.metrics["computed_schedule"]:
            plt.plot(v.metrics["sched_arr_time"], 0, 'o', color=v.color)
        if v.metrics["reached_merge_point"]:
            arrival_time = v.metrics["actual_arr_time"]
            dist_at_arrival = v.get("dist2int", v.metrics["actual_arr_time"])
            plt.plot(arrival_time, dist_at_arrival, 'b*')
    plt.plot(plt.xlim(), [vehicles[0].params["ENTRY_RADIUS"]]*2, "r--")
    plt.plot(plt.xlim(), [vehicles[0].params["SCHEDULE_ZONE"]]*2, "--", color="orange")
    plt.plot(plt.xlim(), [vehicles[0].params["COORD_ZONE"]]*2, "b--")
    plt.title("Merging Operation Summary")
    plt.plot([], [], 'ok', label="scheduled arrival time")
    plt.plot([], [], 'b*', label="actual arrival time")
    plt.xlabel("time (s)")
    plt.ylabel("distance to merge point (m)")
    plt.legend()
    plt.grid()
    if save:
        plt.savefig(os.path.join(v.output_dir, "summary"))


def plot_spacing(vehicles, merge_id=1, save=False):
    plt.figure()
    spacing_value = 30
    for v1, v2 in itertools.combinations(vehicles, 2):
        time_range, dist = compute_separation(v1, v2, merge_id=merge_id)
        if len(dist) > 0:
            plt.plot(time_range, dist, label="vehicle"+str(v1.id)+" to vehicle"+str(v2.id))
            d, t_min = min(zip(dist, time_range))
    plt.plot(plt.xlim(), [spacing_value]*2, 'm--', label="Minimum allowed spacing")
    plt.legend()
    plt.grid()
    plt.ylim((0, plt.ylim()[1]))
    if merge_id == "all":
        title = "spacing"
        plt.title("Vehicle Horizontal Spacing (entire flight)")
    else:
        title = "spacing_merge" + str(merge_id)
        plt.title("Vehicle Horizontal Spacing (during merge %d)" % merge_id)
    if save:
        plt.savefig(os.path.join(v1.output_dir, title))


def plot_speed(vehicles, merge_id=1, save=False):
    for v in vehicles:
        plt.figure()
        time_range = v.get("t", merge_id=merge_id)
        line1, = plt.plot(time_range, v.get("speed", time_range))
        line2, = plt.plot(time_range, v.get("mergeSpeed", time_range), '--')
        line3, = plt.plot(time_range, v.get("commandedSpeed", time_range), '-.')
        line1.set_label("vehicle"+str(v.id)+" actual speed")
        line2.set_label("vehicle"+str(v.id)+" merge speed")
        line3.set_label("vehicle"+str(v.id)+" commanded speed")
        if merge_id != "all":
            if v.metrics["coord_time"] is not None:
                plt.axvspan(v.metrics["coord_time"],
                            v.metrics["sched_time"], color="blue", alpha=0.3)
            if v.metrics["sched_time"] is not None:
                plt.axvspan(v.metrics["sched_time"],
                            v.metrics["entry_time"], color="orange", alpha=0.5)
            if v.metrics["entry_time"] is not None:
                plt.axvspan(v.metrics["entry_time"],
                            v.metrics["actual_arr_time"], color="red", alpha=0.3)
        plt.xlabel('time (s)')
        plt.ylabel('speed (m/s)')
        plt.legend()
        plt.grid()
        if merge_id == "all":
            title = "speed_" + str(v.id)
            plt.title("vehicle%d Speed (entire flight)" % v.id)
        else:
            title = "speed_%s_merge%d" % (v.id, merge_id)
            plt.title("vehicle%s Speed (during merge %d)" % (v.id, merge_id))
        if save:
            plt.savefig(os.path.join(v.output_dir, title))


def plot_roles(vehicles, merge_id=1, save=False):
    labels = ["NEUTRAL", "FOLLOWER", "CANDIDATE", "LEADER"]
    plt.figure()
    for v in vehicles:
        plt.plot(v.get("t"), v.get("nodeRole"), label="vehicle"+str(v.id))
    plt.grid()
    plt.legend()
    plt.title("Raft communication node roles")
    plt.yticks(range(4), labels)
    plt.xlabel("Time (s)")
    if save:
        plt.savefig(os.path.join(v.output_dir, "nodeRoles"))


def plot_flight_trace(vehicles, merge_id=1, save=False):
    plt.figure()
    for v in vehicles:
        time_range = v.get("t", merge_id=merge_id)
        lon = v.get("lon", time_range)
        lat = v.get("lat", time_range)
        trace, = plt.plot(lon, lat, label="vehicle"+str(v.id))
        plt.plot(lon[0], lat[0], 'o', color=trace.get_color())
        plt.plot(lon[-1], lat[-1], 'x', color=trace.get_color())
    plt.grid()
    plt.legend()
    plt.title("Flight Trace")
    plt.xlabel("Longitude (deg)")
    plt.ylabel("Latitude (deg)")
    if save:
        plt.savefig(os.path.join(v.output_dir, "flight_trace"))


def process_data(data_location):
    vehicles = []
    group = data_location.strip("/").split("/")[-1]
    i = 0
    found_log = True
    while found_log:
        found_log = False
        for f in os.listdir(data_location):
            if f.startswith("merger_appdata_"+str(i)):
                found_log = True
                filename = os.path.join(data_location, f)
                data = ReadMergerAppData(filename, i, group)
                data.params = read_params(data_location)
                vehicles.append(data)
        i += 1
    return vehicles


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Validate Icarous simulation data")
    parser.add_argument("data_location", help="directory where log files are")
    parser.add_argument("--merge_id", default=1, type=int, help="merge point id to analyze")
    parser.add_argument("--plot", action="store_true", help="plot the scenario")
    parser.add_argument("--save", action="store_true", help="save the results")
    parser.add_argument("--show", action="store_true", help="show the plots")
    args = parser.parse_args()

    # Read merger log data
    vehicles = process_data(args.data_location)

    # Compute metrics
    compute_metrics(vehicles, merge_id=args.merge_id)
    write_metrics(vehicles, merge_id=args.merge_id)

    # Generate plots
    if args.plot:
        plot_roles(vehicles, merge_id=args.merge_id, save=args.save)
        plot_speed(vehicles, merge_id=args.merge_id, save=args.save)
        plot_speed(vehicles, merge_id="all", save=args.save)
        plot_spacing(vehicles, merge_id=args.merge_id, save=args.save)
        plot_spacing(vehicles, merge_id="all", save=args.save)
        plot_flight_trace(vehicles, merge_id="all", save=args.save)
        plot_summary(vehicles, merge_id=args.merge_id, save=args.save)
    if args.show:
        plt.show()
    else:
        plt.close("all")
