#ifndef INTERFACES_H
#define INTERFACES_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_FIX_NAME_SIZE 20

/**
 * @enum wp_metric_e
 * @brief waypoint metrix
 */
typedef enum {
    TCP_NONE      = 0, ///< None
    TCP_BOT       = 1, ///< Beginnning of turn
    TCP_EOT       = 2, ///< End of vertical turn
    TCP_MOT       = 4, ///< Middle of turn
    TCP_EOTBOT    = 5, ///< End/Beginning of turn
    TCP_NONEg     = 6, ///< None
    TCP_BGS       = 7, ///< Beginning of ground speed change
    TCP_EGS       = 8, ///< End of ground speed change
    TCP_EGSBGS    = 9, ///< End/Beginning of speed change
    TCP_NONEv     = 10,///< None
    TCP_BVS       = 11,///< Beginning of vertical speed change
    TCP_EVS       = 12,///< Beginning of vertical speed change
    TCP_EVSBVS    = 13,///< End/Beginning of vertical speed change
}tcp_e;

typedef enum{
    BANDREGION_UNKNOWN  = 0,
    BANDREGION_NONE     = 1,
    BANDREGION_FAR      = 2,
    BANDREGION_MID      = 3,
    BANDREGION_NEAR     = 4,
    BANDREGION_RECOVERY = 5
}bandRegion_e;

/**
 * @enum objectType_e
 * @brief object type
 */
typedef enum{
    _TRAFFIC_SIM_,   ///< traffic data from simulation
    _TRAFFIC_ADSB_,  ///< traffic data from ADSB
    _TRAFFIC_RADAR_, ///< traffic data from radar
    _TRAFFIC_FLARM_, ///< traffic data from flarm
    _OBSTACLE_       ///< obstacle
}objectType_e;



/**
 * @struct waypoint_t
 * @brief waypoint data.
 */
typedef struct __attribute__((__packed__))
{
    uint16_t  index;                        /**< waypoint index */
    double  time;                           /**< ETA at this waypoint */
    char    name[MAX_FIX_NAME_SIZE];        /**< waypoint name */
    double  latitude; 					    /**< latitude in degrees */
    double  longitude; 					    /**< longitude in degrees */
    double  altitude;                       /**< altitude Meters */
    tcp_e tcp[3];                           /**< see @see wp_metric_e */
    double  tcpValue[3];                    /**< wp_metric value applicable at this waypoint */
    char info[100];                         /**< info field */
}waypoint_t;

typedef struct __attribute__((__packed__)){
    double time;                             /**< band time */
    int numConflictTraffic;                  /**< total number of conflict traffic */
    int numBands;                            /**< total number of bands (max:5)*/
    int type[20];                            /**< type of each band @see Region */
    double min[20];                          /**< min values of each band */
    double max[20];                          /**< max values of each band */
    int recovery;                            /**< 1 if recovery band is available */
    int currentConflictBand;                 /**< current ownship conflict band */
    double timeToViolation[2];               /**< time to violation */
    double timeToRecovery;                   /**< time to recovery */
    double minHDist;                         /**< min horizontal distance */
    double minVDist;                         /**< min vertical distance */
    double resUp;                            /**< resolution up */
    double resDown;                          /**< resolution down */
    double resPreferred;                     /**< preferred resolution */
}bands_t;

typedef struct __attribute__((__packed__)){
    uint8_t numConflicts;                    /**< number of conflicts */
    uint16_t numFences;                      /**< total fences */
    uint8_t conflictFenceIDs[5];             /**< conflicting fence ids */
    uint8_t conflictTypes[5];                /**< type of conflict (keep in: 0/ keep out:1) */
    double timeToViolation[5];               /**< time to violating constraint at current speed */
    double recoveryPosition[3];              /**< recovery position */
}geofenceConflict_t;

typedef struct __attribute__((__packed__)){
    bool fenceConflict;                      /**< fence conflicts */
    bool trafficConflict;                    /**< traffic conflict */ 
    bool lineOfSight2goal;                   /**< line of sight to goal is avaialble */
    uint8_t conflictFenceID;                 /**< conflicting fence ids */
    char conflictCallSign[20];               /**< callsign of conflicting traffic */
    double timeToTrafficViolation;           /**< time to violating constraint at current speed */
    double timeToFenceViolation;             /**< time to violating constraint at current speed */
    double recoveryPosition[3];              /**< recovery position */
    double offsets1[3];                      /**< perp, striaght and time offsets with respect to nextWP on primary plan */
    double offsets2[3];                      /**< perp, striaght and time offsets with respect to nextWP on current plan */
    int nextWP;                              /**< current nextWP */
    int nextFeasibleWP;                      /**< next feasible waypoint */
}trajectoryMonitorData_t;


typedef struct{
    int resolutionType;             // Type of resolutions to use, one of resolutionType_e
    double DTHR;                    // Well clear radius threshold (ft)
    double ZTHR;                    // Well clear vertical distance threshold (ft)
    double allowedXtrackDeviation;  // Allowed deviation from flight plan (m)
    double planLookaheadTime;       // lookahead time for traffic and fence conflicts
    double persistenceTime;         // traffic conflict persistence time
    bool return2NextWP;             // Return to path behavior. 0 return to nearest point. 1. next WP
    bool active;                    // Make cognition active                   
    bool returnVector;              // Use vectors to command the vehicle to return to mission
    bool verifyPlanConflict;        // Verify if imminent conflict is a real conflicty by comparing it with current plan
    int PriorityTakeoff;
    int PrioirtyNominalDeparture;
    int PriorityPrimaryPlanComplete;
    int PrioritySecondaryPlanComplete;
    int PriorityReplanning;
    int PriorityMerging;
    int PriorityFenceConflict;
    int PriorityTrafficConflict1;
    int PriorityTrafficConflict2;
    int PriorityTrafficConflict3;
    int PriorityFlightPlanDeviation;
    int PriorityDitching;
    int PriorityTODReached;
}cognition_params_t;

#endif