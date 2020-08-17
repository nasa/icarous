# PyIcarous

PyIcarous is a python wrapper for the core ICAROUS C++ modules. 
PyIcarous enables us to simulate ICAROUS (faster than real time).
This is very helpful to perform batch simulations and analysis for ICAROUS.

To get started, first ensure the core ICAROUS C++ modules are compiled. 
Follow the instructions under Icarous/Modules/README.md to setup the core ICAROUS modules.

## Example
A simple example simulating an encounter with a traffic intruder is provided. To simulate
and visualize an animation of the simulation, try the following script:

```
python3 RunPySim.py --help
python3 RunPySim.py -t data/traffic.txt

```

## Visualization
The above simulation produces a .json log file with the callsign of the vehicle (SPEEDBIRD). 
Use the `VisualizeLog.py` script to visualize the simulation.
```
python3 VisualizeLog.py --help
python3 VisualizeLog.py SPEEDBIRD.json
```
