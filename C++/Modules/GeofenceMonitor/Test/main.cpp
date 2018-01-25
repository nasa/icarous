#include <iostream>
#include <Icarous_msg.h>
#include "GeofenceMonitor.h"


int main() {

    char filename[]="../Test/icarous.txt";
    FlightData fdata(filename);
    GeofenceMonitor fenceMonitor(&fdata);

    // Create a keep in geofence with 4 vertices
    geofence_t vertex[4];

    for(int i=0;i<4;i++){
        vertex[i].index = 0;
        vertex[i].type = KEEP_IN;
        vertex[i].totalvertices = 4;
        vertex[i].vertexIndex = i;
        vertex[i].floor = 0;
        vertex[i].ceiling = 100;
    }

    vertex[0].latitude  = 37.102545;
    vertex[0].longitude = -76.387163;

    vertex[1].latitude = 37.102344;
    vertex[1].longitude = -76.387163;

    vertex[2].latitude  = 37.102351;
    vertex[2].longitude = -76.386844;

    vertex[3].latitude = 37.102575;
    vertex[3].longitude = -76.386962;

    // Input the geofence data
    for(int i=0;i<4;i++)
        fdata.InputGeofenceData(&vertex[i]);

    Position so = Position::makeLatLonAlt(37.1021913,"deg", -76.3869528,"deg", 5.0,"m");
    Velocity vo = Velocity::makeTrkGsVs(90.0,"deg",  0.94,"m/s", 0.0,"fpm");

    double position[3] = {so.latitude(),so.longitude(),so.alt()};
    double velocity[3] = {vo.track("deg"),vo.gs(),vo.vs()};

    fenceMonitor.CheckViolation(position,velocity[0],velocity[1],velocity[2]);

    bool violation;
    bool conflict;
    int fenceid;
    double recPosition[3] = {0,0,0};

    fenceMonitor.GetConflict(0,fenceid,conflict,violation,recPosition);

    std::cout<<"CONFLICT: "<<conflict<<std::endl;
    std::cout<<"VIOLATION: "<<violation<<std::endl;
    printf("Recovery Position: %f,%f,%f\n",recPosition[0],recPosition[1],recPosition[2]);

    return 0;
}