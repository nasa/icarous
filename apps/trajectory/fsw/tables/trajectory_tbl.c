//
// Created by Swee Balachandran on 11/13/17.
//

#include "cfe.h"
#include "cfe_tbl_filedef.h"  /* Required to obtain the CFE_TBL_FILEDEF macro definition */

#include "trajectory_tbl.h"

TrajectoryTable_t Trajectory_TblStruct = {
  5,                                // obstacle buffer
  50,                               // max ceiling

  false,                            // enable 3D
  1.0,                              // astar grid size (only for grid search)
  2.0,                              // speed used in astar search
  1.0,                              // astar look ahead time for next node
  "../ram/DaidalusQuadConfig.txt",  // daa configuration file for Astar

  1.0,                              // rrt resolution speed
  2000,                             // max rrt iterations
  1.0,                              // Time step used in Range Kutta integration.
  5,                                // Number of Runger Kutta iterations in each rrt iteration.
  5.0,                              // final goal capture radius
  "../ram/DaidalusQuadConfig.txt",  // daa configuration file for RRT

  6.0,                              // Xtrack deviation allowed
  0.6,                              // Xtrack error gain.
  1.0,                              // Resolution speed for maneuvers
  _ASTAR                            // Search algorithm for planning
};


/*
** The macro below identifies:
**    1) the data structure type to use as the table image format
**    2) the name of the table to be placed into the cFE Table File Header
**    3) a brief description of the contents of the file image
**    4) the desired name of the table image binary file that is cFE compatible
*/
CFE_TBL_FILEDEF(Trajectory_TblStruct, TRAJECTORY.TrajectoryTable, Trajectory parameters, trajectory_tbl.tbl )
