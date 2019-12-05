---
layout: default 
title: Geofence
parent: Applications
nav_order: 3
---

# Geofence monitor

The geofence handler application monitors for geofence (keep in/out) geofence
conflicts. Geofece conflict data is published constantly at 30 Hz under the
ICAROUS_GEOFENCE_MONITOR_MID topic. This app also accepts queries to check if a
given path segment between two waypoints is a feasible path segment.

## Parameters

The following parameters control the behavior of the geofence app:
- **LOOKAHEAD**: A horizon (in seconds) to look into the future for a geofence conflict.
- **HTHRESHOLD**: Buffer by which the horizontal dimensions of the geofence are expanded (for keep-out) / contracted (for keep-in)
- **VTHRESHOLD**: Buffer by which the vertical dimensions of the geofence are expanded (for keep-out) / contracted (for keep-in)
- **HSTEPBACK**: Horizontal distance to step back after a geofence conflict
- **VSTEPBACK**: Vertical distance to step back if a geofence roof/floor were to be violated.

### Caveats
- Make sure **HSTEPBACK** is greater than (**LOOKAHEAD** x speed of vehicle) to ensure vehicle can remain free of geofence conflict after stepping back.
