#include <iostream>
#include "GeofenceMonitor.h"
#include <assert.h>

#define ASSERT(condition,statement) {\
    assert(condition);               \
    std::cout<<statement<<std::endl; \
}

int main() {


    // input parameters for geofence monitor
    double lookahead = 5;
    double hthreshold = 2;
    double vthreshold = 1;
    double hstepback  = 1;
    double vstepback  = 1;
    double params[5] = {lookahead,hthreshold,vthreshold,hstepback,vstepback};

    bool violation;
    bool conflict;
    int fenceid;
    double recPosition[3] = {0,0,0};
    int test;

    double vertexOut[4][2];
    double vertexIn[4][2];

    GeofenceMonitor fenceMonitor(params);

    vertexOut[0][0]= 37.102545;
    vertexOut[0][1]= -76.387163;

    vertexOut[1][0]= 37.102344;
    vertexOut[1][1]= -76.387163;

    vertexOut[2][0]= 37.102351;
    vertexOut[2][1]= -76.386844;

    vertexOut[3][0]= 37.102575;
    vertexOut[3][1]= -76.386962;


    vertexIn[0][0]= 37.102794;
    vertexIn[0][1]= -76.387528;

    vertexIn[1][0]= 37.102148;
    vertexIn[1][1]= -76.387426;

    vertexIn[2][0]= 37.102109;
    vertexIn[2][1]= -76.386539;

    vertexIn[3][0]= 37.102911;
    vertexIn[3][1]= -76.386512;

    // Input the geofence data
    fenceMonitor.InputGeofenceData(KEEP_IN,0,4,0,100,vertexIn);
    fenceMonitor.InputGeofenceData(KEEP_OUT,1,4,0,100,vertexOut);

    double positionA[3] = {37.102599,-76.386631,5.0};
    double velocityA[3] = {90, 0, 0};

    conflict = fenceMonitor.CheckViolation(positionA,velocityA[0],velocityA[1],velocityA[2]);
    ASSERT(conflict == false,"Point A inside keep-in/outside keep out test passed");

    double positionB[3] = {37.102451,-76.387053,5.0};
    double velocityB[3] = {90.0,0,0};

    conflict = fenceMonitor.CheckViolation(positionB,velocityB[0],velocityB[1],velocityB[2]);
    ASSERT(conflict == true,"Point B inside keep-in/inside keep out test passed");
    fenceMonitor.GetConflict(0,fenceid,conflict,violation,recPosition,test);

    ASSERT(violation == true,"Point B violation output passed");
    ASSERT(conflict == true,"Point B conflict output passed");
    ASSERT(test == 1,"Point B keep out violation true");

    double velocityC[3] = {90.0,10.0,0};
    conflict = fenceMonitor.CheckViolation(positionA,velocityC[0],velocityC[1],velocityC[2]);
    ASSERT(conflict == true,"Point A inside keep-in/outside keep out with velocity conflict test passed");


    double positionC[3] = {37.103348,-76.38664,0.0};
    conflict = fenceMonitor.CheckViolation(positionC,velocityA[0],velocityA[1],velocityA[2]);
    ASSERT(conflict == true, "Point C outside keep-in test passed");

    fenceMonitor.GetConflict(0,fenceid,conflict,violation,recPosition,test);
    ASSERT(violation == true,"Point C violation output passed");
    ASSERT(conflict == true,"Point C conflict output passed");
    ASSERT(test == 0,"Point B keep in violation true");

    bool feasibility;
    feasibility = fenceMonitor.CheckWPFeasibility(positionA,positionC);
    ASSERT(feasibility==false, "WP feasibility check between points A and C passed");

    feasibility = fenceMonitor.CheckWPFeasibility(positionA,positionB);
    ASSERT(feasibility==false, "WP feasibility check between points A and B passed");


    feasibility = fenceMonitor.CheckWPFeasibility(positionB,positionC);
    ASSERT(feasibility==false, "WP feasibility check between points B and C passed");

    return 0;
}