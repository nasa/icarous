/**
 * @file traffic_tbl.h
 * @brief traffic table definition
 */

#ifndef ICAROUS_CFS_TRAFFIC_TBL_H
#define ICAROUS_CFS_TRAFFIC_TBL_H

#include <stdbool.h>

/**
 * @struct TrafficTable_t
 * @brief input table parameters for traffic application
 */
typedef struct
{
    char configFile[50];   ///< Daidalus configuration file
    bool log;              ///< Enable Daidalus log output
    bool updateParams;     ///< Enable parameter updates through SB messages
}TrafficTable_t;


#endif //ICAROUS_CFS_TRAFFIC_TBL_H
