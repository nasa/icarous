import os
import sys
import json
import numpy as np
import itertools
from matplotlib import pyplot as plt
from scipy.interpolate import interp1d

sys.path.append("../Batch")
import BatchGSModule as GS

sys.path.append("../MergingAnalysis")
import MergerLogAnalysis as MA


# Validation functions
def validate_merging_data(vehicles, output_dir="", test=False):
    '''Check simulation data for test conditions'''
    # Define conditions
    conditions = []
    conditions.append(verify_merge_time_separation)
    conditions.append(verify_valid_schedule)
    conditions.append(verify_merge_order)
    conditions.append(verify_separation_distance)

    # Check conditions
    results = [c(vehicles) for c in conditions]

    # Print results
    print_results(results, output_dir.split('_')[-1].strip('/'))

    if test:
        # Assert conditions
        for r in results:
            assert r[0], r[1]

    return results


def verify_valid_schedule(vehicles):
    '''Check that the scheduled arrival times are acceptable'''
    condition_name = "Valid Schedule"
    min_separation_time = 20 - 1

    scheduled_arrivals = sorted([v.metrics["sched_arr_time"] for v in vehicles])
    separation_times = np.diff(scheduled_arrivals)

    if all((s > min_separation_time for s in separation_times)):
        return True, "Computed valid merge point arrival schedule", condition_name
    else:
        return False, "Failed to find valid schedule", condition_name


def verify_merge_order(vehicles):
    '''Check that the vehicles merged in the scheduled order'''
    condition_name = "Merge Order"

    if not verify_valid_schedule(vehicles):
        return True, "N/A - no valid schedule", condition_name

    scheduled_times = sorted([(v.metrics["sched_arr_time"], v.id) for v in vehicles])
    scheduled_order = [name for time, name in scheduled_times]
    actual_times = sorted([(v.metrics["actual_arr_time"], v.id) for v in vehicles])
    actual_order = [name for time, name in actual_times]

    if scheduled_order == actual_order:
        return True, "Arrival order matches schedule", condition_name
    else:
        return False, "Arrival order doesn't match schedule", condition_name


def verify_merge_time_separation(vehicles):
    '''Check merging data for arrival time separation'''
    condition_name = "Arrival Time Separation"
    min_separation_time = 20

    arrival_times = sorted([v.metrics["actual_arr_time"] for v in vehicles])
    separation_times = np.diff(arrival_times)

    print("\nArrival times:")
    for i in range(len(arrival_times)):
        line = ("%.2f" % (arrival_times[i]))
        if i > 0:
            line += ("\t(%.2fs)" % separation_times[i-1])
        print(line)

    if all((s > min_separation_time for s in separation_times)):
        return True, "Sufficient time spacing at merge point", condition_name
    else:
        return False, "Insufficient time spacing at merge point", condition_name


def verify_separation_distance(vehicles):
    '''Check merging data for minimum separation'''
    condition_name = "Spacing"
    DTHR = 30

    print("\nMinimum separation distances")
    for v1, v2 in itertools.combinations(vehicles, 2):
        time_range, dist = MA.compute_separation(v1, v2)
        min_separation = min(dist)
        print("v%s to v%s min separation: %.2fm" % (v1.id, v2.id, min_separation))
        if (min_separation < DTHR):
            msg = ("v%s and v%s violated minimum separation (%.2f < %.2f)" %
                   (v1.id, v2.id, min_separation, DTHR))
            return False, msg, condition_name

    return True, "Acceptable spacing throughout flight", condition_name


def print_results(results, scenario_name):
    ''' print results of a test scenario '''
    if all([res for res, msg, name in results]):
        print("\n\033[32m\"%s\" scenario PASSED:\033[0m" % scenario_name)
    else:
        print("\n\033[31m\"%s\" scenario FAILED:\033[0m" % scenario_name)
    for result, msg, name in results:
        if result:
            print("\t\033[32m* %s - PASS:\033[0m %s" % (name, msg))
        else:
            print("\t\033[31mX %s - FAIL:\033[0m %s" % (name, msg))


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Validate Icarous simulation data")
    parser.add_argument("data_location", help="directory where log files are")
    parser.add_argument("--merge_id", default=1, type=int, help="merge point id to analyze")
    parser.add_argument("--num_vehicles", default=10, type=int, help="number of vehicles")
    parser.add_argument("--plot", action="store_true", help="plot the scenario")
    parser.add_argument("--save", action="store_true", help="save the results")
    parser.add_argument("--test", action="store_true", help="assert test conditions")
    args = parser.parse_args()

    # Read merger log data
    vehicles = []
    group = args.data_location.strip("/").split("/")[-1]
    for i in range(args.num_vehicles):
        filename = "merger_appdata_" + str(i) + ".txt"
        filename = os.path.join(args.data_location, filename)
        if not os.path.isfile(filename):
            break
        data = MA.ReadMergerAppData(filename, vehicle_id=i, merge_id=args.merge_id, group=group)
        vehicles.append(data)

    # Generate plots
    if args.plot:
        MA.plot(vehicles, "dist2int", save=args.save)
        MA.plot_roles(vehicles, save=args.save)
        MA.plot_speed(vehicles, save=args.save)
        MA.plot_separation(vehicles, save=args.save)
        plt.show()

    # Compute metrics
    MA.compute_metrics(vehicles, plot=args.plot, save=args.save)
    MA.write_metrics(vehicles)

    # Check test conditions
    validate_merging_data(vehicles, output_dir=args.data_location, test=args.test)
