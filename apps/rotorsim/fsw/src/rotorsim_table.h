/**
 * @file rotorsim_table.h
 * @brief Definition of rotorcraft simulation table
 *
 * @author: Swee Balachandran
 *
 */

#ifndef ICAROUS_CFS_ROTORSIM_TABLE_H
#define ICAROUS_CFS_ROTORSIM_TABLE_H

/**
 * @struct RotorsimTable_t
 * @brief Rotorsim table data
 * @ingroup ROTORSIM_TABLES
 */

#define NUM_SIM_WP 5

typedef struct
{
    double missionSpeed;              /**< Reference speed for the mission */
    double originLL[2];               /**< origin lat,lon */
    double initialAlt;                /**< Initial altitude */
    int numWP;                        /**< Total number of waypoints (max: NUM_SIM_WP) */
    double flightPlan[NUM_SIM_WP][3]; /**< Entry flightplan */
}RotorsimTable_t;


#endif //ICAROUS_CFS_PLEXIL_TABLE_H
