//
// Created by research133 on 3/27/18.
//

#ifndef BSPLINES_H
#define BSPLINES_H

#include "kdtree.h"
#include <list>

#define OBS_THRESH 3.0

class Bsplines {
private:
    double *tVec;
    double *KntVec;
    int order;
    int lenT;
    int lenK;
    int ndim;
    double obsloc[3];

public:
    double *ctrlPt0;
    std::list<KDTREE*> kdtreeList;
    std::list<KDTREE*>::iterator it;
    Bsplines(){};
    Bsplines(int ndim,int lent,double* tVec,int lenk,double* KntVec,int order);
    void SetSplineProperties(int ndim,int lent,double* tVec,int lenk,double* KntVec,int order);
    void SetInitControlPts(double* ctrlPt);
    void SetObstacles(double x,double y,double z);
    double Beta(double x);
    double Nfac(double t,int i,int k);
    double dist(double x1,double y1,double x2,double y2);
    double Objective2D(const double *x);
    double HdgConstraint(double *x);
    void ObsDerivative(const double* x0,double *grad);
};


#endif //BSPLINES_BSPLINES_H
