import sys
import os
import numpy as np
import random

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
from communicationmodels.channelmodels import Message


class Vehicle:
    def __init__(self, callsign, pos, vel, transmitter=None, receiver=None):
        self.callsign = callsign
        self.transmitter = transmitter
        self.receiver = receiver
        self.pos = pos
        self.vel = vel
        self.current_time = 0
        self.log_data = {"time": [self.current_time],
                         "pos": [pos],
                         "vel": [vel],
                         "traffic": {}}

    def step(self, dT):
        self.current_time += dT
        self.pos[0] += self.vel[0]*dT
        self.pos[1] += self.vel[1]*dT
        self.pos[2] += self.vel[2]*dT
        self.record_ownship()

    def transmit(self):
        if self.transmitter is None:
            return
        data = {"pos": self.pos[:], "vel": self.vel[:]}
        msg = self.transmitter.transmit(self.current_time,
                                        self.callsign,
                                        self.pos[:],
                                        data)
        return msg

    def receive(self):
        if self.receiver is None:
            return
        msgs = self.receiver.receive(self.current_time, self.pos[:])
        for m in msgs:
            if m.sender_id == self.callsign:
                continue
            self.input_traffic(m.sender_id, m.data["pos"], m.data["vel"])

    def input_traffic(self, traffic_id, t_pos, t_vel):
        if traffic_id not in self.log_data["traffic"]:
            self.log_data["traffic"][traffic_id] = {"time": [],
                                                    "pos": [],
                                                    "vel": []}
        self.log_data["traffic"][traffic_id]["time"].append(self.current_time)
        self.log_data["traffic"][traffic_id]["pos"].append(t_pos)
        self.log_data["traffic"][traffic_id]["vel"].append(t_vel)

    def record_ownship(self):
        self.log_data["time"].append(self.current_time)
        self.log_data["pos"].append(self.pos[:])
        self.log_data["vel"].append(self.vel[:])

    def write_log(self):
        import json
        logname = "%s.json" % self.callsign
        with open(logname, 'w') as f:
            json.dump(self.log_data, f)


def run_simulation(vehicles, channel, time_limit=5000, traf_range=20000):
    duration = 0
    dT = 0.05
    while duration < time_limit:
        print('%.2f' % duration, end='\r')
        for v in vehicles:
            v.step(dT)
        duration += dT

        # Randomly set v2 position
        vehicles[1].pos = [2*traf_range*random.random()-traf_range,
                           2*traf_range*random.random()-traf_range,
                           0]

        # Exchange sensor data
        for v in vehicles:
            v.transmit()
        for v in vehicles:
            v.receive()
        comm_channel.flush()

    for v in vehicles:
        v.write_log()


def plot_reception_results(receiving_vehicle, sending_vehicle, traf_range):
    from matplotlib import pyplot as plt
    callsign = receiving_vehicle.callsign
    sensor_type = receiving_vehicle.receiver.sensorType
    log = receiving_vehicle.log_data

    # Plot received messages
    for traf_id, traf_log in log["traffic"].items():
        rx_tpos = np.array(traf_log["pos"])
        label = "%s as sensed by %s %s" % (traf_id, callsign, sensor_type)
        plt.plot(rx_tpos[:, 0], rx_tpos[:, 1], '.', label=label)
    pos = np.array(receiving_vehicle.log_data["pos"])
    plt.plot(pos[:, 0], pos[:, 1], '*-', label=callsign)
    plt.legend(loc=3)
    plt.savefig("sensortest.png")
    plt.show()

    # Plot heatmap
    plt.figure()
    tx_tpos = np.array(sending_vehicle.log_data["pos"])
    bins = np.linspace(-traf_range, traf_range, 100)
    hist_rx, _, _ = np.histogram2d(rx_tpos[:, 0], rx_tpos[:, 1], bins=bins)
    hist_tx, x, y = np.histogram2d(tx_tpos[:, 0], tx_tpos[:, 1], bins=bins)
    extent = [x[0], x[-1], y[0], y[-1]]
    hist = hist_rx/hist_tx*100
    np.seterr(divide='ignore', invalid='ignore')
    hist = np.divide(hist_rx, hist_tx,
                     out=np.zeros_like(hist_rx), where=hist_tx != 0)*100
    plt.imshow(hist, extent=extent)
    cb = plt.colorbar()
    cb.set_label("Percent Messages Received")
    plt.ylabel('y')
    plt.xlabel('x')
    rm = comm_channel.reception_model.model_name
    plt.title("Random Simulation - %s" % rm)
    plt.savefig("sensortest_map.png")
    plt.show()


if __name__ == "__main__":
    import channelmodels as cm
    import sensormodels

    # Set up a comm channel with a propagation model and a reception model
    propagation_model = cm.propagationmodels.FreeSpacePropagation(L=1)
    #propagation_model = cm.propagationmodels.TwoRayGroundPropagation()
    #propagation_model = cm.propagationmodels.NoLossPropagation()
    #reception_model = cm.receptionmodels.ConstantReception(rx_rate=0.8)
    reception_model = cm.receptionmodels.RayleighReception()
    #reception_model = cm.receptionmodels.NakagamiReception(m=3)
    #reception_model = cm.receptionmodels.DeterministicReception()
    #reception_model = cm.receptionmodels.PerfectReception()
    comm_channel = cm.ChannelModel(propagation_model=propagation_model,
                                   reception_model=reception_model)

    # Create transmitters/receivers on the comm channel
    adsb_in = sensormodels.ADSBReceiver(comm_channel)
    adsb_out = sensormodels.ADSBTransmitter(comm_channel, update_interval=0)

    # Create vehicle simulations that have transmitters/receivers
    v1 = Vehicle("SPEEDBIRD", [0, 0, 0], [0, 0, 0], receiver=adsb_in)
    v2 = Vehicle("INTRUDER", [10, 10, 0], [0, 0, 0], transmitter=adsb_out)
    vehicles = [v1, v2]

    # Run a test simulation
    traf_range = 30000
    run_simulation(vehicles, comm_channel, traf_range=traf_range)
    plot_reception_results(v1, v2, traf_range=traf_range)
