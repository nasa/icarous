---
layout: default
title: Flight setup 
nav_order: 7
---


## Hardware configuration

ICAROUS has been flight tested with the [arducopter](https://ardupilot.org/) autopilot. The [arducopter]({{ site.nasatree }}/apps/arducopter) application translates ICAROUS command into mavlink commands that the arudocopter autopilot can understand. The
arducopter autopilot can be configured to output data on one of its serial
ports. The device running Icarous (this example shows a Jetson TX2) can be
connected to the autopilot via its serial I/O ports. Refer to
[this article](https://ardupilot.org/dev/docs/companion-computer-nvidia-tx2.html) for 
more details on connecting an ardupilot autopilot to a companion computer. To interface with a different autopilot system, you will have to define a custom autopilot interface application. A very simple example applications is provided in [apInterface]( {{ site.nasatree }}/apps/apInterface).

## Software in the loop simulation 

ICAROUS can also be run in a Software In The Loop (SITL) simulation. This
requires connecting ICAROUS to the simulator. The arducopter SITL can be
configured to ouptut data on a local socket. Thte arducopter interface app can
in turn be configured to connect to this SITL over the socket. Refer to
[this article](https://ardupilot.org/dev/docs/sitl-simulator-software-in-the-loop.html#sitl-simulator-software-in-the-loop)
for more details about the SITL framework.

## Connecting ICAROUS to a GCS

The [gsInterface]( {{ site.nasatree }}/apps/gsInterface) app can be configured to connect to a GCS using a telemetry
radio via a serial or socket connection. Data packets sent to the GCS use the
mavlink protocol. [WebGS](https://github.com/nasa/webgs) can be used as a GCS to interface with ICAROUS.
Webgs enables uploading geofences, flight plans and also simulating traffic intruders and visualizing traffic
avoidance data  (kinematic track bands).
