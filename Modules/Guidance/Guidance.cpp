#include "Guidance.hpp"
#include "EuclideanProjection.h"
#include "Projection.h"
#include "UtilFunctions.h"

Guidance::Guidance(const GuidanceParams_t* inputParams){
    std::memcpy(&params,inputParams,sizeof(GuidanceParams_t)); 
    mode = GUIDE_NOOP;
    wpReached = false;
}

void Guidance::SetGuidanceParams(const GuidanceParams_t* inputParams){
    std::memcpy(&params,inputParams,sizeof(GuidanceParams_t)); 
}

void Guidance::SetAircraftState(const larcfm::Position &pos,const larcfm::Velocity &vel){
    currentPos = pos;
    currentVel = vel;
}

void Guidance::SetGuidanceMode(const GuidanceMode gmode,const std::string planID,const int nextWP){
    mode = gmode;
    if(gmode == VECTOR){
        return;
    }
    nextWpId[planID] = nextWP;
    activePlanId = planID;
    currentPlan = GetPlan(activePlanId);
}

void Guidance::ChangeWaypointSpeed(const std::string planID,const int wpid,const double val,const bool updateAll){
   larcfm::Plan* fp = GetPlan(planID);
   if (fp == nullptr) return;

   double speed = val;
   
   int wpidprev = wpid > 0?wpid - 1:nextWpId[planID]-1;
   int newInd = wpidprev + 1;
   //larcfm::Position prev_position = fp->getPos(wpidprev);
   //double start_time = fp->time(wpidprev);
   //double old_time   = fp->time(newInd);
   //double dist = prev_position.distanceH(currentPos);
   //double new_time = start_time + dist/speed;
   //double delta = old_time - new_time;
   if (updateAll){
        //fp->timeShiftPlan(newInd,delta);
        for(int i=newInd;i<fp->size();++i){
              wpSpeeds[planID][i] = speed;
        }
   }
   else{
        //fp->setTime(newInd,new_time);
        wpSpeeds[planID][newInd] = speed;
   }
}

void Guidance::ChangeWaypointETA(const std::string planID,const int wpid,const double val,bool updateAll){
   if (wpid == 0) return;
   larcfm::Plan* fp = GetPlan(planID);
   if (fp == nullptr) return;

   int wpidprev = wpid - 1;
   larcfm::Position prev_position = fp->getPos(wpidprev);
   double old_time   = fp->time(wpid);
   double new_time   = val;
   double delta = old_time - new_time;
   if (updateAll)
       fp->timeShiftPlan(wpid,delta);
   else
       fp->setTime(wpid,val);

}


void Guidance::SetVelocityCommands(const larcfm::Velocity &inputs){
    outputCmd = inputs;
}

void Guidance::GetOutput(GuidanceOutput_t& output){

   output.guidanceMode = mode;
   output.nextWP = -1;
   if(currentPlan != nullptr){
        strcpy(output.activePlan,activePlanId.c_str());
        output.nextWP = nextWpId[activePlanId];
        output.wpReached = wpReached;
        output.distH2WP = distH2nextWP;
        output.distV2WP = distV2nextWP;
        output.xtrackDev = xtrackDist;
   }

   output.velCmd[0] = larcfm::Units::to(larcfm::Units::deg,outputCmd.trk());
   output.velCmd[1] = larcfm::Units::to(larcfm::Units::mps,outputCmd.gs());
   output.velCmd[2] = larcfm::Units::to(larcfm::Units::mps,outputCmd.vs());

}

larcfm::Plan* Guidance::GetPlan(const std::string &plan_id){
    for(auto &it : planList){
        if(it.getID() == plan_id){
            auto *fp = &it;
            return fp;
        }
    };
    return NULL;
}

