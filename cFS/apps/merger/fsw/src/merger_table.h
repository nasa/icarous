/**
 * @file merger_table.h
 * @brief Definition of merger table
 *
 * @author: Swee Balachandran
 *
 */

#ifndef ICAROUS_CFS_MERGER_TABLE_H
#define ICAROUS_CFS_MERGER_TABLE_H

/**
 * @defgroup MERGER_TABLES
 * @brief input tables used by the merger monitoring application
 * @ingroup TABLES
 */

/**
 * @struct MergerTable_t
 * @brief Merger table data
 * @ingroup MERGER_TABLES
 */
typedef struct
{
    double maxVehicleSpeed;                        ///< Maximum vehicle speed
    double minVehicleSpeed;                        ///< Minimum vehicle speed
    double corridorWidth;                          ///< Corridor width
    double entryRadius;                            ///< Radius within which maneuvering is essential
    double coordZone;                              ///< Radius within which coordination is essential (data exchange)
    double minSeparationDistance;                  ///< Minimum separation to maintain from other vehicle.
    double minSeparationTime;                      ///< Minimum separation time.
    double maxVehicleTurnRadius;                   ///< Vehicle turn radius
    double IntersectionLocation[5][3];             ///< Intersection locations
}MergerTable_t;


#endif //ICAROUS_CFS_MERGER_TABLE_H
