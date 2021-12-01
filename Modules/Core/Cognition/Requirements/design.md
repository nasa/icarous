# Cognition 

## What is it?

The Cognition module is, at its core, a simple mapping between event triggers and handlers. It uses the Event Manager module to define trigger functions and also associate specfic handlers with trigger functions. Using the Event Manager library, Cognition builds an elaborate decision making framework. In the context of Icarous, one of the main roles of Cognition is to detect anomalies/off-nomial conditions and triggers specific resolutions to mitigate those anomalies.  


## How does it work?

- Refer to the Triggers.hpp file for a list of all the defined trigger functions. These triggers outline the various events Cognition is monitoring for.
- Refer to the Handlers.hpp file for a list of available handlers. 
- CoreLogic.cpp defines the mapping between triggers, handlers and their associated priroity values.

## Usage
- Using the examples in the above files, you should be able to construct new trigger functions and associate them with custom handler functions.


## Requirements 

Below is a list of requirements that drive the definition of various triggers and handlers currently in use in Cognition:

### High level 
- Detect conflicts.
- Resolve imminent conflicts.
- Return to mission and resume. 
- Remain passive when no conflicts are detected.

### Type of conflicts monitored.
- Fence violations
- Well clear violations
- Flight plan deviations
- External ditch requests.

### Resolution types
- Vector based resolutions: Heading, Speed and Altitude change by following time varying velocity vectors.
- Vector based resolutions for traffic resolutions are computed based on DAIDALUS guidance.
- Trajectory resolutions: Full path from origin to destination avoiding all obstacles.

## NOTES
- A nominal path has id "Plan0"
- A resolution path has id "Plani" where i > 0. e.g. Plan1,Plan2,... 
- A ditch path has id "DitchPath"
- Although a ditching path is also a resolution, it is treated differently due to integration with safe2ditch.
- After executing a resolution plan, transition back to nominal path (to the next feasible waypoint)
- A feasible waypoint is a waypoint that is reachable.
- There can be multiple resolution paths during the course of a mission.
- The Event Manager framework that is being used only enables attaching a unique handler to a given trigger.
- However, we can define multiple triggers for the same conflict if we want to attach different handlers to the same conflict.
  you may want to do this if you want different behaviors (that can be configured by the user) for a given conflict. 
 


