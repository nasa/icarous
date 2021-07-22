# Pycarous

Pycarous is a python wrapper for the core ICAROUS C++ modules. 
Pycarous enables us to simulate ICAROUS (faster than real time).
This is very helpful to perform batch simulations and analysis for ICAROUS.

## Compile core ICAROUS Modules

First, follow the [module instructions](../../Modules/README.md) to compile the core ICAROUS modules.

## Install dependencies

Pycarous requires several python packages. These packages can be easily installed as follows:

    pip3 install -r requirements.txt

Note: If you are using windows, consult the [Gotchas](#Gotchas-on-Windows) section at the end.

## Generate Cython bindings

```
python3 setup.py build_ext --inplace
```

## Running a simulation
The `RunPySim.py` script can be used to simulate a scenario. Provide as input the flightplan the unmanned vehicle is required to fly, zero or more Keep-in and/or Keep-out geofence constraints the vehicle should satisfy and zero or more intruders. 

- Input flightplans are provided in the [mavlink format](https://mavlink.io/en/file_formats/#mission_plain_text_file). 
- Geofences are defined using a simple yaml file. Refer to [geofence.yaml](data/geofence.yaml) for an example. 
- Intruder initial conditions are defined using a simple yaml file. See [traffic.yaml](data/traffic.yaml) for an example.

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

## Gotchas on Windows

- The python3 version should have been compiled using the same compiler used to generate the core Modules. For the Modules, we've recommended using the MinGW compiler tool chain. If you are using msys2 as recommended, you should be able to install a compatible python3 and python3-pip version from [msys2](https://www.msys2.org/).
- Make sure the windows environment path variable contains the `bin` folder where python3 and python3-pip are installed. The default place is `C:\msys64\mingw64\bin`.
- Unfortunately, not all packages can be installed via pip3. We recommend looking at the `requirements.txt` file for required packages and manually installing them via msys2. msys2 contains MinGW compatible package distributions. You should be able to search for the packages on msys2 as follows:
```
pacman -S package_name 
```
