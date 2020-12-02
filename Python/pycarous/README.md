# Pycarous

Pycarous is a python wrapper for the core ICAROUS C++ modules. 
Pycarous enables us to simulate ICAROUS (faster than real time).
This is very helpful to perform batch simulations and analysis for ICAROUS.

## Install dependencies

Pycarous requires several python packages. These packages can be easily installed as follows:

    pip3 install -r requirements.txt

## Compile ICAROUS Modules
Follow the [module instructions](../../Modules/README.md) to compile the core ICAROUS modules.

## Running a simulation
The `RunPySim.py` script can be used to simulate a scenario. Provide as input the flightplan the unmanned vehicle is required to fly, zero or more Keep-in and/or Keep-out geofence constraints the vehicle should satisfy and zero or more intruders. 

- Input flightplans are provided in the [mavlink format](https://mavlink.io/en/file_formats/#mission_plain_text_file). 
- Geofences are defined using a simple custom xml schema. Refer to [geofence.xml](data/geofence.xml) for an example. 
- Intruder initial conditions are defined using a simple text file. See [traffic.txt](data/traffic.txt) for an example.

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
The above simulation produces a .json log file containing the callsign of the vehicle. Use the `VisualizeLog.py` script to visualize the simulation.
```
python3 VisualizeLog.py simlog-SPEEDBIRD.json
```
