//
// Created by swee on 6/24/18.
//

#ifndef ICAROUS_CFS_UTILFUNCTIONS_H
#define ICAROUS_CFS_UTILFUNCTIONS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

double ComputeDistance(double positionA[],double positionB[]);
double ComputeHeading(double positionA[],double positionB[]);
void ComputeOffsetPosition(double position[],double track,double dist,double output[]);
void ConvertNED2LLA(double gpsOrigin[],double NED[],double outputLLA[]);
void ConvertLLA2NED(double gpsOrigin[],double LLA[],double outputNED[]);
bool CheckTurnConflict(double low,double high,double fromHeading,double toHeading);
void ConvertVnedToTrkGsVs(double vn,double ve,double vz,double *Trk,double *Gs,double *Vs);
void ConvertTrkGsVsToVned(double Trk,double Gs,double Vs,double *vn,double *ve,double *vd);
void ComputeTrackingResolution(double targetPos[],double currentPos[],double currentVel[],double heading,double distH,double distV,
                                      double PropGains[],double outputVel[],double *outputHeading);
double SaturateVelocity(double V, double Vsat);
void ComputeLatLngAlt(double origin[],double xyz[],double output[]);



#ifdef __cplusplus
};
#endif


#endif //ICAROUS_CFS_UTILFUNCTIONS_H
