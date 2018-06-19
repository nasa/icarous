//
// Created by Swee Balachandran on 12/21/17.
//

#include "fence.h"
#include "GeofenceMonitor_proxy.h"

int main(int argc,char** argv){

    // input parameters for geofence monitor
    double lookahead = 5;
    double hthreshold = 2;
    double vthreshold = 1;
    double hstepback  = 1;
    double vstepback  = 1;

    double params[5] = {lookahead,hstepback,vthreshold,hstepback,vstepback};


    GeofenceMonitor* gfMonitor;

    gfMonitor = new_GeofenceMonitor(params);

    // Create a keep in geofence with 4 vertices
    double vertex[50][2];

    vertex[0][0]= 37.102545;
    vertex[0][1]= -76.387163;

    vertex[1][0]= 37.102344;
    vertex[1][1]= -76.387163;

    vertex[2][0]= 37.102351;
    vertex[2][1]= -76.386844;

    vertex[3][0]= 37.102575;
    vertex[3][1]= -76.386962;

    GeofenceMonitor_InputGeofenceData(gfMonitor,KEEP_IN,0,4,0,100,vertex);


    double position[3] = {37.102192,-76.386942,5.000000};
    double velocity[3] = {90,1,0};

    GeofenceMonitor_CheckViolation(gfMonitor,position,velocity[0],velocity[1],velocity[2]);

    bool violation;
    bool conflict;
    int fenceid;
    double recPosition[3] = {0,0,0};
    int type;
    GeofenceMonitor_GetConflict(gfMonitor,0,&fenceid,&conflict,&violation,recPosition,&type);
    int n = GeofenceMonitor_GetNumConflicts(gfMonitor);
    printf("CONFLICT: %d, VIOLATION: %d, numConflict:%d\n",conflict,violation,n);
    printf("Recovery Position: %f,%f,%f\n",recPosition[0],recPosition[1],recPosition[2]);

    double fromPosition[3] = {37.102188,-76.387013,5.000000};
    double toPosition[3] = {37.102186,-76.387060,5.000000};

    bool val = GeofenceMonitor_CheckWPFeasibility(gfMonitor,fromPosition,toPosition);

    printf("wp feasbility: %d\n",val);

    return 0;

}
