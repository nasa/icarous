[![Build Status](https://travis-ci.org/nasa/icarous.svg?branch=master)](https://travis-ci.org/nasa/icarous)

![](ICAROUS-logo.jpeg "")

Independent Configurable Architecture for Reliable Operations of
Unmanned Systems (ICAROUS)
========

ICAROUS (Independent Configurable Architecture for Reliable Operations of
Unmanned Systems) is a software architecture that enables the robust integration
of mission specific software modules and highly assured core software
modules for building safety-centric autonomous unmanned aircraft
applications. The set of core software modules includes formally
verified algorithms that detect, monitor, and control conformance
to safety criteria; avoid stationary obstacles and maintain a safe
distance from other users of the airspace; and compute resolution
and recovery maneuvers, autonomously executed by the autopilot, when
safety criteria are violated or about to be violated. ICAROUS is implemented using the
NASA's core Flight Systems (cFS) middleware. The aforementioned functionalities are implemented as
cFS applications which interact via a publish/subscribe messaging
service provided by the cFS Software Bus.

### User Guide

https://nasa.github.io/icarous/

### Current Releases

- ICAROUS  V-2.1.7 - March 22, 2019

### License

The code in this repository is released under NASA's Open Source
Agreement.  See the directory [`LICENSES`](LICENSES); see also the copyright notice at the end of this file. 

### Contact

[C&eacute;sar A. Mu&ntilde;oz](http://shemesh.larc.nasa.gov/people/cam) (cesar.a.munoz@nasa.gov), NASA Langley Research Center.

### REQUIRED PACKAGES

The following repositories are required to run (or support software-in-the-loop tests for) ICAROUS (You will also have to update the submodules in these repositories if available)

- [Ardupilot](https://github.com/ArduPilot/ardupilot.git) (Only required for software in the loop simulations)
- [mavlink](https://github.com/ArduPilot/mavlink.git) 
- [MAVProxy](https://github.com/ArduPilot/MAVProxy.git)
- [NLOPT](https://nlopt.readthedocs.io/en/latest/) (required only if using splines based path planner)

### Initialize and update submodules

```
    $git submodule update --init --recursive
```

### COMPILING ICAROUS

ICAROUS makes use of the cmake build system. In order to setup a build, the following environment variables must be defined. These can be added to you `~/.bashrc` script. Conveniently, you can also source the SetEnv.sh script in the repository.

- PLEXIL_HOME= absolute path to the Modules/Plexil folder found in the Icarous repository.
- OSAL_HOME= absolute path to the CFS/osal folder found in the Icarous repository.
- JAVA_HOME= absolute path to the Java installation directory. Typically located under `/usr/lib/jvm/java-*`
- Optional NLOPT_PATH = absolute path to the nlopt library location (libnlopt.a) [Optional-only required if splines based planner is being used].

```
    $mkdir build
    $cd build && cmake ..
    $make cpu1-install
```

Run cmake with `-DSITL=ON` option to compile for software-in-the-loop simulations. Use `-DSPLINES=ON` to compile splines planner (This requires the nlopt library).

For compilation on an ARM processor, follow the instructions in patches/arm before running cmake.

ICAROUS and cFS can be compiled and run on OSX as well. Follow instructions in patches/osx. NOTE: Due to the lack of extended posix functionality on OSX, the patches provide implementations of osal's timer api. However, the scheduler application doesn't work well with this patch. We recommend using the macScheduler app instead.

### LAUNCHING ICAROUS

The generated executable file is installed under `cFS/bin/cpu1`. Launch Icarous using the following command:

```
    $sudo ./core-cpu1 -I 0 -C 1
```

Note that you need root previleges. When running ICAROUS on an embedded platform, one can make use of the `nohup` or `screen` command to avoid termination when a terminal is closed.

The intf_tbl.c located under `cFS/apps/ardupilot/fsw/tables` defines parameters required to configure the serial port settings to connect to an autopilot. Similarly gsIntf_tbl.c contains parameters that configure the connection to a ground station (radio link or a socket connection).

The various parameters that control the behavior of ICAROUS can be found in `cFS/bin/ram/icarous.txt`. The default parameters found in icarous.txt were selected after several flight tests to yield acceptable performances.

### INTERACTING WITH ICAROUS

We strongly recommend using MAVProxy as a ground station to communicate with ICAROUS. The custom MAVProxy modules provided in `Python/CustomModules` help upload geofence and visualize track bands.

- Install the custom modules provided as part of the ICAROUS repository:

```
    $cd Python/CustomModules
    $bash SetuMavProxy.sh <Location of MAVProxy/>
```

The MAVProxy ground station can be lauched using the run script located under the Scripts folder:

    $./runGS.sh

Waypoints can be uploaded from MAVProxy using the `wp` command, e.g.,

	wp load Examples/InputData/flightplan.txt

Geofence can be uploaded from MAVProxy using the `geofence` command, e.g.,

	geofence load Examples/InputData/geofence.xml

(Optional: To automatically check that geofences are composed of "nice" polygons before uploading, download PolyCARP from https://github.com/nasa/PolyCARP.git and add the Python folder in this repository to the PYTHONPATH environment variable.)
	
Once waypoints and geofence are uploaded, the mission can be started from MAVProxy as follows:

    long MISSION_START

### RUNNING ICAROUS WITH THE ARDUPILOT SITL

Setup and launch the ardupilot SITL as described in <http://ardupilot.org/dev/docs/setting-up-sitl-on-linux.html>. By default, the ardupilot SITL is configured to output packets to udp port 14551. To interface Icarous with the ardupilot SITL, generate the SITL build and compile as follows:

```
    $mkdir build-stil
    $cd build-sitl && cmake .. -DSITL=ON
    $make cpu1-install
```

### DDS APP FOR ICAROUS (Optional)

ICAROUS provides an application to communicate using OpenSplice DDS. Enable this application as follows:

1. Download and install an OpenSplice DDS 32bit version

2. Create an OpenSplice configuration file (.xml) with **domain id** 100.  Follow the examples provided in the OpenSplice DDS distribution directory within `etc/conf`.

3. Add `dds_interface` to the `TGT1_APPLIST` in the file `cFS/apps/Icarous_defs/targets.cmake` to compile the dds application:

4. Define the following environment variable for building:

     - `OSPL_HOME`: the path of the OpenSplice local installation; for example, `/opt/OpenSplice/HDE/x86.linux`

5. Define the following environment variables before executing ICAROUS:

     - `OSPL_URI`: the URI of the OpenSplice `*.xml` configuration file for **domain id** 100 (the one of *Step 2*); for example, `file:///opt/OpenSplice/HDE/x86.linux/etc/icarous-ospl.xml` (note the three initial slashes)
     - `OSPL_HOME`: the path of the OpenSplice local installation; for example, `/opt/OpenSplice/HDE/x86.linux`
     - `LD_LIBRARY_PATH`: it has to include the library path of the OpenSplice local installation in order for ICAROUS to find them; for example, `/opt/OpenSplice/HDE/x86.linux/lib`

   An example command could look like this:
   
```
OSPL_URI="file:///opt/OpenSplice/HDE/x86.linux/etc/icarous-ospl.xml" \
OSPL_HOME="/opt/OpenSplice/HDE/x86.linux" \
LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/opt/OpenSplice/HDE/x86.linux/lib" \
sudo ./core-cpu1
```


### Detect and Avoid (DAA) and Geofencing Capabilities

ICAROUS integrates NASA's open source software packages [DAIDALUS](http://shemesh.larc.nasa.gov/fm/DAIDALUS)
(Detect and Avoid Alerting Logic for Unmanned Systems) and
[PolyCARP](http://shemesh.larc.nasa.gov/fm/PolyCARP) (Algorithms and Software
for Computations with Polygons). DAIDALUS provides detect and avoid
capabilities, while PolyCARP provides geofencing capabilities.

## Logo

The ICAROUS logo was designed by 
[Mahyar Malekpour](http://shemesh.larc.nasa.gov/people/mrm/publications.htm#ETC), NASA Langley Research Center.

### Copyright Notice

Copyright 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration. All Rights Reserved.

No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."

Waiver and Indemnity: RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
