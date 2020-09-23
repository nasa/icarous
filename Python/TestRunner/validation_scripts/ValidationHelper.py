import argparse
import numpy as np
import os
import re


def update_params(params, updates):
    for update in updates:
        param_id, param_value = update
        if param_id not in params:
            print("** Warning, unrecognized validation parameter: %s" % p[0])
        params[param_id] = float(param_value)
    return params
        

def find_logs(flight_dir, pattern):
    log_files = []
    if os.path.isfile(flight_dir) and re.match(pattern, flight_dir):
        log_files = [flight_dir]

    elif os.path.isdir(flight_dir):
        for root, dirs, files in os.walk(flight_dir):
            for f in files:
                if re.match(pattern, f):
                    log_files.append(os.path.join(root, f))
        print("Found %d simulation data files in %s" % (len(log_files), flight_dir))

    else:
        print("File not found: %s" % flight_dir)

    return log_files


def print_results(results, scenario_name):
    """ Print results of a test scenario """
    if all([res for res, msg, name in results]):
        print("\033[32m\"%s\" scenario PASSED:\033[0m" % scenario_name)
    else:
        print("\033[31m\"%s\" scenario FAILED:\033[0m" % scenario_name)
    for result, msg, name in results:
        if result:
            print("\t\033[32m* %s - PASS:\033[0m %s" % (name, msg))
        else:
            print("\t\033[31mX %s - FAIL:\033[0m %s" % (name, msg))


def record_results(results, scenario_name, output_file="ValidationResults.csv"):
    """ Save validation results to a csv file """
    import pandas as pd
    if os.path.isfile(output_file):
        table = pd.read_csv(output_file, index_col=0)
    else:
        table = pd.DataFrame({})
    metrics = {r[2]:r[0] for r in results}
    index = scenario_name
    metrics = pd.DataFrame(metrics, index=[index])
    table = metrics.combine_first(table)
    table = table[metrics.keys()]
    table.to_csv(output_file)


def LLA2NED(origin, position):
    """
    Convert from geodetic coordinates to NED coordinates
    :param origin:  origin of NED frame in geodetic coordinates
    :param position: position to be converted to NED
    :return: returns position in NED
    """
    R    = 6371000  # radius of earth
    oLat = origin[0]*np.pi/180
    oLon = origin[1]*np.pi/180

    if(len(origin) > 2):
        oAlt = origin[2]
    else:
        oAlt = 0

    pLat = position[0]*np.pi/180
    pLon = position[1]*np.pi/180

    if(len (origin) > 2):
        pAlt = position[2]
    else:
        pAlt = 0

    # convert given positions from geodetic coordinate frame to ECEF
    oX   = (R+oAlt)*np.cos(oLat)*np.cos(oLon)
    oY   = (R+oAlt)*np.cos(oLat)*np.sin(oLon)
    oZ   = (R+oAlt)*np.sin(oLat)

    Pref = np.array([[oX], [oY], [oZ]])

    pX   = (R+pAlt)*np.cos(pLat)*np.cos(pLon)
    pY   = (R+pAlt)*np.cos(pLat)*np.sin(pLon)
    pZ   = (R+pAlt)*np.sin(pLat)

    P    = np.array([[pX], [pY], [pZ]])

    # Convert from ECEF to NED
    Rne = np.array([[-np.sin(oLat)*np.cos(oLon), -np.sin(oLat)*np.sin(oLon), np.cos(oLat) ],
                    [-np.sin(oLon),               np.cos(oLon),              0            ],
                    [-np.cos(oLat)*np.cos(oLon), -np.cos(oLat)*np.sin(oLon), -np.sin(oLat)]])

    Pn   = np.dot(Rne, (P - Pref))

    if(len (origin) > 2):
        return [Pn[0, 0], Pn[1, 0], Pn[2, 0]]
    else:
        return [Pn[0, 0], Pn[1, 0]]

