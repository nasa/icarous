//
// Created by research133 on 4/3/18.
//
#include "PathPlanner.h"
#include "Bsplines.h"
#include <nlopt.h>

double objfunc(unsigned n,const double *x,double *grad,void* data) {
    Bsplines *_bsplines = (Bsplines *) data;

    double fval = _bsplines->Objective2D(x);

    if (grad) {
        _bsplines->Gradient2D(x, grad);
    }
    return fval;
}

void PathPlanner::InitializeBSplinesParameters(bool enable3D,double computationT,int numControlPts,int lenTVec) {
    _bsplines_enable3D = enable3D;
    _bsplines_compTime = computationT;
    _bsplines_numControlPts = numControlPts;
    _bsplines_lenTVec = lenTVec;
}

int64_t PathPlanner::FindPathBSplines(char planID[]){

    // Reroute flight plan
    std::vector<Vect3> TrafficPos;
    std::vector<Vect3> TrafficVel;


    EuclideanProjection proj = Projection::createProjection(startPos.mkAlt(0));

    Plan currentFP;
    Position prevWP;
    Position nextWP;
    double computationTime = 1;
    double dist = startVel.gs()*computationTime;

    Position start = startPos.linearDist2D(startVel.trk(), dist);
    Position goalPos = endPos;

    Vect3 initPosR3 = proj.project(startPos);
    Vect3 gpos = proj.project(goalPos);

    Bsplines splinePath;
    int ndim = 8;
    int lenT = 40;

    double KnotVec[8] = {0,0,0,0,4,4,4,4};
    double tVec[40];

    for(int i=0;i<lenT;i++){
        tVec[i] = i*0.1;
    }

    Vect3 heading2goal = (gpos - initPosR3);
    double dist2goal = heading2goal.norm();
    heading2goal = heading2goal*(1/dist2goal);

    double CtrlPt0[8] = {initPosR3.x,initPosR3.y,
                         0.7*initPosR3.x + 0.3*gpos.x,0.7*initPosR3.y + 0.3*gpos.y,
                         0.3*initPosR3.x + 0.7*gpos.x,0.3*initPosR3.y + 0.7*gpos.y,
                         gpos.x,gpos.y};

    int n = 0;
    for(int i=1;i<(ndim/2)-1;++i){
        CtrlPt0[2*i]   = initPosR3.x + i*dist2goal/(ndim/2) * heading2goal.x;
        CtrlPt0[2*i+1] = initPosR3.x + i*dist2goal/(ndim/2) * heading2goal.y;
    }

    splinePath.SetSplineProperties(8,40,tVec,8,KnotVec,3);
    splinePath.SetObstacleProperties(3,50,5,30);
    splinePath.SetInitControlPts(CtrlPt0);

    for(fence gf: fenceList){
        if (gf.GetType() == KEEP_OUT){
            Poly3D obs = gf.GetPolyMod()->poly3D(proj);
            splinePath.SetObstacles(obs);
        }
    }

    double gradient[8] = {0,0,0,0,0,0,0,0};

    nlopt_opt opt;
    opt = nlopt_create(NLOPT_LD_SLSQP,8);

    nlopt_set_min_objective(opt,objfunc,&splinePath);

    nlopt_set_xtol_rel(opt, 1e-4);

    double minf;
    if (nlopt_optimize(opt, CtrlPt0, &minf) < 0) {
        printf("nlopt failed!\n");
        return -1;
    }
    else {
        //printf("found minimum\n");
        Plan output;
        int count = 0;
        double ETA = 0.0;
        double speed = 1.0; //TODO: maybe change this speed to something based on mission?
        for(int i=0;i<lenT;++i){
            double point[2];
            splinePath.GetPoint(tVec[i],CtrlPt0,point);
            Vect3 _locxyz(point[0],point[1],initPosR3.z);
            Position wp(proj.inverse(_locxyz));
            if(count == 0){
                ETA = 0;
            }
            else{
                Position prevWP = output.point(count-1).position();
                double distH    = wp.distanceH(prevWP);
                ETA             = ETA + distH/speed;
            }
            NavPoint np(wp,ETA);
            output.addNavPoint(np);
            count++;
        }
        output.setName(string(planID));
        flightPlans.push_back(output);
        return output.size();
    }
}

