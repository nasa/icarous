import os
import sys
import json
import numpy as np
from scipy.interpolate import interp1d
from polygon_contain import Vector, definitely_inside

sys.path.append(os.path.dirname(os.path.realpath(__file__)))
from ValidationHelper import (print_results,
                              record_results,
                              find_logs,
                              update_params)

validation_params = {
     "h_allow": 0.85,       # use h_allow*DTHR to check WC violations
     "v_allow": 0.65,       # use v_allow*ZTHR to check WC violations
     "wp_radius": 5,        # dist (m) to consider a waypoint reached
    }


def GetPolygons(fenceList):
    """
    Constructs a list of Polygon (for use with PolyCARP) given a list of fences
    :param fenceList: list of fence dictionaries
    :return: list of polygons (a polygon here is a list of vertices)
    """
    Polygons = []
    for fence in fenceList:
        polygon = [Vector(vertex[1], vertex[0]) for vertex in fence]
        Polygons.append(polygon)
    return Polygons


def get_planned_waypoints(simdata):
    """ Return the list of wps that are reachable without violating a geofence """
    if not simdata.get("geofences"):
        # All waypoints should be reachable
        return range(len(simdata["waypoints"]))

    planned_wps = []
    origin = simdata["ownship"]["position"][0]
    geofences = GetPolygons(simdata["ownship"]["localFences"])
    keep_in_fence = geofences[0]
    keep_out_fences = geofences[1:]
    waypoints = simdata["ownship"]["localPlans"][0]

    for i, wp in enumerate(waypoints):
        reachable = True
        s = Vector(wp[2], wp[1])

        # Check for keep in fence violations
        if not definitely_inside(keep_in_fence, s, 0.01):
            reachable = False

        # Check for keep out fence violations
        for fence in keep_out_fences:
            if definitely_inside(fence, s, 0.01):
                reachable = False

        if reachable:
            planned_wps.append(i)

    return planned_wps


def validate_sim_data(simdata, name="test", save=False, test=False):
    '''Check simulation data for test conditions'''
    # Define conditions
    conditions = []
    conditions.append(verify_waypoint_progress)
    conditions.append(verify_geofence)
    conditions.append(verify_traffic)

    # Check conditions
    results = [c(simdata) for c in conditions]

    # Print results
    print_results(results, name)
    if save:
        record_results(results, name)

    # Assert conditions
    if test:
        for r in results:
            assert r[0], r[1]

    return results


def verify_waypoint_progress(simdata):
    '''Check simulation data for progress towards goal'''
    condition_name = "Waypoint Progress"
    wp_radius = validation_params["wp_radius"]
    origin = simdata["ownship"]["position"][0]
    waypoints = simdata["ownship"]["localPlans"][0]
    pos_local = simdata["ownship"]["positionNED"]

    reached = []
    for pos in pos_local:
        # Check for reached waypoints
        for wp_seq, wp_pos in enumerate(waypoints):
            # If within wp_radius, add to reached
            dist = np.sqrt((pos[0] - wp_pos[1])**2 + (pos[1] - wp_pos[2])**2)
            if dist < wp_radius and wp_seq not in reached:
                #print("reached", wp[3], pos)
                reached.append(wp_seq)

    reached_expected = get_planned_waypoints(simdata)
    #print("reached  %s" % reached)
    #print("expected %s" % reached_expected)
    if set(reached) == set(reached_expected):
        return True, "Reached all planned waypoints", condition_name
    else:
        return False, "Did not reach all planned waypoints", condition_name


def verify_geofence(simdata):
    '''Check simulation data for geofence violations'''
    condition_name = "Geofencing"

    geofences = GetPolygons(simdata["ownship"]["localFences"])
    if len(geofences) == 0:
        return True, "No Geofences", condition_name
    keep_in_fence = geofences[0]
    keep_out_fences = geofences[1:]

    pos_local = simdata["ownship"]["positionNED"]
    pos_vector = [Vector(pos[1], pos[0]) for pos in pos_local]

    for i, s in enumerate(pos_vector):
        # Check for keep in fence violations
        if not definitely_inside(keep_in_fence, s, 0.01):
            t = simdata["ownship"]["t"][i]
            msg = "Keep In Geofence Violation at time = %fs" % t
            return False, msg, condition_name

        # Check for keep out fence violations
        for fence in keep_out_fences:
            if definitely_inside(fence, s, 0.01):
                t = simdata["ownship"]["t"][i]
                msg = "Keep Out Geofence Violation at time = %fs" % t
                return False, msg, condition_name

    return True, "No Geofence Violation", condition_name