void Guidance::InputFlightplanData(const std::string &plan_id,
                                   const double scenario_time,
                                   const int wp_id,
                                   const larcfm::Position &wp_position,
                                   const bool eta,
                                   const double wp_value){
    larcfm::Plan* fp = GetPlan(plan_id);

    // Initialize the feasibility of this waypoint to true
    if(fp != NULL) {
        // Existing flight plan
        double wp_time;
        if(wp_id == 0){
            fp->clear();
            wp_time = scenario_time;
        }else if(eta){
            // wp metric is ETA
            wpSpeeds[plan_id].push_back(-1);
            wp_time = wp_value;
        }else{
            double speed = wp_value;
            int prev_wp_id = wp_id - 1;
            if(prev_wp_id < 0){
                prev_wp_id = 0;
            }
            larcfm::Position prev_position = fp->getPos(prev_wp_id);
            double start_time = fp->time(prev_wp_id);
            double dist = prev_position.distanceH(wp_position);
            wp_time = start_time + dist/speed;
            wpSpeeds[plan_id].push_back(speed);
        }
        fp->add(wp_position,wp_time);
        return;
    }else{
        // New flight plan
        larcfm::Plan newPlan(plan_id);
        newPlan.add(wp_position,scenario_time);
        planList.push_back(newPlan);
        std::vector<bool> init(1,true);
        nextWpId[plan_id] = 1;
        wpSpeeds[plan_id].push_back(-1);
        return;
    }
}

double Guidance::GetApproachPrecision(const larcfm::Position &position,const larcfm::Velocity &velocity, const larcfm::Position &waypoint){
    // Look at the dot product of current velocity
    // with the position of the intersection in relative NED coordinates
    // dot product >= 0 IMPLIES we are approaching the intersection
    const larcfm::EuclideanProjection proj = larcfm::Projection::createProjection(position);
    const larcfm::Vect3 pos_wp = proj.project(waypoint);

    const double vel_norm   = velocity.norm2D();
    const double dist_norm  = pos_wp.norm2D();
    const double vel_hat[2] =  {velocity.x/vel_norm, velocity.y/vel_norm};
    const double dist_hat[2] = {pos_wp.x/dist_norm, pos_wp.y/dist_norm};

    const double dot_VelocityIntersection = vel_hat[0] * dist_hat[1] +
                                      vel_hat[1] * dist_hat[0];

    return dot_VelocityIntersection;
}

double Guidance::ComputeSpeed(const larcfm::NavPoint &nextPos, double refSpeed){

   // If speed is provided for nextWP, use given speed
   if (refSpeed > 0) {
       if (refSpeed <= params.minSpeed) {
           return params.minSpeed;
       } else if (refSpeed >= params.maxSpeed) {
           return params.maxSpeed;
       } else {
           return refSpeed;
       }
   } else {
       double distH = currentPos.distanceH(nextPos.position());
       double nextWP_STA = nextPos.time();
       double maxSpeed = params.maxSpeed;
       double minSpeed = params.minSpeed;
       double newSpeed;
       newSpeed = distH / (nextWP_STA - currTime);
       if (newSpeed > maxSpeed) {
           newSpeed = maxSpeed;
       } else {
           if (newSpeed < minSpeed) {
               newSpeed = minSpeed;
           }
       }
       return newSpeed;
   }
}

double Guidance::ComputeClimbRate(const larcfm::Position &position,const larcfm::Position &nextWaypoint,double speed){
    const double deltaH = nextWaypoint.alt() - position.alt();
    double climbrate;
    if (fabs(deltaH) > params.climbAngleVRange &&
        position.distanceH(nextWaypoint)  > params.climbAngleHRange){
        // Over longer altitude changes and distances, control the ascent/descent angle
        double angle = params.climbFpAngle;
        if (deltaH < 0){
            angle = -angle;
        }
        const double cfactor = tan(angle * M_PI/180);
        climbrate = cfactor * speed;
    } else {
        // Over shorter altitude changes and distances, use proportional control
        climbrate = deltaH * params.climbRateGain;
    }
    if (climbrate > params.maxClimbRate) {
        climbrate = params.maxClimbRate;
    } else if (climbrate < params.minClimbRate) {
        climbrate = params.minClimbRate;
    }
    return climbrate;
}

