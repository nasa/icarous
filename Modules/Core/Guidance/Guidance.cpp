#include "Guidance.hpp"
#include "EuclideanProjection.h"
#include "Projection.h"
#include "UtilFunctions.h"
#include "WP2Plan.hpp"

#define RAD2DEG 180/M_PI
#define DEG2RAD M_PI/180

Guidance::Guidance(const GuidanceParams_t* inputParams){
    std::memcpy(&params,inputParams,sizeof(GuidanceParams_t)); 
    mode = GUIDE_NOOP;
    wpReached = false;
    currTime = 0;
    prevTrackControllerTime = 0.0;
    prevTrackControllerTarget = 0.0;
    currentPlan = NULL;
}

void Guidance::SetGuidanceParams(const GuidanceParams_t* inputParams){
    std::memcpy(&params,inputParams,sizeof(GuidanceParams_t)); 
}

void Guidance::SetAircraftState(const larcfm::Position &pos,const larcfm::Velocity &groundSpeed){
    currentPos = pos;
    currentGroundSpeed = groundSpeed;
}

void Guidance::SetWindData(const double windFrom,const double windSpeed){
    wind = larcfm::Velocity::makeTrkGsVs(windFrom+180,"degree",windSpeed,"m/s",0,"m/s");
    currentAirspeed = larcfm::Velocity::make(currentGroundSpeed.vect2() - wind.vect2());
}

void Guidance::SetGuidanceMode(const GuidanceMode gmode,const std::string planID,const int nextWP,const bool eta){
    mode = gmode;
    etaControl = eta;
    inTurn = false;
    if(gmode == VECTOR || gmode == LAND){
        return;
    }
    larcfm::Plan* fp = GetPlan(planID);
    if(fp != nullptr){
        if(nextWP == 0){
            double diff = currTime - fp->getFirstTime();
            fp->timeShiftPlan(0,diff);
        }else{
            currentPlan = fp;
            nextWpId[planID] = nextWP;
            activePlanId = planID;
            int prevTrkTCP = fp->prevTrkTCP(nextWP);
            if(prevTrkTCP > 0){
                if (fp->getTcpData(prevTrkTCP).isBOT()) {
                    inTurn = true;
                }
            }
        }
    }
}

int Guidance::GetWaypoint(std::string planID,int id, waypoint_t& wp){
   larcfm::Plan *fp = GetPlan(planID);
   if(fp == nullptr){
       return 0;
   }else{
       GetWaypointFromPlan(fp,id,wp); 
       return fp->size();
   }
}

void Guidance::ChangeWaypointSpeed(const std::string planID,const int wpid,const double val){
   prevPlan = planID;
   larcfm::Plan* fp = GetPlan(planID);
   if (fp == nullptr) return;
   std::string speedChange("PlanSpeedChange");

   int wpidprev = wpid > 0?wpid - 1:nextWpId[planID]-1;
   int newInd = wpidprev + 1;
   if(fabs(val - fp->gsIn(newInd)) < 1e-3){
       activePlanId = planID;
       currentPlan = fp;
       nextWpId[planID] = newInd;
       return;
   }

   larcfm::Plan fp2 = fp->copy();
   fp2.setID(speedChange);

   double speed = val;
   
   int size = fp2.size();
   double prevTime = fp->time(newInd-1);
   for(int i=newInd;i<size;++i){
       double old_time   = fp2.time(i);
       double dist       = fp2.pathDistance(i-1,i);
       double new_time   = prevTime + dist/speed;
       double delta      = new_time - old_time;
       fp2.timeShiftPlan(i,delta);
       prevTime = new_time;
   }

   fp = GetPlan(speedChange);
   if(fp == nullptr){
       planList.push_back(fp2);
   }else{
       fp->clear();
       *fp = fp2;
   }
   activePlanId = speedChange;
   currentPlan = GetPlan(speedChange);
   nextWpId[speedChange] = newInd;
}

