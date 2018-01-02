//
// Created by Swee Balachandran on 12/7/17.
//

#ifndef GEOFENCEMONITOR_H
#define GEOFENCEMONITOR_H

#include "FlightData.h"
#include "AircraftState.h"
#include "Plan.h"

typedef struct{
    int fenceId;
    bool violationStatus;
    bool conflictstatus;
    double recoveryPoint[3];
}GeofenceConflict;

class GeofenceMonitor {

private:
    double BUFF = 0.1;
    FlightData *fdata;
    CDPolycarp geoPolyCarp;
    PolycarpResolution geoPolyResolution;
    PolycarpDetection geoPolyDetect;
    CDIIPolygon geoCDIIPolygon;
    std::list<GeofenceConflict> conflictList;
    bool CollisionDetection(fence* gf,Position* pos, Vect2* v,double startTime, double stopTime);
    
public:
    GeofenceMonitor(FlightData* fd);
    void CheckViolation(double position[],double track,double groundSpeed,double verticalSpeed);
    bool CheckWPFeasibility(double fromPosition[],double toPosition[]);
    int GetNumConflicts();
    void GetConflict(int id,int& fenceId,bool& conflict,bool& violation,double recoveryPoint[]);
    void GetClosestRecoveryPoint(double currentPosition[],double recoveryPosition[]);
};


#endif //GEOFENCEMONITOR_H
