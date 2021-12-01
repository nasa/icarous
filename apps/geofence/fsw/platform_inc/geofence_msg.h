/**
 * @file geofence_msg.h
 * @brief Defintion of messages used by geofence_msg.h
 */

#ifndef ICAROUS_CFS_GEOFENCE_MSG_H
#define ICAROUS_CFS_GEOFENCE_MSG_H

#include "cfe.h"


/**
 * @struct pathFeasibilityCheck_t
 * @brief request to check if a path is feasible between two given points. Also used as a response.
 *
 * Requests are published under the GEOFENCE_PATH_CHECK_MID topic.
 * Responses are published under the GEOFENCE_PATH_CHECK_RESULT_MID topic.
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
    double fromPosition[3];                  /**< Initial position in lat (deg), lon (deg) and alt (m) */
    double toPosition[3];                    /**< To position in lat (deg), lon (deg) and alt (m) */
    bool feasibleA;                          /**< true if from position is free from conflict */
    bool feasibleB;                          /**< true if to position is free from conflict */
    bool feasibleAB;                         /**< true if path is feasible (free from geofence conflicts) */
}pathFeasibilityCheck_t;

/**
 * @struct geofence_parameters_t
 * @brief data structure containing information about the parameters used by the geofence app
 */
typedef struct{
   uint8_t  TlmHeader[CFE_SB_TLM_HDR_SIZE];
   double lookahead;
   double hthreshold;
   double vthreshold;
   double hstepback;
   double vstepback;
}geofence_parameters_t;

#endif //ICAROUS_CFS_GEOFENCE_MSG_H
