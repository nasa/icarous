#include "TrafficMonitor.h"
#include "DaidalusMonitor.hpp"

void* newDaidalusTrafficMonitor(const char *callsign,const char *carg2){
    TrafficMonitor* obj = new DaidalusMonitor(callsign,std::string(carg2));
    return (void*)obj;
}

void TrafficMonitor_UpdateParameters(void * obj, const char * carg2){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    monitor->UpdateParameters(std::string(carg2));
}

int TrafficMonitor_InputIntruderData(void * obj, int source, int id, char* calls, double *pos,double * vel, double time,double sumPos[6],double sumVel[6]){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    object tf = {std::string(calls),
                 source,
                 id,
                 time,
                 larcfm::Position::makeLatLonAlt(pos[0],"degree",pos[1],"degree",pos[2],"m"),
                 larcfm::Velocity::makeTrkGsVs(vel[0],"degree",vel[1],"m/s",vel[2],"m/s")};
    std::memcpy(tf.posSigma,sumPos,sizeof(double)*6);
    std::memcpy(tf.velSigma,sumVel,sizeof(double)*6);
    return monitor->InputIntruderData(tf); 
}

void TrafficMonitor_InputOwnshipData(void * obj, double * position, double * velocity, double time,double sumPos[6],double sumVel[6]){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    larcfm::Position pos = larcfm::Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"m");
    larcfm::Velocity vel = larcfm::Velocity::makeTrkGsVs(velocity[0],"degree",velocity[1],"m/s",velocity[2],"m/s");
    monitor->InputOwnshipData(pos,vel,time,sumPos,sumVel);
}

void TrafficMonitor_MonitorTraffic(void* obj,double* windfrom){
    larcfm::Velocity wind = larcfm::Velocity::makeTrkGsVs(windfrom[0]+180,"degree",windfrom[1],"m/s",0,"m/s");
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    monitor->MonitorTraffic(wind);
}

bool TrafficMonitor_CheckPointFeasibility(void * obj, double * position,double speed){
    TrafficMonitor* monitor = (TrafficMonitor*)obj;
    larcfm::Position pos = larcfm::Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"m");
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
