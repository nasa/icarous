//
// Created by Swee Balachandran on 12/14/17.
//
#include <cstring>
#include <list>
#include <string>
#include <sstream>
#include <PlanWriter.h>
#include <PlanReader.h>
#include <PlanUtil.h>
#include <sys/time.h>
#include <TrajGen.h>

#include "TrajManager.h"
#include "TrajManager.hpp"
#include "WP2Plan.hpp"

TrajManager::TrajManager(std::string callsign) {
    char            fmt1[64];
    struct timespec  tv;
    numPlans = 0;
    clock_gettime(CLOCK_REALTIME,&tv);
    double localT = tv.tv_sec + static_cast<float>(tv.tv_nsec)/1E9;
    sprintf(fmt1,"log/Path-%s-%f.log",callsign.c_str(), localT);
    log.open(fmt1);
}

void TrajManager::InputGeofenceData(int type,int index, int totalVertices, double floor, double ceiling, double pos[][2]){

    fenceObject fence;
    std::vector<larcfm::Position> vertices;
    for(int i=0;i<totalVertices;++i) {
        larcfm::Position vertex = larcfm::Position::makeLatLonAlt(pos[i][0],"degree",pos[i][1],"degree",0,"m");
        vertices.push_back(vertex);
    }
    fence.fenceType = type;
    fence.id = index;
    fence.polygon = larcfm::SimplePoly::mk(vertices,floor,ceiling);
    fenceList.push_back(fence);
}

void TrajManager::InputGeofenceData(int type,int index, int totalVertices, double floor, double ceiling, std::vector<larcfm::Position> &vertices){
    fenceObject fence;
    fence.fenceType = type;
    fence.id = index;
    fence.polygon = larcfm::SimplePoly::mk(vertices,floor,ceiling);
    fenceList.push_back(fence);
}


void TrajManager::ClearFences() {
    fenceList.clear();
}

int TrajManager::InputTraffic(std::string callsign, larcfm::Position &position, larcfm::Velocity &velocity,double time) {
 
    pObject obj = {.callsign = callsign, 
                   .id = 0,
                   .time = time,
                   .position = position,
                   .velocity = velocity};
    trafficList[obj.callsign] = obj;
    return trafficList.size();
}

void TrajManager::UpdateDubinsPlannerParameters(DubinsParams_t& params) {
    dbPlanner.SetParameters(params);
    wellClearDistH = params.wellClearDistH;
    wellClearDistV = params.wellClearDistV;

    // Log params to file
    log<<"Planner parameters"<<std::endl;
    log<<" Well clear H [m]: "<<params.wellClearDistH<<std::endl;
    log<<" Well clear V [m]: "<<params.wellClearDistV<<std::endl;
    log<<" Turn rate [deg/s]: "<<params.turnRate<<std::endl;
    log<<" Max GS [m/s]: "<<params.maxGS<<std::endl;
    log<<" Min GS [m/s]: "<<params.minGS<<std::endl;
    log<<" Max VS [m/s]: "<<params.maxVS<<std::endl;
    log<<" Min VS [m/s]: "<<params.minVS<<std::endl;
    log<<" Horizontal accel [m/s^2]: "<<params.hAccel<<std::endl;
    log<<" Horizontal daccel [m/s^2]: "<<params.hDaccel<<std::endl;
    log<<" Vertical accel [m/s^2]: "<<params.vAccel<<std::endl;
    log<<" Vertical daccel [m/s^2]: "<<params.vDaccel<<std::endl;
    log<<" Climb speed [m/s]: "<<params.climbgs<<std::endl;
    log<<" Obstacle buffer [m]: "<<params.vertexBuffer<<std::endl;
    log<<" Altitude sections: "<<params.zSections<<std::endl;
    log<<" Altitude ceiling [m]: "<<params.maxH<<std::endl;
}

