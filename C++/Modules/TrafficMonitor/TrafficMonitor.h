//
// Created by Swee Balachandran on 12/15/17.
//
#ifndef TRAFFICMONITOR_H
#define TRAFFICMONITOR_H

#include "Daidalus.h"
#include "KinematicMultiBands.h"
#include "FlightData.h"

using namespace larcfm;

class TrafficMonitor {
private:

    double conflictTimeElapsed;
    double daaLookahed;
    bool conflict;
    bool log;

    time_t conflictStartTime;
    time_t startTime;

    FlightData* fdata;
    visbands_t visBands;
    ofstream logfileIn;
    ofstream logfileOut;

public:
    Daidalus DAA;
    KinematicMultiBands KMB;

    TrafficMonitor(FlightData* fdata);
    bool CheckTurnConflict(double low,double high,double newHeading,double oldHeading);
    bool MonitorTraffic(bool visualize,double gpsTime,double position[],double velocity[]);
    void GetVisualizationBands(visbands_t& bands);
};


#endif //TRAFFICMONITOR_H
