from matplotlib import pyplot as plt

import receptionmodels


rx_threshold = 1e-10    # W
tx_power = 40           # W
freq = 978e6            # Hz
h_t = 100               # height of transmitting antenna (m)
h_r = 100               # height of receiving antenna (m)

models = [
          receptionmodels.PerfectReception(),
          receptionmodels.ConstantReception(rx_rate=0.9),
          receptionmodels.DeterministicReception(),
          receptionmodels.RayleighReception(),
          receptionmodels.NakagamiReception(m=3),
         ]

# Plot comparison
plt.figure()
for rm in models:
    pm = rm.propagation_model
    CR = pm.inverse(rx_threshold, tx_power, freq, h_t, h_r)
    print(rm.model_name)
    print("\tdeterministic communication range: %.2f m" % CR)
    rm.plot(rx_threshold, tx_power, freq, h_t, h_r, save=False, show=False)
plt.savefig("receptionmodels.png")
plt.show()

# Plot reception maps
for rm in models:
    plt.figure()
    rm.heatmap(rx_threshold, tx_power, freq, h_t, h_r, save=True, show=True)
