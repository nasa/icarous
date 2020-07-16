#ifndef TRAFFIC_C_WRAPPERS_H
#define TRAFFIC_C_WRAPPERS_H

#include "Interfaces.h"

#ifdef __cplusplus
extern "C" {
#endif


void* new_TrafficMonitor(bool carg1, /*aaa*/ char * carg2);
void TrafficMonitor_UpdateDAAParameters(void * carg1, /*aaa*/ char * carg2,bool log);
int TrafficMonitor_InputTraffic(void * carg1, int carg2, char* calls,/*aaa*/ double * carg3, /*aaa*/ double * carg4, double carg5);
void TrafficMonitor_MonitorTraffic(void * carg1, /*aaa*/ double * carg2, /*aaa*/ double * carg3, double carg4);
bool TrafficMonitor_MonitorWPFeasibility(void * carg1, /*aaa*/ double * carg2, /*aaa*/ double * carg3, /*aaa*/ double * carg4);
bool TrafficMonitor_CheckSafeToTurn(void * carg1, /*aaa*/ double * carg2, /*aaa*/ double * carg3, double carg4, double carg5);
bool TrafficMonitor_CheckTurnConflict(void * carg1, double carg2, double carg3, double carg4, double carg5);
void TrafficMonitor_GetTrackBands(void *obj,bands_t);
void TrafficMonitor_GetGSBands(void * obj,bands_t);
void TrafficMonitor_GetVSBands(void * carg1, int * carg2, int * carg3, double * carg4, double * carg5, int * carg6, int * carg7, double * carg8, double * carg9, double * carg10, double * carg11, double * carg12, double * carg13, double * carg14);
void TrafficMonitor_GetAltBands(void * carg1, int * carg2, int * carg3, double * carg4, double * carg5, int * carg6, int * carg7, double * carg8, double * carg9, double * carg10, double * carg11, double * carg12, double * carg13, double * carg14);
void delete_TrafficMonitor(void * carg1);
int TrafficMonitor_GetTrafficAlerts(void* carg1,int carg2,char* carg3,int* carg4);

#ifdef __cplusplus
}
#endif


#endif