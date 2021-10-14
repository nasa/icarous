#ifndef GUIDANCE_H
#define GUIDANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <Interfaces.h>

/**
 * @enum guidance_mode_e
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
    double defaultWpSpeed;
    double captureRadiusScaling;
    double guidanceRadiusScaling;
    double turnRateGain;
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
    bool maintainEta;
}GuidanceParams_t;

typedef struct{
    char activePlan[25];
    int guidanceMode;
    int nextWP;
    double distH2WP;
    double distV2WP;
    double xtrackDev;
    double velCmd[3];
    double target[3];
    bool wpReached;
}GuidanceOutput_t;

void* InitGuidance(char config[]);
void guidReadParamFromFile(void* obj,char config[]);
void guidSetParams(void* obj,GuidanceParams_t* params);
void guidSetAircraftState(void* obj, double position[],double velocity[]);
void guidSetWindData(void* obj,double windFrom,double windSpeed);
void guidInputFlightplanData(void* obj,char planID[],waypoint_t wpts[],int totalWP,double initHeading,bool kinematize,double repairTurnRate);
void RunGuidance(void* obj,double time);
void guidInputVelocityCmd(void* obj,double velcmd[]);
void SetGuidanceMode(void* obj,GuidanceMode mode,char* planID,int nextWP,bool eta);
void ChangeWaypointSpeed(void* obj,char planID[],int wpID,double val,bool updateAll);
void ChangeWaypointAlt(void* obj,char planID[],int wpID,double val,bool updateAll);
void ChangeWaypointETA(void* obj,char planID[],int wpID,double val,bool updateAll);
void guidGetOutput(void* obj,GuidanceOutput_t* output);
int guidGetWaypoint(void* obj, char planID[],int id, waypoint_t *wp);

#ifdef __cplusplus
}
#endif

#endif