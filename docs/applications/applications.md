---
layout: default 
title: Applications
has_children: true
nav_order: 4
permalink: applications
---

The ICAROUS architecture is modular and the core functionalities are
organized into individual apps. These apps communicate among each other via
the cFS software bus to ensure a UAS can operate safely. The apps to run
should be listed in the Icarous_defs/apps.cmake file.

The cmake build system configures the executable folder along with the
appropriate startup configuration to script that will start all the apps
defined in CMake/apps.cmake.

## Apps to run

### Core apps
The following suite of apps must be run to provide geofencing, traffic resolution and trajectory management capabilities:
- Geofence
- Traffic
- Trajectory
- Cognition or Plexil
- Guidance
- Scheduler

### Interface apps
* GsInterface app must be run to send/receive data to ICAROUS. 
* A suitable autopilot interface application to send/receive commands/data to an external autopilot.

NOTE: Arducopter app serves a dual role. It can send data to the autopilot and also to a GCS using the mavlink routing capability. See [ardupilot]().

### Creating your own apps

You can add new functionality, incorporate new sensors and actuators using
your own apps. The ICAROUS repository provides a cFS app template generator.
This provides most of the standard boiler plate stuff that one would need to
get started with a cFS application. To use the template generator, make sure
you have the pystache package installed.
```
pip install pystache
```

Generate a bare bones application as follows:

```
cd Python/Template
python CreateApp.py --APP_NAME camera --AUTHOR John\ Doe
```

