---
layout: default 
title: Cognition
parent: Applications
nav_order: 8
---

# Cognition

This app provides decision making functionality in ICAROUS. Decision
procedures are encoded as deterministic finite state machines. Currently, the app focuses on the following:
- Provides response to geofence constraint violations. Prevents keep in/keep out geofence violation by responding with a bounce back maneuver. Provides reroute trajectories to safely get to the next feasible waypoint.
- Provides traffic avoidance capability. Traffic avoidance can be done via heading changes, altitude or speed changes. Search based traffic resolution also account for geofence constraints in the airspace.
- To explicity specify the type of resolution, see RES_TYPE parameter. For scenarios involving both geofence and traffic constraints only RES_TYPE 4 should be used.
- Ensures the vehicle stays within a corridor around the current flight plan. 
