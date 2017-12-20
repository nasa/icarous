%module TrafficMonitor
%{
/* Includes the header in the wrapper code */
#include "TrafficMonitor.h"
%}

/* Parse the header file to generate wrappers */
%include "../TrafficMonitor.h"