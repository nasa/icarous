//
// Created by Swee Balachandran on 12/14/17.
//

#ifndef PATHPLANNER_DYNAMICS_H
#define PATHPLANNER_DYNAMICS_H

#include "RRTplanner.hpp"

void RRT_F(double X[], double U[],double Y[],int trafficSize);

void RRT_U(node_t& nn, node_t& qn,double U[]);


#endif //PATHPLANNER_DYNAMICS_H
