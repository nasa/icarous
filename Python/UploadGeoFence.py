#!/usr/bin/env python


'''
Uploads a geofence to ICAROUS
'''

# call script with -h for help statement on inputs

from pymavlink import mavutil


SCRIPT_SYSID = 200

# some constants for clarity
FENCE_ENABLE = 1
FENCE_DISABLE = 0
FENCE_TYPE_KEEPIN = 0
FENCE_TYPE_KEEPOUT = 1


# Constants only used by main (if script is being run directly)
ICAROUS_SYSID = 100
ICAROUS_COMPID = 0
ALT_DEFAULT_MIN = 0.0  # 0 AGL
ALT_DEFAULT_MAX = 400.0  # 400 AGL


# Simple lat/long pair class
class GeoPoint:
    def __init__(self, lat, lon):
        self.lat = lat
        self.lon = lon
    def __repr__(self):
        return "<" + str(self.lat) + ", " + str(self.lon) + ">"


# made status function in case later output is redirected to log file instead of stdout
def status(m):
    print m

def getFenceFromWayPointFile( fin ):
    points = []
    minAlt = None;
    maxAlt = None;
    for line in fin:
        if line.startswith('#'):
            #ignore comment line
            continue
        vals = line.split('\t')
        if len(vals) < 12 or vals[0] == "0" or vals[3] != "16":
            # ignore the "Home" waypoint due to its hardcoded altitude, ignore non-waypoint mission items, ignore lines of unexpected size
            continue

        # point[2:] = 0.0

        lat = float( vals[8] )
        lon = float( vals[9] )

        alt = float( vals [10] )
        points.append( GeoPoint(lat=lat, lon=lon) )
        if minAlt is None or alt < minAlt:
            minAlt = vals[10]
        if maxAlt is None or alt > maxAlt:
            maxAlt = vals[10]
    return points, minAlt, maxAlt
# end getFenceFromWayPointFile

def getFenceFromPolygonFile( fin ):
    points = []
    for line in fin:
        if line.startswith('#'):
            #ignore comment line
            continue
        vals = line.split(' ')
        lat = float( vals[0] )
        lon = float( vals[1] )
        points.append(GeoPoint(lat=lat, lon=lon))
    return points
# end getFenceFromPolygonFile


# End getFenceFromFile

def uploadGeofence(master, point_list, ceiling, floor, fence_idx=0, fence_type=FENCE_TYPE_KEEPIN  ):
    status("Beginning upload of " + ("keepin" if fence_type is FENCE_TYPE_KEEPIN else "keepout") + " geofence: idx "
            + str(fence_idx) + ", floor " + str(floor) + ", ceiling " + str(ceiling) + ", " + str(point_list))


    attempts = 0
    # 1. GCS sends MAV_CMD_DO_FENCE_ENABLED (CMD #207)
    outMsg = master.mav.command_long_encode(master.target_system, master.target_component, mavutil.mavlink.MAV_CMD_DO_FENCE_ENABLE, 0,
                                            FENCE_ENABLE, fence_idx, fence_type, len(point_list), floor, ceiling, 0)

    while True:
        # Send the current outgoing message
        status("Attempt " + str(attempts) + ", Sending message: " + str(outMsg))
        master.mav.send(outMsg)

        # wait for reply
        status("Waiting for reply")
        inMsg = master.recv_match(blocking=True, type=["FENCE_FETCH_POINT", "COMMAND_ACK"], timeout=50)
        #inMsg = master.recv_msg();
        if inMsg is None:
            print("Did not receive a response, resending message")
            attempts += 1
            continue


        status("Received response: " + str(inMsg))
        if inMsg.get_type() == "FENCE_FETCH_POINT":
            # ==== 2. ICAROUS sends FENCE_FETCH_POINT ('type = FENCE_FETCH_POINT'), we need to wait for it (timeout is in seconds) ====
            if inMsg.target_system == SCRIPT_SYSID:
                # process
                idx = inMsg.idx
                attempts = 0 # reset attempt counter

                # ==== 3. GCS sends MAVLINK_MSG_ID_FENCE_POINT(type='FENCE_POINT') ====
                outMsg = master.mav.fence_point_encode(master.target_system, master.target_component, idx, len(point_list), point_list[idx].lat, point_list[idx].lon)
            else:
                status("Received fence point request targeted to another system: " + inMsg.target_system)
                continue
        elif inMsg.get_type() == "COMMAND_ACK":
            # ==== 5. ICAROUS sends COMMAND_ACK to signal we are done (type = 'COMMAND_ACK') ====
            if inMsg.result == 1:
                status("Geofence accepted")
                outMsg = master.mav.command_long_encode(master.target_system, master.target_component, mavutil.mavlink.MAV_CMD_DO_FENCE_ENABLE, 0,FENCE_ENABLE, fence_idx, fence_type, len(point_list), floor, ceiling, 0)

            elif inMsg.command == mavutil.mavlink.MAV_CMD_DO_FENCE_ENABLE:
                if inMsg.status == mavutil.mavlink.MAV_RESULT_ACCEPTED:
                    status("Geofence accepted")
                    return True
                else:
                    status("Geofence not accepted, got ACK status: " + str(inMsg.status))
                    return False
        else:
            # should not happen
            status("WARNING: Reached code branch thought unreachable")
        # ==== 4. repeat steps 2-3 as necessary ====
    # End of While Loop
