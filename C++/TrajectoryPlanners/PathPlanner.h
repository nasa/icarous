//
// Created by Swee Balachandran on 12/14/17.
//

#ifndef PATHPLANNER_H
#define PATHPLANNER_H

#include "ParameterData.h"
#include "FlightData.h"

typedef enum algType{
    _ASTAR_,_RRT_
}algorithm;

using namespace larcfm;

class PathPlanner{

private:

    FlightData* fdata;

public:
    PathPlanner(std::string filename);
    void FindPath(algorithm, double flatitude,double flongitude,double faltitude,
                  double tlatitude, double tlongitude,double taltitude,
                  double trk,double gs,double vs);

    void FindPathAstar(double flatitude,double flongitude,double faltitude,
                        double tlatitude, double tlongitude,double taltitude);
    void FindPathRRT(double flatitude,double flongitude,double faltitude,
                         double tlatitude, double tlongitude,double taltitude,
                     double trk,double gs,double vs);

    Plan ComputeGoAbovePlan(Position start,Position goal,double altFence,double rSpeed);



};


#endif //PATHPLANNER_H
