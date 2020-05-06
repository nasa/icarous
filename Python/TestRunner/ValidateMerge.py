import os
import sys
import json
import numpy as np
import pandas as pd
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
    conditions.append(verify_merge_complete)
    conditions.append(verify_merge_time_separation)
    conditions.append(verify_valid_schedule)
    conditions.append(verify_merge_order)
    conditions.append(verify_merge_spacing)
    conditions.append(verify_spacing)

    # Check conditions
    results = [c(vehicles) for c in conditions]

    # Print results
    print_results(results, output_dir.split('_')[-1].strip('/'))
    record_results(results, output_dir.split('/')[-1])

    if test:
        # Assert conditions
        for r in results:
            assert r[0], r[1]

    return results


def verify_valid_schedule(vehicles):
    '''Check that the scheduled arrival times are acceptable'''
    condition_name = "Valid Schedule"
    min_separation_time = 20 - 1

    computed_schedule = [v.metrics["computed_schedule"] for v in vehicles]
    scheduled_arrivals = sorted([v.metrics["sched_arr_time"] for v in vehicles])
    separation_times = np.diff(scheduled_arrivals)

    if all((s > min_separation_time for s in separation_times)):
        if any(computed_schedule):
            return True, "Computed valid merge point arrival schedule", condition_name
        else:
            return True, "No merge conflict - no scheduling required", condition_name
    else:
        return False, "Failed to find valid schedule", condition_name


def verify_merge_complete(vehicles):
    '''Check that all vehicles reached the merge point'''
    condition_name = "Merge Complete"

    reached_merge_point = [v.metrics["reached_merge_point"] for v in vehicles]

    if len(reached_merge_point) == 0:
        return False, "No merging vehicles", condition_name
    if all(reached_merge_point):
        return True, "All vehicles reached the merge point", condition_name
    else:
        return False, "Not all vehicles reached the merge point", condition_name


def verify_merge_order(vehicles):
    '''Check that the vehicles merged in the scheduled order'''
    condition_name = "Merge Order"

    scheduled_times = sorted([(v.metrics["sched_arr_time"], v.id) for v in vehicles])
    scheduled_order = [name for time, name in scheduled_times]
    actual_times = sorted([(v.metrics["actual_arr_time"], v.id) for v in vehicles
                           if v.metrics["reached_merge_point"]])
    actual_order = [name for time, name in actual_times]

    print("\nScheduled order: %s" % scheduled_order)
    print("Actual order: %s" % actual_order)

    if not verify_merge_complete(vehicles)[0]:
        return False, "N/A - not all vehicles reached merge point", condition_name
    if not verify_valid_schedule(vehicles)[0]:
        return False, "N/A - no valid schedule", condition_name

    if scheduled_order == actual_order:
        return True, "Arrival order matches schedule", condition_name
    else:
        return False, "Arrival order doesn't match schedule", condition_name


def verify_merge_time_separation(vehicles):
    '''Check merging data for arrival time separation'''
    condition_name = "Arrival Time Separation"
    #min_separation_time = 20
    min_separation_time = 10

    arrival_times = sorted([v.metrics["actual_arr_time"] for v in vehicles
                            if v.metrics["reached_merge_point"]])
    separation_times = np.diff(arrival_times)

    print("\nArrival times:")
    for i in range(len(arrival_times)):
        line = ("%.2f" % (arrival_times[i]))
        if i > 0:
            line += ("\t(+ %.2fs)" % separation_times[i-1])
        print(line)

    if not verify_merge_complete(vehicles)[0]:
        return False, "N/A - not all vehicles reached merge point", condition_name

    if all((s > min_separation_time for s in separation_times)):
        return True, "Sufficient time spacing at merge point", condition_name
    else:
        return False, "Insufficient time spacing at merge point", condition_name


