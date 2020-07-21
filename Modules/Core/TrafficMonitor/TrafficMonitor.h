#ifndef TRAFFIC_C_WRAPPERS_H
#define TRAFFIC_C_WRAPPERS_H

#include "Core/Interfaces/Interfaces.h"

#ifdef __cplusplus
extern "C" {
#endif


void* newDaidalusTrafficMonitor(bool carg1, char * carg2);
void delDaidalusTrafficMonitor(void * obj);
void TrafficMonitor_UpdateParameters(void * obj, char * carg2,bool log);
int TrafficMonitor_InputIntruderData(void * obj, int carg2, char* calls, double *position,double * velocity, double time);
void TrafficMonitor_InputOwnshipData(void * obj, double * position, double * velocity, double time);
void TrafficMonitor_MonitorTraffic(void* obj);
bool TrafficMonitor_CheckPointFeasibility(void * obj, double * point, double speed);
void TrafficMonitor_GetTrackBands(void *obj,bands_t*);
void TrafficMonitor_GetSpeedBands(void *obj,bands_t*);
void TrafficMonitor_GetAltBands(void *obj,bands_t*);
void TrafficMonitor_GetVerticalSpeedBands(void *obj,bands_t*);
int TrafficMonitor_GetTrafficAlerts(void* obj,int id,char* calls,int* alert);

#ifdef __cplusplus
}
#endif


#endif