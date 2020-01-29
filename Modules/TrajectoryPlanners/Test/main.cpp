//
// Created by Swee Balachandran on 12/18/17.
//
#include "PathPlanner.h"

int main(int argc,char** argv){


    PathPlanner planner(2,30);
    char filename[] = "../Test/DaidalusQuadConfig.txt";
    planner.InitializeAstarParameters(false,2,1,1,filename);
    planner.InitializeRRTParameters(1.0,2000,1,5,5,filename);

    double pos1[3] = {37.1021769,-76.3872069,5};
    double pos2[3] = {37.102192,-76.386940,5};
    double pos3[3] = {37.102066,-76.387047,5.000000};
    double pos4[3] = {37.101978,-76.387115,5.000000};

    char input_name[] = "Plan0";
    planner.InputFlightPlan(input_name,0,pos1,1);
    planner.InputFlightPlan(input_name,1,pos2,1);
    planner.InputFlightPlan(input_name,2,pos3,1);
    planner.InputFlightPlan(input_name,3,pos4,1);
    planner.InputFlightPlan(input_name,4,pos1,1);

    // Create a keep in geofence with 4 vertices
    double vertex_gf1[4][2];
    double vertex_gf2[4][2];

    vertex_gf1[0][0]= 37.102739;
    vertex_gf1[0][1]= -76.387493;

    vertex_gf1[1][0]= 37.102611;
    vertex_gf1[1][1]= -76.386055;

    vertex_gf1[2][0]= 37.103578;
    vertex_gf1[2][1]= -76.386039;

    vertex_gf1[3][0]= 37.103631;
    vertex_gf1[3][1]= -76.387589;


    vertex_gf2[0][0]= 37.103040;
    vertex_gf2[0][1]= -76.386965;

    vertex_gf2[1][0]= 37.102992;
    vertex_gf2[1][1]= -76.386503;

    vertex_gf2[2][0]= 37.103222;
    vertex_gf2[2][1]= -76.386475;

    vertex_gf2[3][0]= 37.103293;
    vertex_gf2[3][1]= -76.386946;


    planner.InputGeofenceData(KEEP_IN,0,4,-100,100,vertex_gf1);
    planner.InputGeofenceData(KEEP_OUT,1,4,-100,100,vertex_gf2);


    double positionA[3] = {37.102865,-76.387257,0};
    double velocityA[3] = {90,1,0};

    double positionB[3] = {37.103522,-76.386211,0};

    Position pos = Position::makeLatLonAlt(positionA[0],(char*)"degree",positionA[1],(char*)"degree",positionA[2],"m");
    EuclideanProjection projection =  Projection::createProjection(pos);

    int status1 = planner.FindPath(_GRID_,(char*)"PlanA",positionA,positionB,velocityA);

    if (status1 > 0)
        planner.OutputFlightPlan(&projection,(char*)"PlanA",(char*)"fence1.txt",(char*)"waypoints1.txt");
    else
        std::cout<<"Grid Astar algorithm couldn't find solution"<<std::endl;

    int status2 = planner.FindPath(_ASTAR_,(char*)"PlanB",positionA,positionB,velocityA);

    if (status2 > 0)
        planner.OutputFlightPlan(&projection,(char*)"PlanB",(char*)"fence2.txt",(char*)"waypoints2.txt");
    else
        std::cout<<"Astar algorithm couldn't find solution"<<std::endl;

    int status3 = planner.FindPath(_RRT_,(char*)"PlanC",positionA,positionB,velocityA);

    if (status3 > 0)
        planner.OutputFlightPlan(&projection,(char*)"PlanC",(char*)"fence3.txt",(char*)"waypoints3.txt");
    else
        std::cout<<"RRT algorithm couldn't find solution"<<std::endl;


    int status4 = planner.FindPath(_SPLINES_,(char*)"PlanD",positionA,positionB,velocityA);

    if (status4 > 0)
        planner.OutputFlightPlan(&projection,(char*)"PlanD",(char*)"fence4.txt",(char*)"waypoints4.txt");
    else
        std::cout<<"SPLINES algorithm couldn't find solution"<<std::endl;


}


