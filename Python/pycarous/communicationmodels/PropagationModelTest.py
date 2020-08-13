import sys
import os
from matplotlib import pyplot as plt

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
from communicationmodels import propagationmodels


rx_threshold = 1e-10    # W
tx_power = 40           # W
freq = 978e6            # Hz
h_t = 100               # height of transmitting antenna (m)
h_r = 100               # height of receiving antenna (m)

models = [
          propagationmodels.NoLossPropagation(),
          propagationmodels.FreeSpacePropagation(),
          propagationmodels.TwoRayGroundPropagation(),
         ]

for pm in models:
    CR = pm.inverse(rx_threshold, tx_power, freq, h_t, h_r)
    print(pm.model_name)
    print("\tcommunication range: %.2f m" % CR)

    plt.figure()
    pm.plot(rx_threshold, tx_power, freq, h_t, h_r, save=True, show=True)
