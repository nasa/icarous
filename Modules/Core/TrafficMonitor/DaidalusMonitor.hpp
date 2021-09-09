#ifndef DAIDALUS_MONITOR_H
#define DAIDALUS_MONITOR_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "TrafficMonitor.hpp"
#include "Daidalus.h"

class DaidalusMonitor: public TrafficMonitor {
private:
    bool log;
    std::string callsign;
    double conflictTimeElapsed;
    double alertingTime;
    bool conflictTrack;
    bool conflictSpeed;
    bool conflictVerticalSpeed;

    bool daaViolationTrack;
    bool daaViolationSpeed;
    bool daaViolationVS;
    bool daaViolationAlt;

    double conflictStartTime;
    double startTime;
    
    std::ofstream logfileIn;
    std::ofstream logfileOut;

    int numTrackBands;
    int numSpeedBands;
    int numVerticalSpeedBands;
    int numAltitudeBands;

    std::vector<std::vector<double>> trackInterval;
    std::vector<std::vector<double>> speedInterval;
    std::vector<std::vector<double>> vsInterval;
    std::vector<std::vector<double>> altInterval;

    std::vector<int> trackIntTypes;
    std::vector<int> speedIntTypes;
    std::vector<int> vsIntTypes;
    std::vector<int> altIntTypes;

    double prevLogTime;
    double staleThreshold;
    std::map<std::string,int> trafficAlerts;
    bool CheckSafeToTurn(double position[],double velocity[],double fromHeading,double toHeading);
    bool CheckTurnConflict(double low, double high, double newHeading, double oldHeading);

    larcfm::Daidalus DAA1;   // DAA1 object used for regular traffic monitor
    larcfm::Daidalus DAA2;   // DAA2 object used for stateless queries

    int dataSource;

    double maxVS;
    double minVS;
    double ZTHR;
    double DTHR;
    double timeIntervalOfConflictLow;
    double timeIntervalOfConflictHigh;
public:

    DaidalusMonitor(std::string callsign, std::string daaConfig);
    void MonitorTraffic(larcfm::Velocity windfrom);
    std::string GetAlerter(object& intruder);
    bool CheckPositionFeasibility(const larcfm::Position pos,const double speed);
    void UpdateParameters(std::string daaParameters);
    int GetTrafficAlerts(int index,std::string& trafficID,int& alertLevel);
    bands_t GetTrackBands(void);
    bands_t GetSpeedBands(void);
    bands_t GetAltBands(void);
    bands_t GetVerticalSpeedBands(void);

};

#endif