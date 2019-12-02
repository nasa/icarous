
#ifndef CUSTOM_DATASTRUCTURE_H
#define CUSTOM_DATASTRUCTURE_H

#include "Icarous_msg.h"
#include "traffic_msg.h"
#include "trajectory_msg.h"

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
    bool trafficTrackConflict;
    bool trafficSpeedConflict;
    bool trafficAltConflict;
    bool flightPlanConflict;
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
    double preferredSpeed;
    double preferredAlt;
    bands_t trkBands;
    bands_t gsBands;
    bands_t altBands;

    double ditchsite[3];
    bool ditch;
    bool resetDitch;
    bool endDitch;
    bool ditchGuidanceRequired;
    bool ditchRequested;
    bool restartMission;
    traffic_parameters_t trafficparameters;
}plexilCustomData_t;

plexilCustomData_t plexilCustomData;

#endif //CUSTOM_DATASTRUCTURE_H
