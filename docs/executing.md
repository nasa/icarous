---
layout: default
title: Execution
nav_order: 3
---

## Running Icarous in simulation

The [RunPySim.py]({{ site.nasablob }}/Python/pycarous/RunPySim.py) script can be used to simulate a scenario. Provide as input the flightplan the unmanned vehicle is required to fly, zero or more Keep-in and/or Keep-out geofence constraints the vehicle should satisfy and zero or more intruders. 

- Input flightplans are provided in the [mavlink format](https://mavlink.io/en/file_formats/#mission_plain_text_file). 
- Geofences are defined using a simple yaml file. Refer to [geofence.yaml]({{ site.nasablob }}/Python/pycarous/data/geofence.yaml) for an example. 
- Intruder initial conditions are defined using a simple yaml file. See [traffic.yaml]({{ site.nasablob }}/Python/pycarous/data/traffic.yaml) for an example.
- Icarous configurations are defined in a .txt file. see [IcarousConfig.txt]({{ site.nasablob }}/Python/pycarous/data/IcarousConfig.txt)

Refer to the help for more information on flags:
```
python3 RunPySim.py --help
```

## Example
A simple example simulating an encounter with a traffic intruder is provided. To simulate
and visualize an animation of the simulation, try the following script:

```
python3 RunPySim.py -t data/traffic.txt
```

## Visualization
The above simulation produces a log/simlog-SPEEDBIRD.json log file. Use the `VisualizeLog.py` script to visualize the simulation.
```
python3 VisualizeLog.py log/simlog-SPEEDBIRD.json
```
Checkout the help for more options:
```
python3 VisualizeLog.py --help
```


## Executing cFS

The cFS core executive is launched by running the `core-cpu1` executable. This
is installed in [exe/cpu1]({{ site.nasablob }}/exe/cpu1) after running `make install` in the previous
section.

This cFS executable in turn launches the varous applications configured to
run in the exe/cpu1/cfe_core_cpu1.scr. The CMake build system in the
ICAROUS repository automatically generates a cfe_core_cpu1.scr file with the
appropriate apps that are configured to run. More information about specific
applications and their functions can be found in the applications section.

```
# running the cFS executive
./core-cpu1 -C 1 -I 0
```

The argument `C` denotes the cpu id, and `I` denotes the spacecraft id. 

Various configuration files are located under [exe/ram]({{ site.nasablob }}/exe/ram). IcarousConfig.txt contain
key configuration parameters used by the underlying core modules.

With cFS running, you should be able to receive telemetry on a ground station (if you are running the gsInterface application).

