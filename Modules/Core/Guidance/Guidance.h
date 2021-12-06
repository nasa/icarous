/**
 * @file Guidance.h
 */

#ifndef GUIDANCE_H
#define GUIDANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <Interfaces.h>

/**
 * @enum GuidanceMode
 * @brief various guidance modes
 */
typedef enum{
    FLIGHTPLAN,             ///< Follow a flight plan
    VECTOR,                 ///< Follow the given velocity vector
    POINT2POINT,            ///< Fly to a specific position
    TAKEOFF,                ///< Takeoff mode
    LAND,                   ///< Landing mode
    SPEED_CHANGE,           ///< Change speed
    ALT_CHANGE,             ///< Change alt
    GUIDE_NOOP,
}GuidanceMode;

/**
 * @struct GuidanceParams_t
 * @brief Structure to hold parameters required for guidance
 */
typedef struct{
    double defaultWpSpeed;         ///< default waypoint speed
    double captureRadiusScaling;   ///< capture radius scaling
    double guidanceRadiusScaling;  ///< guidance radius scaling
    double turnRateGain;           ///< turn rate gain
    double climbFpAngle;           ///< climb flight path angle
    double climbAngleVRange;       ///< vertical altitude climb threhsold
    double climbRateGain;          ///< climb rate gain
    double maxClimbRate;           ///< max climb rate limit
    double minClimbRate;           ///< min climb rate limit
    double maxCap;                 ///< max capture radius
    double minCap;                 ///< min capture radius
    double maxSpeed;               ///< max ground speed limit
    double minSpeed;               ///< min ground speed limit
    bool yawForward;               ///< yaw forward 
    bool maintainEta;              ///< maintain ETA constraints
}GuidanceParams_t;

/**
 * @struct GuidanceOutput_t
 * @brief Guidance output object
 * 
 */
typedef struct{
    char activePlan[25]; ///< active plan id
    int guidanceMode;    ///< current guidance mode
    int nextWP;          ///< next waypoint index
    double distH2WP;     ///< horizontal distance to next waypoint
    double distV2WP;     ///< vertical distance to next waypoint
    double xtrackDev;    ///< cross track deviation
    double velCmd[3];    ///< output velocity command
    double target[3];    ///< target position
    bool wpReached;      ///< waypoint arrival status
    bool yawForward;     ///< yaw forward status
}GuidanceOutput_t;

void* InitGuidance(const char config[]);
void guidReadParamFromFile(void* obj,const char config[]);
void guidSetParams(void* obj,GuidanceParams_t* params);
void guidSetAircraftState(void* obj, double position[],double velocity[]);
void guidSetWindData(void* obj,double windFrom,double windSpeed);
void guidInputFlightplanData(void* obj,char planID[],waypoint_t wpts[],int totalWP,double initHeading,bool kinematize,double repairTurnRate);
void RunGuidance(void* obj,double time);
void guidInputVelocityCmd(void* obj,double velcmd[]);
void SetGuidanceMode(void* obj,GuidanceMode mode,const char* planID,int nextWP,bool eta);
void ChangeWaypointSpeed(void* obj,char planID[],int wpID,double val,bool updateAll);
void ChangeWaypointAlt(void* obj,char planID[],int wpID,double val,bool updateAll);
void ChangeWaypointETA(void* obj,char planID[],int wpID,double val,bool updateAll);
void guidGetOutput(void* obj,GuidanceOutput_t* output);
int guidGetWaypoint(void* obj, char planID[],int id, waypoint_t *wp);

#ifdef __cplusplus
}
#endif

#endif
