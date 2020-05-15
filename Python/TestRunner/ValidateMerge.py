import os
import sys
import json
import numpy as np
import itertools
from matplotlib import pyplot as plt
from scipy.interpolate import interp1d

sys.path.append("../pyicarous")
import BatchGSModule as GS

sys.path.append("../MergingAnalysis")
import MergerLogAnalysis as MA


DEFAULT_VALIDATION_PARAMS = {"arr_time_allow": 5,   # allowable arrival time error
                             "sched_time_allow": 1, # allowable schedule time error
                             "h_allow": 0.85}       # use h_allow*DTHR to check spacing
params = dict(DEFAULT_VALIDATION_PARAMS)


# Validation functions
def validate_merging_data(vehicles, params=DEFAULT_VALIDATION_PARAMS,
                          name="test", test=False, save=False):
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
    results = [c(vehicles, params) for c in conditions]

    # Print results
    print_results(results, name)
    if save:
        record_results(results, name)

    # Assert conditions
    if test:
        for r in results:
            assert r[0], r[1]

    return results


def verify_valid_schedule(vehicles, params=DEFAULT_VALIDATION_PARAMS):
    '''Check that the scheduled arrival times are acceptable'''
    condition_name = "Valid Schedule"
    min_sep_time_sched = vehicles[0].params["MIN_SEP_TIME"] - params["sched_time_allow"]
    min_sep_time_actual = vehicles[0].params["MIN_SEP_TIME"] - params["arr_time_allow"]

    computed_schedule = [v.metrics["computed_schedule"] for v in vehicles]
    scheduled_arrivals = sorted([v.metrics["sched_arr_time"] for v in vehicles])
    actual_arrivals = sorted([(v.metrics["actual_arr_time"], v.id) for v in vehicles
                              if v.metrics["reached_merge_point"]])
    scheduled_sep = np.diff(scheduled_arrivals)
    actual_sep = np.diff(actual_arrivals)

    schedule_exists = any(computed_schedule)
    schedule_ok = all((s > min_sep_time_sched for s in scheduled_sep))
    arrival_ok = all((s > min_sep_time_actual for s in actual_sep))

    if schedule_exists and schedule_ok:
        return True, "Computed valid merge point arrival schedule", condition_name
    elif schedule_exists and not schedule_ok:
        return False, "Computed invalid merge point arrival schedule", condition_name
    elif not schedule_exists and arrival_ok:
        return True, "No merge conflict - no scheduling required", condition_name
    else:
        return False, "No schedule found", condition_name


def verify_merge_complete(vehicles, params=DEFAULT_VALIDATION_PARAMS):
    '''Check that all vehicles reached the merge point'''
    condition_name = "Merge Complete"

    reached_merge_point = [v.metrics["reached_merge_point"] for v in vehicles]

    if all(reached_merge_point):
        return True, "All vehicles reached the merge point", condition_name
    else:
        return False, "Not all vehicles reached the merge point", condition_name


def verify_merge_order(vehicles, params=DEFAULT_VALIDATION_PARAMS):
    '''Check that the vehicles merged in the scheduled order'''
    condition_name = "Merge Order"
    min_sep_time = vehicles[0].params["MIN_SEP_TIME"] - params["arr_time_allow"]

    computed_schedule = [v.metrics["computed_schedule"] for v in vehicles]
    scheduled_arrivals = sorted([(v.metrics["sched_arr_time"], v.id) for v in vehicles])
    scheduled_order = [name for time, name in scheduled_arrivals]
    actual_arrivals = sorted([(v.metrics["actual_arr_time"], v.id) for v in vehicles
                           if v.metrics["reached_merge_point"]])
    actual_order = [name for time, name in actual_arrivals]
    actual_sep = np.diff([a[0] for a in actual_arrivals])

    schedule_exists = any(computed_schedule)
    order_ok = (scheduled_order == actual_order)
    arrival_sep_ok = all((s > min_sep_time for s in actual_sep))

    if schedule_exists:
        if order_ok:
            return True, "Merge order matches scheduled order", condition_name
        else:
            return False, "Merge order does not match scheduled order", condition_name
    else:
        if arrival_sep_ok:
            return True, "No merge conflict - no scheduling required", condition_name
        else:
            return False, "No schedule found", condition_name


