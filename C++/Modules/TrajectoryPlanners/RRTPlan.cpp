//
// Created by Swee Balachandran on 12/14/17.
//
#include "PathPlanner.h"
#include "RRTplanner.h"
#include "Dynamics.h"

int64_t PathPlanner::FindPathRRT(char planID[],double fromPosition[],double toPosition[],double trk,double gs,double vs){

    double maxInputNorm = fdata->paramData.getValue("RES_SPEED");

    // Reroute flight plan
    std::vector<Vect3> TrafficPos;
    std::vector<Vect3> TrafficVel;

    Position currentPos = Position::makeLatLonAlt(fromPosition[0], "degree",fromPosition[1], "degree", fromPosition[2], "m");
    Position endPos     = Position::makeLatLonAlt(toPosition[0], "degree", toPosition[1], "degree", toPosition[2], "m");
    Velocity currentVel = Velocity::makeTrkGsVs(trk,"degree",gs,"m/s",vs,"m/s");

    double computationTime = 1;

    EuclideanProjection proj = Projection::createProjection(currentPos.mkAlt(0));

    for(int i=0;i<fdata->GetTotalTraffic();++i){
        double x,y,z,vx,vy,vz;
        fdata->GetTraffic(i,&x,&y,&z,&vx,&vy,&vz);
        Velocity Vel = Velocity::makeVxyz(vx,vy,vz);
        Position Pos = Position::makeLatLonAlt(x,"degree",y,"degree",z,"m");
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

    int Nsteps = 2000;
    int dTsteps  = 5;
    double dT  = 1;
    double maxD = 5.0;

    Poly3D bbox;
    std::list<Poly3D> obstacleList;
    for(int i=0;i<fdata->GetTotalFences();++i){
        if (fdata->GetGeofence(i)->GetType() == KEEP_IN){
            bbox = fdata->GetGeofence(i)->GetPoly()->poly3D(proj);
        }else{
            obstacleList.push_back(fdata->GetGeofence(i)->GetPolyMod()->poly3D(proj));
        }
    }

    Vect3 initPosR3 = proj.project(currentPos);

    Vect3 gpos = proj.project(goalPos);
    node_t goal;
    goal.pos = gpos;

    string daaConfig = fdata->paramData.getString("DAA_CONFIG");

    RRTplanner RRT(bbox,dTsteps,dT,maxD,maxInputNorm,RRT_F,RRT_U,daaConfig.c_str());
    RRT.Initialize(initPosR3,currentVel,obstacleList,TrafficPos,TrafficVel,goal);


    bool goalFound = false;
    for(int i=0;i<Nsteps;i++){
        RRT.RRTStep();
        if(RRT.CheckGoal()){
            //printf("Goal found\n");
            goalFound = true;
            break;
        }
    }

    if(goalFound){
        Plan output;
        output = RRT.GetPlan(proj);
        output.setName(string(planID));
        flightPlans.push_back(output);
        return output.size();
    }else{
        return -1;
    }
}
