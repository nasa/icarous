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
                      "mergePassID": [],
                      "lat": [],
                      "lon": [],
                      "alt": []}
        self.pass_ids = {-1: [None], "all": [None]}
        self.metrics = {}

    def get(self, x, t=None, merge_id=None, pass_id=None, default="extrapolate"):
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
                        if self.state["intID"][i] == merge_id
                        and pass_id is None
                        or self.state["mergePassID"][i] == pass_id]


def ReadMergerAppData(filename, vehicle_id, group="test"):
    with open(filename, 'r') as fp:
        fp.readline()
        data_string = fp.readlines()

    data = MergerData(vehicle_id, group=group)
    data.output_dir = os.path.dirname(filename)
    prev_intID = -1
    time_last_merging = -10
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

        if data.state["mergingStatus"][-1] == 0:
            intID = -1
            data.state["intID"][-1] = intID

        # Keep track of number of times passing through each merge fix
        if intID != -1:
            if intID not in data.pass_ids:
                data.pass_ids[intID] = set()
                data.metrics[intID] = {}
            # Increment pass_id if it's been 10 seconds since last merging
            if (t - time_last_merging > 10) or intID != prev_intID:
                pass_id = len(data.pass_ids[intID]) + 1
            data.pass_ids[intID].add(pass_id)
            time_last_merging = t
            prev_intID = intID
            data.state["mergePassID"].append(pass_id)
        else:
            data.state["mergePassID"].append(None)

    # Ignore passes through a merge fix that last < 1 second total
    for mid in data.pass_ids:
        for pid in list(data.pass_ids[mid]):
            t = data.get("t", merge_id=mid, pass_id=pid)
            if t[-1] - t[0] < 1:
                data.pass_ids[mid].remove(pid)

    return data


def read_params(data_location, vehicle_id=0):
    simlog = next(f for f in os.listdir(data_location)
                  if f.endswith(".json") and str(vehicle_id) in f)
    with open(os.path.join(data_location, simlog)) as f:
        data = json.load(f)
    params = data["parameters"]
    return params


def compute_metrics(vehicles, merge_id=1):
    for v in vehicles:
        if merge_id not in v.pass_ids:
            continue
        for pass_id in v.pass_ids[merge_id]:
            metrics = {}
            metrics["group"] = v.group
            metrics["merge_id"] = merge_id
            metrics["pass_id"] = pass_id
            metrics["vehicle_id"] = v.id

            election_time = compute_election_time(v, vehicles, merge_id, pass_id)
            metrics["time_to_become_leader"] = election_time
            t = v.get("t", merge_id=merge_id, pass_id=pass_id)
            zone = v.get("zone", merge_id=merge_id, pass_id=pass_id)
            dist2int = v.get("dist2int", merge_id=merge_id, pass_id=pass_id)
            coordT = next((t[i] for i in range(len(t)) if zone[i] == 1), None)
            metrics["coord_time"] = coordT
            schedT = next((t[i] for i in range(len(t)) if zone[i] == 2), None)
            metrics["sched_time"] = schedT
            entryT = next((t[i] for i in range(len(t)) if zone[i] == 3), None)
            metrics["entry_time"] = entryT
            metrics["reached_merge_point"] = (min(dist2int) < 10)
            if schedT is not None:
                metrics["initial_speed"] = v.get("speed", schedT)
            else:
                metrics["initial_speed"] = None
            if entryT is not None:
                metrics["computed_schedule"] = (v.get("numSch", entryT) > 0)
                metrics["sched_arr_time"] = v.get("currArrTime", entryT)
            else:
                metrics["computed_schedule"] = False
                metrics["sched_arr_time"] = 0

            if metrics["reached_merge_point"]:
                _, metrics["actual_arr_time"] = min(zip(dist2int, t))
            else:
                metrics["actual_arr_time"] = None

            if metrics["computed_schedule"]:
                consensus_times = compute_consensus_times(v, merge_id, pass_id)
                metrics["mean_consensus_time"] = np.mean(consensus_times)
                metrics["merge_speed"] = v.get("mergeSpeed", entryT)
            else:
                metrics["mean_consensus_time"] = None
                metrics["merge_speed"] = None

            speed = average_speed(v, metrics["entry_time"], metrics["actual_arr_time"])
            metrics["actual_speed_to_merge"] = speed

            min_sep = compute_min_separation(v, vehicles, merge_id=merge_id)
            metrics["min_sep_during_merge"] = min_sep
            min_sep = compute_min_separation(v, vehicles, merge_id="all")
            metrics["min_sep_during_flight"] = min_sep

            spacing_speed = v.get("speed", merge_id="spacing")
            metrics["mean_non-merging_speed"] = np.mean(spacing_speed)
            v.metrics[merge_id][pass_id] = metrics


