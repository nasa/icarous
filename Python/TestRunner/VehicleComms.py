import numpy as np
from matplotlib import pyplot as plt


roles = {"NEUTRAL": 0,
         "FOLLOWER": 1,
         "CANDIDATE": 2,
         "LEADER": 3}

class RaftData:
    def __init__(self, v_id):
        self.id = v_id
        self.t = []
        self.log_entries = []

        self.role_changes = []
        self.current_role = None

    def log(self, t, role, mtype, mf_id, hb_id=None, leader_id=None):
        self.t.append(t)
        self.log_entries.append(LogEntry(t, role, mtype, mf_id, hb_id, leader_id))

        if self.current_role is not None:
            self.role_changes[-1][2] = t
        else:
            self.role_changes.append([role, t, None])
        if role != self.current_role:
            self.role_changes[-1][2] = t
            self.role_changes.append([role, t, None])
        self.current_role = role

    def get_role(self, t):
        return next((rc[0] for rc in self.role_changes if rc[1] < t < rc[2]),
                    roles["NEUTRAL"])


class LogEntry:
    def __init__(self, t, role, mtype, mf_id, hb_id=None, leader_id=None):
        self.t = t
        self.role = role
        self.mtype = mtype
        self.mf_id = mf_id
        self.hb_id = hb_id
        self.leader_id = leader_id


def ReadLogData(filename, vehicle_id=0, mf=1):
    with open(filename, 'r') as fp:
        data_string = fp.readlines()[3:]

    Vehicle = RaftData(v_id=vehicle_id)
    for line in data_string:
        # Skip lines containing other types of data
        if "MFID" not in line:
            continue
        line = line.rstrip('\n')
        entries = line.split('|')

        t = float(entries[0])
        mf_id = int(entries[1].split()[1])
        role = roles[entries[2].strip()]
        mtype = entries[3].strip().split()[0]

        # Skip lines that don't match the selected merge point
        if mf !=0 and mf_id not in [-1, mf]:
            continue

        # Record heartbeats sent or received
        if mtype == "SEND_HBEAT":
            leader_id = vehicle_id
            hb_id = int(entries[3].split("ID:")[1].split()[0])
        elif mtype == "RECV_HBEAT":
            leader_id = int(entries[3].split("NODE:")[1].strip())
            hb_id = int(entries[3].split("ID:")[1].split()[0])
        else:
            leader_id = None
            hb_id = None

        Vehicle.log(t, role, mtype, mf_id, hb_id, leader_id)

    return Vehicle


def analyze_latency(sending_vehicle, receiving_vehicle, verbose=True, plot=True):
    sentHB = [e for e in sending_vehicle.log_entries if e.mtype == "SEND_HBEAT"]
    recvHB = [e for e in receiving_vehicle.log_entries if e.mtype == "RECV_HBEAT"
              and e.leader_id == sending_vehicle.id]

    sent_times = []
    received_times = []
    delay = []
    missed_times = []

    for HB in sentHB:
        t_sent = HB.t
        msg_id = HB.hb_id
        # Ignore message unless sender was leader and receiver was follower
        if not (sending_vehicle.get_role(t_sent) == roles["LEADER"] and
          receiving_vehicle.get_role(t_sent) == roles["FOLLOWER"]):
            continue
        sent_times.append(t_sent)
        t_recv = next((hb.t for hb in recvHB if hb.hb_id == msg_id
                      and hb.leader_id == sending_vehicle.id), None)
        if t_recv is not None:
            delay.append(t_recv - t_sent)
            received_times.append(t_recv)
        else:
            missed_times.append(t_sent)
    n_recv = len(received_times)
    n_sent = len(sent_times)

    # if 0 messages were sent, no need to perform analysis
    if n_sent == 0:
        return

    if verbose:
        print("\nRaft communication, vechicle%d to vehicle%d:" %
                  (sending_vehicle.id, receiving_vehicle.id))
        print("vehicle%d sent %d" % (sending_vehicle.id, n_sent))
        print("vehicle%d received %d" % (receiving_vehicle.id, n_recv))
        print("vehicle%d missed %d" % (receiving_vehicle.id, len(missed_times)))
        print("percentage received: %.2f%%" % (n_recv/n_sent*100))
        print("average latency: %.2fs" % np.mean(delay))
        print("longest delay: %.2fs" % max(delay))
        print("mean time between heartbeats: %.2fs" % np.mean(np.diff(received_times)))

    if plot:
        plt.figure()
        plt.title("Raft communication, vechicle%d to vehicle%d" %
                  (sending_vehicle.id, receiving_vehicle.id))
        for r, d in zip(received_times, delay):
            plt.plot([r, r-d], [0, -1e-3], '-', color='gray')
        plt.plot(received_times, delay, label="Received HB latency")
        plt.plot(sent_times, [-1e-3]*len(sent_times), 'b*', label="Sent HB")
        plt.plot(received_times, [0]*len(received_times), 'g*', label="Received HB")
        plt.plot(missed_times, [-1e-3]*len(missed_times), 'r*', label="Missed HB")
        plt.ylabel("Delay: time received - time sent (s)")
        plt.xlabel("Time (s)")
        plt.legend()
        plt.grid()


def plot_roles(vehicles):
    plt.figure()
    plt.title("Raft Node Roles")
    colors = ['y', 'r', 'b', 'g']
    labels = ["Neutral", "Follower", "Candidate", "Leader"]
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


if __name__ == "__main__":
    import argparse
    import itertools
    import os

    parser = argparse.ArgumentParser(description="Validate Icarous simulation data")
    parser.add_argument("data_location", help="directory where log files are")
    parser.add_argument("--merge_id", default=1, type=int, help="merge point id to analyze")
    parser.add_argument("--num_vehicles", default=10, type=int, help="number of vehicles")
    parser.add_argument("--plot", action="store_true", help="plot the scenario")
    parser.add_argument("--save", action="store_true", help="save the results")
    args = parser.parse_args()

    # Read raft log data
    vehicles = {}
    for i in range(args.num_vehicles):
        filename = "raftLog_" + str(i) + ".txt"
        filename = os.path.join(args.data_location, filename)
        if not os.path.isfile(filename):
            break
        vehicles[i] = ReadLogData(filename, vehicle_id=i, mf=args.merge_id)

    # Analyze communication between each pair of vehicles
    for v1, v2 in itertools.combinations(vehicles.values(), 2):
        analyze_latency(v1, v2)
        analyze_latency(v2, v1)

    plot_roles(vehicles.values())

    plt.show()
