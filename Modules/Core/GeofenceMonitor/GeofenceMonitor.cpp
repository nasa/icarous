//
// Created by Swee Balachandran on 12/7/17.
//

#include "GeofenceMonitor.h"
#include "GeofenceMonitor.hpp"

GeofenceMonitor::GeofenceMonitor(double *params):geoPolyCarp(0.01,0.001,false) {
    lookahead  = params[0];
    hthreshold = params[1];
    vthreshold = params[2];
    hstepback  = params[3];
    vstepback  = params[4];
}

void GeofenceMonitor::SetGeofenceParameters(double *params) {
    lookahead  = params[0];
    hthreshold = params[1];
    vthreshold = params[2];
    hstepback  = params[3];
    vstepback  = params[4];
}

bool GeofenceMonitor::CollisionDetection(fence* gf,Position* pos,Vect2* v,double startTime,double stopTime){
    int n = fenceList.size();
    for(int i=0;i<n;i++){

        Vect2 currentPos = gf->GetProjection()->project(*pos).vect2();
        Vect2 polygonVel(0,0);
        bool insideBad = false;
        if(gf->GetType() == KEEP_OUT){
            insideBad = true;
        }

        std::vector<Vect2> fenceVertices = *gf->getCartesianVertices();
        bool val = geoPolyDetect.Static_Collision_Detector(startTime,stopTime,fenceVertices,
                                                           polygonVel,currentPos,*v,BUFF,insideBad);

        return val;
    }

    return true;
}

bool GeofenceMonitor::CheckViolation(double position[],double trk,double gs,double vs){

    Position currentPosLLA = Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"m");
    Velocity currentVel    = Velocity::makeTrkGsVs(trk,"degree",gs,"m/s",vs,"m/s");
    int n = fenceList.size();
    double entryTime =-1.0, exitTime =-1.0;
    bool conflict = false;
    bool violation = false;
    Position recoveryPoint;
    conflictList.clear();
    GeofenceConflict gcf;


    for(int i = 0;i<n;i++) {
        fence *gf = GetGeofence(i);
        if(gf == NULL){
            continue;
        }

        EuclideanProjection *proj = gf->GetProjection();
        Vect3 currentPosR3 = proj->project(currentPosLLA);


        if (gf->GetType() == KEEP_IN) {
            if (geoPolyCarp.nearEdge(currentPosR3, *(gf->GetPoly3D()), hthreshold, vthreshold)) {
                conflict = true;
                //printf("Conflict keep in fence\n");
            } else {
                conflict = false;
            }

            Vect2 vel = currentVel.vect2();
            if(vel.norm() >= 0.1){
                conflict = conflict | CollisionDetection(gf, &currentPosLLA, &vel, 0, lookahead);
            }

            if (geoPolyCarp.definitelyInside(currentPosR3, *(gf->GetPoly3D()))) {
                violation = false;
            } else {
                violation = true;
                //printf("violation keep in fence\n");
            }

            std::vector<Vect2> fenceVertices = *(gf->getModCartesianVertices());
            Vect2 recPointR2 = geoPolyResolution.inside_recovery_point(BUFF, hstepback,
                                                                       fenceVertices, currentPosR3.vect2());
            double ceiling = gf->GetCeiling();
            double alt;
            if (std::abs(currentPosR3.z - ceiling) <= vthreshold) {
                alt = ceiling - vstepback;
            } else {
                alt = currentPosR3.z;
            }

            LatLonAlt LLA = proj->inverse(recPointR2, currentPosLLA.alt());
            recoveryPoint = Position::makeLatLonAlt(LLA.latitude(), "degree",
                                                    LLA.longitude(), "degree",
                                                    alt, "m");
        } else {
            std::vector<Vect2> fenceVertices = *(gf->getCartesianVertices());
            Vect2 recPointR2 = geoPolyResolution.outside_recovery_point(BUFF, hstepback,
                                                                        fenceVertices, currentPosR3.vect2());
            LatLonAlt LLA = proj->inverse(recPointR2, currentPosLLA.alt());
            recoveryPoint = Position::makeLatLonAlt(LLA.latitude(), "degree",
                                                    LLA.longitude(), "degree",
                                                    LLA.altitude(), "ft");

            Vect2 vel = currentVel.vect2();
            if (CollisionDetection(gf, &currentPosLLA, &vel, 0, lookahead) && vel.norm() >= 0.1) {
                conflict = true;
                //printf("keep out conflict\n");
            } else {
                conflict = false;
            }

            if (geoPolyCarp.definitelyInside(currentPosR3, *(gf->GetPoly3D()))) {
                violation = true;
                //printf("violation keep out fence\n");
            } else {
                violation = false;
            }
        }

        if (conflict || violation) {
            gcf.fenceId = gf->GetID();
            gcf.conflictstatus = conflict;
            gcf.violationStatus = violation;
            gcf.recoveryPoint[0] = recoveryPoint.latitude();
            gcf.recoveryPoint[1] = recoveryPoint.longitude();
            gcf.recoveryPoint[2] = recoveryPoint.alt();
            gcf._gf = gf;
            conflictList.push_back(gcf);
        }
    }


    if(conflictList.size() > 0){
        return true;
    }else{
        return false;
    }
}


