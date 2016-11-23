![](logo/ICAROUS.jpeg "")

Integrated Configurable Algorithms for Reliable Operations of Unmanned Systems
========

This software release contains ICAROUS and PolyCARP software
distributions.

ICAROUS (Integrated Configurable Algorithms for Reliable Operations of
Unmanned Systems) is a software architecture that enables the robust integration
of mission specific software modules and highly assured core software
modules for building safety-centric autonomous unmanned aircraft
applications. The set of core software modules include formally
verified algorithms that (1) detect, monitor, and control conformance
to safety criteria; (2) avoid stationary obstacles and maintain a safe
distance from other users of the airspace; and (3) compute resolution
and recovery maneuvers, autonomously executed by the autopilot, when
safety criteria are violated or about to be violated.

PolyCARP (Algorithms and Software for Computations with Polygons)
is a package of algorithms, including both their formal
models and software implementations, for computing containment,
collision, resolution, and recovery information for polygons. The
intended applications of PolyCARP are related, but not limited, to
safety critical systems in air traffic management.

### Documentation

The API document for ICAROUS is still work in progress. In the meantime,
please refer to [`Java/README.md`](Java/README.md) for instructions on running
ICAROUS, to the directory `TestCases` for examples of Software in the Loop
test cases, and to
[`Java/IcarousExample.java`](Java/IcarousExample.java) for a simple
example of ICAROUS' DAA (Detect and Avoid) and Geo-fencing
capabilities. This later example is also available in [C++](C++/IcarousExample.java)

For technical information about the definitions and algorithms in this
repository, visit [http://shemesh.larc.nasa.gov/fm/ICAROUS](http://shemesh.larc.nasa.gov/fm/ICAROUS).

### Current Release

ICAROUS 1.0 (November 18, 2016) 

### License

This code is released under NASA's Open Source Agreement. For more
information see files in directory LICENSES.

### Contact

[C&eacute;sar A. Mu&ntilde;oz](http://shemesh.larc.nasa.gov/people/cam) (cesar.a.munoz@nasa.gov), NASA Langley Research Center.

## Logo

The ICAROUS logo was designed by 
[Mahyar Malekpour](http://shemesh.larc.nasa.gov/people/mrm/publications.htm#ETC), NASA Langley Research Center.
