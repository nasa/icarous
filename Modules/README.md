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
 - TargetTracker: Target tracking and fusion capability (requires GNU Scientific Library - GSL).
 - Utils: Helper functions.
 - Interfaces: Datastructures shared among all the core modules

## Prerequisites

- GNU Scientific Library (Only needed when using the TargetTracker module). Refer to [GSL](https://www.gnu.org/software/gsl/) for installation instructions.

## Compilation
Compile modules as follows:

### Linux/OSX

```
mkdir build
cd build
cmake ..
make -j8
```

The compiled libraries are located under `lib`

### Setup library paths
- Define a new environment variable called `ICAROUS_HOME` to contain the path to the Icarous folder. We recommend using the export command in your ~/.bash_profile (OSX) or ~/.bashrc (Linux) for persistence. For example,
```
export ICAROUS_HOME=/path/to/icarous
```
- Append the `lib` folder to your `DYLD_LIBRARY_PATH` (OSX) or `LD_LIBRARY_PATH` (Linux) variable. 
```
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$ICAROUS_HOME/Modules/lib
```

### Windows
- We recommend using [vscode](https://code.visualstudio.com/) for compiling on windows.
- We recommnd using [msys2](https://www.msys2.org/) to install required packaged. 
- Install MinGW-w64 compiler. See [msys2](https://www.msys2.org/) for installation instructions.
- Install cmake.
- Define environment variable called `ICAROUS_HOME` that contains the path of the Icarous root directory.
- Include the following in your vscode `settings.json` file (change path values as needed):
```
"cmake.cmakePath": "C:\\msys64\\mingw64\\bin\\cmake.exe",
    "cmake.mingwSearchDirs": [
      "C:\\msys64\\mingw64\\bin"
   ],
"cmake.generator": "MinGW Makefiles"
```
- Include the following in your vscode `cmake-tools-kits.json` file (change path values as needed):
```
  {
    "name": "Mingw64 GCC 9.3.0",
    "compilers": {
      "C": "C:\\msys64\\mingw64\\bin\\gcc.exe",
      "CXX": "C:\\msys64\\mingw64\\bin\\g++.exe"
    },
    "preferredGenerator": {
      "name": "MinGW Makefiles",
      "platform": "x64"
    },
    "environmentVariables": {
      "CMT_MINGW_PATH": "C:/msys64/mingw64/bin/"
    }
  }
```
- Add `%ICAROUS_HOME%\Modules\lib` to the windows path.