bool GeofenceMonitor::CheckWPFeasibility(double fromPosition[],double toPosition[]) {
    Position currentPos = Position::makeLatLonAlt(fromPosition[0], "degree", fromPosition[1], "degree", fromPosition[2],"m");
    Position nextPos = Position::makeLatLonAlt(toPosition[0], "degree", toPosition[1], "degree", toPosition[2], "m");
    double heading2WP = currentPos.track(nextPos);
    double dist = currentPos.distanceH(nextPos);
    // Velocity components assuming speed is 1 m/s
    double vy = cos(heading2WP);
    double vx = sin(heading2WP);
    Vect2 vel(vx, vy);

    double time = dist;
    bool val = false;
    for (int i = 0; i < fenceList.size(); ++i) {
        val = val || CollisionDetection(GetGeofence(i),&currentPos, &vel, 0, time);
    }

    return !val;
}

int GeofenceMonitor::GetNumConflicts() {
    return conflictList.size();
}

void GeofenceMonitor::GetConflict(int id, int& fenceId, bool& conflict, bool& violation, double recoveryPoint[],int& type) {
    int count = -1;
    for(GeofenceConflict it:conflictList){
        count++;
        if (count == id){
            type = (it._gf->GetType()==KEEP_IN)?0:1;
            fenceId = it.fenceId;
            conflict = it.conflictstatus;
            violation = it.violationStatus;
            recoveryPoint[0] = it.recoveryPoint[0];
            recoveryPoint[1] = it.recoveryPoint[1];
            recoveryPoint[2] = it.recoveryPoint[2];
        }
    }
}

void  GeofenceMonitor::GetClosestRecoveryPoint(double currPos[],double recoveryPos[]) {
    double dist;
    double mindist = MAXDOUBLE;
    double _recPos[3];
    for(GeofenceConflict it:conflictList){
            int fenceId = it.fenceId;
            bool conflict = it.conflictstatus;
            bool violation = it.violationStatus;
            _recPos[0] = it.recoveryPoint[0];
            _recPos[1] = it.recoveryPoint[1];
            _recPos[2] = it.recoveryPoint[2];

           dist = sqrt(pow(_recPos[0] - currPos[0],2) + pow(_recPos[1] - currPos[1],2));
          if(dist < mindist){
              mindist = dist;
              recoveryPos[0] = _recPos[0];
              recoveryPos[1] = _recPos[1];
              recoveryPos[2] = _recPos[2];

          }
    }
}

