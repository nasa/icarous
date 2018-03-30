//
// Created by research133 on 3/27/18.
//

#include "Bsplines.h"
#include "math.h"


Bsplines::Bsplines(int _ndim,int lent, double *_tVec, int lenk, double *_KntVec, int _order) {
    lenT = lent;
    lenK = lenk;
    tVec = _tVec;
    KntVec = _KntVec;
    order = _order;
    ndim = _ndim;
}

void Bsplines::SetSplineProperties(int _ndim, int _lenT, double *_tVec, int _lenK, double *_KntVec, int _order) {
    lenT = _lenT;
    lenK = _lenK;
    tVec = _tVec;
    KntVec = _KntVec;
    order = _order;
    ndim = _ndim;
}

void Bsplines::SetInitControlPts(double *ctrlPt) {
    ctrlPt0 = ctrlPt;
}

void Bsplines::SetObstacles(double x, double y, double z) {
    obsloc[0] = x;
    obsloc[1] = y;
}

double Bsplines::Nfac(double t,double* knotVec,int i,int k){
    if (k==0){
        if ((t >= knotVec[i]) && (t < knotVec[i+1])){
            return 1;
        }else{
            return 0;
        }
    }else{
        double facA,facB;
        double facAN = t - knotVec[i];
        double facAD = knotVec[i+k] - knotVec[i];
        double facBN = knotVec[i+k+1] - t;
        double facBD = knotVec[i+1];
        fabs(facAD)<1e-5?facA=0:facA=facAN/facAD;
        fabs(facBD)<1e-5?facB=0:facB=facBN/facBD;
        double Nikm1   = Nfac(t,knotVec,i,k-1);
        double Nip1km1 = Nfac(t,knotVec,i+1,k-1);
        return facA*Nikm1 + facB*Nip1km1;
    };
}

double Bsplines::Beta(double x){
    double val;
    x>=OBS_THRESH?val=0:val=x*1.0/OBS_THRESH + (1.0/x)*OBS_THRESH - 2.0;
    return val;
}

double Bsplines::dist(double x1,double y1,double x2,double y2){
    return sqrt( pow((x1 - x2),2) + pow((y1 - y2),2));
}

double Bsplines::Objective2D(double *x){
    double cost = 0;
    double oldVal[2] = {0.0,0.0};
    int numPts = ndim/2;
    for(int i=0;i<lenT;i++){
        double sumX = 0;
        double sumY = 0;
        for(int j=0;j<numPts;j++){
            double pt[2] = {x[2*j],x[2*j+1]};
            double nfac = Nfac(tVec[i],KntVec,j,order);
            sumX += pt[0]*nfac;
            sumY += pt[1]*nfac;
        }

        double dist2obs = dist(obsloc[0],obsloc[1],sumX,sumY);
        double obsCost  = Beta(dist2obs);
        double pathlen;
        i>0?pathlen=dist(oldVal[0],oldVal[1],sumX,sumY):pathlen = 0;

        cost += pathlen + dist2obs;
        oldVal[0] = sumX;
        oldVal[1] = sumY;
    }



    return cost;
}

void Bsplines::ObsDerivative(double *x){
    
}

double Bsplines::HdgConstraint(double *x) {
    double hdgChange = 0;
    int numPts = ndim/2;
    for(int j=0;j<numPts-2;j++){
        double A[2] = {x[2*j],x[2*j+1]};
        double B[2] = {x[2*j+2],x[2*j+3]};
        double C[2] = {x[2*j+4],x[2*j+5]};

        double val1 = 360 + atan2(B[1] - A[1],B[0]-A[0])*180/M_PI;
        double val2 = 360 + atan2(C[1] - B[1],C[0]-B[0])*180/M_PI;
        double hdg1 = fmod(val1,360);
        double hdg2 = fmod(val2,360);

        hdgChange += fabs(hdg2 - hdg1);
    }

    return hdgChange;
}