def write_metrics(vehicles, output_file="MergingMetrics.csv"):
    """ Add vehicle metrics to a csv table """
    if os.path.isfile(output_file):
        table = pd.read_csv(output_file, index_col=0)
    else:
        table = pd.DataFrame({})

    for v in vehicles:
        for merge_id in v.metrics.keys():
            for pass_id in v.metrics[merge_id].keys():
                index = "%s_%d_%d_%d" % (v.group, merge_id, v.id, pass_id)
                metrics = pd.DataFrame(v.metrics[merge_id][pass_id], index=[index])
                table = metrics.combine_first(table)
                headers = v.metrics[merge_id][pass_id].keys()
                table = table[headers]
    table.to_csv(output_file)


def get_leader(vehicles, t):
    if type(t) is not list:
        t = [t]
    for v in vehicles:
        v.roles = v.get("nodeRole", t)
    leader = [next((v.id for v in vehicles if v.roles[i] == 3), None)
              for i in range(len(t))]
    return leader


def compute_election_time(ownship, vehicles, merge_id=1, pass_id=None):
    """ Compute the time it took for each vehicle to be elected leader """
    time_range = ownship.get("t", merge_id=merge_id, pass_id=pass_id)
    leader = get_leader(vehicles, time_range)

    time_no_leader = next((t for t, l in zip(time_range, leader)
                           if l is None), time_range[0])
    time_elected = next((t for t, l in zip(time_range, leader)
                         if l is ownship.id), None)

    if time_elected is not None:
        time_to_become_leader = time_elected - time_no_leader
        return time_to_become_leader
    else:
        return None


def compute_consensus_times(vehicle, merge_id=1, pass_id=None):
    """ Compute the time it took to reach consensus """
    A = vehicle.get("t", merge_id=merge_id, pass_id=pass_id)
    B = vehicle.get("numSch", merge_id=merge_id, pass_id=pass_id)
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


def compute_separation(v1, v2, merge_id=1, pass_id=None):
    time_range = v1.get("t", merge_id=merge_id, pass_id=pass_id)
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


def plot(vehicles, *fields, merge_id="all", save=False, fmt=""):
    plt.figure()
    for v in vehicles:
        time_range = v.get("t", merge_id=merge_id)
        for f in fields:
            plt.plot(time_range, v.get(f, time_range), fmt,
                     label="vehicle"+str(v.id))
    plt.title(fields[0] + " vs time")
    plt.xlabel("time (s)")
    plt.ylabel(fields[0])
    plt.legend()
    plt.grid()
    if save:
        plt.savefig(os.path.join(v.output_dir, fields[0]))


def plot_summary(vehicles, merge_id=1, save=False):
    plt.figure()

    for v in vehicles:
        if merge_id not in v.pass_ids:
            continue
        for pass_id in v.pass_ids[merge_id]:
            t = v.get("t", merge_id=merge_id, pass_id=pass_id)
            line, = plt.plot(t, v.get("dist2int", merge_id=merge_id, pass_id=pass_id))
            color = line.get_color()
            label = "vehicle%d" % v.id
            if pass_id > 1:
                label += " (pass %d)" % pass_id
            line.set_label(label)
            if v.metrics[merge_id][pass_id]["computed_schedule"]:
                schedT = v.metrics[merge_id][pass_id]["sched_arr_time"]
                plt.plot(schedT, 0, 'o', color=color)
            if v.metrics[merge_id][pass_id]["reached_merge_point"]:
                arrT = v.metrics[merge_id][pass_id]["actual_arr_time"]
                dist_at_arrival = v.get("dist2int", arrT)
                plt.plot(arrT, dist_at_arrival, 'b*')

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


