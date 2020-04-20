/**
 * @file Guidance_msg.h
 */
#ifndef _GUIDANCE_MSG_H_
#define _GUIDANCE_MSG_H_

/**
 * @enum guidance_mode_e
 * @brief various guidance modes
 */
typedef enum{
    PRIMARY_FLIGHTPLAN,     ///< Follow the primary mission flight plan
    SECONDARY_FLIGHTPLAN,   ///< Follow the secondary flight plan (for detours/reroutes)
    VECTOR,                 ///< Follow the given velocity vector
    POINT2POINT,            ///< Fly to a specific position
    ORBIT,                  ///< Orbit around a given point
    HELIX,                  ///< A helical orbit
    TAKEOFF,                ///< Takeoff mode
    LAND,                   ///< Landing mode
    SPEED_CHANGE,           ///< Speed change command
    NOOP                    ///< No operations
}guidance_mode_e;

typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
    guidance_mode_e mode;                    /**< Guidance mode currently being used */
    uint32_t totalWP;                        /**< Total number of waypoints in flightplan */
    uint32_t nextWP;                         /**< Next WP - Same as current WP vehicles is flying towards */
    double dist2nextWP;                      /**< Distance to next WP */
    double crossTrackDeviation;              /**< Cross track deviation */
    double velCmd[3];                        /**< Velocity command currently being used */
}guidance_status_t;

/**
 * @struct guidance_parameters_t
 * @brief parameters used by the guidance application
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];
    double defaultWpSpeed;        /**< Default waypoint to waypoint speed (m/s) */
    double captureRadiusScaling;  /**< Scaling factor to compute capture radius to determine if a wp has been reached */
    double guidanceRadiusScaling; /**< Scaling factor to compute guidance radius used for waypoint navigation */
    double xtrkDev;               /**< Max deviation from flight plan to use guidance radius navigation method (m)*/
    double climbAngle;            /**< Desired angle to climb/descend at (deg) */
    double climbAngleVRange;      /**< Use proportional control when within climbAngleVRange of target altitude (m) */
    double climbAngleHRange;      /**< Use proportional control when within climbAngleHRange of target wp (m) */
    double climbRateGain;         /**< Gain used for proportional altitude control*/
    double maxClimbRate;          /**< Maximum allowed climb rate (m/s) */
    double minClimbRate;          /**< Minimum allowed climb rate (maximum descend rate) (m/s) */
    double maxCap;                /**< Maximum allowed capture radius */
    double minCap;                /**< Minimum allowed for capture radius */    
    double maxSpeed;              /**< Maximum vehicle speed */
    double minSpeed;              /**< Minimum vehicle speed */
    bool yawForward;             /**< When true, yaw the vehicle so that it points in the direction of travel */
}guidance_parameters_t;

#endif