# End uploadGeofence()




if __name__ == '__main__':
    from argparse import ArgumentParser

    parser = ArgumentParser(description=__doc__)

    parser.add_argument("file", help="File to load geofence from. Expects a Mission Planner/APMPlanner/QGroundControl waypoint file OR a Mission Planner polygon file")
    parser.add_argument("device", help="MAVLink device to upload to. Expects same notation as MAVProxy")
    parser.add_argument("--baudrate", type=int,
                        help="Connection baud rate", default=115200)
    parser.add_argument("--keepout", help="Geofence is a keep out area (otherwise it is considered a keep in)", action='store_true')
    parser.add_argument("--id", help="ID number of the geofence", type=int, default=0)

    args = parser.parse_args()

    status("Starting")
    master = None
    try:
        master = mavutil.mavlink_connection(args.device, input=False, baud=args.baudrate, source_system=SCRIPT_SYSID,
                                            dialect="ardupilotmega")
    except Exception as msg:
        status("Something went wrong opening " + args.device + ": " + msg.message)


    #set defaults
    fence_type = FENCE_TYPE_KEEPOUT if args.keepout else FENCE_TYPE_KEEPIN
    points = []
    minAlt = ALT_DEFAULT_MIN
    maxAlt = ALT_DEFAULT_MAX

    #load file
    fin = open(args.file)
    line = fin.readline()
    if  line.startswith('QGC WPL'):
        points, tempMin, tempMax = getFenceFromWayPointFile( fin )
        if tempMin is None or tempMax is None:
            #failed to get min/max alt, abort
            status("Failed to determine min/max altitude from file")
            fin.close()
            exit(1)
        minAlt = float(tempMin)
        maxAlt = float(tempMax)
    elif line.startswith('#saved by Mission Planner'):
        points = getFenceFromPolygonFile( fin )
    else:
        status("Unrecognized file format")
        fin.close()
        exit(1)
    fin.close()

    if len(points) < 3:
        status("Only got " + str(len(points)) + " points, not a valid fence")
        exit(1)


    #process
    if master is not None:
        print "Connected to " + master.address
        master.target_system = ICAROUS_SYSID
        master.target_component = ICAROUS_COMPID

#        # send a heartbeat for testing (activates listening program used for debug)
#        master.mav.heartbeat_send(mavutil.mavlink.MAV_TYPE_GCS, mavutil.mavlink.MAV_AUTOPILOT_INVALID,
#                                  mavutil.mavlink.MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, 0,
#                                  mavutil.mavlink.MAV_STATE_STANDBY, 1)

        # Actually get to work
        uploadGeofence(master, fence_idx=args.id, point_list=points, ceiling=maxAlt, floor=minAlt, fence_type=fence_type)

    status("Done")
