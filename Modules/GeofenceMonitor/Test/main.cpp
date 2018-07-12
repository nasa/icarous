#include <iostream>
#include "GeofenceMonitor.h"


int main() {


    // input parameters for geofence monitor
    double lookahead = 5;
    double hthreshold = 2;
    double vthreshold = 1;
    double hstepback  = 1;
    double vstepback  = 1;

    double params[5] = {lookahead,hstepback,vthreshold,hstepback,vstepback};

    GeofenceMonitor fenceMonitor(params);

    double vertex[4][2];

    vertex[0][0]= 37.102545;
    vertex[0][1]= -76.387163;

    vertex[1][0]= 37.102344;
    vertex[1][1]= -76.387163;

    vertex[2][0]= 37.102351;
    vertex[2][1]= -76.386844;

    vertex[3][0]= 37.102575;
    vertex[3][1]= -76.386962;

    // Input the geofence data
    fenceMonitor.InputGeofenceData(KEEP_IN,0,4,0,100,vertex);

    Position so = Position::makeLatLonAlt(37.1021913,"deg", -76.3869528,"deg", 5.0,"m");
    Velocity vo = Velocity::makeTrkGsVs(90.0,"deg",  0.94,"m/s", 0.0,"fpm");

    double position[3] = {so.latitude(),so.longitude(),so.alt()};
    double velocity[3] = {vo.track("deg"),vo.gs(),vo.vs()};

    fenceMonitor.CheckViolation(position,velocity[0],velocity[1],velocity[2]);

    bool violation;
    bool conflict;
    int fenceid;
    double recPosition[3] = {0,0,0};
    int test;

    fenceMonitor.GetConflict(0,fenceid,conflict,violation,recPosition,test);

    std::cout<<"CONFLICT: "<<conflict<<std::endl;
    std::cout<<"VIOLATION: "<<violation<<std::endl;
    printf("Recovery Position: %f,%f,%f\n",recPosition[0],recPosition[1],recPosition[2]);

    return 0;
}