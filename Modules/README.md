# ICAROUS Modules

ICAROUS modules encapsulate the core functionalities required to enable autonomous operations of unmanned systems.

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
 - Utils: Helper functions.
 - Interfaces: Datastructures shared among all the core modules

## Compilation
Compile modules as follows:

```
mkdir build
cd build
cmake ..
make -j8
```

The compiled libraries are located under `lib`

## Setup library paths
Add the `lib` folder to your `DYLD_LIBRARY_PATH` (OSX) or `LD_LIBRARY_PATH` (Linux) variable. We recommend using the export command in your ~/.bash_profile (OSX) or ~/.bashrc (Linux) for persistence. 

For example,

```
# `/path/to/lib` here should refer to the absolute path for the Icarous/Modules/lib folder
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/path/to/lib
```