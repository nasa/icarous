
#ifndef CUSTOM_DATASTRUCTURE_H
#define CUSTOM_DATASTRUCTURE_H

#include "msgdef/ardupilot_msg.h"
#include "msgdef/traffic_msg.h"
#include "msgdef/trajectory_msg.h"

typedef struct{
    double position[3];
    double velocity[3];
    int missionStart;
    int armStatus;
    int takeoffStatus;
    int nextMissionWP;
    int totalMissionWP;
    bool keepInConflict;
    bool keepOutConflict;
    double recoveryPosition[3];
    bool trafficConflict;
    bool flightPlanConflict;
    bool ditch;
    double allowedXtrackError;
    double xtrackError;
    double resolutionSpeed;
    algorithm_e searchType;
    int totalFences;
    int totalTraffic;
    int nextFeasibleWP;
    bool directPathToFeasibleWP;
    flightplan_t resolutionTraj;
    flightplan_t missionFP;
    PlexilMsg pendingRequest;
    double interceptManeuver[3];
    double interceptHeadingToPlan;
    int trafficResType;
    double preferredTrack;
    bands_t trkBands;
}plexilCustomData_t;

plexilCustomData_t plexilCustomData;

#endif //CUSTOM_DATASTRUCTURE_H
