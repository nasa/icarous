#ifndef INTERFACES_H
#define INTERFACES_H

#include <stdint.h>
#include <stdbool.h>

typedef struct __attribute__((__packed__)){
    double time;                             /**< band time */
    int numConflictTraffic;                  /**< total number of conflict traffic */
    int numBands;                            /**< total number of bands (max:5)*/
    int type[20];                            /**< type of each band @see Region */
    double min[20];                          /**< min values of each band */
    double max[20];                          /**< max values of each band */
    int recovery;                            /**< 1 if recovery band is available */
    int currentConflictBand;                 /**< current ownship conflict band */
    double timeToViolation;                  /**< time to violation */
    double timeToRecovery;                   /**< time to recovery */
    double minHDist;                         /**< min horizontal distance */
    double minVDist;                         /**< min vertical distance */
    double resUp;                            /**< resolution up */
    double resDown;                          /**< resolution down */
    double resPreferred;                     /**< preferred resolution */
    bool wpFeasibility1[50];                 /**< feasibility to waypoints in flightplan 1 */
    bool wpFeasibility2[50];                 /**< feasibility to waypoints in flightplan 2 */
    bool fp1ClosestPointFeasible;            /**< feasibility of nearest point on flightplan 1 */
    bool fp2ClosestPointFeasible;            /**< feasibility of nearest point on flightplan 2 */
}bands_t;

typedef struct __attribute__((__packed__)){
    uint8_t numConflicts;                    /**< number of conflicts */
    uint16_t numFences;                      /**< total fences */
    uint8_t conflictFenceIDs[5];             /**< conflicting fence ids */
    uint8_t conflictTypes[5];                /**< type of conflict (keep in: 0/ keep out:1) */
    double timeToViolation[5];               /**< time to violating constraint at current speed */
    double recoveryPosition[3];              /**< recovery position */
    bool waypointConflict1[50];              /**< 0 if waypoint is conflict free. 1 if waypoint conflict due to geofence */
    bool directPathToWaypoint1[50];          /**< 1 if direct path exists, 0 if no direct path exist */
    bool waypointConflict2[50];              /**< 0 if waypoint is conflict free. 1 if waypoint conflict due to geofence */
    bool directPathToWaypoint2[50];          /**< 1 if direct path exists, 0 if no direct path exist */
}geofenceConflict_t;

typedef struct{
    int resolutionType;             // Type of resolutions to use, one of resolutionType_e
    double DTHR;                    // Well clear radius threshold (ft)
    double ZTHR;                    // Well clear vertical distance threshold (ft)
    int searchType;                 // Search algorithm to use
    double resolutionSpeed;         // Speed to use during resolutions
    double allowedXtrackDeviation;  // Allowed deviation from flight plan (m)
    double XtrackGain;
}cognition_params_t;


#endif