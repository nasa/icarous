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

- ICAROUS  V-2.1.25 - December 4, 2019

### License

The code in this repository is released under NASA's Open Source
Agreement.  See the directory [`LICENSES`](LICENSES); see also the copyright notice at the end of this file. 

### Contact

[C&eacute;sar A. Mu&ntilde;oz](http://shemesh.larc.nasa.gov/people/cam) (cesar.a.munoz@nasa.gov), NASA Langley Research Center.

### Initialize and update submodules

```
    $bash UpdateModules.sh
```

### COMPILING ICAROUS

```
    $make 
    $make install
```

### LAUNCHING ICAROUS

The generated executable file is installed under `cFS/bin/cpu1`. Launch Icarous using the following command:

```
    $sudo ./core-cpu1 -I 0 -C 1
```

The intf_tbl.c located under `cFS/apps/ardupilot/fsw/tables` defines parameters required to configure the serial port settings to connect to an autopilot. Similarly gsIntf_tbl.c contains parameters that configure the connection to a ground station (radio link [SERIAL] or a socket connection [SOCKET]).

For list of parameters that Icarous uses, please refer to Examples/InputData/icarous_default.parm

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

Parameters can be uploaded from MAVProxy using the `param` command, e.g.,

    param load Examples/InputData/icarous_default.parm

(Optional: To automatically check that geofences are composed of "nice" polygons before uploading, download PolyCARP from https://github.com/nasa/PolyCARP.git and add the Python folder in this repository to the PYTHONPATH environment variable.)
	
Once waypoints and geofence are uploaded, the mission can be started from MAVProxy as follows:

    long MISSION_START


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
