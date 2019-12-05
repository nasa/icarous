---
layout: home
title: Home
nav_order: 1
---

<div>
 <p align="center">
 <img src="{{ site.baseurl }}/assets/ICAROUS-logo.jpeg" style="width:45%;">
 </p>
</div>

## A Brief Introduction 

<div style="text-align:justify">
<p>ICAROUS stands for Independent Configurable Architecture for Reliable Operations of Unmanned Systems. It is a software architecture that enables the robust integration of mission specific software modules and highly assured core algorithms for building safety-centric autonomous unmanned aircraft applications. ICAROUS is a research effort that originally started under the auspices of the Unmanned Air Traffic Management (UTM) project. ICAROUS is a research in progress and is constantly evolving with the addition of new functionalities for single and multiple UAS. The primary goal of ICAROUS is to provide autonomy to enable beyond visual line of sight (BVLOS) missions for UAS without the need for constant human supervision/intervention. To this end, ICAROUS uses various tools and algorithms to provide an integrated reasoning, decision making and planning framework to enable numerous UAS applications while ensuring that relevant safety properties are never violated. The development of ICAROUS emphasises heavily on the use of various tools and techniques in formal methods such as model checking and theorem proving.</p>
</div>

<div style="text-align:justify">
<p> Most off the shelf autopilot systems for small UAS come with limited computing capabilities that are suffcient to perform control and navigation functions. However, as UAS applications are rapidly evovling there is a high demand to perform sophisticated decision making such as avoiding other UAS in the airspace, respecting airspace constraints such as geofences and other mission specific tasks. ICAROUS provides an onboard decision making capability to address such complex situations. ICAROUS currently does not provide UAS control functions and depends on an external autopilot that can provide basic control functionalities (e.g. ArduPilot, PX4, Piccolo etc...). ICAROUS runs on a companion computer and communicates with the autopilot to orchestrate resolution maneuves to avoid imminent conflicts.</p> 
</div>

## Software Components 
<div style="text-align:justify">
ICAROUS is implemented in a distributed fashion enabling modularity and facilitating rapid incorporation of new features. The various core functionalities are each distributed across one or more applications. ICAROUS depends on an interprocess communication framework that provides publish/subscribe capabilities to exchange data between applications. The default implementation provided in the repository uses NASA's <a href="https://cfs.gsfc.nasa.gov/">core Flight Systems (cFS)</a> middleware to achieve this distributed architecture. Each ICAROUS functionality is implemented as a cFS application. Such a distributed architecture enables ICAROUS to be easily integrated with any autopilot system by developing a suitable interface application. The default distribution of ICAROUS provides a MAVLink interface application for ArduPilot autopilots.
<br>
<p align="center">
<img src="{{ site.baseurl }}/assets/Architecture.png" style="width:75%;">
<figcaption style="text-align:center;">Distributed and modular architecture of ICAROUS</figcaption>
</p>
</div>

## Core Functionalities
<div style="text-align:justify">
<ul>
 <li> Geofence containment: ICAROUS uses PolyCARP to constantly monitor for imminent keep-in and keep-out geofence violations based on the current position and velocity of the UAS. In each case, ICAROUS prevents the violation by flying to the next feasible waypoint.</li>
<li>Traffic avoidance: Given information about intruder vehicles, ICAROUS detects imminent loss of separation and performs track angle based maneuvers to avoid a well clear violation.</li>
<li>Path planning: ICAROUS currently uses search algorithms to reroute the current flight plan to avoid static (keep-out geofences)/dynamic (intruder traffic) obstacles. ICAROUS currently assumes that intruders move with a constant velocity.</li>
<li>Flight plan conformance: ICAROUS monitors cross track deviation from a given flight plan and maneuvers to maintain deviations within a user defined threshold.</li>
</ul>
</div>

## Extended Functionalities 
<div style="text-align:justify">
The distributed nature of ICAROUS enables addition of various functionalities to ICAROUS. A few examples are mission specific functions such as algorithms for tracking objects, surveillance, inspection etc. Applications can also be developed for multiple UAVs running ICAROUS to coordinate various mission tasks in a distributed manner.
</div>

