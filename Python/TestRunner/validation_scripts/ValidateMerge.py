import os
import sys
import numpy as np
import copy

sys.path.append("../MergingAnalysis")
import MergerLogAnalysis as MA
sys.path.append(os.path.dirname(os.path.realpath(__file__)))
from ValidationHelper import print_results, record_results, update_params


validation_params = {
    "arr_time_allow": 5,    # allowable arrival time error (s)
    "sched_time_allow": 1,  # allowable schedule time error (s)
    "h_allow": 0.85,        # use h_allow*DTHR to check spacing
    }


# Validation functions
def validate_merging_data(vehicles, merge_id=1, name="test",
                          test=False, save=False):
    '''Check simulation data for test conditions'''

    merging_vehicles = []
    for v in vehicles:
        for m in v.metrics.get(merge_id, {}).values():
            new_v = copy.deepcopy(v)
            new_v.metrics = m
            merging_vehicles.append(new_v)
    if not merging_vehicles:
        print("No vehicles merged at point %d" % merge_id)
        return

    # Define conditions
    conditions = []
    conditions.append(verify_merge_complete)
    conditions.append(verify_merge_time_separation)
    conditions.append(verify_valid_schedule)
    conditions.append(verify_merge_order)
    conditions.append(verify_merge_spacing)
    conditions.append(verify_spacing)

    # Check conditions
    results = [c(merging_vehicles) for c in conditions]

    # Print results
    print_results(results, name)
    if save:
        record_results(results, name)

    # Assert conditions
    if test:
        for r in results:
            assert r[0], r[1]

    return results


def verify_valid_schedule(vehicles):
    '''Check that the scheduled arrival times are acceptable'''
    condition_name = "Valid Schedule"
    min_sep_time_sched = (vehicles[0].params["MIN_SEP_TIME"] -
                          validation_params["sched_time_allow"])
    min_sep_time_actual = (vehicles[0].params["MIN_SEP_TIME"] -
                           validation_params["arr_time_allow"])

    computed_schedule = [v.metrics["computed_schedule"] for v in vehicles]
    scheduled_arrivals = sorted([v.metrics["sched_arr_time"] for v in vehicles])
    actual_arrivals = sorted([v.metrics["actual_arr_time"] for v in vehicles
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


def verify_merge_complete(vehicles):
    '''Check that all vehicles reached the merge point'''
    condition_name = "Merge Complete"

    reached_merge_point = [v.metrics["reached_merge_point"] for v in vehicles]

    if all(reached_merge_point):
        return True, "All vehicles reached the merge point", condition_name
    else:
        return False, "Not all vehicles reached the merge point", condition_name


def verify_merge_order(vehicles):
    '''Check that the vehicles merged in the scheduled order'''
    condition_name = "Merge Order"
    min_sep_time = (vehicles[0].params["MIN_SEP_TIME"] -
                    validation_params["arr_time_allow"])

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


def verify_merge_time_separation(vehicles):
    '''Check merging data for arrival time separation'''
    condition_name = "Arrival Time Separation"
    min_sep_time = (vehicles[0].params["MIN_SEP_TIME"] -
                    validation_params["arr_time_allow"])

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


def verify_merge_spacing(vehicles):
    '''Check minimum horizontal separation during merging operation'''
    condition_name = "Merge Spacing"
    DTHR = (vehicles[0].params["DET_1_WCV_DTHR"]*0.3048*
            validation_params["h_allow"])

    separation_ok = True
    for v in vehicles:
        min_sep = v.metrics["min_sep_during_merge"]
        if min_sep < DTHR:
            msg = "%s violated min spacing (%.2fm < %.2fm)" % (v.id, min_sep, DTHR)
            separation_ok = False
            break

    if separation_ok:
        return True, "Acceptable spacing during merge", condition_name
    else:
        return False, msg, condition_name


def verify_spacing(vehicles):
    '''Check minimum horizontal separation during entire flight'''
    condition_name = "Spacing"
    DTHR = (vehicles[0].params["DET_1_WCV_DTHR"]*0.3048*
            validation_params["h_allow"])

    separation_ok = True
    for v in vehicles:
        min_sep = v.metrics["min_sep_during_flight"]
        if min_sep < DTHR:
            msg = "%s violated min spacing (%.2fm < %.2fm)" % (v.id, min_sep, DTHR)
            separation_ok = False
            break

    if separation_ok:
        return True, "Acceptable spacing during entire flight", condition_name
    else:
        return False, msg, condition_name


def run_validation(flight_dir, merge_id=1, test=False, plot=False, save=False):
    # Read merger log data (just during merge operation)
    vehicles = MA.process_data(flight_dir)

    # Compute merging metrics
    MA.compute_metrics(vehicles, merge_id)
    MA.write_metrics(vehicles)

    # Check test conditions
    scenario_name = os.path.basename(os.path.normpath(flight_dir))
    validate_merging_data(vehicles, merge_id, scenario_name, test, save)
    # Generate plots
    if plot:
        from matplotlib import pyplot as plt
        MA.plot_roles(vehicles, merge_id=merge_id, save=save)
        MA.plot_speed(vehicles, merge_id=merge_id, save=save)
        MA.plot_speed(vehicles, merge_id="all", save=save)
        MA.plot_spacing(vehicles, merge_id=merge_id, save=save)
        MA.plot_spacing(vehicles, merge_id="all", save=save)
        MA.plot_flight_trace(vehicles, merge_id="all", save=save)
        MA.plot_summary(vehicles, merge_id=merge_id, save=save)
        MA.plot_spacing_summary(vehicles, save=save)
        plt.show()
        plt.close("all")


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Validate Icarous simulation data")
    parser.add_argument("data_location", help="directory containing merger log files")
    parser.add_argument("--merge_id", default=1, type=int, help="merge point id to analyze")
    parser.add_argument("--many", action="store_true", help="validate many flights")
    parser.add_argument("--plot", action="store_true", help="plot the scenario")
    parser.add_argument("--save", action="store_true", help="save the results")
    parser.add_argument("--test", action="store_true", help="assert test conditions")
    parser.add_argument("--param", nargs=2, action="append", default=[],
                        metavar=("KEY", "VALUE"), help="set validation parameter")
    args = parser.parse_args()

    validation_params = update_params(validation_params, args.param)

    # Gather all simulation data files
    flights = []
    if args.many:
        for f in os.listdir(args.data_location):
            if os.path.isdir(os.path.join(args.data_location, f)):
                flights.append(os.path.join(args.data_location, f))
    else:
        flights.append(args.data_location)

    for flight_dir in flights:
        run_validation(flight_dir, args.merge_id, args.test, args.plot, args.save)

