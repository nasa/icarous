#include <iostream>
#include <fstream>
#include <LinearConstraint.h>
#include <LinearEquation.h>
#include <OptQNIPS.h>
#include <OptFDNIPS.h>
#include <BoundConstraint.h>
#include <NonLinearInequality.h>
#include <OptPDS.h>

#include "NLF.h"
#include "OptQNewton.h"
#include "Bsplines.h"

#define OBS_THRESH 3.0

using NEWMAT::ColumnVector;

void initObj(int ndim,ColumnVector& x);
void Objective(int ndim,const ColumnVector& x,double &fx,int& result);
void hConstraint(int ndim,const ColumnVector& x,ColumnVector &cx,int& result);

Bsplines splinePath;

int main() {

    int ndim = 8;

    double KnotVec[8] = {0,0,0,0,4,4,4,4};
    double tVec[400];

    for(int i=0;i<400;i++){
        tVec[i] = i*0.01;
    }

    double CtrlPt0[8] = {0.0,1.0,
                          3.0,4.0,
                          4.0,2.0,
                          7.0,3.0};

    splinePath.SetSplineProperties(8,400,tVec,8,KnotVec,3);
    splinePath.SetInitControlPts(CtrlPt0);
    splinePath.SetObstacles(5.0,3.0,0.0);

    NEWMAT::Matrix Alhs(4,8);
    NEWMAT::ColumnVector rhs(4);

    for(int i=1;i<=4;i++){
        for(int j=1;j<=8;j++){
            Alhs(i,j) = 0;
        }
    }

    Alhs(1,1) = 1;
    Alhs(2,2) = 1;
    Alhs(3,7) = 1;
    Alhs(4,8) = 1;

    rhs(1) = 0.0;
    rhs(2) = 1.0;
    rhs(3) = 7.0;
    rhs(4) = 3.0;

    ColumnVector lower(ndim), upper(ndim);

    for(int i=1;i<=ndim;++i){
        lower(i) = 0;
        upper(i) = 10;
    }

    OPTPP::Constraint c1 = new OPTPP::BoundConstraint(ndim, lower, upper);

    OPTPP::Constraint lcon = new OPTPP::LinearEquation(Alhs,rhs);


    OPTPP::OptppArray<OPTPP::Constraint> conArray;
    conArray.append(c1);
    conArray.append(lcon);
    OPTPP::CompoundConstraint* constraints = new OPTPP::CompoundConstraint(conArray);

    OPTPP::NLF0 nlp(ndim, Objective, initObj,constraints);

    OPTPP::OptPDS objfcn(&nlp);

    //objfcn.setSearchStrategy(OPTPP::TrustRegion);
    objfcn.setMaxFeval(5000);
    objfcn.setFcnTol(1.e-2);
    objfcn.setMaxBacktrackIter(25);

    // The "0" in the second argument says to create a new file.  A "1"
    // would signify appending to an existing file.

    if (!objfcn.setOutputFile("example1.out", 0))
        cerr << "main: output file open failed" << endl;

    objfcn.optimize();

    objfcn.printStatus("Solution from quasi-newton");

    ColumnVector x_sol = nlp.getXc();

    for(int i=1;i<=4;i++){
        printf("%f,%f\n",x_sol(2*(i-1) + 1),x_sol(2*(i-1) + 2));
    }

    objfcn.cleanup();

}

void initObj(int ndim,ColumnVector& x){

    if(ndim != 8){
        printf("problem dimension is wrong\n");
        exit (1);
    }

    // column vectors indexed from 1
    for(int i=1;i<=ndim/2;++i) {
        x(i) = splinePath.ctrlPt0[i];
    }
}

void Objective(int ndim,const ColumnVector& x,double &fx,int& result){

    double X[25];
    memset(X,0,sizeof(double)*25);
    // column vectors indexed from 1
    for(int i=1;i<=ndim;i++){
        X[i-1] = x(i);
    }

    fx = splinePath.Objective2D(X);
    result = OPTPP::NLPFunction;
}
