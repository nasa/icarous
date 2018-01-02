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

    vertex[0].latitude  = 37.1020599;
    vertex[0].longitude = -76.3869966;

    vertex[1].latitude = 37.1022559;
    vertex[1].longitude = -76.3870096;

    vertex[2].latitude  = 37.1022474;
    vertex[2].longitude = -76.3872689;

    vertex[3].latitude = 37.1020174;
    vertex[3].longitude = -76.3872664;

    for(int i=0;i<4;i++) {
        SwigObj vertexWrapper;
        vertexWrapper.obj = (void*)&vertex[i];
        FlightData_InputGeofenceData(fdata, &vertexWrapper);
    }

    double position[3] = {37.102192,-76.386942,5.000000};
    double velocity[3] = {90,1,0};

    GeofenceMonitor_CheckViolation(gfMonitor,position,velocity[0],velocity[1],velocity[2]);

    bool violation;
    bool conflict;
    int fenceid;
    double recPosition[3] = {0,0,0};

    GeofenceMonitor_GetConflict(gfMonitor,0,&fenceid,&conflict,&violation,recPosition);
    int n = GeofenceMonitor_GetNumConflicts(gfMonitor);
    printf("CONFLICT: %d, VIOLATION: %d, numConflict:%d\n",conflict,violation,n);
    printf("Recovery Position: %f,%f,%f\n",recPosition[0],recPosition[1],recPosition[2]);

    return 0;

}