void Guidance::ChangeWaypointAlt(const std::string planID,const int wpid,const double val,const bool updateAll){
   // For altitude changes, create a copy of the original plan
   // and modify altitudes in the copy.
   prevPlan = planID;
   larcfm::Plan* fp = GetPlan(planID);
   if (fp == nullptr) return;
   int wpidprev = wpid > 0?wpid - 1:nextWpId[planID]-1;
   int newInd = wpidprev + 1;

   // New Plan
   std::string newPlanID("PlanAltChange");

   // Reuse existing PlanAltChange if available
   larcfm::Plan *oldPlan = GetPlan(newPlanID);
   if (oldPlan != nullptr) oldPlan->clear();

   larcfm::Plan fp2  = fp->copy();
   fp2.setID("PlanAltChange");
   

   // Find diff altitude so that all remaining waypoints
   // can be shifted by the diff altitude
   double currentRefAlt = fp->getPos(newInd).alt();
   double diffAlt = val - currentRefAlt;

   // If no altitude difference, revert back to using nominal plan
   if(fabs(diffAlt) < 1e-4){
       activePlanId = planID;
       currentPlan = GetPlan(activePlanId);
       nextWpId[planID] = newInd;
       return;
   }
   
   // Shift altitudes in plan copy
   for(int i=newInd;i<fp2.size();++i){
        larcfm::NavPoint point = fp2.point(i);
        fp2.remove(i);
        double oldAlt = point.position().alt();
        double newAlt = oldAlt + diffAlt;
        fp2.add(point.position().mkAlt(newAlt),point.time());
        if(!updateAll){
            break;
        }
   }

   // Add new plan to list if not already available
   if(oldPlan == nullptr){
       planList.push_back(fp2);
   }else{
       *oldPlan = fp2;
   }

   // Set pointers to the new plan
   activePlanId = newPlanID;
   currentPlan = GetPlan(activePlanId);
   nextWpId[newPlanID] = newInd;
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
    double windSpeed = wind.gs();
    double maxSpeed = params.maxSpeed + windSpeed;
    double minSpeed = params.minSpeed + windSpeed;
    double gs =inputs.gs();
    if (gs < minSpeed){
        gs = minSpeed;
    }else if(gs > params.maxSpeed){
        gs= maxSpeed;
    }
    larcfm::Velocity outVel = inputs.mkGs(gs);
    outputCmd = outVel;
}

