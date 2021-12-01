---
layout: default 
title: Organization
nav_order: 2
---

# ICAROUS Modules

[ICAROUS modules]({{ site.nasatree }}/Modules) encapsulate the core functionalities required to enable autonomous operations of unmanned systems. These libraries can be used to build complex autonomy related applications.

## ACCoRD
A library developed at NASA Langley for airspace applications. ACCoRD contains several useful utilities:
 - Data structures for representing aircraft state data (position, velocity, attitude, etc) in various coordinate frames and units. 
 - Data structures to encode EUTL flightplans and manipulations on flightplans.
 - Functions to perform common transformations on aircraft data. 
 - Geofence functionality (PolyCARP) 
 - Sense and avoid functionality (DAIDALUS)

## Core
Suite of libraries implementing autonomous decision making, path planning, traffic avoidance and guidance:
 - EventManager: A template library implementing an event handling framework.
 - Cognition: Decision making functionality impelemented using the EventManager.
 - TrafficMonitor: Traffic avoidance capability (default implementation uses DAIDALUS). Extensible to other sense and avoid tools (e.g. ACAS-X).
 - TrajectoryMananger: Path planning capability and trajectory monitoring capability with respect to other intruders and geofences.
 - Guidance: Trajectory following functionality.
 - Merger: Distributed merging functionality.
 - GeofenceMonitor: Geofence monitoring functionality.
 - TargetTracker: Target tracking and fusion capability.
 - Utils: Helper functions.
 - Interfaces: Datastructures shared among all the core modules

# Core Flight Systems (cFS) Integration

 cFS is a middleware (written in C) developed at NASA. cFS serves as a platform to build distributed applications. The cFS software bus (SB) provides a publish-subscribe framework with which distributed applications can communicate with each other. The ICAROUS repository provides several cFS apps that make use of the core modules described above. These applications interact with each other to provide decision making, path planning, guidance and traffic/obstacle avoidance capabilities for unmanned aerial vehicles. Sensor interface applications read data from onboard sensors and publish data (aircraft and environment state information) on the SB. Other applications consume this data as input to the various underlying algorithms (i.e. decision making, planning, control etc). Guidance/Control commands are published on the SB. These commands are consumed by an autopilot interface application to send to the onboard autopilot. A ground station interfaction application can send/receive telemetry/commands from a ground station. cFS applications are located under [apps]({{ site.nasatree }}/apps).

# Python simulation 

 The core ICAROUS modules described above are also integrated into a python simulation framework to simulate aircraft flying various flightplans and interacting with other intruders in the airspace. See [Python/pycarous/README.md]({{ site.nasablob }}/Python/pycarous/README.md) for further details regarding the simulation framework.