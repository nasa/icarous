//
// Created by swee on 6/24/18.
//

#ifndef ICAROUS_CFS_UTILFUNCTIONS_H
#define ICAROUS_CFS_UTILFUNCTIONS_H

#include <stdbool.h>
#include <Interfaces.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
   char key[50];
   char valueString[50];
   char unitString[10];
   double value;
}ParsedParam_t;

double ComputeDistance(double positionA[],double positionB[]);
double ComputeHeading(double positionA[],double positionB[]);
void ComputeOffsetPosition(double position[],double track,double dist,double output[]);
void ConvertEND2LLA(double gpsOrigin[],double NED[],double outputLLA[]);
void ConvertRAE2LLA(double lat,double lon,double heading,double range,double azimuth,double elevation,double output[]);
void ConvertLLA2END(double gpsOrigin[],double LLA[],double outputNED[]);

/* Convert Vn, Ve, Vd to Trk, Ground speed and climb rate */
void ConvertVnedToTrkGsVs(double vn,double ve,double vz,double *Trk,double *Gs,double *Vs);

/* Convert track, ground speed and climb rate to Vn, Ve, vd */
void ConvertTrkGsVsToVned(double Trk,double Gs,double Vs,double *vn,double *ve,double *vd);
void ComputeTrackingResolution(double targetPos[],double currentPos[],double currentVel[],double heading,double distH,double distV,
                                      double PropGains[],double outputVel[],double *outputHeading);
double SaturateVelocity(double V, double Vsat);
void ComputeLatLngAlt(double origin[],double xyz[],double output[]);
double ComputeXtrackDistance(double wpA[],double wpB[],double position[],double offset[]);
void GetPositionOnPlan(double wpA[],double wpB[],double currentPos[],double position[]);
void ManueverToIntercept(double wpA[],double wpB[],double currPosition[],double velocity[],
                        double xtrkDevGain,double resolutionSpeed,double allowedDev);
double GetInterceptHeadingToPlan(double wpA[],double wpB[],double currentPos[]);
void ComputeWaypointsETA(double scenarioTime,int numWP, double wpSpeed[], waypoint_t wpts[]);
int ParseParameterFile(char filename[],ParsedParam_t params[]);

#ifdef __cplusplus
};
#endif


#endif //ICAROUS_CFS_UTILFUNCTIONS_H
