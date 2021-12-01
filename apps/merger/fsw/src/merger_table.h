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
    double scheduleZone;                           ///< Radius within which scheduling takes place
    double minSeparationDistance;                  ///< Minimum separation to maintain from other vehicle.
    double minSeparationTime;                      ///< Minimum separation time.
    double maxVehicleTurnRadius;                   ///< Vehicle turn radius
    double IntersectionLocation[20][3];            ///< Intersection locations
    uint32_t IntersectionID[20];                   ///< Intersection IDs
    int    startIntersection;                      ///< Starting intersection
    double missionSpeed;                           ///< Default mission speed
}MergerTable_t;


#endif //ICAROUS_CFS_MERGER_TABLE_H
