![](logo/ICAROUS.jpeg "")

Integrated Configurable Algorithms for Reliable Operations of Unmanned Systems
========

ICAROUS (Integrated Configurable Algorithms for Reliable Operations of
Unmanned Systems) is a software architecture that enables the robust integration
of mission specific software modules and highly assured core software
modules for building safety-centric autonomous unmanned aircraft
applications. The set of core software modules include formally
verified algorithms that detect, monitor, and control conformance
to safety criteria; avoid stationary obstacles and maintain a safe
distance from other users of the airspace; and compute resolution
and recovery maneuvers, autonomously executed by the autopilot, when
safety criteria are violated or about to be violated.

### Current Release

ICAROUS V-1.1 - March 18, 2017

### License

The code in this repository is released under NASA's Open Source
Agreement.  See the directory [`LICENSES`](LICENSES); see also the copyright notice at the end of this file. 

### Contact

[C&eacute;sar A. Mu&ntilde;oz](http://shemesh.larc.nasa.gov/people/cam) (cesar.a.munoz@nasa.gov), NASA Langley Research Center.

### REQUIRED PACKAGES

The following repositories are required to run icarous. 

- [Ardupilot](https://github.com/ArduPilot/ardupilot.git) (Only required for software in the loop simulations)
- [mavlink](https://github.com/ArduPilot/mavlink.git)
- [MAVProxy](https://github.com/ArduPilot/MAVProxy.git)
- [Java simple serial connector (JSSC)](https://code.google.com/archive/p/java-simple-serial-connector/) (Already provided in Java/lib.)

### BEFORE LAUNCHING ICAROUS

ICAROUS uses several messages that are not part of the common/ardupilotmega MAVlink message set. To ensure that the MAVProxy ground station can receive these custom messages, perform the following steps:

- copy icarous.xml in the `msg/` folder in the icarous repository to `mavlink/message_definitions/v1.0`
- (re)install pymavlink library to incorporate the new dialect (icarous.xml). To do this, goto `mavlink/pymavlink` and run:

```
    $python setup.py install
```

- Install the custom modules provided as part of the ICAROUS repository:

```
    $cd Python/CustomModules
    $bash SetuMavProxy.sh <Location of MAVProxy/>
```

### LAUNCHING ICAROUS

The various parameters that control the behavior of ICAROUS can be found in `params/icarous.txt` under Java or C++. The default parameters found in icarous.txt were selected after several flight tests to yield acceptable performances. For convenience, a script (run.sh) is provided to launch ICAROUS and other supporting applications. The run script also helps configure several port options.

To run ICAROUS on a companion computer for Ardupilot, first make sure that the pixhawk port and baud rate settings in the run script are the same as the telemetry port settings on the pixhawk. First lauch the ICAROUS application as follows:

	$nohup ./run.sh PX4 > pxout.txt &

Note that the nohup (no hang up) command enables the application to run as a background process without interruption. To enable interaction with ICAROUS and the pixhawk through a ground station uplink, launch the radio interface application:

    $nohup ./run.sh RADIO > radioout.txt &

Once the above two applications are launched, you should be able to receive data via the telemetry link connected to the ground station (e.g. mission planner/mavproxy). When using radios with the SiK firmware, it is recommended that the firmware is configured with the following settings:

* MAVLink - None (Raw data mode).
* Op Resend - disabled.
* ECC - disabled.
The Sik radio firmware can be configured using Mission planner or APM Planner.

We strongly recommend using MAVProxy as a ground station to communicate with ICAROUS. To simplify uploading geofence data to ICAROUS, additional modules are provided in `Python/CustomModules`. The MAVProxy ground station can be lauched using the run script:

    $./run.sh GS

Waypoints can be uploaded from MAVProxy using the `wp` command. An example waypoint is provided in `Java/params`

	wp load params/flightplan.txt

Geofence can be uploaded from MAVProxy using the `geofence` command. An example geofence file is provided in `Java/params`

	geofence load params/geofence.xml

Once waypoints and geofence are uploaded, the mission can be started from MAVProxy as follows:

    long MISSION_START

Update Icarous parameters as follows:

    icparams load params/icarous.txt

### Running ICAROUS with the ardupilot SITL


Setup and launch the ardupilot SITL as described in <http://ardupilot.org/dev/docs/setting-up-sitl-on-linux.html>. Specify the SITL host address in the run script. By default, the ardupilot SITL is configured to output packets to udp port 14551. Launch ICAROUS in the SITL mode as follows:

    $./run.sh SITL

While running ICAROUS in the SITL framework, it is possible to communicate with ICAROUS via a ground station using the UDP sockets instead of using a radio link. See `GS_MASTER` variable in the run.sh script to enable the ground station over a UDP socket.

### Detect and Avoid (DAA) and Geofencing Capabilities

ICAROUS integrates NASA's open source software packages [DAIDALUS](http://shemesh.larc.nasa.gov/fm/DAIDALUS)
(Detect and Avoid Alerting Logic for Unmanned Systems) and
[PolyCARP](http://shemesh.larc.nasa.gov/fm/PolyCARP) (Algorithms and Software
for Computations with Polygons). DAIDALUS provides detect and avoid
capabilities, while PolyCARP provides geofencing capabilities.

These capabilities are illustrated in the sample programs
[`DAAGeofencingExample.java`](Java/src/DAAGeofencingExample.java) and
[`DAAGeofencingExample.cpp`](C++/src/DAAGeofencingExample.cpp).

## Logo

The ICAROUS logo was designed by 
[Mahyar Malekpour](http://shemesh.larc.nasa.gov/people/mrm/publications.htm#ETC), NASA Langley Research Center.

### Copyright Notice

Copyright 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration. All Rights Reserved.

No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."

Waiver and Indemnity: RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
