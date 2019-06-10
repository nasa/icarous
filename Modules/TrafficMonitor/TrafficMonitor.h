//
// Created by Swee Balachandran on 12/15/17.
//
#ifndef TRAFFICMONITOR_H
#define TRAFFICMONITOR_H

#include "Daidalus.h"
#include "KinematicMultiBands.h"
#include <string>
#include <fstream>
#include "GenericObject.h"

typedef GenericObject TrafficObject;

class TrafficMonitor {
private:

    double conflictTimeElapsed;
    double daaLookahed;
    bool conflictTrack;
    bool conflictSpeed;
    bool conflictVerticalSpeed;
    bool log;

    bool daaViolationTrack;
    bool daaViolationSpeed;
    bool daaViolationVS;
    bool daaViolationAlt;

    time_t conflictStartTime;
    time_t startTime;
    
    std::ofstream logfileIn;
    std::ofstream logfileOut;

    int numTrackBands;
    int numSpeedBands;
    int numVerticalSpeedBands;
    int numAltitudeBands;

    double trackIntervals[20][2];
    double speedIntervals[20][2];
    double vsIntervals[20][2];
    double altIntervals[20][2];

    int trackIntTypes[20];
    int speedIntTypes[20];
    int vsIntTypes[20];
    int altIntTypes[20];

    std::list<TrafficObject> trafficList;
    double prevLogTime;
    double elapsedTime;

public:
    larcfm::Daidalus DAA;
    larcfm::KinematicMultiBands KMB;

    TrafficMonitor(bool reclog,char daaConfig[]);
    void UpdateDAAParameters(char daaParameters[]);
    int InputTraffic(int id,double position[],double velocity[],double elapsedTime);
    void MonitorTraffic(double position[],double velocity[],double elapsedTime);
    bool MonitorWPFeasibility(double position[],double velocity[],double wp[]);
    bool CheckSafeToTurn(double position[],double velocity[],double fromHeading,double toHeading);
    bool CheckTurnConflict(double low, double high, double newHeading, double oldHeading);

    void GetTrackBands(int& numBands,int* bandTypes,double* low,double *high,
                       int& recovery,
                       int& currentConflict,
                       double& tViolation,
                       double& tRecovery,
                       double& minHDist,
                       double& minVDist,
                       double& resUp,
                       double& resDown,
                       double& redPref);

    void GetGSBands(int& numBands,int* bandTypes,double* low,double *high,
                    int& recovery,
                    int& currentConflict,
                    double& tViolation,
                    double& tRecovery,
                    double& minHDist,
                    double& minVDist,
                    double& resUp,
                    double& resDown,
                    double& redPref);

    void GetVSBands(int& numBands,int* bandTypes,double* low,double *high,
                    int& recovery,
                    int& currentConflict,
                    double& tViolation,
                    double& tRecovery,
                    double& minHDist,
                    double& minVDist,
                    double& resUp,
                    double& resDown,
                    double& redPref);

    void GetAltBands(int& numBands,int* bandTypes,double* low,double *high,
                    int& recovery,
                    int& currentConflict,
                    double& tViolation,
                    double& tRecovery,
                    double& minHDist,
                    double& minVDist,
                    double& resUp,
                    double& resDown,
                    double& redPref);

};


#endif //TRAFFICMONITOR_H
