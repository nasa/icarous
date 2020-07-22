#include <iostream>
#include <fstream>


#include <math.h>
#include <nlopt.h>
#include "Bsplines.hpp"


double objfunc(unsigned n,const double *x,double * grad,void* data);

int main() {

    Bsplines splinePath;
    int ndim = 8;

    double KnotVec[8] = {0,0,0,0,4,4,4,4};
    double tVec[40];

    for(int i=0;i<40;i++){
        tVec[i] = i*0.1;
    }

    double CtrlPt0[8] = {0.0,1.0,
                          72.0,72.0,
                          27.0,27.0,
                          90.0,90.0};

    larcfm::Poly2D _obs1;
    _obs1.addVertex(40,40);
    _obs1.addVertex(60,40);
    _obs1.addVertex(60,60);
    _obs1.addVertex(40,60);
    larcfm::Poly3D obs1(_obs1,-10,10);

    splinePath.SetSplineProperties(8,40,tVec,8,KnotVec,3);
    splinePath.SetObstacleProperties(4,4,20,100);
    splinePath.SetInitControlPts(CtrlPt0);
    splinePath.SetObstacles(obs1);

    double gradient[8] = {0,0,0,0,0,0,0,0};

    double lb[8] = {0.0,0.0,0.0,0.0,
                    0.0,0.0,0.0,0.0};

    double ub[8] = {100.0,100.0,100.0,100.0,
                    100.0,100.0,100.0,100.0};

    nlopt_opt opt;
    opt = nlopt_create(NLOPT_LD_SLSQP,8);
    //nlopt_set_lower_bounds(opt,lb);
    //nlopt_set_upper_bounds(opt,ub);
    nlopt_set_min_objective(opt,objfunc,&splinePath);

    nlopt_set_xtol_rel(opt, 1e-4);

    double minf;
    if (nlopt_optimize(opt, CtrlPt0, &minf) < 0) {
        printf("nlopt failed!\n");
    }
    else {
        printf("found minimum\n");
        for(int i=0;i<8;++i){
            printf("%f\n",CtrlPt0[i]);
        }
    }
}

double objfunc(unsigned n,const double *x,double *grad,void* data) {
    Bsplines *_bsplines = (Bsplines *) data;

    double fval = _bsplines->Objective2D(x);

    if (grad) {
        _bsplines->Gradient2D(x, grad);
    }
    return fval;
}