void GeofenceMonitor::GetConflictStatus(bool conflicts[]) {

    conflicts[0] = false; //Keep in conflict
    conflicts[1] = false; // Keep out conflict
    for(GeofenceConflict it:conflictList){

        int id = it.fenceId;
        fence *gf = GetGeofence(id);
        if(gf->GetType() == KEEP_IN){
            conflicts[0] |= it.conflictstatus;
        }else{
            conflicts[1] |= it.conflictstatus;
        }
    }
}

void GeofenceMonitor::InputGeofenceData(int type,int index, int totalVertices, double floor, double ceiling, double pos[][2]){


    double ResolBUFF = hthreshold;
    fence newfence(index,(FENCE_TYPE)type,totalVertices,floor,ceiling);

    for(int i=0;i<totalVertices;++i) {
        newfence.AddVertex(i, pos[i][0], pos[i][1], ResolBUFF);
    }
    if(fenceList.size() > index){
        fenceList.clear();
        for(int i=geoPolyPath.size()-1;i>=0;i--){
            geoPolyPath.remove(i);
        }
    }

    fenceList.push_back(newfence);
    if(newfence.GetType() == KEEP_OUT)
        geoPolyPath.addPolygon(*newfence.GetPolyMod(),Velocity::makeVxyz(0,0,0),0);
}

fence* GeofenceMonitor::GetGeofence(int id) {
    for(fenceListIt = fenceList.begin();fenceListIt != fenceList.end();++fenceListIt){
        if (id == fenceListIt->GetID()){
            return &(*(fenceListIt));
        }
    }
    return NULL;
}

void GeofenceMonitor::ClearFences() {
    fenceList.clear();
}

void *new_GeofenceMonitor(double *params){
    GeofenceMonitor *obj = new GeofenceMonitor(params);
    return (void*) obj;
}

void GeofenceMonitor_SetGeofenceParameters(void *obj, double *params){
    GeofenceMonitor *gf = (GeofenceMonitor*)obj;
    gf->SetGeofenceParameters(params);
}

void GeofenceMonitor_InputGeofenceData(void *obj, int type, int index, int totalVertices, double floor, double ceiling, double (*vertices)[2]){
    GeofenceMonitor *gf = (GeofenceMonitor*)obj;
    gf->InputGeofenceData(type, index, totalVertices, floor, ceiling, vertices);
}

bool GeofenceMonitor_CheckViolation(void * obj, double *position, double track, double groundSpeed, double verticalSpeed){
    GeofenceMonitor *gf = (GeofenceMonitor*)obj;
    return gf->CheckViolation(position, track, groundSpeed, verticalSpeed);
}

bool GeofenceMonitor_CheckWPFeasibility(void * obj, double * fromPosition, double * toPosition){
    GeofenceMonitor *gf = (GeofenceMonitor*)obj;
    return gf->CheckWPFeasibility(fromPosition, toPosition);
}

int GeofenceMonitor_GetNumConflicts(void * obj){
    GeofenceMonitor *gf = (GeofenceMonitor*)obj;
    return gf->GetNumConflicts();
}

void GeofenceMonitor_GetConflictStatus(void * obj, bool*conflictStatus){
    GeofenceMonitor *gf = (GeofenceMonitor*)obj;
    gf->GetConflictStatus(conflictStatus);
}

void GeofenceMonitor_GetConflict(void * obj, int id, int * fenceId, bool * conflict, bool * violation, double * recoveryPos, int * type){
    GeofenceMonitor *gf = (GeofenceMonitor*)obj;
    gf->GetConflict(id, *fenceId, *conflict, *violation, recoveryPos, *type);

}

void GeofenceMonitor_GetClosestRecoveryPoint(void * obj, double * currentPos, double *recoveryPos){
    GeofenceMonitor *gf = (GeofenceMonitor*)obj;
    gf->GetClosestRecoveryPoint(currentPos, recoveryPos);
}

void GeofenceMonitor_ClearFences(void * obj){
   GeofenceMonitor *gf = (GeofenceMonitor*)obj;
   gf->ClearFences();
}

void delete_GeofenceMonitor(void * obj){
   delete((GeofenceMonitor*)obj);
}

