/**
 * @file geofence_table.h
 * @brief Definition of geofence table
 *
 * @author: Swee Balachandran
 *
 */

#ifndef ICAROUS_CFS_GEOFENCE_TABLE_H
#define ICAROUS_CFS_GEOFENCE_TABLE_H

/**
 * @struct GeofenceTable_t
 * @brief Geofence table data
 * @ingroup GEOFENCE_TABLES
 */
typedef struct
{
    double lookahead;     /**< look ahead horizon */
    double hbuffer;       /**< polygon expanded horizontally by this buffer */
    double vbuffer;       /**< polygon expanded vertically by this much */
    double hstepback;     /**< step back horizontally resolution */
    double vstepback;     /**< step back vertically for resolution */
}GeofenceTable_t;


#endif //ICAROUS_CFS_PLEXIL_TABLE_H
