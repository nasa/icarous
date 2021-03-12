//
// Created by Swee Balachandran on 12/7/17.
//

#ifndef GEOFENCEMONITOR_H
#define GEOFENCEMONITOR_H


#include "AircraftState.h"
#include "Plan.h"
#include "fence.h"
#include <list>

typedef struct{
    int fenceId;
    bool violationStatus;
    bool conflictstatus;
    double recoveryPoint[3];
    fence* _gf;
}GeofenceConflict;

class GeofenceMonitor {

private:
    double BUFF = 0.1;

    double lookahead;
    double hthreshold;
    double vthreshold;
    double hstepback;
    double vstepback;

    std::list<fence> fenceList;
    std::list<fence>::iterator fenceListIt;
    larcfm::CDPolycarp geoPolyCarp;
    larcfm::PolycarpResolution geoPolyResolution;
    larcfm::PolycarpDetection geoPolyDetect;
    larcfm::PolyPath geoPolyPath;
    larcfm::CDIIPolygon geoCDIIPolygon;
    std::list<GeofenceConflict> conflictList;
    bool CollisionDetection(fence* gf,larcfm::Position* pos, larcfm::Vect2* v,double startTime, double stopTime);
    fence* GetGeofence(int id);
public:
    GeofenceMonitor(double[]);
    void SetGeofenceParameters(double []);
    void InputGeofenceData(int type,int index, int totalVertices, double floor, double ceiling, double pos[][2]);
    bool CheckViolation(double position[],double track,double groundSpeed,double verticalSpeed);
    bool CheckWPFeasibility(double fromPosition[],double toPosition[]);
    int GetNumConflicts();
    void GetConflictStatus(bool conflicts[]);
    void GetConflict(int id,int& fenceId,uint8_t& conflict,uint8_t& violation,double recoveryPoint[],uint8_t& type);
    void GetClosestRecoveryPoint(double currentPosition[],double recoveryPosition[]);
    void ClearFences();
};


#endif //GEOFENCEMONITOR_H