int TrajManager::FindPath(std::string planID, larcfm::Position fromPosition, larcfm::Position toPosition,
                               larcfm::Velocity fromVelocity,larcfm::Velocity toVelocity) {

    startPos = fromPosition;
    endPos   = toPosition;
    startVel = fromVelocity;
    endVel = toVelocity;

    LogInput();

    int64_t retval = -1;
    std::list<larcfm::Plan>::iterator it;
    for(it=flightPlans.begin();it != flightPlans.end(); ++ it){
        if (it->getID() != planID){
            continue;
        }
        flightPlans.erase(it);
        break;
    }
    retval = FindDubinsPath(planID);

    // Writing to file for debugging
    if(retval > 0){
        numPlans++;
        struct timeval  tv;
        gettimeofday(&tv, NULL);
        char buffer[100];
        sprintf(buffer,"%f",(double)tv.tv_sec + (double)(tv.tv_usec)/1E6);
        log<<"Time after computation:"<<std::string(buffer)<<std::endl;
    }
    return retval;

}

int64_t TrajManager::FindDubinsPath(std::string planID){

    // Reroute flight plan
    std::vector<larcfm::Vect3> TrafficPos;
    std::vector<larcfm::Velocity> TrafficVel;

    double computationTime = 2;

    double dist = startVel.gs()*computationTime;
    larcfm::EuclideanProjection proj = larcfm::Projection::createProjection(startPos.mkAlt(0));

    for(auto traffic: trafficList){
        larcfm::Velocity Vel = traffic.second.velocity;
        larcfm::Position Pos = traffic.second.position.linearDist2D(Vel.trk(),dist);
        larcfm::Vect3 tPos = proj.project(Pos);
        tPos.linear(Vel,computationTime);
        TrafficPos.push_back(tPos);
        TrafficVel.push_back(Vel);
    }

    larcfm::Position start = startPos.linearDist2D(startVel.trk(), dist);
    larcfm::Position goalPos = endPos;
    larcfm::Vect3 initPosR3 = proj.project(start);
    larcfm::Vect3 gpos = proj.project(goalPos);


    larcfm::Poly3D bbox;
    std::list<larcfm::Poly3D> obstacleList;
    for(fenceObject &gf: fenceList){
        if (gf.fenceType == fenceObject::FENCE_TYPE::KEEP_IN){
            bbox = gf.polygon.poly3D(proj);
        }else{
            obstacleList.push_back(gf.polygon.poly3D(proj));
        }
    }
    bool status = false;
    double sFac = 1;
    while (!status) {
        dbPlanner.Reset();
        dbPlanner.ShrinkTrafficVolume(sFac);
        dbPlanner.SetBoundary(bbox);
        dbPlanner.SetObstacles(obstacleList);
        dbPlanner.SetVehicleInitialConditions(initPosR3, startVel);
        dbPlanner.SetGoal(gpos, endVel);
        dbPlanner.SetTraffic(TrafficPos, TrafficVel);
        status = dbPlanner.ComputePath(computationTime);
        sFac *= 0.5;

        if(trafficList.size() == 0){
            status = true;
        }
    }

    larcfm::Plan output;
    output.add(startPos,0);
    dbPlanner.GetPlan(proj,output);
    output.setID(std::string(planID));
    flightPlans.push_back(output);
    return output.size();
}

int TrajManager::GetWaypoint(std::string planID, int id, waypoint_t & wp) {
   larcfm::Plan *fp = GetPlan(planID);
   if(fp == nullptr){
       return 0;
   }else{
       GetWaypointFromPlan(fp,id,wp);
       return fp->size();
   }
}

larcfm::Plan* TrajManager::GetPlan(std::string planID){
    for(auto &it : flightPlans){
        if(it.getID() == planID){
            auto *fp = &it;
            return fp;
        }
    };
    return NULL;
}

int TrajManager::GetTotalWaypoints(std::string planID){
    larcfm::Plan* fp = GetPlan(planID);
    if (fp != NULL)
        return fp->size();
    else
        return 0;
}

void TrajManager::SetPlanOffset(std::string planID,int ind,double offsetT){
    larcfm::Plan* fp = GetPlan(planID);
    if (fp != NULL){
        fp->timeShiftPlan(ind,offsetT);
    }
}