void Guidance::ComputePlanGuidance(){
    bool finalleg = false;
    int nextWP = nextWpId[activePlanId]%currentPlan->size();
    if (nextWP == currentPlan->size() - 1){
        finalleg = true;
    }
    const larcfm::Position nextWPPos = currentPlan->getPos(nextWP);

    const double distH = currentPos.distanceH(nextWPPos);
    const double distV = fabs(currentPos.distanceV(nextWPPos));
    const double currSpeed = currentVel.norm2D();
    double speedRef = 0.0;

    if(wpSpeeds[activePlanId][nextWP] <= 0){ 
        speedRef = ComputeSpeed(currentPlan->point(nextWP));
    }else{
        speedRef = ComputeSpeed(currentPlan->point(nextWP),wpSpeeds[activePlanId][nextWP]);
    }
    double capture_radius = speedRef * params.captureRadiusScaling;
    if (params.minCap >= capture_radius){
        capture_radius = params.minCap;
    }else if(capture_radius > params.maxCap){
        capture_radius = params.maxCap;
    }
    double xdev = 0.0;
    const larcfm:: Position newPositionToTrack = ComputeOffSetPositionOnPlan(speedRef,xdev);

    // Reduce speed if approaching final waypoint or if turning sharply
    const double ownship_heading = currentVel.trk() * 180/M_PI;
    const double target_heading = currentPos.track(newPositionToTrack) * 180/M_PI;
    const double turn_angle = fabs(fmod(180 + ownship_heading - target_heading, 360) - 180);
    if((finalleg && distH < capture_radius) || turn_angle > 60){
        const double range = params.maxSpeed - params.minSpeed;
        if( speedRef > (params.minSpeed + range * 0.6) ){
            speedRef = speedRef/2;
        }
    }

    // Compute velocity command to next waypoint
    const double heading = currentPos.track(newPositionToTrack) * 180/M_PI;
    double climbrate = ComputeClimbRate(currentPos, nextWPPos, speedRef);

    // Smooth the output
    double n_gs, n_vs, n_heading;
    double ownship_gs = currSpeed;
    double ownship_vd = -currentVel.z;
    double gs_range = params.maxSpeed - params.minSpeed;
    double vs_range = (params.maxClimbRate - params.minClimbRate);
    double heading_change = fabs(fmod(180 + ownship_heading - heading, 360) - 180);

    if (fabs(speedRef - ownship_gs) > gs_range / 2)
        n_gs = 0.3;
    else
        n_gs = 0.95;

    if (fabs(climbrate - (-ownship_vd)) > vs_range / 2)
        n_vs = 0.3;
    else
        n_vs = 0.95;

    if (heading_change > 45)
        n_heading = 0.3;
    else
        n_heading = 0.95;

    speedRef = (1 - n_gs) * ownship_gs + n_gs * speedRef;
    climbrate = (1 - n_vs) * ownship_vd + n_vs * climbrate;

    outputCmd = larcfm::Velocity::makeTrkGsVs(heading,"degree",speedRef,"m/s",climbrate,"m/s");

    const double approachPrec = GetApproachPrecision(currentPos,currentVel,nextWPPos);
    // If distance to next waypoint is < captureRadius, switch to next waypoint
    if (distH <= capture_radius && 
        distV <= params.climbAngleVRange) {
        wpReached = true;
        nextWP++;
        nextWpId[activePlanId] = nextWP;
    }else{
        wpReached = false;
    }

    // log relevant information
    distH2nextWP = distH;
    distV2nextWP = distV;
    xtrackDist   = xdev;
}

