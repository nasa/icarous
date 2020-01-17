---
layout: default
title: Interactions
nav_order: 5
---

# Interacting with Icarous

The gsInterface app must be run inorder to interact with Icarous using a suitable mavlink compatible ground station. We recommend the following ground stations for interacting with Icarous:

- [MAVProxy](https://github.com/ArduPilot/MAVProxy)
- [WebGS](https://github.com/nasa/webgs)

## Using MAVProxy

Install the custom mavproxy modules using the script: [Python/CustomModules](https://github.com/nasa/icarous/tree/master/Python/CustomModules)

```
bash SetupMavProxy.sh <path/to/parentfolder/of/MAVProxy>
```

Launch the MAVProxy ground station using the provided script: [Scripts/runGS.sh](https://github.com/nasa/icarous/blob/master/Scripts/runGS.sh)

```
./runGS.sh
```

### Uploading flightplan

Upload a flightplan using the MAVProxy command line:

```
wp load <path/to/file>
```

Flight plans can also drawn on the map console.

### Uploading geofences

The Icarous repository provides custom module to work draw or upload keep in and keep out geofences to Icarous.
To upload a fence from the MAVProxy command line:
```
geofence load <path/to/filename>
```
See [this file](https://github.com/nasa/icarous/blob/master/Examples/InputData/geofence2.xml) for the input xml file format. You can also right click on the map and draw geofences.

### Uploading parameters

Upload parameters from the MAVProxy command line:

```
param load <path/to/filename>
```

Specific parameters can be changed using the `set` command

```
param set <NAME> <VALUE>
```

List all parameters:
```
param show *
```

### Starting the mission

Send the start command to Icarous from MAVProxy:
```
long MISSION_START
```

### Simulating traffic intruders

Using the custom traffic module, you can simulate intruders with a constant velocity in the airspace. The simulated intruders are sent to Icarous via MAVLink messages and Icarous will react to these intruders if there is a conflict.

```
traffic load <range> <bearing> <altitude> <speed> <heading> <verticalspeed>
```
Here `range`,`bearing` and `altitude` describe the initial condition of the traffic vehicle with respect to the ownship. `speed`,`heading` and `verticalspeed` describe the velocity of the intruder.