void TrajManager::InputFlightPlan(const std::string &plan_id, const std::list<waypoint_t> &waypoints, const double initHeading,bool repair,double repairTurnRate){
    larcfm::Plan* fp = GetPlan(plan_id);
    larcfm::Plan newPlan(plan_id); 
    if (fp != NULL){
        fp->clear();
        ConvertWPList2Plan(fp,plan_id,waypoints,initHeading,repair,repairTurnRate);
    }else{
        fp = &newPlan;
        ConvertWPList2Plan(fp,plan_id,waypoints,initHeading,repair,repairTurnRate);
        flightPlans.push_back(newPlan);
    }
    if(plan_id == "Plan0"){
        larcfm::Plan combinedPlan = fp->copy();
        combinedPlan.setID("Plan+");
        flightPlans.push_back(combinedPlan);
    }
}

std::string TrajManager::PlanToString(std::string planID){
    larcfm::Plan *fp = GetPlan(planID);
    if(fp != nullptr){
       larcfm::PlanWriter planWriter; 
       std::ostringstream planString;
       planWriter.open(&planString);
       planWriter.writePlan(*fp, true);
       return planString.str();
    }
    return "";
}

void TrajManager::StringToPlan(std::string planID,std::string inputString){
    larcfm::Plan output;
    std::string inputPlan(inputString);
    larcfm::PlanReader planReader;
    std::istringstream planString;
    planString.str(inputPlan);
    planReader.open(&planString);
    output = planReader.getPlan(0);

    // Check if a plan already exists with the given ID and replace it if 
    // it does exist
    std::list<larcfm::Plan>::iterator it;
    for(it=flightPlans.begin();it != flightPlans.end(); ++it){
        if (it->getID()!=planID){
           flightPlans.erase(it);
           break;
        }
    }

    std::string planName(planID);
    output.setID(planName);
    flightPlans.push_back(output);
}

void TrajManager::CombinePlan(std::string planA,std::string planB,int index){
    larcfm::Plan *fp1 = GetPlan(planA);
    larcfm::Plan *fp2 = GetPlan(planB);
    if(fp2 == nullptr){
        return;
    }
    larcfm::Plan *fp3 = GetPlan("Plan+"); 
    fp3->clear();
    *fp3 = fp1->copy();
    fp3->setID("Plan+");
    int fp1len = fp3->size();
    int starti = 0;
    if (index > 0){
       starti = index; 
    }
    int size1 = fp1->size();
    larcfm::Position pos0 = fp1->getPos(size1-1);
    for(int i=starti;i<fp2->size();++i){
        if(index < 0){
            larcfm::Position pos1 = fp2->getPos(i);
            if(fabs(pos1.distanceH(pos0) + pos1.distanceV(pos0)) < 1e-3){
                index = i + 1;
            }
            continue;
        }

        double delta = fp2->time(i) - fp2->time(i-1);
        std::string info = fp2->getInfo(i);
        std::string name = fp2->getName(i);
        double time = fp3->getLastTime() + delta;
        larcfm::NavPoint oldPoint = fp2->point(i);
        fp3->add(oldPoint.position(),time);
        fp3->setInfo(fp1len + i-index,info);
        fp3->setName(fp1len + i-index,name);
    }
}

void TrajManager::ClearAllPlans() {
    flightPlans.clear();
}

