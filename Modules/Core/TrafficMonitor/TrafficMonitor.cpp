#include "TrafficMonitor.h"
#include "DaidalusMonitor.hpp"
#include <cstring>

void* newDaidalusTrafficMonitor(bool carg1, char * carg2){
    TrafficMonitor* obj = new DaidalusMonitor(carg1,std::string(carg2));
    return (void*)obj;
}

void TrafficMonitor_UpdateParameters(void * obj, char * carg2,bool log){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    monitor->UpdateParameters(std::string(carg2),log);
}

int TrafficMonitor_InputIntruderData(void * obj, int id, char* calls, double *pos,double * vel, double time){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    object tf = {std::string(calls),
                 id,
                 time,
                 larcfm::Position::makeLatLonAlt(pos[0],"degree",pos[1],"degree",pos[2],"m"),
                 larcfm::Velocity::makeTrkGsVs(vel[0],"degree",vel[1],"m/s",vel[2],"m/s")};
    return monitor->InputIntruderData(tf); 
}

void TrafficMonitor_InputOwnshipData(void * obj, double * position, double * velocity, double time){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    larcfm::Position pos = larcfm::Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"degree");
    larcfm::Velocity vel = larcfm::Velocity::makeTrkGsVs(velocity[0],"degree",velocity[1],"m/s",velocity[2],"m/s");
    monitor->InputOwnshipData(pos,vel,time);
}

void TrafficMonitor_MonitorTraffic(void* obj){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    monitor->MonitorTraffic();
}

bool TrafficMonitor_CheckPointFeasibility(void * obj, double * position,double speed){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    larcfm::Position pos = larcfm::Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"degree");
    return monitor->CheckPositionFeasibility(pos,speed);
}

void TrafficMonitor_GetTrackBands(void *obj,bands_t* band){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    *band = monitor->GetTrackBands();
}

void TrafficMonitor_GetSpeedBands(void *obj,bands_t* band){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    *band = monitor->GetSpeedBands();
}

void TrafficMonitor_GetAltBands(void *obj,bands_t* band){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    *band = monitor->GetAltBands();
}

void TrafficMonitor_GetVerticalSpeedBands(void *obj,bands_t* band){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    *band = monitor->GetVerticalSpeedBands();
}

int TrafficMonitor_GetTrafficAlerts(void* obj,int id,char* calls,int* alert){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    std::string _callSign;
    int _alert;
    int size = monitor->GetTrafficAlerts(id,_callSign,_alert);
    std::strcpy(calls,_callSign.c_str());
    *alert = _alert;
    return size;
}

void delDaidalusTrafficMonitor(void * obj){
    DaidalusMonitor* monitor = (DaidalusMonitor*)obj;
    delete(monitor);
}