![](ICAROUS-logo.jpeg "")

Independent Configurable Architecture for Reliable Operations of Unmanned Systems (ICAROUS)
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

- ICAROUS  V-2.2.5 - March 8, 2021

### Pycarous

Refer to [Python/pycarous/README.md](Python/pycarous/README.md) for more information about the Icarous python framework.


### License

The code in this repository is released under NASA's Open Source
Agreement.  See the directory [`LICENSES`](LICENSES); see also the copyright notice at the end of this file. 

### Contact
Maria Consiglio (maria.c.consiglio@nasa.gov), NASA Langley Research Center.

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
