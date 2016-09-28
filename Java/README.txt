ICAROUS: Integrated Configurable Algorithms for Reliable Operations of Unmanned Systems
-------------------------------------------------------------------------
Version:  Java
Release: FormalATM-v2.5.5, September 26, 2016
Authors: Cesar Munoz, George Hagen, Anthony Narkawicz
Contact: Cesar A. Munoz (Cesar.A.Munoz@nasa.gov)

Copyright: Copyright (c) 2014 United States Government as represented by 
the National Aeronautics and Space Administration.  No copyright 
is claimed in the United States under Title 17, U.S.Code. All Other 
Rights Reserved.

Introduction
------------
ICAROUS (Integrated Configurable Algorithms for Reliable Operations of
Unmanned Systems) is a software architecture that enables the robust
integration of mission specific software modules and highly assured
core software modules for building safety-centric autonomous unmanned
aircraft applications. The set of core software modules include
formally verified algorithms that (1) detect, monitor, and control
conformance to safety criteria; (2) avoid stationary obstacles and
maintain a safe distance from other users of the airspace; and (3)
compute resolution and recovery maneuvers, autonomously executed by
the autopilot, when safety criteria are violated or about to be
violated.

PolyCARP (Algorithms and Software for Computations with Polygons) is a
package of algorithms, including both their formal models and software
implementations, for computing containment, collision, resolution, and
recovery information for polygons. The intended applications of
PolyCARP are related, but not limited, to safety critical systems in
air traffic management.

Usage Rights
------------

This software is released under the NASA Open Source Agreement,
version 1.3.  The usage agreement is fully described in the file
NASA_Open_Source_Agreement.pdf.  As part of this agreement, you are
requested to email the following information: first and last name;
email address; and affiliation to j.m.maddalon@nasa.gov. This
information will be used for statistical purposes only.

Description of Files
--------------------

README.txt : This file.
src/: Directory of java code. 
lib/: Directory of jar file. 
doc/: Directory of documentation.
DaidalusQuadConfig.txt: DAIDALUS configuration file for
a small rotorcarft.
IcarousExample.java: Example application.
Makefile: Unix make file to compile example application.

Compiling example applications
------------------------------

To compile example application in a Unix environment, type

$ make 

The code is compatible with Java development kit version 1.8.0_45.

Running example applications
----------------------------

To run the example application in a Unix environment, type

$ ./IcarousExample