std::vector<double> TrajManager::ComputePlanOffsets(std::string planID,int nextWP,larcfm::Position currentPos,larcfm::Velocity currentVel, double timeAtPos){
    
    larcfm::Plan* fp = GetPlan(planID);
    larcfm::Position posA = fp->getPos(nextWP-1);
    larcfm::Position posB = fp->getPos(nextWP);
    std::vector<double> offsets(3);
    bool mot = fp->isMOT(nextWP);
    bool eot = fp->isEOT(nextWP);
    if( mot || eot ){
        int id = fp->prevTRK(nextWP);
        larcfm::Position center = fp->getTcpData(id).turnCenter();
        double trk1 = (fp->getPos(nextWP-1).vect2() - center.vect2()).trk();
        double trk2 = (fp->getPos(nextWP).vect2() - center.vect2()).trk();
        double trk3 = (currentPos.vect2() - center.vect2()).trk();
        double turnHeading = std::fmod(2*M_PI + fp->trkOut(nextWP),2*M_PI)*180/M_PI;
        double turnRadius  = fp->getTcpData(id).getRadiusSigned();
        int turnDirection = (turnRadius>0?+1:-1);
        double currentIdealTrk = (trk3 + (turnDirection > 0? M_PI/2: - M_PI/2));
        double currentActualTrk = currentVel.trk();
        double turnTargetDelta = std::fmod(larcfm::Util::turnDelta(trk1,trk2,turnDirection),2*M_PI);
        double turnCurrentDelta = std::fmod(larcfm::Util::turnDelta(trk1,trk3,turnDirection),2*M_PI);
        double targetTime  = fp->time(nextWP);
        double turnRate    = std::fmod((fp->gsIn(nextWP)/fabs(turnRadius))*180/M_PI,360) * turnDirection;
        double timeRemainingTurn = (turnTargetDelta - turnCurrentDelta)/(fabs(turnRate)*M_PI/180);
        double actualTimeRemaining = fp->time(nextWP) - timeAtPos;
        offsets[0] = currentPos.distanceH(center) - fabs(turnRadius);
        offsets[1] = turnCurrentDelta/turnTargetDelta;
        offsets[2] = timeRemainingTurn - actualTimeRemaining;

    }else{
        larcfm::EuclideanProjection proj = larcfm::Projection::createProjection(posA);
        larcfm::Vect2 A(0, 0);
        larcfm::Vect2 B = proj.project(posB).vect2();
        larcfm::Vect2 C = proj.project(currentPos).vect2();
        larcfm::Vect2 AB = B - A;
        larcfm::Vect2 AC = C - A;

        double distAB = AB.norm();
        double perpProj = fabs(AC.dot(AB.PerpL().Hat()));
        double straightProj = distAB > 0 ? AC.dot(AB.Hat()) / distAB: 1;

        double gs = fp->gsIn(nextWP);
        double expectedTimeAtPos = fp->time(nextWP - 1) + (gs > 1e-3 ? (straightProj * distAB) / gs : 0);

        offsets[0] = perpProj;
        offsets[1] = straightProj;
        offsets[2] = expectedTimeAtPos - timeAtPos;
    }

    return offsets;
}

double TrajManager::FindTimeToFenceViolation(larcfm::Poly3D polygon, larcfm::Vect3 so, larcfm::Velocity vel) {
    std::vector<larcfm::Vect2> vertices = polygon.getVerticesRef();
    int n = vertices.size();
    double floor = polygon.getBottom();
    double roof = polygon.getTop();
    std::vector<double> timeX;
    for (int i = 0; i < vertices.size(); ++i)
    {
        larcfm::Vect2 A = vertices[i%n];
        larcfm::Vect2 B = vertices[(i+1)%n];
        double z1 = floor;
        double z2 = roof;

        larcfm::Vect3 l0 = so;
        larcfm::Vect3 p0 = larcfm::Vect3(A, z1);

        larcfm::Vect3 n = larcfm::Vect3((B - A).PerpL(), 0);
        larcfm::Vect3 l = vel;

        double num = (p0 - l0).dot(n);
        double den = l.dot(n);
        if (fabs(den) > 1e-3)
        {
            double d = num / den;
            if (d >= 0){
                timeX.push_back(d);
            }
        }else{
            std::vector<double> val(0);
            return 0; 
        }
    }
    std::sort(timeX.begin(),timeX.end());
    return timeX.front();
}

