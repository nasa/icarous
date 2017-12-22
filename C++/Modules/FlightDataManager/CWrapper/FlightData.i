%module FlightData
%{
/* Includes the header in the wrapper code */
#include "FlightData.h"
%}

/* Parse the header file to generate wrappers */
%include "../FlightData.h"
