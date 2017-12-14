//
// Created by Swee Balachandran on 12/14/17.
//
#include "PathPlanner.h"
#include "SeparatedInput.h"
#include "DensityGrid.h"
#include "DensityGridAStarSearch.h"
#include "RRTplanner.h"
using namespace std;

void PathPlanner::PathPlanner(string filename) {


}

void PathPlanner::FindPathAstar(double flatitude, double flongitude, double faltitude,
                                  double tlatitude, double tlongitude, double taltitude) {

    double gridsize          = paramData.getValue("GRIDSIZE");
    double buffer            = paramData.getValue("BUFFER");
    double lookahead         = paramData.getValue("LOOKAHEAD");
    double resolutionSpeed   = paramData.getValue("RES_SPEED");
    double maxAlt            = paramData.getValue("MAX_CEILING");
    double Hthreshold        = paramData.getValue("HTHRESHOLD");
    double altFence          = maxAlt - 1;

    Position startPos = Position::makeLatLonAlt(flatitude,"degree",flongitude,"degree",faltitude,"m");
    Position endPos   = Position::makeLatLonAlt(tlatitude,"degree",flongitude,"degree",faltitude,"m");

    BoundingRectangle BR;
    int totalfences = fdata->GetTotalFences();
    for(int i=0;i<totalfences;++i){
        Geofence *gf = fdata->GetGeofence(i);
        if (gf->GetType() == KEEP_IN){
            for(int j=0;j<gf->GetSize();++j)
                BR.add(gf->GetPoly()->getVertex(j));
        }
    }

    NavPoint initpos(startPos,0);
    DensityGrid DG(BR,initpos,endPos,(int)buffer,gridsize,true);
    DG.snapToStart();
    DG.setWeights(5.0);

    for(int i=0;i<totalfences;++i){
        Geofence *gf = fdata->GetGeofence(i);
        if (gf->GetType() == KEEP_OUT){
            DG.setWeightsInside(*gf->GetPoly(),100.0);
        }
    }

    DensityGridAStarSearch DGAstar;
    std::vector<std::pair<int,int>> GridPath = DGAstar.optimalPath(DG);
    std::vector<std::pair<int,int>>::iterator gpit;

    Plan ResolutionPlan1;
    //Create a plan out of the grid points
    if(!GridPath.empty()) {
        std::list<Position> PlanPosition;
        double currHeading = 0.0;
        double nextHeading = 0.0;

        // Reduce the waypoints based on heading
        PlanPosition.push_back(startPos);
        double startAlt = startPos.alt();

        for (gpit = GridPath.begin(); gpit != GridPath.end(); ++gpit) {
            Position pos1 = DG.getPosition(*gpit);

            if (gpit == GridPath.begin()) {
                ++gpit;
                Position pos2 = DG.getPosition(*gpit);
                --gpit;
                currHeading = pos1.track(pos2);
                continue;
            }

            if (++gpit == GridPath.end()) {
                --gpit;
                PlanPosition.push_back(pos1.mkAlt(startAlt));
                break;
            } else {
                Position pos2 = DG.getPosition(*gpit);
                --gpit;
                nextHeading = pos1.track(pos2);
                if (std::abs(nextHeading - currHeading) > 0.01) {
                    PlanPosition.push_back(pos1.mkAlt(startAlt));
                    currHeading = nextHeading;
                }
            }

        }
        PlanPosition.push_back(endPos);

        double ETA = 0.0;
        NavPoint wp0(PlanPosition.front(), 0);
        ResolutionPlan1.addNavPoint(wp0);

        int count = 0;
        std::list<Position>::iterator it;
        for (it = PlanPosition.begin(); it != PlanPosition.end(); ++it) {
            Position pos = *it;
            if (count == 0) {
                ETA = 0;
            } else {
                Position prevWP = ResolutionPlan1.point(count - 1).position();
                double distH = pos.distanceH(prevWP);
                ETA = ETA + distH / resolutionSpeed;
            }
            NavPoint np(pos, ETA);
            ResolutionPlan1.addNavPoint(np);
            count++;
        }
    }else{
        std::cout<<"grid path is empty"<<std::endl;
    }

    Plan ResolutionPlan2 = ComputeGoAbovePlan(startPos,endPos,altFence,resolutionSpeed);

    double length1 = ResolutionPlan1.pathDistance();
    double length2 = ResolutionPlan2.pathDistance();

    if( (altFence >= maxAlt) ){
        length2 = MAXDOUBLE;
    }
}

Plan PathPlanner::ComputeGoAbovePlan(Position start,Position goal,double altFence,double rSpeed){
    // Compute go above plan
    Plan ResolutionPlan2;
    double ETA = 0;
    double distH,distV;

    NavPoint nvpt1(start,ETA);
    ResolutionPlan2.addNavPoint(nvpt1);

    // Second waypoint directly above WP1
    Position wp2 = start.mkAlt(altFence+1);
    distV = wp2.distanceV(start);
    ETA = ETA + distV/rSpeed;
    NavPoint nvpt2(wp2,ETA);
    ResolutionPlan2.addNavPoint(nvpt2);

    // Third waypoint directly above exit point
    Position wp3 = goal.mkAlt(altFence+1);
    distH = wp3.distanceH(wp2);
    ETA = ETA + distH/rSpeed;
    NavPoint nvpt3(wp3,ETA);
    ResolutionPlan2.addNavPoint(nvpt3);

    // Final waypoint
    distV = goal.distanceH(wp3);
    ETA = ETA + distV/rSpeed;
    NavPoint nvpt4(goal,ETA);
    ResolutionPlan2.addNavPoint(nvpt4);

    return ResolutionPlan2;
}

void PathPlanner::FindPathRRT(double flatitude,double flongitude,double faltitude,
                 double tlatitude, double tlongitude,double taltitude,
                 double trk,double gs,double vs){
    double maxInputNorm = fdata->paramData.getValue("RES_SPEED");

    // Reroute flight plan

    std::vector<Vect3> TrafficPos;
    std::vector<Vect3> TrafficVel;

    Position currentPos = Position::makeLatLonAlt(flatitude, "degree", flongitude, "degree", faltitude, "m");
    Position endPos     = Position::makeLatLonAlt(tlatitude, "degree", tlongitude, "degree", taltitude, "m");
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

    int Nsteps = 500;
    int dTsteps  = 5;
    double dT  = 1;
    double maxD = 5.0;




    Poly3D bbox;
    std::list<Poly3D> obstacleList;
    for(int i=0;i<fdata->GetTotalFences();++i){
        if (fdata->GetGeofence(i)->GetType() == KEEP_IN){
            bbox = fdata->GetGeofence(i)->GetPoly()->poly3D(proj);
        }else{
            obstacleList.push_back(fdata->GetGeofence(i)->GetPoly()->poly3D(proj));
        }
    }

    Vect3 initPosR3 = proj.project(currentPos);

    Vect3 gpos = proj.project(goalPos);
    node_t goal;
    goal.pos = gpos;


    RRTplanner RRT(bbox,dTsteps,dT,maxD,maxInputNorm,F,U,"../DaidalusQuadConfig.txt");
    RRT.Initialize(initPosR3,currentVel,obstacleList,TrafficPos,TrafficVel,goal);
    RRT.SetGoal(goal);

    for(int i=0;i<Nsteps;i++){
        RRT.RRTStep();
        if(RRT.CheckGoal()){
            //printf("Goal found\n");
            break;
        }
    }
}

