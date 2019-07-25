/**
 * @file traffic_tbl.h
 * @brief traffic table definition
 */

#ifndef ICAROUS_CFS_TRAFFIC_TBL_H
#define ICAROUS_CFS_TRAFFIC_TBL_H

#include <stdbool.h>

/**
 * @defgroup TRAFFIC_TABLES
 * @brief traffic tables
 * @ingroup TABLES
 */

/**
 * @struct TrafficTable_t
 * @brief input table parameters for traffic application
 * @ingroup TRAFFIC_TABLES
 */
typedef struct
{
    char configFile[50];    ///< Daidalus configuration file
    bool log;               ///< Enable Daidalus log output
    uint32_t trafficSource; ///< Traffic source (ADSB, SIM, RADAR)

}TrafficTable_t;


#endif //ICAROUS_CFS_TRAFFIC_TBL_H
