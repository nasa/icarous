/**
 * @file guidanceFunctions.h
 * @brief Function declaration, MACRO and data structure definitions for the functions used by the Guidance application
 */

#ifndef GUIDANCE_FUNCTIONS_H
#define GUIDANCE_FUNCTIONS_H


#define _GNU_SOURCE

#include <math.h>
#include <string.h>
#include <time.h>
#include "UtilFunctions.h"

/**
 * @struct GuidanceParams_t
 * @brief Structure to hold parameters required for guidance
 */
typedef struct{
    double defaultWpSpeed;
    double captureRadiusScaling;
    double guidanceRadiusScaling;
    double xtrkDev;
    double climbFpAngle;
    double climbAngleVRange;
    double climbAngleHRange;
    double climbRateGain;
    double maxClimbRate;
    double minClimbRate;
    double maxCap;
    double minCap;
    double maxSpeed;
    double minSpeed;
    bool yawForward;
}guidanceParams_t;

/**
 * @struct GuidanceInput_t
 * @brief Structure to hold inputs required for guidance functions
 */
typedef struct{
    double       position[3];           ///< Vehicle current position (lat, lon, alt)
    double       velocity[3];           ///< Vehicle current velocity (vn, ve, vd)
    double       prev_waypoint[5];      ///< Previous waypoint (lat, lon, alt, 0/1, speed(0)/time(1))
    double       curr_waypoint[5];      ///< Current target waypoint (lat, lon, alt, 0/1, speed(0)/time(1))
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
int ComputeFlightplanGuidanceInput(guidanceInput_t* guidanceInput, guidanceOutput_t* guidanceOutput, guidanceParams_t* guidanceParams);

/**
 * Compute vertical rate needed to climb/descend at a given angle and speed
 */
double ComputeClimbRate(double position[3],double nextWaypoint[3],double speed,guidanceParams_t* guidanceParams);

/**
 * Computes the point on the flight plan to fly towards
 */
void ComputeOffSetPositionOnPlan(double speedRef,guidanceInput_t* guidanceInput, guidanceParams_t* guidanceParams, double outputLLA[]);

/**
 * Compute the correct intersection with the circle and flight plan
 */
void GetCorrectIntersectionPoint(double _wpA[],double _wpB[],double r,double output[]);

/**
 * Point to point control
 */
bool Point2PointControl(double position[3], double target_point[3], double speed, guidanceParams_t* guidanceParams, double velCmd[3]);

/**
 * Compute distance between two points
 */
double distance(double x1,double y1,double x2,double y2);

/*
 * Compute speed to the next WP
 */
double ComputeSpeed(double currPosition[5],double nextWP[5],double currSpeed,guidanceParams_t* guidanceParams);

#endif //GUIDANCE_FUNCTIONS_H
