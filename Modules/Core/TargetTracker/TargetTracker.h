#ifndef TARGET_TRACKER_H
#define TARGET_TRACKER_H

#ifdef __cplusplus
extern "C" {
#endif


void* new_TargetTracker(char* callsign);
void TargetTracker_SetHomePosition(void* obj,double position[3]);
void TargetTracker_InputMeasurement(void* obj,char* callsign,double time,double position[3],double velocity[3],double sigmaPos[6],double sigmaVel[6]);
void TargetTracker_InputCurrentState(void* obj,double position[3],double velocity[3]);
int TargetTracker_GetTotalIntruders(void* obj,double time);
void TargetTracker_GetIntruderData(void* obj,int i,char* callsign,double* time,double position[3],double velocity[3],double sigmaPos[6],double sigmaVel[6]);


#ifdef __cplusplus
}
#endif



#endif