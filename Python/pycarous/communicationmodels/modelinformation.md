# Information on models used to simulate V2V communication

## General Sources:
* https://ieeexplore.ieee.org/document/8411465/references#references conducted
a survey of communication channel modeling for UAS
* https://utm.arc.nasa.gov/docs/2018-Xue-Aviation-June.pdf describes Fe3
framework for Monte Carlo UAS simulations, including simulation of V2V communications
* https://www.researchgate.net/publication/220926758_Enabling_efficient_and_accurate_large-scale_simulations_of_VANETs_for_vehicular_traffic_management
evaluated multiple reception models for largescale simulation of vehicular
networks. They fit the models to match increasing communications density
* http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.143.2467&rep=rep1&type=pdf
evaluates a few probabilistic reception models to improve performance of a
largescale simulation framework
* T. S. Rappaport. Wireless Communications: Principles & Practise.
Prentice Hall Communications Engineering and Emerging Technologies Series.
Prentice Hall PTR, Upper Saddle River, NJ 07458, 1996.

## Propagation Models:
These models predict the received power of a given signal at the receiver
location.
### No Loss
  * This is a completely unrealistic model implemented as a placeholder. The
received power at any location is the same as the transmitted power.
### Free Space Path Loss
  * Theoretical model for path loss of radio signals in open space,
widely used in telecommunications engineering
  * Source: https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=1697062
  * P_r = P_t * (G_t * G_r * w^2)/((4pi)^2 * d^2)
    * P_r is received power
    * P_t is transmitted power
    * w is wavelength of transmission
    * d is horizontal separation between transmitter and receiver
### Two Ray Ground
  * Similar to Free Space Path Loss, but accounts for both a direct LOS path
and a reflected path from the ground plane. This model has been proven to be
more accurate than FSPL at longer separation distances (T. S. Rappaport.,
Wireless communications, principles and practice. Prentice Hall, 1996.). A
crossover distance can be computed to decide when to switch between the two
models.
  * P_r = P_t * (G_t * G_r * h_t^2 * h_r^2)/(d^4)
    * P_r is received power
    * P_t is transmitted power
    * h_t is height of the transmitter
    * h_r is height of the receiver
    * d is horizontal separation between transmitter and receiver
  * d_c = (4 * PI * h_t * h_r) / w
    * d_c is crossover distance (to switch from FSPL)
    * w is transmission wavelength

## Reception Models:
These models give a probability that a transmitted message will be received by
a particular receiver. In general, the chance of reception decreases with
increasing range.
### Constant Reception
  * This is a completely unrealistic model implemented as a placeholder. Every
message has a fixed probability of being received, regardless of separation
between transmitter/receiver.
  * Parameters:
    * rx_rate: The probability of receiving a message (0 <= rx_rate <=1)
### Perfect Reception
  * This is a completely unrealistic model implemented as a placeholder. Every
message is always received, regardless of separation between
transmitter/receiver.
### Deterministic Reception
  * A propagation model defines an area around a transmitter where received
power is greater than a given receiver sensitivity. In a deterministic
reception model, any receiver within range receives 100% of messages, and any
receiver outside the range receives no messages.
  * The model ignores many factors such as multipath fading and interference.
These effects are complex and best accounted for with a probabilistic model.
### Rayleigh
  * Received power follows a Rayleigh distribution. The average power is
determined by one of the deterministic propagation models
  * Assumes many scattering objects (High impact from environment) not
realistic for direct line of sight communication
  * http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.143.2467&rep=rep1&type=pdf
### Nakagami
* Received power follows a Nakagami distribution, with fading parameter (m)
accounting for various levels of fading effects. The distribution simplifies to
Rayleigh for m=1 and fading effects decrease with increasing values of m.
* Nakagami distribution: Nakagami, M. ((1960) "The m-Distribution, a general
formula of intensity of rapid fading". In William C. Hoffman, editor,
Statistical Methods in Radio Wave Propagation: Proceedings of a Symposium held
June 1820, 1958, pp. 336. Pergamon Press., doi:10.1016/B978-0-08-009306-2.50005-4)
* Used in many studies of VANETs (vehicular adhoc networks) and proven to
reflect environmental conditions accurately in these scenarios
(https://dl.acm.org/doi/10.1145/1023875.1023890)
* Additional concept: Vary Nakagami m parameter based on distance (assume line
of sight when close by): http://www.csl.mtu.edu/cs5461/www/Reading/Moreno-vanet04.pdf
* Parameters:
  * m (default=3): Nakagami fading parameter. Distribution simplifies to
Rayleigh for m = 1. Larger values of m correspond to weaker multipath fading
effects
