---
layout: default 
title: Parameters
nav_order: 6
---

## ICAROUS Parameters

List of parameters used by all the apps in the Icarous repository.

| Parameter Name |  Description |
|----------------|--------------|
| TRAFFIC_SRC    | Select traffic source (0: All sources, 1: ADSB, 2: Radar) |
| RES_TYPE       | Sense and avoid resolution type (0: Speed, 1: Altitude, 2: Track, 3: Vertical speed, 4: Search resolution)|
| LOGDAADATA     | Record DAA data (0/1)|
| LOOKAHEAD_TIME | Lookahead horizon used by DAIDALUS (s)   |
| AL_1_ALERT_T   | Alertor 1 Lower bound alert time for conflicts (s) |
| AL_1_E_ALERT_T | Alertor 1 Upper bound alert time for conflicts (s) |
| AL_1_SPREAD_ALT| Alertor 1 altitude spread (ft)          |
| AL_1_SPREAD_VS | Alertor 1 vertial speed spread (fpm) |
| AL_1_SPREAD_TRK| Alertor 1 track spread (deg)|
| AL_1_SPREAD_GS | Alertor 1 ground speed spread (knots)|
| CA_BANDS       | Conflict avoidance bands (0/1)|
| CA_FACTOR      | Conflict factor (0<=CA_FACTOR<=1)|
| CONFLICT_CRIT  | Conflict criteria (0/1)|
| CONFLICT_LEVEL | Conflict level (natural number)|
| CONTOUR_THR    | Contour threshold (deg)|
| DET_1_WCV_DTHR | Aletor 1 radius (ft)|
| DET_1_WCV_TCOA | Alertor 1 time to co-altitude (s)|
| DET_1_WCV_TTHR | Alertor 1 tau threshold (s)|
| DET_1_WCV_ZTHR | Alertor 1 height (ft)|
| ALT_STEP       | Altitude step (ft)|
| GS_STEP        | Ground speed step (knots)|
| TRK_STEP       | Track step (deg)|
| VS_STEP        | Vertical speed step (fpm)|
| HORIZONTAL_NMAC| Horizonal NMAC distance (ft)|
| VERTICAL_NMAC  | Vertical NMAC distance (ft)|
| VERTICAL_ACCL  | Max vertical acceleration  of vehicle (m/s/s)| 
| HORIZONTAL_ACCL| Max horiztal acceleration of vehicle (m/s/s) |
| VERTICAL_RATE  | Vertical speed of vehicle used for climbs (fpm)|
| RIGHT_TRK      | Right heading constraint (deg)|
| LEFT_TRK       | Left heading constraint (deg)|
| MAX_ALT        | Max altitude (ft)|
| MAX_GS         | Max ground speed (knots)|
| MAX_VS         | Max vertical speed (fpm)|
| MIN_ALT        | Min altitude (ft)|
| MIN_GS         | Min ground speed (knots)|
| MIN_VS         | Min vertical speed (fpm)|
| BANK_ANGLE     | Band angle used for turns (deg)|
| TURN_RATE      | Turn rate used for turns (deg/s)|
| RECOVERY_ALT   | Recovery altitude bands (0/1)|
| RECOVERY_CRIT  | Recover criteria (0/1)|
| RECOVERY_GS    | Recovery ground speed bands (0/1)|
| RECOVERY_TRK   | Recovery track bands (0/1)|
| RECOVERY_VS    | Recovery vertical speed bands (0/1)|
| RECOV_STAB_TIME| Recovery stability time (s)|
| MIN_HORIZ_RECOV| Minimum horizontal recovery distance (ft)|
| MIN_VERT_RECOV | Minimum vertical recovery distance (ft)|
| ASTAR_ENABLE3D | Enable 3D Astar serach  (0/1)|
| ASTAR_GRIDSIZE | Grid size used by Astar search (m)|
| ASTAR_LOOKAHEAD| Lookahead time used by Astar (s)|
| ASTAR_RESSPEED | Speed used by Astar search (m/s)|
| COMMAND        | Tracking command |
| DISTH          | Horizontal tracking distance (m)|
| DISTV          | Vertical tracking distance (m)|
| HEADING        | Tracking bearing (deg)|
| TRACKINGOBJID  | Tracking object ID |
| PGAINX         | Proportional gain x |
| PGAINY         | Proportional gain y |
| PGAINZ         | Proportional gain z |
| RESSPEED       | Speed used for resolutions (m/s)|
| OBSBUFFER      | Obstacle buffers (m)|
| RRT_CAPR       | RRT capture radius (m)|
| RRT_DT         | Timestep used in RRT algorithm (s)|
| RRT_MACROSTEPS | Number of macro steps used by RRT |
| RRT_NITERATIONS| Max number of RRT iterations |
| RRT_RESSPEED   | Speed used in RRT search (m/s)|
| SEARCHALGORITHM| Search algorithm to be used (0: Grid, 1: Astar, 2: RRT, 3: Splines)|
| LOOKAHEAD      | Lookahead time used for detecting geofence constraints (s)|
| HTHRESHOLD     | Horizontal buffer for geofences (m)|
| VTHRESHOLD     | Ceiling/roof buffer for geofences (m)|
| HSTEPBACK      | Distance to stepback in a geofence resolution (m)|
| VSTEPBACK      | Vertial distance to stepback in a geofence resolution (m)|
| MAXCEILING     | Max mission ceiling (m)|
| XTRKDEV        | Permissible cross track devition from flightplan (m)|
| XTRKGAIN       | Proportional gain used for cross track control |
| DEF_WP_SPEED   | Default waypoint speed (used when flight speed is not provided by the flightplan) |
| CAP_R_SCALING  | Capture radius scaling applied to speed - used to determine waypoint transitions |
| GUID_R_SCALING | Guidance scaling parameter |
| CLIMB_ANGLE    | Angle for climb or descents (deg) |
| CLIMB_ANGLE_VR | Vertical climb threshold (m) |
| CLIMB_ANGLE_HR | Horizontal climb threshold (m) |
| CLIMB_RATE_GAIN| climb controller gain |
| MAX_CLIMB_RATE | max climb rate (m/s) |
| MIN_CLIMB_RATE | min climb rate (m/s) |
| MAX_CAP        | max limit on capture radius |
| MIN_CAP        | min limit on capture radius |
| YAW_FORWARD    | set heading to match track (0/1) |

- Note (0/1) represent False/True

## Parameter Guidelines

- Set DEF_WP_SPEED to match mission speed used for the flightplan
- Set RES_SPEED to match DEF_WP_SPEED
- Set GUID_R_SCALING to match the DEF_WP_SPEED  
- In scenarios where traffic avoidance is required in the presence of geofences, RES_TYPE=4 (search resolution) should be used.