def verify_merge_time_separation(vehicles, params=DEFAULT_VALIDATION_PARAMS):
    '''Check merging data for arrival time separation'''
    condition_name = "Arrival Time Separation"
    min_sep_time = vehicles[0].params["MIN_SEP_TIME"] - params["arr_time_allow"]

    reached_merge_point = [v.metrics["reached_merge_point"] for v in vehicles]
    merge_complete = all(reached_merge_point)
    arrival_times = sorted([v.metrics["actual_arr_time"] for v in vehicles
                            if v.metrics["reached_merge_point"]])
    separation_times = np.diff(arrival_times)

    print("\nArrival times:")
    for i in range(len(arrival_times)):
        line = ("%.2f" % (arrival_times[i]))
        if i > 0:
            line += ("\t(+ %.2fs)" % separation_times[i-1])
        print(line)

    if not merge_complete:
        return False, "N/A - not all vehicles reached merge point", condition_name

    if all((s > min_sep_time for s in separation_times)):
        return True, "Sufficient time spacing at merge point", condition_name
    else:
        return False, "Insufficient time spacing at merge point", condition_name


def verify_merge_spacing(vehicles, params=DEFAULT_VALIDATION_PARAMS):
    '''Check minimum horizontal separation during merging operation'''
    condition_name = "Merge Spacing"
    DTHR = vehicles[0].params["DET_1_WCV_DTHR"]*0.3048*params["h_allow"]

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


def verify_spacing(vehicles, params=DEFAULT_VALIDATION_PARAMS):
    '''Check minimum horizontal separation during entire flight'''
    condition_name = "Spacing"
    DTHR = vehicles[0].params["DET_1_WCV_DTHR"]*0.3048*params["h_allow"]

    flight_dir = vehicles[0].output_dir
    vehicles_entire_flight = MA.process_data(flight_dir, merge_id="all")

    separation_ok = True
    print("\nMinimum separation distances during entire flight")
    for v1, v2 in itertools.combinations(vehicles_entire_flight, 2):
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
    import pandas as pd
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


def run_validation(flight_dir, test=False, plot=False, save=False):
    # Read merger log data (just during merge operation)
    vehicles = MA.process_data(flight_dir)
    # Read merger log data (for entire flight)
    vehicles_entire_flight = MA.process_data(flight_dir, merge_id="all")

    # Compute merging metrics
    MA.compute_metrics(vehicles)
    MA.write_metrics(vehicles)

    # Check test conditions
    scenario_name = os.path.basename(os.path.normpath(flight_dir))
    validate_merging_data(vehicles, params, scenario_name, test, save)

    # Generate plots
    if plot:
        MA.plot_roles(vehicles, save=save)
        MA.plot_speed(vehicles, save=save)
        MA.plot_speed(vehicles_entire_flight, save=save)
        MA.plot_spacing(vehicles, save=save)
        MA.plot_spacing(vehicles_entire_flight, save=save)
        MA.plot_flight_trace(vehicles_entire_flight, save=save)
        MA.plot_summary(vehicles, save=save)
        plt.show()
    plt.close("all")


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Validate Icarous simulation data")
    parser.add_argument("data_location", help="directory where log files are")
    parser.add_argument("--merge_id", default=1, type=int, help="merge point id to analyze")
    parser.add_argument("--num_vehicles", default=10, type=int, help="number of vehicles")
    parser.add_argument("--many", action="store_true", help="validate many flights")
    parser.add_argument("--plot", action="store_true", help="plot the scenario")
    parser.add_argument("--save", action="store_true", help="save the results")
    parser.add_argument("--test", action="store_true", help="assert test conditions")
    parser.add_argument("--param", nargs=2, action="append", default=[],
                        metavar=("KEY", "VALUE"), help="set validation parameter")
    args = parser.parse_args()
    args = parser.parse_args()

    # Set validation parameters
    for p in args.param:
        if p[0] not in params:
            print("** Warning, unrecognized validation parameter: %s" % p[0])
            continue
        params[p[0]] = float(p[1])

    # Gather all simulation data files
    flights = []
    if args.many:
        for f in os.listdir(args.data_location):
            flights.append(os.path.join(args.data_location, f))
    else:
        flights.append(args.data_location)

    for flight_dir in flights:
        run_validation(flight_dir, args.test, args.plot, args.save)
