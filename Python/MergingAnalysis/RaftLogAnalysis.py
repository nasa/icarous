import numpy as np
from matplotlib import pyplot as plt
import os


class RaftData:
    def __init__(self, v_id):
        self.id = v_id
        self.log_entries = []
        self.role_changes = []
        self.current_role = None

    def get_role(self, t):
        if t is None:
            return "NEUTRAL"
        return next((rc[0] for rc in self.role_changes if rc[1] < t < rc[2]), "NEUTRAL")


class LogEntry:
    def __init__(self, t, role, mtype, mf_id, hb_id=None, leader_id=None):
        self.t = t                  # Time the message was received
        self.role = role            # Raft node role (NEUTRAL, FOLLOWER, CANDIDATE, LEADER)
        self.mtype = mtype          # SEND_HBEAT, RECV_HBEAT, ALIVE, ...
        self.mf_id = mf_id          # Current merge fix id
        self.hb_id = hb_id          # Heartbeat id (for SEND_HBEAT or RECV_HBEAT)
        self.leader_id = leader_id  # The id of the leader node (for RECV_HBEAT)


def ReadRaftLog(filename, vehicle_id=0, mf=1):
    with open(filename, 'r') as fp:
        data_string = fp.readlines()[3:]

    vehicle = RaftData(v_id=vehicle_id)
    vehicle.output_dir = os.path.dirname(filename)
    for line in data_string:
        # Skip lines containing other types of data
        if "MFID" not in line:
            continue
        line = line.rstrip('\n')
        entries = line.split('|')

        t = float(entries[0])
        mf_id = int(entries[1].split()[1])
        role = entries[2].strip()
        mtype = entries[3].strip().split()[0]

        # Skip until time starts
        if t < 1e-3:
            continue
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

        # Record changes in node role
        if vehicle.current_role is not None:
            vehicle.role_changes[-1][2] = t
        if role != vehicle.current_role:
            vehicle.role_changes.append([role, t, t])
        vehicle.current_role = role

        # Record a raft log entry
        vehicle.log_entries.append(LogEntry(t, role, mtype, mf_id, hb_id, leader_id))

    return vehicle


def analyze_latency(sending_vehicle, receiving_vehicle, verbose=True, plot=False, save=False):
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
        # Ignore heartbeats except when sender was leader and receiver was follower
        if not (sending_vehicle.get_role(t_sent) == "LEADER" and
          receiving_vehicle.get_role(t_sent) == "FOLLOWER"):
            continue
        sent_times.append(t_sent)
        t_recv = next((hb.t for hb in recvHB if hb.hb_id == msg_id
                      and hb.leader_id == sending_vehicle.id), None)
        if t_recv is not None:
            received_times.append(t_recv)
            delay.append(t_recv - t_sent)
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
        if save:
            name = "raft_comms_v%d_to_v%d" % (sending_vehicle.id, receiving_vehicle.id)
            plt.savefig(os.path.join(receiving_vehicle.output_dir, name))


def plot_roles(vehicles, save=False):
    plt.figure()
    plt.title("Raft Node Roles")
    colors = ['y', 'r', 'b', 'g']
    labels = ["NEUTRAL", "FOLLOWER", "CANDIDATE", "LEADER"]
    for v in vehicles:
        for rc in v.role_changes:
            role, t0, tEnd = rc
            color = colors[labels.index(role)]
            plt.plot([t0, tEnd], [v.id, v.id], '-', c = color,
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
    if save:
        plt.savefig(os.path.join(v.output_dir, "raft_roles"))


if __name__ == "__main__":
    import argparse
    import itertools
    import os

    parser = argparse.ArgumentParser(description="Validate Icarous simulation data")
    parser.add_argument("data_location", help="directory where log files are")
    parser.add_argument("--merge_id", default=1, type=int, help="merge point id to analyze")
    parser.add_argument("--num_vehicles", default=10, type=int, help="number of vehicles")
    parser.add_argument("--verbose", action="store_true", help="print output information")
    parser.add_argument("--plot", action="store_true", help="plot the scenario")
    parser.add_argument("--save", action="store_true", help="save the results")
    args = parser.parse_args()

    # Read raft log data
    vehicles = []
    for i in range(args.num_vehicles):
        filename = "raftLog_" + str(i) + ".txt"
        filename = os.path.join(args.data_location, filename)
        if not os.path.isfile(filename):
            break
        data = ReadRaftLog(filename, vehicle_id=i, mf=args.merge_id)
        vehicles.append(data)

    # Analyze communication between each pair of vehicles
    for v1, v2 in itertools.combinations(vehicles, 2):
        analyze_latency(v1, v2, verbose=args.verbose, plot=args.plot, save=args.save)
        analyze_latency(v2, v1, verbose=args.verbose, plot=args.plot, save=args.save)

    # Generate plots
    if args.plot:
        plot_roles(vehicles, save=args.save)
        plt.show()
