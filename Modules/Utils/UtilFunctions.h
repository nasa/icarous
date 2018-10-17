//
// Created by swee on 6/24/18.
//

#ifndef ICAROUS_CFS_UTILFUNCTIONS_H
#define ICAROUS_CFS_UTILFUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

double ComputeDistance(double positionA[],double positionB[]);
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
