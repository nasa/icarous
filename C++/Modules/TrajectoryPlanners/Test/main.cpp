//
// Created by Swee Balachandran on 12/18/17.
//
#include "FlightData.h"
#include "PathPlanner.h"

int main(int argc,char** argv){

    char filename[]="../Test/icarous.txt";
    FlightData fdata(filename);
    PathPlanner planner(&fdata);

    // Create a keep in geofence with 4 vertices
    geofence_t vertex_gf1[4];
    geofence_t vertex_gf2[3];

    for(int i=0;i<4;i++){
        vertex_gf1[i].index = 0;
        vertex_gf1[i].type = KEEP_IN;
        vertex_gf1[i].totalvertices = 4;
        vertex_gf1[i].vertexIndex = i;
        vertex_gf1[i].floor = -100;
        vertex_gf1[i].ceiling = 100;
    }

    for(int i=0;i<3;i++){
        vertex_gf2[i].index = 1;
        vertex_gf2[i].type = KEEP_OUT;
        vertex_gf2[i].totalvertices = 3;
        vertex_gf2[i].vertexIndex = i;
        vertex_gf2[i].floor = -100;
        vertex_gf2[i].ceiling = 100;
    }


    vertex_gf1[0].latitude  = 37.1020599;
    vertex_gf1[0].longitude = -76.3869966;

    vertex_gf1[1].latitude = 37.1022559;
    vertex_gf1[1].longitude = -76.3870096;

    vertex_gf1[2].latitude  = 37.1022474;
    vertex_gf1[2].longitude = -76.3872689;

    vertex_gf1[3].latitude = 37.1020174;
    vertex_gf1[3].longitude = -76.3872664;


    vertex_gf2[0].latitude  = 37.1021970;
    vertex_gf2[0].longitude = -76.3871620;

    vertex_gf2[1].latitude = 37.1021160;
    vertex_gf2[1].longitude = -76.3871900;

    vertex_gf2[2].latitude  = 37.1021340;
    vertex_gf2[2].longitude = -76.3870780;

    // Input the geofence data
    for(int i=0;i<4;i++)
        fdata.InputGeofenceData(&vertex_gf1[i]);

    for(int i=0;i<3;i++)
        fdata.InputGeofenceData(&vertex_gf2[i]);

    double positionA[3] = {37.102177,-76.387206,0};
    double velocityA[3] = {90,1,0};

    double positionB[3] = {37.102185,-76.387065,0};


    Position pos = Position::makeLatLonAlt(positionA[0],"degree",positionA[1],"degree",positionA[2],"m");
    EuclideanProjection projection =  Projection::createProjection(pos);

    int status1 = planner.FindPath(_ASTAR_,"PlanA",positionA,positionB,velocityA);
    int status2 = planner.FindPath(_RRT_,"PlanB",positionA,positionB,velocityA);

    if (status1 > 0)
        planner.OutputFlightPlan(&projection,"PlanA","fence1.txt","waypoints1.txt");
    else
        std::cout<<"Astar algorithm couldn't find solution"<<std::endl;

    if (status2 > 0)
        planner.OutputFlightPlan(&projection,"PlanB","fence2.txt","waypoints2.txt");
    else
        std::cout<<"RRT algorithm couldn't find solution"<<std::endl;

}


