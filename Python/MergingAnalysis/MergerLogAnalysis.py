import numpy as np
import math
import os
from matplotlib import pyplot as plt
from scipy.interpolate import interp1d
import itertools
import pandas as pd


RADIUS_OF_EARTH = 6378100.0


class MergerData:
    def __init__(self, v_id, merge_id=1, group="test"):
        self.id = v_id
        self.merge_id = merge_id
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

    def get(self, x, t=None):
        """Return the value of state[x] at time t"""
        if t is None:
            t = self.t
        return interp1d(self.state["t"], self.state[x], axis=0,
                        bounds_error=False, fill_value="extrapolate")(t)


def ReadMergerAppData(filename, vehicle_id, merge_id=1, group="test"):
    with open(filename, 'r') as fp:
        fp.readline()
        data_string = fp.readlines()

    data = MergerData(vehicle_id, merge_id=merge_id, group=group)
    data.output_dir = os.path.dirname(filename)
    started_merge = False
    for line in data_string:
        line = line.rstrip('\n')
        entries = line.split(',')
        intID = int(entries[1])
        lon = float(entries[14])

        # Wait for reasonable lat/lon
        if abs(lon) < 1:
            continue

        if merge_id != "all":
            if not started_merge and intID != merge_id:
                continue
            if started_merge and intID != merge_id:
                break
        started_merge = True

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


def compute_metrics(vehicles):
    for v in vehicles:
        zone = v.get("zone")
        status = v.get("mergingStatus")
        numSch = v.get("numSch")
        dist2int = v.get("dist2int")
        v.metrics["group"] = v.group
        v.metrics["merge_id"] = v.merge_id
        v.metrics["vehicle_id"] = v.id
        v.metrics["coord_time"] = next(v.t[i] for i in range(len(v.t)) if zone[i] == 1)
        v.metrics["sched_time"] = next(v.t[i] for i in range(len(v.t)) if zone[i] == 2)
        v.metrics["entry_time"] = next(v.t[i] for i in range(len(v.t)) if zone[i] == 3)
        v.metrics["initial_speed"] = v.get("speed", v.metrics["sched_time"])
        v.metrics["computed_schedule"] = (numSch[-1] > numSch[0])
        v.metrics["reached_merge_point"] = (min(dist2int) < 10)
        v.metrics["sched_arr_time"] = v.get("currArrTime")[-1]

        if v.metrics["reached_merge_point"]:
            _, v.metrics["actual_arr_time"] = min(zip(dist2int, v.t))
        else:
            v.metrics["actual_arr_time"] = v.t[-1]

        if v.metrics["computed_schedule"]:
            v.metrics["handoff_time"] = next((v.t[i] for i in range(len(v.t))
                                        if status[i] == 1), v.metrics["actual_arr_time"])
            v.metrics["mean_consensus_time"] = np.mean(compute_consensus_times(v))
            v.metrics["merge_speed"] = v.get("mergeSpeed", v.metrics["entry_time"])
            v.metrics["actual_speed_to_handoff"] = average_speed(v, v.metrics["entry_time"],
                                                                 v.metrics["handoff_time"])
        else:
            v.metrics["handoff_time"] = None
            v.metrics["mean_consensus_time"] = None
            v.metrics["merge_speed"] = None
            v.metrics["actual_speed_to_handoff"] = None

        v.metrics["actual_speed_to_merge"] = average_speed(v, v.metrics["entry_time"],
                                                           v.metrics["actual_arr_time"])

        traffic = set(vehicles)
        traffic.remove(v)
        dist = []
        for traf in traffic:
            t, d = compute_separation(v, traf)
            dist += d
        v.metrics["min_sep_during_merge"] = min(dist)


def compute_consensus_times(vehicle):
    """ Compute the time it took to reach consensus """
    A = vehicle.get("t")
    B = vehicle.get("numSch")
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


def write_metrics(vehicles):
    """ Add vehicle metrics to a csv table """
    if len(vehicles) == 0:
        return
    filename = "MergingMetrics.csv"
    if os.path.isfile(filename):
        table = pd.read_csv(filename, index_col=0)
    else:
        table = pd.DataFrame({})
    for v in vehicles:
        index = v.group+"_"+str(v.merge_id)+"_"+str(v.id)
        metrics = pd.DataFrame(v.metrics, index=[index])
        table = metrics.combine_first(table)
    table = table[v.metrics.keys()]
    table.to_csv(filename)


def average_speed(vehicle, t1, t2):
    dX = vehicle.get("dist2int", t1) - vehicle.get("dist2int", t2)
    dT = t2 - t1
    return abs(dX/dT)


def compute_separation(v1, v2, time_range=None):
    if time_range is None:
        time_range = [t for t in v1.t if v2.t[0] < t < v2.t[-1]]
    else:
        time_range = [time_range]
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


def plot(vehicles, field, save=False):
    plt.figure()
    for v in vehicles:
        plt.plot(v.t, v.get(field), label="vehicle"+str(v.id))
    plt.title(field)
    plt.xlabel("time (s)")
    plt.ylabel(field)
    plt.legend()
    plt.grid()
    if save:
        plt.savefig(os.path.join(v.output_dir, field))


