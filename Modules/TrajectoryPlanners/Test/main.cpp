//
// Created by Swee Balachandran on 12/18/17.
//
#include "PathPlanner.h"
#include "PlanWriter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>



int main(int argc,char** argv){

    PathPlanner planner(2,30);
    planner.InputDataFromLog("flyer3.log"); 
    
    double positionA[3] = {planner.startPos.latitude(),
                           planner.startPos.longitude(),
                           planner.startPos.alt()};
    double velocityA[3] = {planner.startVel.track("degree"),
                           planner.startVel.groundSpeed("m/s"),
                           planner.startVel.verticalSpeed("m/s")};

    double positionB[3] = {planner.endPos.latitude(),
                           planner.endPos.longitude(),
                           planner.endPos.alt()};

    Position pos = Position::makeLatLonAlt(positionA[0],(char*)"degree",positionA[1],(char*)"degree",positionA[2],"m");
    EuclideanProjection projection =  Projection::createProjection(pos);

    if(planner.search == _ASTAR_){
        int status2 = planner.FindPath(_ASTAR_, (char *)"PlanB", positionA, positionB, velocityA);

        if (status2 <= 0)
            std::cout << "Astar algorithm couldn't find solution" << std::endl;
    }

    if (planner.search == _RRT_)
    {
        int status3 = planner.FindPath(_RRT_, (char *)"PlanC", positionA, positionB, velocityA);

        if (status3 <= 0)
            std::cout << "RRT algorithm couldn't find solution" << std::endl;
    }
}


