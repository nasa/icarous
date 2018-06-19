%module TrajectoryPlanner
%{
/* Includes the header in the wrapper code */
#include "PathPlanner.h"
%}

/* Parse the header file to generate wrappers */
%include "../PathPlanner.h"
