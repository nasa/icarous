//
// Created by Swee Balachandran on 12/14/17.
//
#include "PathPlanner.h"
#include "RRTplanner.h"
#include "Dynamics.h"


void PathPlanner::InitializeRRTParameters(double resSpeed,int Nsteps,double dt,int Dt,double capR,char daaConfig[]){
   _rrt_resSpeed = resSpeed;
   _rrt_daaConfig = string(daaConfig);
   _rrt_maxIterations = Nsteps;
   _rrt_dt = dt;
   _rrt_macroSteps = Dt;
   _rrt_goalCaptureRadius = capR;
}

void PathPlanner::UpdateRRTParameters(double resSpeed, int Nsteps, double dt, int Dt, double capR, char *daaConfig) {
    _rrt_resSpeed = resSpeed;
   _rrt_daaConfig = string(daaConfig);
   _rrt_maxIterations = Nsteps;
   _rrt_dt = dt;
   _rrt_macroSteps = Dt;
   _rrt_goalCaptureRadius = capR;
}

int64_t PathPlanner::FindPathRRT(char planID[],double fromPosition[],double toPosition[],double velocity[]){

    double maxInputNorm = _rrt_resSpeed;
    double trk = velocity[0];
    double gs = velocity[1];
    double vs = velocity[2];

    // Reroute flight plan
    std::vector<Vect3> TrafficPos;
    std::vector<Vect3> TrafficVel;

    Position currentPos = Position::makeLatLonAlt(fromPosition[0], "degree",fromPosition[1], "degree", fromPosition[2], "m");
    Position endPos     = Position::makeLatLonAlt(toPosition[0], "degree", toPosition[1], "degree", toPosition[2], "m");
    Velocity currentVel = Velocity::makeTrkGsVs(trk,"degree",gs,"m/s",vs,"m/s");

    LogInput(currentPos,endPos,currentVel);

    double computationTime = 3;

    EuclideanProjection proj = Projection::createProjection(currentPos.mkAlt(0));

    for(GenericObject traffic: trafficList){
        Velocity Vel = traffic.vel;
        Position Pos = traffic.pos;
        Vect3 tPos = proj.project(Pos);
        Vect3 tVel = Vect3(Vel.x,Vel.y,Vel.z);
        tPos.linear(tVel,computationTime);
        TrafficPos.push_back(tPos);
        TrafficVel.push_back(tVel);
    }

    Plan currentFP;
    Position prevWP;
    Position nextWP;
    double dist = currentVel.gs()*computationTime;

    Position start = currentPos.linearDist2D(currentVel.trk(), dist);

    Position goalPos = endPos;

    int Nsteps = _rrt_maxIterations;
    int dTsteps  = _rrt_macroSteps;
    double dT  = _rrt_dt;
    double maxD = _rrt_goalCaptureRadius;

    Poly3D bbox;
    std::list<Poly3D> obstacleList;
    for(fence gf: fenceList){
        if (gf.GetType() == KEEP_IN){
            bbox = gf.GetPolyMod()->poly3D(proj);
        }else{
            obstacleList.push_back(gf.GetPolyMod()->poly3D(proj));
        }
    }

    Vect3 initPosR3 = proj.project(currentPos);

    Vect3 gpos = proj.project(goalPos);
    node_t goal;
    goal.pos = gpos;


    RRTplanner RRT(bbox,dTsteps,dT,maxD,maxInputNorm,RRT_F,RRT_U,_rrt_daaConfig.c_str());
    RRT.Initialize(initPosR3,currentVel,obstacleList,TrafficPos,TrafficVel,goal);
    RRT.SetDAAParameters(daaParameters);

    bool goalFound = false;
    for(int i=0;i<Nsteps;i++){
        RRT.RRTStep();
        if(RRT.CheckGoal()){
            //printf("Goal found : %lu\n",RRT.GetNodeList()->size());
            printf("Iterations: %d,Tree size: %lu\n",i,RRT.GetNodeList()->size());
            goalFound = true;
            break;
        }else if(i==Nsteps-1){
            printf("Reached iteration limit\n");
            printf("Tree size: %lu\n",RRT.GetNodeList()->size());
        }
    }

    if(goalFound){
        Plan output;
        RRT.GetPlan(proj,output);
        output.setID(string(planID));
        flightPlans.push_back(output);
        return output.size();
    }else{
        return -1;
    }
}
