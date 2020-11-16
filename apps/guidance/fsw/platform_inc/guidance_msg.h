/**
 * @file Guidance_msg.h
 */
#ifndef _GUIDANCE_MSG_H_
#define _GUIDANCE_MSG_H_

/**
 * @struct guidance_parameters_t
 * @brief parameters used by the guidance application
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];
    double defaultWpSpeed;        /**< Default waypoint to waypoint speed (m/s) */
    double captureRadiusScaling;  /**< Scaling factor to compute capture radius to determine if a wp has been reached */
    double guidanceRadiusScaling; /**< Scaling factor to compute guidance radius used for waypoint navigation */
    double turnRateGain;          /**< Max deviation from flight plan to use guidance radius navigation method (m)*/
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