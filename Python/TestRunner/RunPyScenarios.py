import sys
import time
import os
from pymavlink import mavwp
import importlib

sys.path.append("../Batch")
from Icarous import *
import BatchGSModule as GS


sim_home = os.getcwd()
icarous_home = os.path.abspath(os.path.join(sim_home, "../.."))
icarous_exe = os.path.join(icarous_home, "exe", "cpu1")


def GetPolygons(origin, fenceList):
    """
    Constructs a list of Polygon (for use with PolyCARP) given a list of fences
    @param fenceList list of fence dictionaries
    @return list of polygons (a polygon here is a list of Vectors)
    """
    Polygons = []
    for fence in fenceList:
        vertices = fence["Vertices"]
        vertices_ned = [list(reversed(GS.LLA2NED(origin, position)))
                        for position in vertices]
        polygon = [(vertex[0], vertex[1]) for vertex in vertices_ned]
        Polygons.append(polygon)
    return Polygons


def RunPyIcarous(scenario):
    """ Set up a python Icarous to run a given scenario """

    # Read flight plan
    wploader = mavwp.MAVWPLoader()
    wploader.load(os.path.join(icarous_home, scenario["waypoint_file"]))
    WP = [(wp.x, wp.y, wp.z, int(wp.seq != 0))
          for wp in wploader.wpoints if wp.command == 16]
    WP_seq = [(wp.x, wp.y, wp.z, wp.seq)
              for wp in wploader.wpoints if wp.command == 16]
    origin = [WP[0][0], WP[0][1]]
    HomePos = WP[0][:3]

    # Start Icarous
    ic = Icarous(HomePos)
    #ic.setpos_uncertainty(0.01, 0.01, 0, 0, 0, 0)

    # Load flight plan
    ic.InputFlightplan(WP, 0)

    # Load geofence
    if scenario.get("geofence_file"):
        geofence_path = os.path.join(icarous_home, scenario["geofence_file"])
        ic.InputGeofence(geofence_path)
        GF = GetPolygons(origin, Getfence(geofence_path))
    else:
        GF = []

    # Load parameters
    params = LoadIcarousParams(os.path.join(icarous_home, scenario["parameter_file"]))
    if scenario.get("param_adjustments"):
        params.update(scenario["param_adjustments"])
    ic.SetParameters(params)

    # Load traffic
    tfList = []
    for tf in scenario["traffic"]:
        StartTraffic(HomePos, tf[0], tf[1], tf[2], tf[3], tf[4], tf[5], tfList)
    #for tf in tfList:
    #    tf.setpos_uncertainty(0.01, 0.01, 0, 0, 0, 0)

    # Run the scenario
    while not ic.CheckMissionComplete():
        status = ic.Run()
        if not status:
            continue

        RunTraffic(tfList)
        for i, tf in enumerate(tfList):
            ic.InputTraffic(i, tf.pos_gps, tf.vel)

    # Save simulation data
    simdata = {"geofences": GF,
               "waypoints": WP_seq,
               "scenario": scenario,
               "params": params,
               "ownship": ic.ownshipLog,
               "traffic": ic.trafficLog}

    return simdata


if __name__ == "__main__":
    import argparse
    import datetime
    import yaml
    from matplotlib import pyplot as plt
    parser = argparse.ArgumentParser(description="Run Icarous test scenarios")
    parser.add_argument("scenario", help="yaml file containing scenario(s) to run ")
    parser.add_argument("--validate", action="store_true",
                        help="check simulation results for test conditions")
    parser.add_argument("--test", action="store_true",
                        help="assert test conditions")
    parser.add_argument("--plot", action="store_true",
                        help="plot the simulation results")
    parser.add_argument("--save", action="store_true",
                        help="save the simulation results")
    parser.add_argument("--num", default="",
                        help="If multiple scenarios in file, just run this one")
    parser.add_argument("output_dir", nargs='?', default="simoutput",
                        help="directory to save output (default: 'simoutput')")
    parser.add_argument("--h_allow", type=float, default=0.85,
                        help="use h_allow*DTHR to check WC violation")
    parser.add_argument("--v_allow", type=float, default=0.85,
                        help="use v_allow*ZTHR to check WC violation")
    parser.add_argument("--wp_radius", type=float, default=5,
                        help="dist (m) to consider a waypoint reached")
    args = parser.parse_args()


    import ValidateSim as VS

    # Load scenarios
    if os.path.isfile(args.scenario):
        f = open(args.scenario, 'r')
        scenario_list = yaml.load(f, Loader=yaml.FullLoader)
        f.close()
    else:
        scenario_list = [json.loads(args.scenario)]

    if args.num:
        scenario_list = [scenario_list[int(args.num)]]

    results = []
    for i, scenario in enumerate(scenario_list):
        # Set up output directory
        name = scenario["name"].replace(' ', '-')
        timestamp = datetime.datetime.now().strftime("%Y-%m-%dT%H_%M_%S")
        output_dir = os.path.join(sim_home, args.output_dir, timestamp+"_"+name)

        # Run the simulation
        print("****"+scenario["name"]+"****")
        simdata = RunPyIcarous(scenario)

        validation_params = {"h_allow": args.h_allow,
                             "v_allow": args.v_allow,
                             "wp_radius": args.wp_radius}

        VF = VS.ValidateFlight(simdata, params=validation_params,
                                output_dir=output_dir)

        # Verify the sim output
        if args.validate:
            result = VF.validate_sim_data(test=args.test)
            results.append(result)
           
        if args.plot:
            VF.plot_scenario(save=args.save)


    # Print summary of results
    if args.validate:
        print("\nTest Scenario Results Summary:")
        print("------------------------------")
        for i in range(len(scenario_list)):
            VF.print_results(results[i], scenario_list[i]["name"])
