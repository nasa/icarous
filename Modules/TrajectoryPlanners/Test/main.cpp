//
// Created by Swee Balachandran on 12/18/17.
//
#include "FlightData.h"
#include "PathPlanner.h"

int main(int argc,char** argv){

    char filename[]="../Test/icarous.txt";
    FlightData fdata(filename);
    PathPlanner planner(&fdata);


    double pos1[3] = {37.1021769,-76.3872069,5};
    double pos2[3] = {37.102192,-76.386940,5};
    double pos3[3] = {37.102066,-76.387047,5.000000};
    double pos4[3] = {37.101978,-76.387115,5.000000};


    planner.InputFlightPlan("Plan0",0,pos1,1);
    planner.InputFlightPlan("Plan0",1,pos2,1);
    planner.InputFlightPlan("Plan0",2,pos3,1);
    planner.InputFlightPlan("Plan0",3,pos4,1);
    planner.InputFlightPlan("Plan0",4,pos1,1);

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

    //double positionA[3] = {37.102177,-76.387206,0};
    //double positionA[3] = {37.102179,-76.387175,4.990000};
    double positionA[3] = {37.10218,-76.38708,0};
    double velocityA[3] = {90,1,0};

    //double positionB[3] = {37.102185,-76.387065,0};
    double positionB[3] = {37.10218,-76.3872,0};
    //double positionB[3] = {37.102180,-76.387140,5.075000};

    double currPos[3] = {37.102178,-76.387177,4.990000};
    double exitPosition[3];

    double offsetPos[3] = {37.102197,-76.387045,4.990000};

    double offsets[2];
    planner.ComputeXtrackDistance_c("Plan0",1,offsetPos,offsets);

    printf("offsets: %f,%f\n",offsets[0],offsets[1]);

    planner.GetInterceptHeadingToPlan_c("Plan0",1,offsetPos);

    planner.GetExitPoint("Plan0",currPos,1,exitPosition);

    std::cout<<"exit position: "<<exitPosition[0]<<","<<exitPosition[1]<<","<<exitPosition[2]<<std::endl;


    Position pos = Position::makeLatLonAlt(positionA[0],"degree",positionA[1],"degree",positionA[2],"m");
    EuclideanProjection projection =  Projection::createProjection(pos);

    int status1 = planner.FindPath(_GRID_,"PlanA",positionA,positionB,velocityA);
    int status2 = planner.FindPath(_ASTAR_,"PlanB",positionA,positionB,velocityA);
    int status3 = planner.FindPath(_RRT_,"PlanC",positionA,positionB,velocityA);
    int status4 = planner.FindPath(_SPLINES_,"PlanD",positionA,positionB,velocityA);

    if (status1 > 0)
        planner.OutputFlightPlan(&projection,"PlanA","fence1.txt","waypoints1.txt");
    else
        std::cout<<"Grid Astar algorithm couldn't find solution"<<std::endl;

    if (status2 > 0)
        planner.OutputFlightPlan(&projection,"PlanB","fence2.txt","waypoints2.txt");
    else
        std::cout<<"Astar algorithm couldn't find solution"<<std::endl;

    if (status3 > 0)
        planner.OutputFlightPlan(&projection,"PlanC","fence3.txt","waypoints3.txt");
    else
        std::cout<<"RRT algorithm couldn't find solution"<<std::endl;

    if (status4 > 0)
        planner.OutputFlightPlan(&projection,"PlanD","fence4.txt","waypoints4.txt");
    else
        std::cout<<"SPLINES algorithm couldn't find solution"<<std::endl;


}


