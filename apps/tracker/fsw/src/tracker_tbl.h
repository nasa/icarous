/**
 * @file tracker_tbl.h
 * @brief trackertable definition
 */

#ifndef ICAROUS_CFS_TRACKER_TBL_H
#define ICAROUS_CFS_TRACKER_TBL_H

/**
 * @defgroup TRACKER_TABLES
 * @brief trackertables
 * @ingroup TABLES
 */

/**
 * @struct trackerTable_t
 * @brief input table parameters for tracker application
 * @ingroup TRACKER_TABLES
 */
typedef struct
{
    double modelUncertaintyP[6];
    double modelUncertaintyV[6];
    double pThreshold;
    double vThreshold;
}trackerTable_t;


#endif //ICAROUS_CFS_TRACKER_TBL_H
