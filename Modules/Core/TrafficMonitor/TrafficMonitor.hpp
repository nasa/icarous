//
// Created by Swee Balachandran on 12/15/17.
//
#ifndef TRAFFICMONITOR_H
#define TRAFFICMONITOR_H

#include <string>
#include <cstring>
#include <Position.h>
#include <Velocity.h>
#include <map>
#include <Core/Interfaces/Interfaces.h>

typedef struct{
    std::string callsign;
    int source;
    int id;
    double time;
    larcfm::Position position;
    larcfm::Velocity velocity;
    double posSigma[6];
    double velSigma[6];
}object;

class TrafficMonitor{
  protected:
    std::map<std::string,object> trafficList;
    larcfm::Position position;
    larcfm::Velocity velocity;
    double elapsedTime;
    double posSigma[6];
    double velSigma[6];
  public:
    virtual int InputIntruderData(const object obj){trafficList[obj.callsign] = obj; return trafficList.size();}
    virtual void InputOwnshipData(const larcfm::Position pos,const larcfm::Velocity vel,double time,double sigPos[6],double sigVel[6]){
        position = pos; velocity = vel; elapsedTime = time; 
        std::memcpy(posSigma,sigPos,sizeof(double)*6);
        std::memcpy(velSigma,sigVel,sizeof(double)*6);
    }
    virtual void MonitorTraffic(larcfm::Velocity windfrom)=0;
    virtual bool CheckPositionFeasibility(const larcfm::Position pos,const double speed) = 0;
    virtual void UpdateParameters(std::string params) = 0;
    virtual int GetTrafficAlerts(int index,std::string& trafficID,int& alertLevel)=0;
    virtual bands_t GetTrackBands(void) = 0;
    virtual bands_t GetSpeedBands(void) = 0;
    virtual bands_t GetAltBands(void) = 0;
    virtual bands_t GetVerticalSpeedBands(void) = 0;
};




#endif //TRAFFICMONITOR_H
