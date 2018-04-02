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

double Bsplines::Nfac(double t,int i,int k){
    if (k==0){
        if ((t >= KntVec[i]) && (t < KntVec[i+1])){
            return 1;
        }else{
            return 0;
        }
    }else{
        double facA,facB;
        double facAN = t - KntVec[i];
        double facAD = KntVec[i+k] - KntVec[i];
        double facBN = KntVec[i+k+1] - t;
        double facBD = KntVec[i+k+1] - KntVec[i+1];
        fabs(facAD)<1e-5?facA=0:facA=facAN/facAD;
        fabs(facBD)<1e-5?facB=0:facB=facBN/facBD;
        double Nikm1   = Nfac(t,i,k-1);
        double Nip1km1 = Nfac(t,i+1,k-1);
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

double Bsplines::Objective2D(const double *x){
    double cost = 0;
    double oldVal[2] = {0.0,0.0};
    int numPts = ndim/2;
    for(int i=0;i<lenT;i++){
        double sumX = 0;
        double sumY = 0;
        for(int j=0;j<numPts;j++){
            double pt[2] = {x[2*j],x[2*j+1]};
            double nfac = Nfac(tVec[i],j,order);
            sumX += pt[0]*nfac;
            sumY += pt[1]*nfac;
        }

        double dist2obs = dist(obsloc[0],obsloc[1],sumX,sumY);
        double obsCost  = Beta(dist2obs);
        double pathlen;
        i>0?pathlen=dist(oldVal[0],oldVal[1],sumX,sumY):pathlen = 0;

        cost += pathlen*pathlen + obsCost;
        oldVal[0] = sumX;
        oldVal[1] = sumY;
    }

    return cost;
}

void Bsplines::ObsDerivative(const double* x0,double *grad){

    int numPts = ndim/2;

    for(int k=0;k<numPts;++k){

        double sumLPx = 0;
        double sumLPy = 0;
        double sumOPx = 0;
        double sumOPy = 0;

        for(int i=0;i<lenT-1;++i){
            double Ptx1 = 0;
            double Ptx2 = 0;
            double Pty1 = 0;
            double Pty2 = 0;
            for(int j=0;j<numPts;++j){
                double P1[2] = {x0[2*j],x0[2*j+1]};
                double nfac1 = Nfac(tVec[i],j,order);
                double nfac2 = Nfac(tVec[i+1],j,order);

                Ptx1 += nfac1*P1[0];
                Pty1 += nfac1*P1[1];

                Ptx2 += nfac2*P1[0];
                Pty2 += nfac2*P1[1];
            }
            sumLPx += 2*(Ptx2 - Ptx1)*(Nfac(tVec[i+1],k,order) - Nfac(tVec[i],k,order));
            sumLPy += 2*(Pty2 - Pty1)*(Nfac(tVec[i+1],k,order) - Nfac(tVec[i],k,order));
        }


        for(int i=0;i<lenT;++i){
            double Ptx1 = 0;
            double Pty1 = 0;

            for(int j=0;j<numPts;++j){
                double P1[2] = {x0[2*j],x0[2*j+1]};
                double nfac1 = Nfac(tVec[i],j,order);
                Ptx1 += nfac1*P1[0];
                Pty1 += nfac1*P1[1];
            }

            double dist2obs = dist(obsloc[0],obsloc[1],Ptx1,Pty1);
            double nfac  = Nfac(tVec[i],k,order);
            double dbeta = (1.0/OBS_THRESH - OBS_THRESH/(dist2obs*dist2obs));
            double dist2obsgradX = dbeta*(1/dist2obs)*(Ptx1-obsloc[0])*nfac;
            double dist2obsgradY = dbeta*(1/dist2obs)*(Pty1-obsloc[1])*nfac;

            if (dist2obs >= OBS_THRESH){
                dist2obsgradX = 0;
                dist2obsgradY = 0;
            }

            sumOPx += dist2obsgradX;
            sumOPy += dist2obsgradY;

        }

        grad[2*k] = sumLPx + sumOPx;
        grad[2*k+1] = sumLPy + sumOPy;

        if( (k==0) || (k==numPts-1)){
            grad[2*k] = 0;
            grad[2*k+1] = 0;
        }


    }
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