def verify_traffic(simdata):
    '''Check simulation data for traffic well clear violations'''
    condition_name = "Traffic Avoidance"
    DMOD = simdata["parameters"]["DET_1_WCV_DTHR"]*0.3048  # Convert ft to m
    ZTHR = simdata["parameters"]["DET_1_WCV_ZTHR"]*0.3048  # Convert ft to m
    h_allow = validation_params["h_allow"]
    v_allow = validation_params["v_allow"]

    waypoints = simdata["ownship"]["localPlans"][0]
    ownship_time = simdata["ownship"]["t"]
    ownship_position = simdata["ownship"]["positionNED"]

    # Interpolate traffic positions
    traffic_position = {}
    for traffic_id, traffic in simdata["traffic"].items():
        t_pos_default = (traffic["positionNED"][0], traffic["positionNED"][-1])
        t_pos = interp1d(traffic["t"], np.array(traffic["positionNED"]),
                         axis=0, bounds_error=False, fill_value=t_pos_default)
        traffic_position[traffic_id] = t_pos

    for t, o_pos in zip(ownship_time, ownship_position):
        o_alt = o_pos[2]
        for traffic_pos in traffic_position.values():
            t_pos = traffic_pos(t)
            t_alt = t_pos[2]

            dist = np.sqrt((o_pos[0] - t_pos[0])**2 + (o_pos[1] - t_pos[1])**2)
            horiz_violation = (dist < DMOD*h_allow)
            vert_violation = (abs(o_alt - t_alt) < ZTHR*v_allow)
            if horiz_violation and vert_violation:
                msg = "Well Clear Violation at t = %fs" % t
                return False, msg, condition_name

    return True, "No Well Clear Violation", condition_name


def plot_scenario(simdata, output_dir="", save=False):
    '''Plot the result of the scenario'''
    from matplotlib import pyplot as plt
    scenario_name = os.path.basename(output_dir)
    fig1 = plt.figure()

    # Plot waypoints
    for plan in simdata["ownship"]["localPlans"]:
        waypoints = np.array(plan)
        plt.plot(waypoints[:, 2], waypoints[:, 1], 's--', color='gray')

    # Plot fences
    for i, fence in enumerate(simdata["ownship"]["localFences"]):
        vertices = np.array(fence + [fence[0]])
        if i == 0:
            color = "orange"
            label = "Keep In Geofence"
        else:
            color = "red"
            label = "Keep Out Geofence"
        plt.plot(vertices[:, 1], vertices[:, 0], '--', color=color, label=label)

    # Plot ownship path
    ownpos_local = np.array(simdata["ownship"]["positionNED"])
    plt.plot(ownpos_local[:, 1], ownpos_local[:, 0], label="Ownship Path")

    # Plot traffic path
    for traf_id, traf in simdata["traffic"].items():
        trafpos_local = np.array(traf["positionNED"])
        plt.plot(trafpos_local[:, 1], trafpos_local[:, 0], label=str(traf_id)+" Path")

    # Set up figure
    plt.title(scenario_name + " - Ground track")
    plt.xlabel("X (m)")
    plt.ylabel("Y (m)")
    plt.legend()
    plt.axis('equal')

    if save:
        output_file = os.path.join(output_dir, "simplot.png")
        plt.savefig(output_file)
        plt.close(fig1)

    # Plot ownship and traffic altitudes   
    fig2 = plt.figure()
    ownship_alt = np.array(simdata['ownship']['position'])[:, 2]
    t = np.array(simdata['ownship']['t'])
    plt.plot(t, ownship_alt, label='ownship')
    for traf_id, traf in simdata["traffic"].items():
        trafpos_alt = np.array(traf["position"])[:, 2]
        plt.plot(traf['t'], trafpos_alt, label=str(traf_id) + " Path")

    plt.title(scenario_name + " - Altitude data")
    plt.xlabel("t (s)")
    plt.ylabel("Alt (m)")
    plt.legend()

    if save:
        output_file = os.path.join(output_dir, "alt.png")
        plt.savefig(output_file)
        plt.close(fig2)

    # Plot onwship speed
    fig3 = plt.figure()
    ownship_vel = simdata['ownship']['velocityNED']
    speed = [np.sqrt(v[0]**2 + v[1]**2 + v[2]**2) for v in ownship_vel]
    plt.plot(t, speed, label='ownship')
    plt.title(scenario_name + " - Speed data")
    plt.xlabel("t (s)")
    plt.ylabel("speed (m/s)")
    plt.legend()

    if save:
        output_file = os.path.join(output_dir, "speed.png")
        plt.savefig(output_file)
        plt.close(fig3)

    if not save:
        plt.show()


def run_validation(flight_dir, test=False, plot=False, save=False):
    # Gather all simulation data files
    log_files = find_logs(flight_dir, ".*\.json$")

    for log in log_files:
        # Read in the simulation data
        output_dir = os.path.dirname(log)
        scenario_name = os.path.basename(os.path.normpath(output_dir))
        with open(log, 'r') as fp:
            simdata = json.load(fp)

        # Check the test conditions
        validate_sim_data(simdata, scenario_name, save, test)

        # Generate plots
        if plot:
            plot_scenario(simdata, output_dir, save)


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Validate Icarous simulation data")
    parser.add_argument("sim_output", help="sim data (.json, or directory containing .json)")
    parser.add_argument("--plot", action="store_true", help="plot the scenario")
    parser.add_argument("--save", action="store_true", help="save the results")
    parser.add_argument("--test", action="store_true", help="assert test conditions")
    parser.add_argument("--param", nargs=2, action="append", default=[],
                        metavar=("KEY", "VALUE"), help="set validation parameter")
    args = parser.parse_args()

    validation_params = update_params(validation_params, args.param)
    run_validation(args.sim_output, args.test, args.plot, args.save)

