//
// Created by Swee Balachandran on 12/15/17.
//
#ifndef TRAFFICMONITOR_H
#define TRAFFICMONITOR_H

#include "Daidalus.h"
#include "KinematicMultiBands.h"
#include "FlightData.h"


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
    larcfm::Daidalus DAA;
    larcfm::KinematicMultiBands KMB;

    TrafficMonitor(FlightData* fdata);
    bool CheckTurnConflict(double low,double high,double newHeading,double oldHeading);
    bool MonitorTraffic(bool visualize,double gpsTime,double position[],double velocity[],double resolution[],visbands_t* trkbands);
    void GetVisualizationBands(visbands_t& bands);
    bool CheckSafeToTurn(double position[],double velocity[],double fromHeading,double toHeading);

};


#endif //TRAFFICMONITOR_H