void Guidance::GetOutput(GuidanceOutput_t& output){

   output.guidanceMode = mode;
   output.nextWP = -1;
   if(currentPlan != nullptr){
        if(activePlanId == "PlanAltChange" || activePlanId == "PlanSpeedChange"){
            strcpy(output.activePlan,prevPlan.c_str());
        }else{
            strcpy(output.activePlan,activePlanId.c_str());
        }
        output.nextWP = nextWpId[activePlanId];
        output.wpReached = wpReached;
        output.distH2WP = distH2nextWP;
        output.distV2WP = distV2nextWP;
        output.xtrackDev = xtrackDist;
   }
   if(currentPlan != NULL){
    int nextWP = std::min(nextWpId[activePlanId],currentPlan->size()-1);
    larcfm::Position pos = currentPlan->getPos(nextWP);
    output.target[0] = pos.latitude();
    output.target[1] = pos.longitude();
    output.target[2] = pos.alt();
   }else{
    output.target[0] = 5000;
    output.target[1] = 5000;
    output.target[2] = 5000;
   }
   output.velCmd[0] = larcfm::Units::to(larcfm::Units::deg,outputCmd.compassAngle());
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

void Guidance::InputFlightplanData(const std::string &plan_id, const std::list<waypoint_t> &waypoints, 
                                   const double initHeading,bool repair,double repairTurnRate){
    larcfm::Plan* fp = GetPlan(plan_id);
    larcfm::Plan newPlan(plan_id); 
    if (fp != NULL){
        fp->clear();
        ConvertWPList2Plan(fp,plan_id,waypoints,initHeading,repair,repairTurnRate);
    }else{
        fp = &newPlan;
        ConvertWPList2Plan(fp,plan_id,waypoints,initHeading,repair,repairTurnRate);
        planList.push_back(newPlan);
    }
    //std::cout<<newPlan.toString()<<std::endl;
}

double Guidance::GetApproachPrecision(const larcfm::Position &position,const larcfm::Velocity &velocity, const larcfm::Position &waypoint){
    // Look at the dot product of current velocity
    // with the position of the intersection in relative NED coordinates
    // dot product >= 0 IMPLIES we are approaching the intersection
    const larcfm::EuclideanProjection proj = larcfm::Projection::createProjection(position);
    const larcfm::Vect3 pos_wp = proj.project(waypoint) - larcfm::Vect3::makeXYZ(0,"m",0,"m",position.alt(),"m") ;
    const larcfm::Vect2 vhat  = velocity.vect2().Hat(); 
    const larcfm::Vect2 pwhat = pos_wp.vect2().Hat();
    return vhat.dot(pwhat);
}

double Guidance::ComputeSpeed(){

   int nextWP = nextWpId[activePlanId];
   const larcfm::NavPoint nextPos = currentPlan->point(nextWP); 
   double refSpeed;
   if (!currentPlan->isLinear()){
      etaControl = true;
   }

   if(activePlanId == "PlanSpeedChange" || activePlanId == "PlanAltChange" || activePlanId == "DitchPath"){
       etaControl = false;
   }

   double windSpeed = wind.gs();
   if (!etaControl) {
       refSpeed = currentPlan->gsIn(nextWP);
       if (refSpeed <= params.minSpeed + windSpeed) {
           double distH = currentPos.distanceH(nextPos.position());
           if(refSpeed < 1e-3 && distH > 3 && params.minSpeed < 1e-3){
              return 1.0;
           }else{
              return params.minSpeed + windSpeed;
           }
       } else if (refSpeed >= params.maxSpeed + windSpeed) {
           return params.maxSpeed + windSpeed;
       } else {
           return refSpeed;
       }
   } else {
       double distH = currentPos.distanceH(nextPos.position());
       double nextWP_STA = nextPos.time();
       double maxSpeed = params.maxSpeed + windSpeed;
       double minSpeed = params.minSpeed + windSpeed;
       double newSpeed;
       double timediff = nextWP_STA - currTime;

       if (distH > 0.5 && timediff > 0.001){
           newSpeed = distH / timediff;
       }else{
           // Speed if eta cannot be met
           newSpeed = maxSpeed * 0.75;
       }

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

double Guidance::ComputeClimbRate(double speedRef){
    const int nextWP = nextWpId[activePlanId];
    const larcfm::Position position = currentPos;
    const larcfm::Position nextWaypoint = currentPlan->getPos(nextWP);
    const double deltaH = nextWaypoint.alt() - position.alt();
    double climbrate = 0;
    if(currentPlan->isLinear() || fabs(deltaH) < 5){
        if (fabs(deltaH) > params.climbAngleVRange &&
            position.distanceH(nextWaypoint) > params.climbAngleHRange) {
            // Over longer altitude changes and distances, control the ascent/descent angle
            double angle = params.climbFpAngle;
            if (deltaH < 0) {
                angle = -angle;
            }
            const double cfactor = tan(angle * M_PI / 180);
            if(speedRef > 1e-3){
                climbrate = cfactor * speedRef;
            }else{
                climbrate = deltaH > 0? params.maxClimbRate : params.minClimbRate;
            }
        }
        else {
            // Over shorter altitude changes and distances, use proportional control
            climbrate = deltaH * params.climbRateGain;
        }
    }else{
        if(fabs(deltaH) > 5){
            climbrate = currentPlan->vsIn(nextWP);
        }
        climbrate += deltaH * params.climbRateGain;
    }
    if (climbrate > params.maxClimbRate) {
        climbrate = params.maxClimbRate;
    }
    else if (climbrate < params.minClimbRate) {
        climbrate = params.minClimbRate;
    }
    return climbrate;
}

double Guidance::ConstrainTurnRate(double targetHeading,double reqTurnRate){
    // Controller time step
    double dt = currTime - prevTrackControllerTime;
    double currHeading = currentGroundSpeed.compassAngle() * RAD2DEG; 

    // If the controller has been on pause, reset the prev target
    if(dt > 1){
       prevTrackControllerTarget = currHeading;
       prevTrackControllerTime = currTime;
       return currHeading;
    }


    // Find shortest turn direction
    double err = larcfm::Util::turnDelta(currentGroundSpeed.compassAngle(),targetHeading*DEG2RAD,reqTurnRate>0?true:false) * RAD2DEG;
    double turnRate;
    //double rightTurn = std::fmod(360 + (targetHeading - currHeading),360);
    //double leftTurn  = std::fmod(360 + (currHeading - targetHeading),360);
    /*
    double sign;

    // Left turn '-'ve, right turn '+'ve
    if (leftTurn <= rightTurn){
       sign = -1; 
       err = leftTurn;
    }else{
       sign = 1;
       err = rightTurn;
    }*/

    // Smooth turn rate to 0 as error decreases to 0
    if (err > 5){
        turnRate = reqTurnRate;
    }else{
        turnRate = err;
    }

    // Integrate tun rate to get reference heading
    double output = std::fmod(360 + (prevTrackControllerTarget + turnRate*dt),360);
    prevTrackControllerTarget = output;
    prevTrackControllerTime = currTime;
    return output;
}

double Guidance::ComputeNewHeading(double& speedRef){


    double outputHeading;
    const int nextWP = nextWpId[activePlanId]%currentPlan->size();
    const double guidance_radius = fmax(1,currentGroundSpeed.gs()*params.guidanceRadiusScaling);

    
    /* Compute target heading */

    /* If prev waypoint is a Track TCP (i.e. BOT or MOT),
     * Compute new heading based on heading delta in segment
     */
    if (nextWP > 1) {
        if (currentPlan->isBOT(nextWP - 1) || currentPlan->isBOT(nextWP - 1) && currentPlan->isEOT(nextWP - 1)) {
            inTurn = true;
        }
        else if (currentPlan->isEOT(nextWP - 1)) {
            inTurn = false;
        }
    }else{
        inTurn = false;
    }

    if(currentPlan->isLinear()){
        inTurn = false;
    }

    if(inTurn){

        int id = currentPlan->prevTRK(nextWP);
        larcfm::Position center = currentPlan->getTcpData(id).turnCenter();
        //larcfm::EuclideanProjection projection = larcfm::Projection::createProjection(center);
        double trk1 = (currentPlan->getPos(nextWP-1).vect2() - center.vect2()).trk();
        double trk2 = (currentPlan->getPos(nextWP).vect2() - center.vect2()).trk();
        double trk3 = (currentPos.vect2() - center.vect2()).trk();
        double turnHeading = std::fmod(2*M_PI + currentPlan->trkOut(nextWP),2*M_PI)*RAD2DEG;
        double turnRadius  = currentPlan->getTcpData(id).getRadiusSigned();
        int turnDirection = (turnRadius>0?+1:-1);
        double currentIdealTrk = (trk3 + (turnDirection > 0? M_PI/2: - M_PI/2));
        double currentActualTrk = currentGroundSpeed.trk();
        double turnTargetDelta = std::fmod(larcfm::Util::turnDelta(trk1,trk2,turnDirection),2*M_PI);
        double turnCurrentDelta = std::fmod(larcfm::Util::turnDelta(trk1,trk3,turnDirection),2*M_PI);
        // If the turn still hasn't begun
        if(turnCurrentDelta >= M_PI*3/2){
            // This assumes that that angle between BOT and EOT is no more than 180 degrees
            turnCurrentDelta = 0.0;
        }
        if(etaControl){
            double targetTime = currentPlan->time(nextWP);
            double turnRate = std::fmod((currentPlan->gsIn(nextWP) / fabs(turnRadius)) * RAD2DEG, 360) * turnDirection;
            double timeRemainingTurn = fabs((turnTargetDelta - turnCurrentDelta) / (turnRate * DEG2RAD));
            double actualTimeRemaining = currentPlan->time(nextWP) - currTime;
            if (actualTimeRemaining < timeRemainingTurn) {
                turnRate *= 1.3;
            }
            else {
                turnRate *= 0.9;
            }

            speedRef = std::min(params.maxSpeed, turnRadius * (turnRate * DEG2RAD));
        }
        double actualRadius = currentPlan->getPos(id).distanceH(center);
        double dist2center = currentPos.distanceH(center);
        double offset = dist2center/fabs(turnRadius) - 1;
        double k = params.turnRateGain;
        double addTurn = offset*k;
        if(addTurn > M_PI/4){
            addTurn = M_PI/4;
        }

        int trkErrorSign = larcfm::Util::turnDir(currentActualTrk,currentIdealTrk);
        double trkError = larcfm::Util::turnDelta(currentActualTrk,currentIdealTrk);
        if(turnRadius > 0){
            outputHeading = currentIdealTrk + addTurn;
        }else{
            outputHeading = currentIdealTrk - addTurn;
        }
        outputHeading = std::fmod(2*M_PI + outputHeading,2*M_PI)*180/M_PI; 
    }else{
        /* If prev waypoint is not Track TCP or is EOT, 
         * use  offset position on plan to get heading.
         */
        larcfm::Position newPositionToTrack;
        double xdev = ComputeOffSetPositionOnPlan(currentPlan, nextWP, guidance_radius, newPositionToTrack);
        double targetHeading = currentPos.track(newPositionToTrack) * RAD2DEG;
        double currHeading = currentGroundSpeed.compassAngle() * RAD2DEG;
        outputHeading = targetHeading;
    }

    return outputHeading;
}


void Guidance::FilterCommand(double &refHeading, double &refSpeed, double &refVS){
    const double currSpeed = currentGroundSpeed.norm2D();
    double n_gs,n_vs;
    double ownship_gs = currSpeed;
    double ownship_vd = currentGroundSpeed.z;
    double ownship_hd = std::fmod(360 + currentGroundSpeed.trk()*RAD2DEG,360);
    double heading_change = std::fmod(360 + (refHeading - ownship_hd),360);
    double gs_range = params.maxSpeed - params.minSpeed;


    // Reduce speed if approaching final waypoint or if turning sharply
    if (currentPlan->isLinear()) {
       const double ownship_heading = currentGroundSpeed.compassAngle();
       const double turn_angle =larcfm::Util::turnDelta(ownship_heading,refHeading * M_PI/180) * 180/M_PI;
       if (fabs(turn_angle) > 60) {
           const double range = params.maxSpeed - params.minSpeed;
           if (refSpeed > (params.minSpeed + range * 0.25)) {
               refSpeed = std::max(params.minSpeed,refSpeed / 4);
           }
       }
    }

    if(distH2nextWP < 1e-3){
        refSpeed = 0.0;
    }

    if (fabs(refSpeed - ownship_gs) > gs_range / 2)
        n_gs = 0.3;
    else
        n_gs = 1.0;

    if (heading_change > 60)
        n_vs = 1;
    else
        n_vs = 1;

    refSpeed = (1 - n_gs) * ownship_gs + n_gs * refSpeed;

    refVS = (1 - n_vs) * ownship_vd + n_vs * refVS;
}

void Guidance::CheckWaypointArrival(){
    int nextWP = nextWpId[activePlanId];
    const larcfm::Position waypoint = currentPlan->getPos(nextWP);

    const double currSpeed = currentGroundSpeed.norm2D();
    distH2nextWP = currentPos.distanceH(waypoint);
    distV2nextWP = fabs(currentPos.distanceV(waypoint));

    // Scale capture radius based on current speed of vehicle
    double capture_radius = currSpeed * params.captureRadiusScaling;
    if (params.minCap >= capture_radius){
        capture_radius = params.minCap;
    }else if(capture_radius > params.maxCap){
        capture_radius = params.maxCap;
    }
    double approachPrec = GetApproachPrecision(currentPos,currentGroundSpeed,waypoint);
    if(currentPlan->isLinear()){
       approachPrec = -1;
    }
    // If distance to next waypoint is < captureRadius, switch to next waypoint
    bool altReached = distV2nextWP <= params.climbAngleVRange;
    altReached = true;
    if (distH2nextWP <= capture_radius && altReached
         && (approachPrec < 0 || currSpeed < 0.5)) {
        wpReached = true;
        nextWP++;
        nextWpId[activePlanId] = nextWP;
    }else{
        wpReached = false;
    }
}

double Guidance::ComputeOffSetPositionOnPlan(larcfm::Plan* fp,int nextWP,double guidance_radius,larcfm::Position &offsetPos){
    double xtrkDev = guidance_radius;

    // Create vectors for wpA->wpB and wpA->ownship_position
    const larcfm::EuclideanProjection projAtWPA = larcfm::Projection::createProjection(currentPlan->getPos(nextWP-1));
    const larcfm::Vect3 AB = projAtWPA.project(currentPlan->getPos(nextWP));
    const larcfm::Vect3 AP = projAtWPA.project(currentPos);

    const double distAB = AB.norm2D();
    const double distAP = AP.norm2D();
    const double distPB = AB.Sub(AP).norm2D();

    if(distAB < 1e-3){
        offsetPos = currentPos;
        return 0.0;
    }

    // Projection of AP onto AB
    const double projection = (AP.dot2D(AB))/pow(AB.norm2D(),2);
    const larcfm::Vect3 closestPoint = larcfm::Velocity::makeXYZ(AB.x*projection,"m",
                                                                 AB.y*projection,"m", 
                                                                 AB.z*projection,"m");
    const double deviation = closestPoint.Sub(AP).norm2D();

    //Convert waypoints to local frame (with ownship position as origin)
    const larcfm::EuclideanProjection projAtPos = larcfm::Projection::createProjection(currentPos);
    const larcfm::Vect3 _wpA = projAtPos.project(currentPlan->getPos(nextWP-1));
    const larcfm::Vect3 _wpB = projAtPos.project(currentPlan->getPos(nextWP));

    double outputEND[3] = {0.0,0.0,0.0};

    if(distPB <= guidance_radius){
        //If within guidance radius from wpB, track to wpB
        offsetPos = fp->getPos(nextWP);
    } else if(distAP <= guidance_radius){
        // If within guidance radius from wpA, use guidance circle method
        const larcfm::Vect3 outputVec = GetCorrectIntersectionPoint(_wpA,_wpB,guidance_radius);
        larcfm::Position outputPos(projAtPos.inverse(outputVec));
        offsetPos = outputPos;
    } else {
        // Otherwise, check projection of position onto flight plan
        if(projection <= 0){
            // If behind wpA, track to wpA
            offsetPos =  fp->getPos(nextWP-1);
        } else if(projection >= 1){
            // If past wpB, track to wpB
            offsetPos =  fp->getPos(nextWP);
        } else {
            // If between wpA and wpB
            if(deviation < xtrkDev){
                // If close enough to flight plan, use guidance circle method
                const larcfm::Vect3 outputVec = GetCorrectIntersectionPoint(_wpA,_wpB,guidance_radius);
                larcfm::Position outputPos(projAtPos.inverse(outputVec));
                offsetPos = outputPos;
            } else {
                // If far from flight plan, track to closest point on flight plan
                const larcfm::Position outputPos(projAtWPA.inverse(closestPoint));
                offsetPos = outputPos;
            }
        }
    }

    return deviation;
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


void Guidance::ComputePlanGuidance(){

    // Get next waypoint index
    int nextWP = nextWpId[activePlanId];

    if(nextWP >= currentPlan->size()){
        // Return with 0 velocity if the flightplan was computed
        wpReached = true;
        outputCmd = larcfm::Velocity::makeTrkGsVs(0.0,"degree",0.0,"m/s",0.0,"m/s");
        return;
    }

    // Compute to new speed to track
    double speedRef = ComputeSpeed();

    // Compute new heading to track
    double newheading = ComputeNewHeading(speedRef);


    // Compute climb rate to track
    double climbrate = ComputeClimbRate(speedRef);

    // Filter output commands
    FilterCommand(newheading,speedRef,climbrate);


    // Construct output velocity object
    outputCmd = larcfm::Velocity::makeTrkGsVs(newheading,"degree",speedRef,"m/s",climbrate,"m/s");

    // Check if the waypoint was reached
    CheckWaypointArrival();
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
            
            if(nextWpId[activePlanId] > 1){
                if (currentPlan->getTcpData(nextWpId[activePlanId] - 1).getInformation() == "<BOD>") {
                    mode = LAND;
                }
            }
            break;
        }

        case VECTOR:{
            break;
        }

        case TAKEOFF:{
            break;
        }

        case LAND:{
            larcfm::Position lastPos = currentPlan->getLastPoint().position();
            double trk = currentPos.track(lastPos) * 180/M_PI;
            double gs = 0.1*currentPos.distanceH(lastPos);
            double vs = std::max(0.5*(lastPos.alt() - currentPos.alt()),params.minClimbRate);
            outputCmd = larcfm::Velocity::makeTrkGsVs(trk,"degree",gs,"m/s",vs,"m/s");
            if(currentPos.distanceV(lastPos) < 1){
                nextWpId[activePlanId] = currentPlan->size();
            }
            break;
        }

        case GUIDE_NOOP:break;
        case SPEED_CHANGE: break;
        case ALT_CHANGE: break;

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

void guidInputFlightplanData(void* obj, char planID[],waypoint_t wpts[],int totalwp,double initHeading,bool kinematize,double repairTurnRate){
     std::string planid(planID);
     std::list<waypoint_t> waypoints(wpts,wpts+totalwp);
     ((Guidance*)obj)->InputFlightplanData(planid,waypoints,initHeading,kinematize,repairTurnRate);
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

void SetGuidanceMode(void*obj, GuidanceMode mode,char planID[], int nextWP,bool eta){
    std::string planid(planID);
    ((Guidance*)obj)->SetGuidanceMode(mode,planid,nextWP,eta);
}

void guidGetOutput(void* obj,GuidanceOutput_t* output){
    ((Guidance*)obj)->GetOutput(*output);
}

void ChangeWaypointSpeed(void* obj,char planID[],int wpID,double speed,bool updateAll){
   std::string planid(planID);
   ((Guidance*)obj)->ChangeWaypointSpeed(planID,wpID,speed);
}

void ChangeWaypointAlt(void* obj,char planID[],int wpID,double speed,bool updateAll){
   std::string planid(planID);
   ((Guidance*)obj)->ChangeWaypointAlt(planID,wpID,speed,updateAll);
}

void ChangeWaypointETA(void* obj,char planID[],int wpID,double eta,bool updateAll){
   std::string planid(planID);
   ((Guidance*)obj)->ChangeWaypointETA(planID,wpID,eta,updateAll);
}

int guidGetWaypoint(void* obj,char planID[],int wpid,waypoint_t* wp){
    std::string planid(planID);
    return ((Guidance*)obj)->GetWaypoint(planid,wpid,*wp);
}

void guidSetWindData(void* obj,double windFrom,double windSpeed){
    return ((Guidance*)obj)->SetWindData(windFrom,windSpeed);
}
