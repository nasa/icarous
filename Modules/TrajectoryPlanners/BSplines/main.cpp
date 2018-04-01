#include <iostream>
#include <fstream>

#include "Bsplines.h"

Bsplines splinePath;

int main() {

    int ndim = 8;

    double KnotVec[8] = {0,0,0,0,4,4,4,4};
    double tVec[40];

    for(int i=0;i<40;i++){
        tVec[i] = i*0.1;
    }

    double CtrlPt0[8] = {0.0,1.0,
                          3.0,4.0,
                          4.0,2.0,
                          7.0,3.0};

    double CtrlPt1[8] = {0.0+0.000001,1.0,
                         3.0,4.0,
                         4.0,2.0,
                         7.0,3.0};

    splinePath.SetSplineProperties(8,40,tVec,8,KnotVec,3);
    splinePath.SetInitControlPts(CtrlPt0);
    splinePath.SetObstacles(5.0,3.0,0.0);

    double gradient[8] = {0,0,0,0,0,0,0,0};

    double val1 = splinePath.Objective2D(CtrlPt0);
    double val2 = splinePath.Objective2D(CtrlPt1);
    double fgrad = (val2- val1)/0.000001;

    splinePath.ObsDerivative(CtrlPt0,gradient);
    double val = splinePath.Nfac(3.9,0,3);
    printf("nfac %f\n",val);
    printf("cost: %f,%f\n",val1,val2);
    printf("gradients: %f,%f\n",fgrad,gradient[0]);
}

