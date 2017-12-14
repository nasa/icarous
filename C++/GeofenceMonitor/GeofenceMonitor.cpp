//
// Created by Swee Balachandran on 12/7/17.
//

#include "GeofenceMonitor.h"

bool GeofenceMonitor::GeofenceMonitor(FlightData *fd) {
    fdata = fd;
}

bool GeofenceMonitor::CollisionDetection(Geofence* gf,Position* pos,Vect2* v,double startTime,double stopTime){
    int n = fdata->GetTotalFences();
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

void GeofenceMonitor::CheckViolation(double latitude,double longitude,double altitude,double gs,double trk,double vs){

    Position currentPosLLA = Position::makeLatLonAlt(latitude,"degree",longitude,"degree",altitude,"m");
    Velocity currentVel    = Velocity::makeTrkGsVs(trk,"degree",gs,"m/s",vs,"m/s");
    int n = fdata->GetTotalFences();
    double entryTime =-1.0, exitTime =-1.0;
    bool conflict = false;
    bool violation = false;
    Position recoveryPoint;
    conflictList.clear();
    GeofenceConflict gcf;

    double lookahead = fdata->paramData.getValue("LOOKAHEAD");
    double hthreshold = fdata->paramData.getValue("HTHRESHOLD");
    double vthreshold = fdata->paramData.getValue("VTHRESHOLD");
    double hstepback = fdata->paramData.getValue("HSTEPBACK");
    double vstepback = fdata->paramData.getValue("VSTEPBACK");


    for(int i = 0;i<n;i++) {
        Geofence *gf = fdata->GetGeofence(i);

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
            conflict = CollisionDetection(gf, &currentPosLLA, &vel, 0, lookahead);

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
            if (CollisionDetection(gf, &currentPosLLA, &vel, 0, lookahead)) {
                conflict = true;
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
            conflictList.push_back(gcf);
        }
    }
}


bool GeofenceMonitor::CheckWPFeasibility(double flatitude,double flongitude,double faltitude,
                                         double tlatitude,double tlongitude,double taltitude) {
    Position currentPos = Position::makeLatLonAlt(flatitude, "degree", flongitude, "degree", faltitude, "m");
    Position nextPos = Position::makeLatLonAlt(tlatitude, "degree", tlongitude, "degree", taltitude, "m");
    double heading2WP = currentPos.track(nextPos);

    // Velocity components assuming speed is 1 m/s
    double vy = cos(heading2WP);
    double vx = sin(heading2WP);
    Vect2 vel(vx, vy);

    bool val = false;
    for (int i = 0; i < fdata->GetTotalFences(); ++i) {
        val = val || CollisionDetection(fdata->GetGeofence(i),&currentPos, &vel, 0, 10000);
    }

    return !val;
}

int GeofenceMonitor::GetNumConflicts() {
    return conflictList.size();
}

void GeofenceMonitor::GetConflict(int id, int& fenceId, bool& conflict, bool& violation, double& recoveryPoint[]) {
    int count = -1;
    for(GeofenceConflict it:conflictList){
        count++;
        if (count == id){
            fenceId = it.fenceId;
            conflict = it.conflictstatus;
            violation = it.violationStatus;
            recoveryPoint[0] = it.recoveryPoint[0];
            recoveryPoint[1] = it.recoveryPoint[1];
            recoveryPoint[2] = it.recoveryPoint[2];
        }
    }
}