def plot_summary(vehicles, save=False):
    plt.figure()
    for v in vehicles:
        plt.plot(v.t, v.get("dist2int"), label="vehicle"+str(v.id))
    for v in vehicles:
        plt.plot(v.metrics["coord_time"],
                 v.get("dist2int", v.metrics["coord_time"]), '*')
        plt.plot(v.metrics["sched_time"],
                 v.get("dist2int", v.metrics["sched_time"]), '*')
        plt.plot(v.metrics["entry_time"],
                 v.get("dist2int", v.metrics["entry_time"]), '*')
        if v.metrics["computed_schedule"]:
            plt.plot(v.metrics["handoff_time"],
                     v.get("dist2int", v.metrics["handoff_time"]), 'r*')
        if v.metrics["reached_merge_point"]:
            plt.plot(v.metrics["actual_arr_time"],
                     v.get("dist2int", v.metrics["actual_arr_time"]), 'b*')
        plt.plot(v.metrics["sched_arr_time"], 0, 'g*')
    plt.title("Merging Operation Summary")
    plt.plot([], [], 'r*', label="merger app gives back control")
    plt.plot([], [], 'g*', label="scheduled arrival time")
    plt.plot([], [], 'b*', label="actual arrival time")
    plt.xlabel("time (s)")
    plt.ylabel("distance to merge point (m)")
    plt.legend()
    plt.grid()
    if save:
        plt.savefig(os.path.join(v.output_dir, "summary"))


def plot_spacing(vehicles, save=False):
    plt.figure()
    for v1, v2 in itertools.combinations(vehicles, 2):
        time_range, dist = compute_separation(v1, v2)
        plt.plot(time_range, dist, label="vehicle"+str(v1.id)+" to vehicle"+str(v2.id))
        d, t_min = min(zip(dist, time_range))
    plt.legend()
    plt.grid()
    plt.ylim((0, plt.ylim()[1]))
    if save:
        if v1.merge_id == "all":
            title = "spacing"
        else:
            title = "spacing_merge" + str(v1.merge_id)
        plt.savefig(os.path.join(v1.output_dir, title))


def plot_speed(vehicles, save=False):
    for v in vehicles:
        plt.figure()
        line1, = plt.plot(v.t, v.get("speed"))
        line2, = plt.plot(v.t, v.get("mergeSpeed"), '--')
        line3, = plt.plot(v.t, v.get("commandedSpeed"), '-.')
        line1.set_label("vehicle"+str(v.id)+" actual speed")
        line2.set_label("vehicle"+str(v.id)+" merge speed")
        line3.set_label("vehicle"+str(v.id)+" commanded speed")
        plt.xlabel('time (s)')
        plt.ylabel('speed (m/s)')
        plt.legend()
        plt.grid()
        if save:
            plt.savefig(os.path.join(v.output_dir, "speed_"+str(v.id)))


def plot_roles(vehicles, save=False):
    plt.figure()
    plt.title("Raft Node Roles")
    colors = ['y', 'r', 'b', 'g']
    labels = ["NEUTRAL", "FOLLOWER", "CANDIDATE", "LEADER"]
    for v in vehicles:
        for rc in v.role_changes:
            role, t0, tEnd = rc
            plt.plot([t0, tEnd], [v.id, v.id], '-', c = colors[role],
                     linewidth=20.0, solid_capstyle="butt")
    for c, l in zip(colors, labels):
        plt.plot([], [], 's', color=c, label=l)
    vids = [v.id for v in vehicles]
    vnames = ["vehicle"+str(v.id) for v in vehicles]
    plt.yticks(vids, vnames)
    plt.ylim([min(vids) - 1, max(vids) + 1])
    plt.xlabel("Time (s)")
    plt.legend()
    plt.grid()
    if save:
        plt.savefig(os.path.join(v.output_dir, "roles"))


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Validate Icarous simulation data")
    parser.add_argument("data_location", help="directory where log files are")
    parser.add_argument("--merge_id", default=1, type=int, help="merge point id to analyze")
    parser.add_argument("--num_vehicles", default=10, type=int, help="number of vehicles")
    parser.add_argument("--plot", action="store_true", help="plot the scenario")
    parser.add_argument("--save", action="store_true", help="save the results")
    args = parser.parse_args()

    # Read merger log data (just during merge operation)
    vehicles = []
    group = args.data_location.strip("/").split("/")[-1]
    for i in range(args.num_vehicles):
        filename = "merger_appdata_" + str(i) + ".txt"
        filename = os.path.join(args.data_location, filename)
        if not os.path.isfile(filename):
            break
        data = ReadMergerAppData(filename, vehicle_id=i, merge_id=args.merge_id, group=group)
        vehicles.append(data)

    # Read merger log data (for entire flight)
    vehicles_entire_flight = []
    for i in range(args.num_vehicles):
        filename = "merger_appdata_" + str(i) + ".txt"
        filename = os.path.join(args.data_location, filename)
        if not os.path.isfile(filename):
            break
        data = ReadMergerAppData(filename, vehicle_id=i, merge_id="all")
        vehicles_entire_flight.append(data)

    # Compute metrics
    compute_metrics(vehicles)
    write_metrics(vehicles)

    # Generate plots
    if args.plot:
        plot_summary(vehicles, save=args.save)
        plot(vehicles, "dist2int", save=args.save)
        plot_roles(vehicles, save=args.save)
        plot_speed(vehicles_entire_flight, save=args.save)
        plot_spacing(vehicles, save=args.save)
        plot_spacing(vehicles_entire_flight, save=args.save)
        plt.show()
