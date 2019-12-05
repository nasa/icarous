---
layout: default 
title: Guidance
parent: Applications
nav_order: 9
---

# Guidance

Provides flight plan following functionality to autopilots that lack an inbuilt guidance and navigation functionality. There are 3 primary used guidance modes:
- PRIMARY_FLIGHTPLAN mode : Used for following the primary mission flight plan.
- SECONFARY_FLIGHTPLAN mode : Used for following a secondary trajectory. Used to perform reroutes to avoid geofence/traffic constraints.
- POINT2POINT mode: Simple mode to fly to a fixed GPS coordinate.
- VECTOR mode: Used to follow a velocity vector.