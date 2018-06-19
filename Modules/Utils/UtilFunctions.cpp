//
// Created by swee on 6/24/18.
//

#include "UtilFunctions.h"
#include <Position.h>
#include <Velocity.h>
#include <math.h>

using namespace larcfm;

double ComputeDistance(double positionA[],double positionB[]){

    Position A = Position::makeLatLonAlt(positionA[0],"degree",positionA[1],"degree",positionA[2],"m/s");
    Position B = Position::makeLatLonAlt(positionB[0],"degree",positionB[1],"degree",positionB[2],"m/s");

    return A.distanceH(B);
}

void ConvertVnedToTrkGsVs(double vn,double ve,double vz,double *Trk,double *Gs,double *Vs){
   double angle = 360 + atan2(ve, vn) * 180 / M_PI;
   *Trk = fmod(angle, 360);
   *Gs = sqrt(pow(vn, 2) + pow(ve, 2));
   *Vs = vz;
}

void ConvertTrkGsVsToVned(double Trk,double Gs,double Vs,double *vn,double *ve,double *vd){
    Velocity vel = Velocity::makeTrkGsVs(Trk, "degree", Gs, "m/s", Vs, "m/s");
    *vn = vel.y;
    *ve = vel.x;
    *vd = vel.z;
}

bool CheckTurnConflict(double low, double high, double newHeading, double oldHeading) {
    if(newHeading < 0){
        newHeading = 360 + newHeading;
    }

    if(oldHeading < 0){
        oldHeading = 360 + oldHeading;
    }

    // Get direction of turn
    double psi   = newHeading - oldHeading;
    double psi_c = 360 - std::abs(psi);
    bool rightTurn = false;
    if(psi > 0){
        if(std::abs(psi) > std::abs(psi_c)){
            rightTurn = false;
        }
        else{
            rightTurn = true;
        }
    }else{
        if(std::abs(psi) > std::abs(psi_c)){
            rightTurn = true;
        }
        else{
            rightTurn = false;
        }
    }

    double A,B,X,Y,diff;
    if(rightTurn){
        diff = oldHeading;
        A = oldHeading - diff;
        B = newHeading - diff;
        X = low - diff;
        Y = high - diff;

        if(B < 0){
            B = 360 + B;
        }

        if(X < 0){
            X = 360 + X;
        }

        if(Y < 0){
            Y = 360 + Y;
        }

        if(A < X && B > Y){
            return true;
        }
    }else{
        diff = 360 - oldHeading;
        A    = oldHeading + diff;
        B    = newHeading + diff;
        X = low + diff;
        Y = high + diff;

        if(B > 360){
            B = B - 360;
        }

        if(X > 360){
            X = X - 360;
        }

        if(Y > 360){
            Y = Y - 360;
        }

        if(A > Y && B < X){
            return true;
        }
    }

    return false;
}
