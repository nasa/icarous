
#ifndef GEOFENCEMONITORC_H
#define GEOFENCEMONITORC_H


#ifdef __cplusplus
extern "C" {
#endif


void *new_GeofenceMonitor(double *params);
void GeofenceMonitor_SetGeofenceParameters(void *obj, double *params);
void GeofenceMonitor_InputGeofenceData(void *obj, int type, int index, int totalVertices, double floor, double ceiling, double (*vertices)[2]);
bool GeofenceMonitor_CheckViolation(void * obj, double *position, double track, double groundSpeed, double verticalSpeed);
bool GeofenceMonitor_CheckWPFeasibility(void * obj, double * fromPosition, double * toPosition);
int GeofenceMonitor_GetNumConflicts(void * obj);
void GeofenceMonitor_GetConflictStatus(void * obj, bool*conflictStatus);
void GeofenceMonitor_GetConflict(void * obj, int id, int * fenceId, bool * conflict, bool * violation, double * recoveryPos, int * type);
void GeofenceMonitor_GetClosestRecoveryPoint(void * obj, double * currentPos, double *recoveryPos);
void GeofenceMonitor_ClearFences(void * obj);
void delete_GeofenceMonitor(void * obj);


#ifdef __cplusplus
}
#endif

#endif