trajectoryMonitorData_t TrajManager::MonitorTrajectory(double time, std::string planID, larcfm::Position pos, larcfm::Velocity vel, int nextWP1,int nextWP2)
{

    trajectoryMonitorData_t data;
    std::memset(&data,0,sizeof(trajectoryMonitorData_t));
    larcfm::Plan *fp = GetPlan(planID);
    if(fp == nullptr){
        return data;
    }
    larcfm::EuclideanProjection projection = larcfm::Projection::createProjection(pos);
    bool fenceConflict = false;
    bool trafficConflict = false;
    std::list<double> gfTimes, tfTimes;
    auto cmp = [](double x, double y) { return x < y; };
    std::vector<double> offsets1 = ComputePlanOffsets(planID,nextWP2,pos,vel,time);
    std::vector<double> offsets2 = ComputePlanOffsets("Plan0",nextWP1,pos,vel,time);
    double toffset = offsets1[2]; // Correct for any time delay (-ve offsets[2] indicate a delay)
    double correctedtime = time + toffset;
    if(offsets1[0] < 50){
        for (auto &gf : fenceList)
        {
            larcfm::Vect3 locpos(0, 0, 0);
            larcfm::CDPolycarp geoPolycarp(gf.polygon.getBottom(), gf.polygon.getTop(), false);

            // Check fence conflict with current position
            if (gf.fenceType == fenceObject::FENCE_TYPE::KEEP_IN)
            {
                larcfm::Poly3D localPoly = gf.polygon.poly3D(projection);
                bool conflict = geoPolycarp.definitelyOutside(locpos, localPoly);
                if (conflict)
                    gfTimes.push_back(0.0);
                // Check for projected fence conflict based on flightplan
                double eps = 1; // a small additional delta to add to the output
                double t = FindTimeToFenceViolation(localPoly, larcfm::Vect3(0, 0, pos.alt()), vel) + eps;
                int seg = fp->getSegment(correctedtime + t);
                if (seg < 0)
                {
                    continue;
                }
                larcfm::Position posOnPlan = fp->posVelWithinSeg(seg, t + correctedtime, fp->isLinear(), fp->gsOut(seg)).first;
                larcfm::Vect3 qPos = projection.project(posOnPlan);
                bool projConflict = geoPolycarp.definitelyOutside(qPos, localPoly);
                projConflict |= geoPolycarp.nearEdge(qPos, localPoly, 2, 2);
                if (projConflict)
                {
                    conflict |= true;
                    gfTimes.push_back(t);
                }
                fenceConflict |= conflict;
            }
            else
            {
                bool conflict = geoPolycarp.definitelyInside(locpos, gf.polygon.poly3D(projection));
                if (conflict)
                    gfTimes.push_back(0.0);
                // Check for projected fence conflict based on flightplan
                larcfm::CDIIPolygon cdiipolygon;
                larcfm::PolyPath objpath(std::to_string(gf.id), gf.polygon);
                bool pathConflict = cdiipolygon.detection(*fp, objpath, correctedtime, fp->getLastTime());
                conflict |= pathConflict;

                int n = cdiipolygon.size();
                std::list<double> timeAs;
                for (int i = 0; i < n; i++)
                {
                    timeAs.push_back(cdiipolygon.getTimeIn(i) - correctedtime);
                }
                if (n > 0)
                {
                    timeAs.sort(cmp);
                    gfTimes.push_back(timeAs.front());
                }
                fenceConflict |= conflict;
            }
        }

        // Check for projected traffic conflict based on flightplan
        for (auto &tp : trafficPlans)
        {
            larcfm::CDII cdii = larcfm::CDII::make(wellClearDistH, "m", wellClearDistV, "m");
            cdii.detection(*fp, tp, time, fp->getLastTime());
            int n = cdii.size();
            if (n > 0)
            {
                tfTimes.push_back(cdii.getTimeIn(0) - correctedtime);
                trafficConflict = true;
            }
        }

        // Check for projected traffic conflicts for state based traffic
        for (auto &tf : trafficList)
        {
            larcfm::Position posA = tf.second.position;
            larcfm::Velocity velA = tf.second.velocity;
            double projT = 1000;
            double timeA = tf.second.time + toffset;
            double timeB = timeA + projT;
            larcfm::Position posB = posA.linear(velA, projT);
            larcfm::Plan tp("traffic");
            tp.add(posA, timeA);
            tp.add(posB, timeB);
            larcfm::CDII cdii = larcfm::CDII::make(wellClearDistH, "m", wellClearDistV, "m");
            cdii.detection(*fp, tp, correctedtime, fp->getLastTime());
            int n = cdii.size();
            if (n > 0)
            {
                tfTimes.push_back(cdii.getTimeIn(0) - correctedtime);
                trafficConflict = true;
            }
        }

        tfTimes.sort(cmp);
        gfTimes.sort(cmp);
    }

    // Check for next feasible waypoint in the main plan
    fp = GetPlan("Plan0");
    int findex = nextWP1;
    auto CheckWPFeasibility = [&](int index) {
        larcfm::Vect3 locpos = projection.project(fp->getPos(index));
        bool conflict = false;
        for (auto &gf : fenceList)
        {
            larcfm::CDPolycarp geoPolycarp(gf.polygon.getBottom(), gf.polygon.getTop(), false);
            // Check fence conflict with waypoint
            if (gf.fenceType == fenceObject::FENCE_TYPE::KEEP_IN)
            {
                conflict |= geoPolycarp.definitelyOutside(locpos, gf.polygon.poly3D(projection));
            }
            else
            {
                conflict |= geoPolycarp.definitelyInside(locpos, gf.polygon.poly3D(projection));
            }
        }
        return conflict;
    };
    int maxwp = fp->size();
    for (; findex < maxwp; ++findex)
    {
        // Check if next wp is before the conflict time
        if(fp->time(findex) < correctedtime + tfTimes.front() && offsets1[0] < 50){
            continue;
        }

        
        // Avoid BOT and MOT
        if (fp->isBOT(findex) || fp->isMOT(findex))
        {
            continue;
        }

        // Avoid other tcps in a turn segment
        if (!fp->isEOT(findex)) {
            int prevTurnTCP = fp->prevTrkTCP(findex);
            if (fp->getTcpData(prevTurnTCP).isBOT() && !fp->getTcpData(prevTurnTCP).isEOT())
            {
                continue;
            }
        }

        // The next segment should be sufficient long to facilitate plan capture
        double segdist = fp->pathDistance(findex,findex+1);

        if(segdist < vel.gs()*3){
            continue;
        }

        // Check fence feasibility
        bool conflict = CheckWPFeasibility(findex);
        if (!conflict)
        {
            break;
        }
    }

    if(findex >= maxwp){
        findex = maxwp-1;
    }
    data.fenceConflict = fenceConflict;
    data.trafficConflict = trafficConflict;
    data.timeToFenceViolation = gfTimes.front();
    data.timeToTrafficViolation = tfTimes.front();
    data.offsets1[0] = offsets1[0];
    data.offsets1[1] = offsets1[1];
    data.offsets1[2] = offsets1[2];
    data.offsets2[0] = offsets2[0];
    data.offsets2[1] = offsets2[1];
    data.offsets2[2] = offsets2[2];
    data.nextWP = nextWP2;
    data.nextFeasibleWP = findex;
    return data;
}

