/**
 * @file guidanceFunctions.h
 * @brief Function declaration, MACRO and data structure definitions for the functions used by the Guidance application
 */

#ifndef GUIDANCE_FUNCTIONS_H
#define GUIDANCE_FUNCTIONS_H

#include <math.h>
#include <string.h>
#include "UtilFunctions.h"
#include "guidance_tbl.h"

/**
 * @struct GuidanceInput_t
 * @brief Structure to hold inputs required for guidance functions
 */
typedef struct{
    double       position[3];           ///< Vehicle current position (lat, lon, alt)
    double       velocity[3];           ///< Vehicle current velocity (vn, ve, vd)
    double       prev_waypoint[3];      ///< Previous waypoint (lat, lon, alt)
    double       curr_waypoint[3];      ///< Current target waypoint (lat, lon, alt)
    double       next_waypoint[3];      ///< Next target waypoint (lat, lon, alt)
    int          num_waypoints;         ///< Total number of waypoints in the flight plan
    int          nextWP;                ///< Next waypoint index
    bool         reachedStatusUpdated;  ///< True if target waypoint has been reached
    double       speed;                 ///< Desired vehicle speed (m/s)
    double       velCmd[3];             ///< Current velocity command
}guidanceInput_t;

/**
 * @struct GuidanceOutput_t
 * @brief Structure to hold outputs from guidance functions
 */
typedef struct{
    double       velCmd[3];             ///< Output velocity command
    int          newNextWP;             ///< Next waypoint index
    bool         reachedStatusUpdated;  ///< True if target waypoint has been reached, otherwise false
}guidanceOutput_t;

/**
 * Compute guidance input for a given flight plan
 */
int ComputeFlightplanGuidanceInput(guidanceInput_t* guidanceInput, guidanceOutput_t* guidanceOutput, guidanceTable_t* guidanceParams);

/**
 * Compute vertical rate needed to climb/descend at a given angle and speed
 */
double ComputeClimbRate(double position[3],double nextWaypoint[3],double speed,guidanceTable_t* guidanceParams);

/**
 * Computes the point on the flight plan to fly towards
 */
void ComputeOffSetPositionOnPlan(guidanceInput_t* guidanceInput, guidanceTable_t* guidanceParams, double outputLLA[]);

/**
 * Compute the correct intersection with the circle and flight plan
 */
void GetCorrectIntersectionPoint(double _wpA[],double _wpB[],double r,double output[]);

/**
 * Point to point control
 */
bool Point2PointControl(double position[3], double target_point[3], double speed, guidanceTable_t* guidanceParams, double velCmd[3]);

/**
 * Compute distance between two points
 */
double distance(double x1,double y1,double x2,double y2);

#endif //GUIDANCE_FUNCTIONS_H
