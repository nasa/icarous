//
// Created by Swee Balachandran on 12/14/17.
//

#include "PathPlanner.h"
#include "DensityGrid.h"
#include "DensityGridAStarSearch.h"


int64_t PathPlanner::FindPathGridAstar(char planID[]) {

    double gridsize          = _astar_gridSize;
    double buffer            = gridsize * 2;
    double resolutionSpeed   = 1;
    double maxAlt            = maxCeiling;
    double Hthreshold        = obsbuffer;
    double altFence;

    BoundingRectangle BR;
    int totalfences = fenceList.size();
    for(int i=0;i<totalfences;++i){
        fence *gf = GetGeofence(i);
        if (gf->GetType() == KEEP_IN){
            altFence = gf->GetCeiling();
            for(int j=0;j<gf->GetSize();++j)
                BR.add(gf->GetPoly()->getVertex(j));
        }
    }

    NavPoint initpos(startPos,0);
    DensityGrid DG(BR,initpos,endPos,(int)buffer,gridsize,true);
    DG.snapToStart();
    DG.setWeights(5.0);

    for(int i=0;i<totalfences;++i){
        fence *gf = GetGeofence(i);
        if (gf->GetType() == KEEP_OUT){
            DG.setWeightsInside(*gf->GetPolyMod(),100.0);
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
        return -1;
    }

    Plan ResolutionPlan2 = ComputeGoAbovePlan(startPos,endPos,altFence,resolutionSpeed);

    double length1 = ResolutionPlan1.pathDistance();
    double length2 = ResolutionPlan2.pathDistance();

    if( (altFence >= maxAlt) ){
        length2 = MAXDOUBLE;
    }

    Plan* output;
    if(length1 < length2){
        output = &ResolutionPlan1;
    }else{
        output = &ResolutionPlan2;
    }

    output->setID(std::string(planID));
    flightPlans.push_back(*output);
    return output->size();
}