def verify_merge_spacing(vehicles):
    '''Check minimum horizontal separation during merging operation'''
    condition_name = "Merge Spacing"
    #DTHR = 30
    DTHR = 25

    separation_ok = True
    print("\nMinimum separation distances during merge")
    for v1, v2 in itertools.combinations(vehicles, 2):
        time_range, dist = MA.compute_separation(v1, v2)
        min_separation, t_min = min(zip(dist, time_range))
        print("v%s to v%s min separation: %.2fm (at %.2fs)" %
              (v1.id, v2.id, min_separation, t_min))
        if (min_separation < DTHR):
            separation_ok = False
            msg = ("v%s and v%s violated minimum separation (%.2f < %.2f)" %
                   (v1.id, v2.id, min_separation, DTHR))

    if separation_ok:
        return True, "Acceptable spacing during merging operation", condition_name
    else:
        return False, msg, condition_name


def verify_spacing(vehicles):
    '''Check minimum horizontal separation during entire flight'''
    condition_name = "Spacing"
    #DTHR = 30
    DTHR = 25

    vehicles = []
    for i in range(args.num_vehicles):
        filename = "merger_appdata_" + str(i) + ".txt"
        filename = os.path.join(args.data_location, filename)
        if not os.path.isfile(filename):
            break
        data = MA.ReadMergerAppData(filename, vehicle_id=i, merge_id="all")
        vehicles.append(data)

    separation_ok = True
    print("\nMinimum separation distances during entire flight")
    for v1, v2 in itertools.combinations(vehicles, 2):
        time_range, dist = MA.compute_separation(v1, v2)
        min_separation, t_min = min(zip(dist, time_range))
        print("v%s to v%s min separation: %.2fm (at %.2fs)" %
              (v1.id, v2.id, min_separation, t_min))
        if (min_separation < DTHR):
            separation_ok = False
            msg = ("v%s and v%s violated minimum separation (%.2f < %.2f)" %
                   (v1.id, v2.id, min_separation, DTHR))

    if separation_ok:
        return True, "Acceptable spacing during entire flight", condition_name
    else:
        return False, msg, condition_name


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


def record_results(results, scenario_name):
    filename = "MergingResults.csv"
    if os.path.isfile(filename):
        table = pd.read_csv(filename, index_col=0)
    else:
        table = pd.DataFrame({})
    metrics = {r[2]:r[0] for r in results}
    index = scenario_name
    metrics = pd.DataFrame(metrics, index=[index])
    table = metrics.combine_first(table)
    table = table[metrics.keys()]
    table.to_csv(filename)


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Validate Icarous simulation data")
    parser.add_argument("data_location", help="directory where log files are")
    parser.add_argument("--merge_id", default=1, type=int, help="merge point id to analyze")
    parser.add_argument("--num_vehicles", default=10, type=int, help="number of vehicles")
    parser.add_argument("--plot", action="store_true", help="plot the scenario")
    parser.add_argument("--save", action="store_true", help="save the results")
    parser.add_argument("--test", action="store_true", help="assert test conditions")
    parser.add_argument("--many", action="store_true", help="validate many flights")
    args = parser.parse_args()

    flights = []
    if args.many:
        for f in os.listdir(args.data_location):
            flights.append(os.path.join(args.data_location, f))
    else:
        flights.append(args.data_location)

    for flight in flights:
        # Read merger log data (just during merge operation)
        vehicles = MA.process_data(flight, args.num_vehicles, args.merge_id)
        # Read merger log data (for entire flight)
        vehicles_entire_flight = MA.process_data(flight, args.num_vehicles, "all")

        # Compute metrics
        MA.compute_metrics(vehicles)
        MA.write_metrics(vehicles)

        # Generate plots
        if args.plot:
            MA.plot(vehicles, "dist2int", save=args.save)
            MA.plot_roles(vehicles, save=args.save)
            MA.plot_speed(vehicles, save=args.save)
            MA.plot_speed(vehicles_entire_flight, save=args.save)
            MA.plot_spacing(vehicles, save=args.save)
            MA.plot_spacing(vehicles_entire_flight, save=args.save)
            MA.plot_summary(vehicles, save=args.save)
            plt.show()

        # Check test conditions
        validate_merging_data(vehicles, output_dir=flight, test=args.test)