def plot_spacing_summary(vehicles, save=False):
    if len(vehicles) < 2:
        return
    fig, (ax1, ax2) = plt.subplots(2, sharex=True)
    spacing_value = vehicles[0].params["DET_1_WCV_DTHR"]*0.3048

    # Plot spacing
    for v1, v2 in itertools.combinations(vehicles, 2):
        time_range, dist = compute_separation(v1, v2, merge_id="all")
        if len(dist) > 0:
            line, = ax1.plot(time_range, dist)
            line.set_label("vehicle%d to vehicle%d spacing" % (v1.id, v2.id))
    ax1.plot(plt.xlim(), [spacing_value]*2, 'm--', label="Minimum allowed spacing")
    ax1.set_ylim((0, ax1.get_ylim()[1]))
    ax1.set_ylabel("Horizontal separation (m)")
    box = ax1.get_position()
    ax1.set_position([box.x0, box.y0, box.width*0.8, box.height])
    ax1.legend(loc="center left", bbox_to_anchor=(1, 0.5), fontsize=6)
    ax1.grid()

    # Plot speeds
    for v in vehicles:
        time_range = v.get("t", merge_id="all")
        xlim = [min(time_range), max(time_range)]
        def_speed = v.params["DEF_WP_SPEED"]
        line1, = ax2.plot(xlim, [def_speed]*2, "--")
        v.color = line1.get_color()

    # Highlight merging
    for v in vehicles:
        time_range = v.get("t", merge_id="all")
        for mid in v.metrics:
            for pid in v.metrics[mid]:
                coordT = v.metrics[mid][pid].get("coord_time")
                arrT = v.metrics[mid][pid].get("actual_arr_time")
                ax1.axvspan(coordT, arrT, color=v.color, alpha=0.3)
                ax2.axvspan(coordT, arrT, color=v.color, alpha=0.3)
        line2, = ax2.plot(time_range, v.get("speed", time_range), color=v.color)
        line2.set_label("vehicle%d speed" % v.id)

    ax2.set_ylabel("Vehicle speed (m/s)")
    ax2.set_xlabel("Time (s)")
    box = ax2.get_position()
    ax2.set_position([box.x0, box.y0, box.width*0.8, box.height])
    ax2.legend(loc="center left", bbox_to_anchor=(1, 0.5), fontsize=6)
    ax2.grid()

    fig.suptitle("Spacing Summary")
    if save:
        plt.savefig(os.path.join(v.output_dir, "spacing_summary"))


def plot_spacing(vehicles, merge_id=1, save=False):
    if len(vehicles) < 2:
        return
    plt.figure()
    spacing_value = vehicles[0].params["DET_1_WCV_DTHR"]*0.3048

    # Plot separation
    for v1, v2 in itertools.combinations(vehicles, 2):
        if merge_id != "all" and merge_id not in v1.metrics:
            continue
        blank, = plt.plot([], [])
        color = blank.get_color()
        line = None
        for pid in v1.pass_ids[merge_id]:
            time_range, dist = compute_separation(v1, v2, merge_id, pid)
            if len(dist) > 0:
                line, = plt.plot(time_range, dist, color=color)
        if line:
            line.set_label("vehicle"+str(v1.id)+" to vehicle"+str(v2.id))

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
        if merge_id == "all":
            title = "speed_" + str(v.id)
            plt.title("vehicle%d Speed (entire flight)" % v.id)
            merge_ids = list(v.metrics.keys())
        else:
            if merge_id not in v.pass_ids:
                continue
            merge_ids = [merge_id]
            title = "speed_%s_merge%d" % (v.id, merge_id)
            plt.title("vehicle%s Speed (during merge %d)" % (v.id, merge_id))

        # Plot speed
        for pass_id in v.pass_ids[merge_id]:
            time_range = v.get("t", merge_id=merge_id, pass_id=pass_id)
            line1, = plt.plot(time_range, v.get("speed", time_range))
            line2, = plt.plot(time_range, v.get("mergeSpeed", time_range), '--')
            line3, = plt.plot(time_range, v.get("commandedSpeed", time_range), '-.')
            line1.set_label("vehicle"+str(v.id)+" actual speed")
            line2.set_label("vehicle"+str(v.id)+" merge speed")
            line3.set_label("vehicle"+str(v.id)+" commanded speed")

        # Highlight merges
        for mid in merge_ids:
            for pid in v.metrics[mid]:
                coordT = v.metrics[mid][pid]["coord_time"]
                schedT = v.metrics[mid][pid]["sched_time"]
                entryT = v.metrics[mid][pid]["entry_time"]
                arrT = v.metrics[mid][pid]["actual_arr_time"]
                plt.axvspan(coordT, schedT, color="blue", alpha=0.3)
                plt.axvspan(schedT, entryT, color="orange", alpha=0.5)
                plt.axvspan(entryT, arrT, color="red", alpha=0.3)

        plt.xlabel('time (s)')
        plt.ylabel('speed (m/s)')
        plt.legend()
        plt.grid()
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
                data.params = read_params(data_location, i)
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
    write_metrics(vehicles)

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
