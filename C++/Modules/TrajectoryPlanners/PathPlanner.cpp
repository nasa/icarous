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
    for(Plan pl: outputPlans){
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
    for(Plan pl: outputPlans){
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