larcfm::Position Guidance::ComputeOffSetPositionOnPlan(double speedRef,double& deviation){
    int nextWP = nextWpId[activePlanId]%currentPlan->size();
    const double guidance_radius = speedRef*params.guidanceRadiusScaling;
    double xtrkDev = params.xtrkDev;
    if(xtrkDev > guidance_radius)
        xtrkDev = guidance_radius;

    // Create vectors for wpA->wpB and wpA->ownship_position
    const larcfm::EuclideanProjection projAtWPA = larcfm::Projection::createProjection(currentPlan->getPos(nextWP-1));
    const larcfm::Vect3 AB = projAtWPA.project(currentPlan->getPos(nextWP));
    const larcfm::Vect3 AP = projAtWPA.project(currentPos);

    const double distAB = AB.norm2D();
    const double distAP = AP.norm2D();
    const double distPB = AB.Sub(AP).norm2D();

    // Projection of AP onto AB
    const double projection = (AP.dot2D(AB))/pow(AB.norm2D(),2);
    const larcfm::Vect3 closestPoint = larcfm::Velocity::makeXYZ(AB.x*projection,"m",
                                                                 AB.y*projection,"m", 
                                                                 AB.z*projection,"m");
    deviation = closestPoint.Sub(AP).norm2D();

    //Convert waypoints to local frame (with ownship position as origin)
    const larcfm::EuclideanProjection projAtPos = larcfm::Projection::createProjection(currentPos);
    const larcfm::Vect3 _wpA = projAtPos.project(currentPlan->getPos(nextWP-1));
    const larcfm::Vect3 _wpB = projAtPos.project(currentPlan->getPos(nextWP));

    double outputEND[3] = {0.0,0.0,0.0};

    if(distPB <= guidance_radius){
        //If within guidance radius of wpB, track to wpB
        return currentPlan->getPos(nextWP);
    } else if(distAP <= guidance_radius){
        // If within guidance radius of wpA, use guidance circle method
        const larcfm::Vect3 outputVec = GetCorrectIntersectionPoint(_wpA,_wpB,guidance_radius);
        larcfm::Position outputPos(projAtPos.inverse(outputVec));
        return outputPos;
    } else {
        // Otherwise, check projection of position onto flight plan
        if(projection <= 0){
            // If behind wpA, track to wpA
            return currentPlan->getPos(nextWP-1);
        } else if(projection >= 1){
            // If past wpB, track to wpB
            return currentPlan->getPos(nextWP);
        } else {
            // If between wpA and wpB
            if(deviation < xtrkDev){
                // If close enough to flight plan, use guidance circle method
                const larcfm::Vect3 outputVec = GetCorrectIntersectionPoint(_wpA,_wpB,guidance_radius);
                larcfm::Position outputPos(projAtPos.inverse(outputVec));
                return outputPos;
            } else {
                // If far from flight plan, track to closest point on flight plan
                const larcfm::Position outputPos(projAtWPA.inverse(closestPoint));
                return outputPos;
            }
        }
    }
}

larcfm::Vect3 Guidance::GetCorrectIntersectionPoint(const larcfm::Vect3 &wpA,const larcfm::Vect3 &wpB,const double r){
    double x1,x2,y1,y2;    
    if( fabs(wpB.x - wpA.x) > 1e-2 ){
        const double m = (wpB.y - wpA.y)/(wpB.x - wpA.x);
        const double c = wpA.y - wpA.x*m;
        const double aq = 1 + pow(m,2);
        const double bq = 2*m*c;
        const double cq = pow(c,2) - pow(r,2);

        const double discr = pow(bq,2) - 4*aq*cq;
        const double discr_sqrt = sqrt(discr);

        if(discr < 0){
           // Complex roots
           larcfm::Vect3 output = wpB;
           return output;
        }
        x1 = (-bq - discr_sqrt)/(2*aq);
        x2 = (-bq + discr_sqrt)/(2*aq);
        y1 = m*x1 + c; 
        y2 = m*x2 + c;
    }else{
        x1 = wpA.x;
        x2 = x1;
        if(r > fabs(x1)){
            y1 = sqrt(pow(r,2) - pow(x1,2)); 
            y2 = -sqrt(pow(r,2) - pow(x1,2));  
        }else{
            y1 = wpA.y;
            y2 = wpB.y;
        }
    }

    // Check which point is closest to waypoint B
    const larcfm::Vect3 p1 = larcfm::Vect3::makeXYZ(x1,"m", y1, "m", 0, "m");
    const larcfm::Vect3 p2 = larcfm::Vect3::makeXYZ(x2,"m", y2, "m", 0, "m");
    double dist1 = p1.distanceH(wpB);
    double dist2 = p2.distanceH(wpB);
    if(dist1 < dist2){
        // Use (x1,y1)
        return p1;
    }else{
        // Use (x2,y2)
        return p2;
    }
}

