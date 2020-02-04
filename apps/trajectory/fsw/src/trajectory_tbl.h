/**
 * @file trajectory_tbl.h
 * @brief trajectory table definition
 */
#ifndef ICAROUS_CFS_TRAJECTORY_TBL_H
#define ICAROUS_CFS_TRAJECTORY_TBL_H

#include <stdbool.h>
#include "trajectory_msg.h"

/**
 * @defgroup TRAJECTORY_TABLE
 * @brief Trajectory table
 * @ingroup TABLES
 * @{
 */

/**
 * @struct TrajectoryTable_t
 * @brief Trajectory table defintion
 */
typedef struct
{
    double obsbuffer;                    ///< buffer [m] added to obstacle (obstacle sides are expanded by this much)
    double maxCeiling;                   ///< Max ceiling for flight [m]

    // Astar parameters
    bool   astar_enable3D;               ///< Enable 3D search for A star algorithm (not used in grid search)
    double astar_gridSize;               ///< A star grid size for grid search (m)
    double astar_resSpeed;               ///< Ownship speed used for A star search (m/s)
    double astar_lookahead;              ///< Lookahead time in seconds (s)
    char astar_daaConfigFile[50];        ///< DAA configuration file for A star serach

    // RRT parameters
    double rrt_resSpeed;                 ///< Ownship speed used for RRT search
    int rrt_numIterations;               ///< Total number of RRT iterations
    double rrt_dt;                       ///< Time step values in (s) used in one Range Kutta integration.
    int rrt_macroSteps;                  ///< Total number of Runga Kutta integration steps in one RRT iteration.
    double rrt_capR;                     ///< Capture circle radius (s) for final goal
    char rrt_daaConfigFile[50];          ///< Input DAA configuration file for RRT search.

    // Flight plan monitoring parameters
    double xtrkDev;                      ///< Allowed cross track deviation.
    double xtrkGain;                     ///< Proportial gain used for cross track deviation maneuver computaion.
    double resSpeed;                     ///< Resolution speed when returning to flight plan.

    // Preferred search algorithm to use
    algorithm_e searchAlgorithm;         ///< Algorithm used for computing a new path.
    bool updateDaaParams;                ///< Update DAA params via SB messages
}TrajectoryTable_t;

/**@}*/
#endif //ICAROUS_CFS_TRAJECTORY_TBL_H
