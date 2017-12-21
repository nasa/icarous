%module GeofenceMonitor
%{
/* Includes the header in the wrapper code */
#include "GeofenceMonitor.h"
%}

/* Parse the header file to generate wrappers */
%include "../../FlightDataManager/FlightData.h"
%include "../GeofenceMonitor.h"
