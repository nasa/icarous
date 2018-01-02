//
// Created by Swee Balachandran on 12/21/17.
//

#include "Icarous_msg.h"
#include "fence.h"
#include "GeofenceMonitor_proxy.h"

int main(int argc,char** argv){

    FlightData* fdata;
    GeofenceMonitor* gfMonitor;

    fdata = new_FlightData("../Test/icarous.txt");
    gfMonitor = new_GeofenceMonitor(fdata);

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

    for(int i=0;i<4;i++) {
        SwigObj vertexWrapper;
        vertexWrapper.obj = (void*)&vertex[i];
        FlightData_InputGeofenceData(fdata, &vertexWrapper);
    }

    double position[3] = {37.1021913,-76.3869528,5.0};
    double velocity[3] = {90,1,0};

    GeofenceMonitor_CheckViolation(gfMonitor,position,velocity[0],velocity[1],velocity[2]);

    bool violation;
    bool conflict;
    int fenceid;
    double recPosition[3] = {0,0,0};

    GeofenceMonitor_GetConflict(gfMonitor,0,&fenceid,&conflict,&violation,recPosition);

    printf("CONFLICT: %d, VIOLATION: %d\n",conflict,violation);
    printf("Recovery Position: %f,%f,%f\n",recPosition[0],recPosition[1],recPosition[2]);

    return 0;

}
