//
// Created by Swee Balachandran on 12/14/17.
//
#include <cstring>
#include "PathPlanner.h"

using namespace std;

PathPlanner::PathPlanner(FlightData* fd) {
    fdata = fd;
}

int64_t PathPlanner::FindPath(algorithm search, char *planID, double *fromPosition, double *toPosition,
                              double trk, double gs,double vs) {

    int64_t retval = -1;
    switch(search){

        case _ASTAR_:
            retval = FindPathAstar(planID,fromPosition,toPosition);
            break;
        case _RRT_:
            retval = FindPathRRT(planID,fromPosition,toPosition,trk,gs,vs);
            break;
        default:
            break;
    }

    return retval;
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

void PathPlanner::GetWaypoint(char *planID, int wpID, double *waypoint) {
    for(Plan pl: flightPlans){
        if(strcmp(pl.getName().c_str(),planID)){
            continue;
        }else{
            Position pos = pl.getPos(wpID);
            waypoint[0] = pos.latitude();
            waypoint[1] = pos.longitude();
            waypoint[2] = pos.alt();
            break;
        }
    }
}

void PathPlanner::OutputFlightPlan(ENUProjection* proj,char* planID,char* fenceFile,char* waypointFile){
    for(Plan pl: flightPlans){
        if(strcmp(pl.getName().c_str(),planID)){
            continue;
        }else {
            std::ofstream fp1;
            std::ofstream fp2;

            if (pl.size() > 0)
                fp1.open(waypointFile);
            else
                break;

            int fenceSize = fdata->GetTotalFences();
            if ( fenceSize > 0) {
                fp2.open(fenceFile);
                for(int i=0;i<fenceSize;i++){
                    fence *gf = fdata->GetGeofence(i);
                    for(int j=0;j<gf->GetSize();j++){
                        Vect3 loc = proj->project(gf->GetPolyMod()->getVertex(j));
                        fp2 << gf->GetID() <<" "<<loc.x<<" "<<
                                                  loc.y<<std::endl;
                    }

                }
                fp2.close();
            }

            for(int i=0;i<pl.size();i++){
                Position pos = pl.getPos(i);
                Vect3 X = proj->project(pos);
                fp1 << X.x <<" "<<X.y<<" "<<X.z<<std::endl;
            }

            fp1.close();

            break;
        }
    }
}

int PathPlanner::GetTotalWaypoints(char planID[]){
    std::list<Plan>::iterator it;
    for(it=flightPlans.begin();it != flightPlans.end(); ++ it){
        if (strcmp(it->getName().c_str(),planID)){
            continue;
        }
        return it->size();
    }
}

void PathPlanner::InputFlightPlan(char planID[],int wpID,double waypoint[],double speed){
    Position pos = Position::makeLatLonAlt(waypoint[0],"degree",waypoint[1],"degree",waypoint[2],"m");
    std::list<Plan>::iterator it;
    for(it=flightPlans.begin();it != flightPlans.end(); ++ it){
        if (strcmp(it->getName().c_str(),planID)){
            continue;
        }
        it->add(pos,(double)wpID);
        return;
    }

    string planName = planID;
    Plan newPlan(planName);
    newPlan.add(pos,(double)wpID);
    flightPlans.push_back(newPlan);
    return;
}

double PathPlanner::Dist2Waypoint(double currPosition[],double nextPosition[]){
    Position A = Position::makeLatLonAlt(currPosition[0],"degree",currPosition[1],"degree",currPosition[2],"m");
    Position B = Position::makeLatLonAlt(nextPosition[0],"degree",nextPosition[1],"degree",nextPosition[2],"m");
    return A.distanceH(B);
}