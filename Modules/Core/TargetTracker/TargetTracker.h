#ifndef TARGET_TRACKER_H
#define TARGET_TRACKER_H

#ifdef __cplusplus
extern "C" {
#endif


void* new_TargetTracker(const char* callsign,const char* configFile);
void TargetTracker_ReadParamsFromFile(void* obj,const char* configFile);
void TargetTracker_SetHomePosition(void* obj,double position[3]);
void TargetTracker_SetModelUncertainty(void* obj,double sigmaP[6],double sigmaV[6]);
void TargetTracker_SetGateThresholds(void* obj,double p, double v);
void TargetTracker_InputMeasurement(void* obj,char* callsign,double time,double position[3],double velocity[3],double sigmaPos[6],double sigmaVel[6]);
void TargetTracker_InputCurrentState(void* obj,double time,double position[3],double velocity[3],double sigmaP[6],double sigmaV[6]);
int TargetTracker_GetTotalIntruders(void* obj);
void TargetTracker_GetIntruderData(void* obj,int i,char* callsign,double* time,double position[3],double velocity[3],double sigmaPos[6],double sigmaVel[6]);
void TargetTracker_UpdatePredictions(void* obj,double time);


#ifdef __cplusplus
}
#endif



#endif