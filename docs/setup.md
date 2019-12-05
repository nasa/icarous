#---
#layout: default
#title: Flight setup 
#nav_order: 5
#---

## Hardware configuration

The figure below indicates the configuration for interfacing a device running
Icarous to an autopilot. This examples uses an [ardupilot]() autopilot. The
ardupilot autopilot can be configured to output data on one of its serial
ports. The device running Icarous (this example shows a Jetson TX2) can be
connected to the autopilot via its serial I/O ports as shown.

## Software in the loop simulation 

ICAROUS can also be run in a Software In The Loop (SITL) simulation. This
requires connecting ICAROUS to the simulator. The ardupilot SITL can be
configured to ouptut data on a local socket. Thte ardupilot interface app can
in turn be configured to connect to this SITL over the socket.

## Connecting ICAROUS to a GCS

The [gsInterface]() app can be configured to connect to a GCS usin a telemtry
radio via a serial or socket connection. Data packets sent to the GCS use the
mavlink protocol. [MAVProxy]() can be used as a GCS to interface with ICAROUS.
The ICAROUS repository provides [custom modules]() for MAVProxy to upload
geofences, flight plans. Also simulate traffic intruders and visualize traffic
avoidance data  (kinematic track bands).

## Upload a flight plan


## Upload a geofence


## Upload parameters


## Start the mission

