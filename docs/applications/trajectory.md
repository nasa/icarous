---
layout: default 
title: Trajectory
parent: Applications
nav_order: 5
---

# Trajectory Monitor

The trajectory monitor application provides flight plan monitoring and path
planning capability. Data about flight plan deviation is published under the
ICAROUS_FLIGHTPLAN_MONITOR_MID topic. The app also processe requests for new
trajectories ICAROUS__TRAJECTORY__REQUEST_MID and computes trajectories based
on the requests. The computed trajectories are published under the
ICAROUS_TRAJECTORY_MID topic.

## Parameters

The following parameters control the behaviour of the output of trajectory monitor:
 - **RESSPEED** : Speed (m/s) to be used when executing resolutions
 - **OBSBUFFER** : Buffer to be added to all obstacles (geofences) when computing trajectories.
 - **RRT_CAPR**: Capture radius used by the RRT algorithm
 - **RRT_DT** : Micro step (in seconds) used in RRT algorithm
 - **RRT_MACROSTEPS** : Macro steps (in seconds) used by RRT algorithm
 - **RRT_NITERATIONS** : Total number of iterations
 - **RRT_RESSPEED** : Speed to be used for RRT search
 - **SEARCHALGORITHM** : Search algorithms to be used. (0: GRID, 1: ASTAR, 2: RRT)