void TrajManager::LogInput(){
    log<<"Start Pos:"<<startPos.toString(8)<<std::endl;
    log<<"Goal Pos:"<<endPos.toString(8)<<std::endl;
    log<<"Start vel:"<<startVel.toString(8)<<std::endl;
    log<<"End vel:"<<endVel.toString(8)<<std::endl;
    log<<"Num traffic:"<<trafficList.size()<<std::endl;
    for(auto traffic: trafficList){
        larcfm::Velocity Vel = traffic.second.velocity;
        larcfm::Position Pos = traffic.second.position;
        log<<"- pos: "<<Pos.toString(8)<<std::endl;
        log<<"- vel: "<<Vel.toString(8)<<std::endl;
    }
    log<<"Num Geofence:"<<fenceList.size()<<std::endl;
    for(fenceObject &gf: fenceList){
        log<<"- id   : "<<gf.id<<std::endl;
        log<<"- type : "<<gf.fenceType<<std::endl;
        log<<"- floor: "<<gf.polygon.getBottom()<<std::endl;
        log<<"- roof : "<<gf.polygon.getTop()<<std::endl;
        log<<"- numV : "<<gf.polygon.size()<<std::endl;
        for(int i = 0; i < gf.polygon.size(); ++i){
            log<<"-- vertex: " << gf.polygon.getVertex(i).toString(5)<<std::endl;
        }
    }
    
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    char buffer[100];
    sprintf(buffer,"%f",(double)tv.tv_sec + (double)(tv.tv_usec)/1E6);
    log<<"Time before computation:"<<std::string(buffer)<<std::endl;

    return;
}