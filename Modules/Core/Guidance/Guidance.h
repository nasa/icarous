#ifndef GUIDANCE_H
#define GUIDANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

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
}GuidanceParams_t;

typedef struct{
    char activePlan[25];
    int guidanceMode;
    int nextWP;
    double distH2WP;
    double distV2WP;
    double xtrackDev;
    double velCmd[3];
    bool wpReached;
}GuidanceOutput_t;

void* InitGuidance(GuidanceParams_t* params);
void guidSetParams(void* obj,GuidanceParams_t* params);
void guidSetAircraftState(void* obj, double position[],double velocity[]);
void guidInputFlightplanData(void* obj, char planID[], double scenarioTIme, 
                         int wpID, double position[], bool eta, double value);
void RunGuidance(void* obj,double time);
void guidInputVelocityCmd(void* obj,double velcmd[]);
void SetGuidanceMode(void* obj,GuidanceMode mode,char* planID,int nextWP);
void ChangeWaypointSpeed(void* obj,char planID[],int wpID,double val,bool updateAll);
void ChangeWaypointETA(void* obj,char planID[],int wpID,double val,bool updateAll);
void guidGetOutput(void* obj,GuidanceOutput_t* output);

#ifdef __cplusplus
}
#endif

#endif