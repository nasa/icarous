/**
 * @file guidance_tbl.h
 * @brief Guidancetable definition
 */

#ifndef ICAROUS_CFS_GUIDANCE_TBL_H
#define ICAROUS_CFS_GUIDANCE_TBL_H

/**
 * @defgroup GUIDANCE_TABLES
 * @brief Guidancetables
 * @ingroup TABLES
 */

/**
 * @struct guidanceTable_t
 * @brief input table parameters for Guidance application
 * @ingroup GUIDANCE_TABLES
 */
typedef struct
{
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
    bool yawForward;
}guidanceTable_t;


#endif //ICAROUS_CFS_GUIDANCE_TBL_H