int Guidance::RunGuidance(double time){
    currTime = time;

    switch(mode){
        case FLIGHTPLAN:
        case POINT2POINT:
        {
            currentPlan = GetPlan(activePlanId);
            if(currentPlan != nullptr)
                ComputePlanGuidance();
            break;
        }

        case VECTOR:{
            break;
        }

        case TAKEOFF:{
            break;
        }

        case LAND:{
            break;
        }

        case GUIDE_NOOP:break;
        case SPEED_CHANGE: break;

    }

    return mode;
}


void* InitGuidance(GuidanceParams_t* params){
    return (void*) new Guidance(params);
}

void guidSetParams(void* obj,GuidanceParams_t* params){
    ((Guidance*)obj)->SetGuidanceParams(params); 
}

void guidSetAircraftState(void* obj, double position[],double velocity[]){
    larcfm::Position pos = larcfm::Position::makeLatLonAlt(position[0],"degree",
                                                           position[1],"degree",
                                                           position[2],"m");
    larcfm::Velocity vel = larcfm::Velocity::makeTrkGsVs(velocity[0],"degree",
                                                         velocity[1],"m/s",
                                                         velocity[2],"m/s");
    ((Guidance*)obj)->SetAircraftState(pos,vel);
}

void guidInputFlightplanData(void* obj, char planID[], double scenarioTime, int wpID, double position[], bool eta, double value){
     std::string planid(planID);
     larcfm::Position pos = larcfm::Position::makeLatLonAlt(position[0],"degree",
                                                           position[1],"degree",
                                                           position[2],"m");
     ((Guidance*)obj)->InputFlightplanData(planid,scenarioTime,wpID,pos,eta,value);
}

void RunGuidance(void* obj,double time){
    ((Guidance*)obj)->RunGuidance(time);
}
void guidInputVelocityCmd(void* obj,double velcmd[]){
    larcfm::Velocity vel = larcfm::Velocity::makeTrkGsVs(velcmd[0],"degree",
                                                         velcmd[1],"m/s",
                                                         velcmd[2],"m/s");

    ((Guidance*)obj)->SetVelocityCommands(vel);
}

void SetGuidanceMode(void*obj, GuidanceMode mode,char planID[], int nextWP){
    std::string planid(planID);
    ((Guidance*)obj)->SetGuidanceMode(mode,planid,nextWP);
}

void guidGetOutput(void* obj,GuidanceOutput_t* output){
    ((Guidance*)obj)->GetOutput(*output);
}

void ChangeWaypointSpeed(void* obj,char planID[],int wpID,double speed,bool updateAll){
   std::string planid(planID);
   ((Guidance*)obj)->ChangeWaypointSpeed(planID,wpID,speed,updateAll);
}

void ChangeWaypointETA(void* obj,char planID[],int wpID,double eta,bool updateAll){
   std::string planid(planID);
   ((Guidance*)obj)->ChangeWaypointETA(planID,wpID,eta,updateAll);
}
