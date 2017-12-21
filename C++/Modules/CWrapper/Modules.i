%module TrafficMonitor
%{
/* Includes the header in the wrapper code */
#include "FlightDataManager.h"
#include "GeofenceMonitor.h"
#include "TrafficMonitor.h"
#include "TrajectoryPlanners.h"
%}

/* Parse the header file to generate wrappers */
%include "../FlightDataManager/FlightDataManager.h"
%include "../GeofenceMonitor/GeofenceMonitor.h"
%include "../TrafficMonitor/TrafficMonitor.h"
%include "../TrajectoryPlanner/TrajectoryPlanner.h"

