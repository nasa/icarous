import os
import sys
import json
import numpy as np
from polygon_contain import Vector, definitely_inside

sys.path.append("../Batch")
import BatchGSModule as GS


DEFAULT_VALIDATION_PARAMS = {"h_allow": 0.85,
                             "v_allow": 1,
                             "wp_radius": 5}


def GetPolygons(origin, fenceList):
    """
    Constructs a list of Polygon (for use with PolyCARP) given a list of fences
    @param fenceList list of fence dictionaries
    @return list of polygons (a polygon here is a list of Vectors)
    """
    Polygons = []
    for fence in fenceList:
        vertices = fence["Vertices"]
        vertices_ned = [list(reversed(GS.LLA2NED(origin[0:2], position)))
                        for position in vertices]
        polygon = [[vertex[0], vertex[1]] for vertex in vertices_ned]
        Polygons.append(polygon)
    return Polygons


class ValidateFlight:
    def __init__(self, simdata, params={}, output_dir=""):
        self.simdata = simdata
        self.params = {**DEFAULT_VALIDATION_PARAMS, **params}
        self.output_dir = output_dir
        self.origin = self.simdata["ownship"]["position"][0]

    def validate_sim_data(self, test=False):
        '''Check simulation data for test conditions'''
        # Define conditions
        conditions = []
        conditions.append(self.verify_waypoint_progress)
        conditions.append(self.verify_geofence)
        conditions.append(self.verify_traffic)

        # Check conditions
        results = [c() for c in conditions]

        # Print results
        print_results(results, self.simdata["scenario"]["name"])

        if test:
            # Assert conditions
            for r in results:
                assert r[0], r[1]

        return results


    def verify_waypoint_progress(self):
        '''Check simulation data for progress towards goal'''
        condition_name = "Waypoint Progress"
        waypoints = self.simdata["waypoints"]
        wp_radius = self.params["wp_radius"]
        lla2ned = lambda x: GS.LLA2NED(self.origin, x)

        reached = []

        for i, pos in enumerate(self.simdata["ownship"]["position"]):
            pos = lla2ned(pos)
            # Check for reached waypoints
            for wp in waypoints:
                wp_pos = lla2ned(wp[0:3])
                wp_seq = wp[3]
                # If within 5m, add to reached
                dist = np.sqrt((pos[0] - wp_pos[0])**2 + (pos[1] - wp_pos[1])**2)
                if dist < wp_radius and wp_seq not in reached:
                    reached.append(wp[3])

        default = [wp[3] for wp in waypoints]
        reached_expected = self.simdata["scenario"].get("planned_wps", default)
        #print("reached  %s" % reached)
        #print("expected %s" % reached_expected)
        if set(reached) == set(reached_expected):
            return True, "Reached all planned waypoints", condition_name
        else:
            return False, "Did not reach all planned waypoints", condition_name


    def verify_geofence(self):
        '''Check simulation data for geofence violations'''
        condition_name = "Geofencing"
        waypoints = self.simdata["waypoints"]
        geofences = GetPolygons(self.origin, self.simdata["geofences"])
        lla2ned = lambda x: GS.LLA2NED(self.origin, x)

        if len(geofences) == 0:
            return True, "No Geofences", condition_name

        for i, pos in enumerate(self.simdata["ownship"]["position"]):
            pos = lla2ned(pos)
            # Check for keep in fence violations
            keep_in_fence = [Vector(*vertex) for vertex in geofences[0]]
            s = Vector(pos[1], pos[0])
            if not definitely_inside(keep_in_fence, s, 0.01):
                t = self.simdata["ownship"]["t"][i]
                msg = "Keep In Geofence Violation at time = %fs" % t
                return False, msg, condition_name

            # Check for keep out fence violations
            for fence in geofences[1:]:
                fencePoly = [Vector(*vertex) for vertex in fence]
                if definitely_inside(fencePoly, s, 0.01):
                    t = self.simdata["ownship"]["t"][i]
                    msg = "Keep Out Geofence Violation at time = %fs" % t
                    return False, msg, condition_name
        return True, "No Geofence Violation", condition_name


    def verify_traffic(self):
        '''Check simulation data for traffic well clear violations'''
        condition_name = "Traffic Avoidance"
        DMOD = self.simdata["parameters"]["DET_1_WCV_DTHR"]*0.3048  # Convert ft to m
        ZTHR = self.simdata["parameters"]["DET_1_WCV_ZTHR"]*0.3048  # Convert ft to m
        h_allow = self.params["h_allow"]
        v_allow = self.params["v_allow"]
        waypoints = self.simdata["waypoints"]
        lla2ned = lambda x: GS.LLA2NED(self.origin, x)

        for i, o_pos in enumerate(self.simdata["ownship"]["position"]):
            o_pos = lla2ned(o_pos)
            o_alt = o_pos[2]
            for traffic_id in self.simdata["traffic"].keys():
                traffic = self.simdata["traffic"][traffic_id]
                t_pos = lla2ned(traffic["position"][i])
                t_alt = t_pos[2]

                dist = np.sqrt((o_pos[0] - t_pos[0])**2 + (o_pos[1] - t_pos[1])**2)
                horiz_violation = (dist < DMOD*h_allow)
                vert_violation = (abs(o_alt - t_alt) < ZTHR*v_allow)
                if horiz_violation and vert_violation:
                    t = self.simdata["ownship"]["t"][i]
                    msg = "Well Clear Violation at t = %fs" % t
                    return False, msg, condition_name

        return True, "No Well Clear Violation", condition_name


    def plot_scenario(self, save=False):
        '''Plot the result of the scenario'''
        from matplotlib import pyplot as plt
        fig = plt.figure()
        WP = self.simdata["waypoints"]
        geofences = GetPolygons(self.origin, self.simdata["geofences"])
        lla2ned = lambda x: GS.LLA2NED(self.origin, x)

        # Plot waypoints
        waypoints = [wp[0:3] for wp in WP]
        waypoints_local = np.array(list(map(lla2ned, waypoints)))
        plt.plot(waypoints_local[:, 1], waypoints_local[:, 0], 'k*:', label="Flight Plan")

        # Plot ownship path
        ownpos_local = np.array(list(map(lla2ned, self.simdata["ownship"]["position"])))
        plt.plot(ownpos_local[:, 1], ownpos_local[:, 0], label="Ownship Path")

        # Plot traffic path
        for traf_id, traf in self.simdata["traffic"].items():
            trafpos_local = np.array(list(map(lla2ned, traf["position"])))
            plt.plot(trafpos_local[:, 1], trafpos_local[:, 0], label=str(traf_id)+" Path")

        # Plot geofences
        for i, vertices in enumerate(geofences):
            vertices.append(vertices[0])
            vertices = np.array(vertices)
            if i == 0:
                plt.plot(vertices[:, 0], vertices[:, 1], 'orange', label="Keep In Geofence")
            else:
                plt.plot(vertices[:, 0], vertices[:, 1], 'r', label="Keep Out Geofence"+str(i))

        # Set up figure
        plt.title(self.simdata["scenario"]["name"]+" Scenario - Simulation Results")
        plt.xlabel("X (m)")
        plt.ylabel("Y (m)")
        plt.legend()
        plt.axis('equal')

        if save:
            if not os.path.isdir(self.output_dir):
                os.makedirs(self.output_dir)
            output_file = os.path.join(self.output_dir, "simplot.png")
            plt.savefig(output_file)
            plt.close(fig)
        else:
            plt.show()


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
    parser.add_argument("sim_output", help="sim data (.json, or directory containing .json)")
    parser.add_argument("--plot", action="store_true", help="plot the scenario")
    parser.add_argument("--save", action="store_true", help="save the results")
    parser.add_argument("--test", action="store_true", help="assert test conditions")
    parser.add_argument("--h_allow", type=float, default=0.85,
                        help="use h_allow*DTHR to check WC violation")
    parser.add_argument("--v_allow", type=float, default=1,
                        help="use v_allow*ZTHR to check WC violation")
    parser.add_argument("--wp_radius", type=float, default=5,
                        help="dist (m) to consider a waypoint reached")
    args = parser.parse_args()

    # Gather all simulation data files
    if os.path.isfile(args.sim_output) and args.sim_output.endswith(".json"):
        data_files = [args.sim_output]
    elif os.path.isdir(args.sim_output):
        data_files = []
        for root, dirs, files in os.walk(args.sim_output):
            data_files += [os.path.join(root, f) for f in files if f.endswith(".json")]
        print("Found %d simulation data files in %s" % (len(data_files), args.sim_output))

    validation_params = {"h_allow": args.h_allow,
                         "v_allow": args.v_allow,
                         "wp_radius": args.wp_radius}

    for data_file in data_files:
        # Read in the simulation data
        with open(data_file, 'r') as fp:
            simdata = json.load(fp)

        # Validate the data
        output_dir = os.path.split(data_file)[0]
        VF = ValidateFlight(simdata, validation_params, output_dir=output_dir)
        VF.validate_sim_data()
        if args.plot:
            VF.plot_scenario(save=args.save)
