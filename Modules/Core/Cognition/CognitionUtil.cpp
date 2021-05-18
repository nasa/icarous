#include "Cognition.hpp"

double ComputeHeading(const larcfm::Position &positionA,const larcfm::Position &positionB){
    return larcfm::Units::to(larcfm::Units::deg,positionA.track(positionB));
}

double ComputeXtrackDistance(const larcfm::Position &prev_wp,
                             const larcfm::Position &next_wp,
                             const larcfm::Position &pos,
                             double offset[]){

    larcfm::EuclideanProjection proj = larcfm::Projection::createProjection(prev_wp);
    larcfm::Vect2 A(0,0);
    larcfm::Vect2 B = proj.project(next_wp).vect2();
    larcfm::Vect2 C = proj.project(pos).vect2();
    larcfm::Vect2 AB = B-A;
    larcfm::Vect2 AC = C-A;

    double distAB   = AB.norm();
    double perpProj = fabs(AC.dot(AB.PerpL().Hat()));
    double straightProj = AC.dot(AB.Hat())/distAB;  
   
    if(offset != NULL){
        offset[0] = perpProj;
        offset[1] = straightProj;
    }

    return perpProj;
}

bool CheckProjectedTrafficConflict(larcfm::Position position, larcfm::Velocity velocity,
                                   larcfm::Position prevWP,larcfm::Position nextWP,
                                   double timeToViolation,double DTHR,double ZTHR){
    bool conflictH,conflictV = true;
    if (!std::isinf(timeToViolation) && !std::isnan(timeToViolation)) {
        double projAlt = position.alt() + velocity.verticalSpeed("m/s") * timeToViolation;
        double targetAlt = nextWP.alt();
        if (fabs(projAlt - targetAlt) > ZTHR) {
            conflictV = false;
        }

        larcfm::Position projPos = position.linearEst(velocity, timeToViolation);
        double offsets[2];
        ComputeXtrackDistance(prevWP, nextWP, projPos, offsets);
        if (offsets[0] <= DTHR/2 && (offsets[1] >= 0 && offsets[1] <= 1))
        {
            conflictH = true;
        }else{
            conflictH = false;
        }
    }
    return conflictH && conflictV;
}

void ManeuverToIntercept(const larcfm::Position &prev_wp,
                         const larcfm::Position &next_wp,
                         const larcfm::Position &curr_position,
                         double x_trk_dev_gain,
                         double resolution_speed,
                         const double allowed_dev,
                         larcfm::Velocity &output_velocity){
    double Vs,Vf,V,sgn;
    double Trk;

    double offsets[2];
    ComputeXtrackDistance(prev_wp,next_wp,curr_position,offsets);
    double cross_track_deviation = offsets[0];

    if(x_trk_dev_gain < 0){
        x_trk_dev_gain = -x_trk_dev_gain;
    }

    Vs = x_trk_dev_gain*cross_track_deviation;
    V  = resolution_speed;

    if(Vs >= 0){
        sgn = 1;
    }
    else{
        sgn = -1;
    }

    if(pow(std::abs(Vs),2) >= pow(V,2)){
        Vs = V*sgn;
    }

    Vf = sqrt(pow(V,2) - pow(Vs,2));

    Trk = prev_wp.track(next_wp);
    double Vn = Vf*cos(Trk) - Vs*sin(Trk);
    double Ve = Vf*sin(Trk) + Vs*cos(Trk);
    double Vu = 0;

    double track = atan2(Ve,Vn)*180/M_PI;
    if(track < 0){
        track = 360 + track;
    }

    output_velocity = larcfm::Velocity::makeTrkGsVs(track,"degree", resolution_speed,"m/s", 0,"m/s");
}

larcfm::Position GetNearestPositionOnPlan(const larcfm::Plan *fp,
                                          const larcfm::Position &current_pos,int &nextWP){
    double offsets[2];
    int totalwp = fp->size();
    larcfm::Position nearest = fp->getPos(nextWP);
    double mindist = MAXDOUBLE;
    for(int i=1;i < totalwp; ++i){
        larcfm::Position prev_wp = fp->getPos(i-1);
        larcfm::Position next_wp = fp->getPos(i);

        /* 
        if(fp->isMOT(i) || fp->isEOT(i)){
            continue;
        }

        // Avoid other tcps in a turn segment
        int prevTurnTCP = fp->prevTrkTCP(i);
        if(prevTurnTCP >= 0){
            if (fp->getTcpData(prevTurnTCP).isBOT() && !fp->getTcpData(prevTurnTCP).isEOT()) {
                continue;
            }
        }*/

        double distAB = prev_wp.distanceH(next_wp); 
        larcfm::Position computedPos;
        if(distAB > 1e-3){
            ComputeXtrackDistance(prev_wp, next_wp, current_pos, offsets);
            double heading2nextWP = prev_wp.track(next_wp);
            double revHeading = next_wp.track(prev_wp);
            if (offsets[1] > 0 && offsets[1] <= 1) {
                computedPos = prev_wp.linearDist2D(heading2nextWP, abs(offsets[1] * distAB));
            }
            else {
                //computedPos = prev_wp.linearDist2D(revHeading, abs(offsets[1] * distAB));
            }
        }else{
            computedPos = next_wp;
        }
        double dist2pos = current_pos.distanceH(computedPos);
        if (dist2pos <= mindist)
        {
            nearest = computedPos;
            mindist = dist2pos;
            nextWP = i;
            if(nextWP == totalwp-1){
                if(dist2pos < 5){
                    nextWP = totalwp;
                }
            }
        }
    }
    return nearest;
}


bool CheckTurnConflict(double low,double high,double new_heading,double old_heading,bool& rightConflict,bool& leftConflict){

    low  = std::fmod(360+low,360);
    high = std::fmod(360+high,360);
    new_heading = std::fmod(360+new_heading,360);
    old_heading = std::fmod(360+old_heading,360);

    double delta = larcfm::Util::turnDelta(old_heading * M_PI/180,new_heading * M_PI/180);
    int turnDir  = larcfm::Util::turnDir(old_heading * M_PI/180,new_heading * M_PI/180);
    bool rightTurn = (turnDir==1);

    if (low > high){
        low -= 360;
    }
    rightConflict = false;
    leftConflict = false;

    bool cond1 = new_heading >= low && new_heading <= high;
    bool cond2 = old_heading >= low && old_heading <= high;

    if(cond1 || cond2){
        rightConflict = true;
        leftConflict = true;
        return true;
    }

    // Check for right turn conflict
    double nheading = new_heading;
    double oheading = old_heading;

    if (nheading < oheading){
        oheading -= 360;
    }
    if( oheading <= low && high <= nheading ){
        rightConflict = true;
    }

    // Check for left turn conflict
    nheading = new_heading;
    oheading = old_heading;

    if (nheading > oheading){
        nheading -= 360;
    }
        
    if( nheading <= low && high <= oheading ){
        leftConflict = true;
    }
    
    if(!rightConflict && !leftConflict){
        if(rightTurn){
            leftConflict = true;
        }else{
            rightConflict = true;
        }
    }

    if(rightConflict && leftConflict){
        return true;
    }else{
        return false;
    }

}

bool ComputeTargetFeasibility(CognitionState_t* state,larcfm::Position target){

    double newtrk = state->position.track(target) * 180 / M_PI;
    double oldtrk = state->velocity.track("degree"); 

    double elapsedTime = state->utcTime - state->trafficConflictStartTime;

    if(elapsedTime <= state->parameters.persistenceTime){
        return false;
    }

    bool conflict = false;
    // Check if it is safe to turn given the current bands available
    // Check feasibility based on trk bands
    state->rightTurnConflict = true;
    state->leftTurnConflict = true;
    for(int i=0;i<state->trkBandNum;++i){
        double low = state->trkBandMin[i];
        double high = state->trkBandMax[i];
        if(state->trkBandType[i] != BANDREGION_NONE && state->trkBandType[i] != BANDREGION_RECOVERY){
            bool leftConflict, rightConflict;
            conflict |= CheckTurnConflict(low,high,newtrk,oldtrk,rightConflict,leftConflict);
            state->rightTurnConflict &= rightConflict;
            state->leftTurnConflict &= leftConflict;
        }
        if(conflict){
            return !conflict; // Negate conflict to denote feasibility
        }
    }

    if(!conflict){
        state->rightTurnConflict = false;
        state->leftTurnConflict = false;
    }

    // Check feasibility based on speed bands
    for(int i=0;i<state->gsBandNum;++i){
        double low = state->gsBandMin[i];
        double high = state->gsBandMax[i];
        if(state->gsBandType[i] != BANDREGION_NONE && state->gsBandType[i] != BANDREGION_RECOVERY){
            double refSpeed = GetPlan(&state->flightPlans,"Plan0")->gsIn(state->nextFeasibleWpId);
            if(refSpeed >= low && refSpeed <= high){
                conflict |= true; 
            }
        }
        if(conflict){
            return !conflict; // Negate conflict to denote feasibility
        }
    }

    // Check feasibility based on alt bands
    for(int i=0;i<state->altBandNum;++i){ 
        double low = state->altBandMin[i];
        double high = state->altBandMax[i];
        if(state->altBandType[i] != BANDREGION_NONE && state->altBandType[i] != BANDREGION_RECOVERY){
            double targetAlt = target.alt();
            if(targetAlt >= low && targetAlt <= high){
                conflict |= true; 
            }
        }
        if(conflict){
            return !conflict; // Negate conflict to denote feasibility
        }
    }
    return !conflict; // Negate conflict to denote feasibility
}

void FindNewPath(CognitionState_t* state,const std::string &PlanID,
                 const larcfm::Position &positionA,
                 const larcfm::Velocity &velocityA,
                 const larcfm::Position &positionB,
                 const larcfm::Velocity &velocityB){
    FpRequest fp_request = {
        "",
        {positionA.latitude(), positionA.longitude(), positionA.alt()},
        {positionB.latitude(), positionB.longitude(), positionB.alt()},
        {larcfm::Units::to(larcfm::Units::deg,velocityA.trk()), 
         larcfm::Units::to(larcfm::Units::mps,velocityA.gs()), 
         larcfm::Units::to(larcfm::Units::mps,velocityA.vs())},
        {larcfm::Units::to(larcfm::Units::deg,velocityB.trk()), 
         larcfm::Units::to(larcfm::Units::mps,velocityB.gs()), 
         larcfm::Units::to(larcfm::Units::mps,velocityB.vs())}
    };
    strcpy(fp_request.name,PlanID.c_str());
    Command cmd = {.commandType=CommandType_e::FP_REQUEST};
    cmd.fpRequest = fp_request;
    state->cognitionCommands.push_back(cmd);

    state->log << state->timeString + "| [PATH_REQUEST] | Computing secondary path: " <<PlanID<<"\n";
}

resolutionType_e GetResolutionType(CognitionState_t* state){
   int resType = state->parameters.resolutionType;
   int resPriority[4];
   if(resType < 9){
      // If only one digit is provide, use that as the resolution
      return (resolutionType_e)resType;
   }else{
       // If more than one digit is provided, consider it as a prioirty list
       // where the most significant digit indicates the highest prioirty 
       for(int i=3; i>=0; --i){
           if(i > 0){
              int fac = pow(10,i);
              resPriority[3-i] = std::min((int) resType/fac,3);
              resType = resType%fac;
           }else{
              resPriority[3-i] =  std::min((int) resType%10,3);
           }
       }
   }

   // Return the first resolution type in the given priority
   // that has no recovery
   for(int i=0;i<=3;++i){
      if(!state->allTrafficConflicts[resPriority[i]]){
          continue;
      }
      if(state->validResolution[resPriority[i]]){
          return (resolutionType_e) resPriority[i];
      }
   }

   // If all the dimensions are in recovery, 
   // return the first resolution in the priority.
   return (resolutionType_e) resPriority[0];
}

void SetGuidanceVelCmd(CognitionState_t* state,const double track,const double gs,const double vs){
    VelocityCommand velocity_command = {
        .vn = gs*cos(track* M_PI/180),
        .ve = gs*sin(track* M_PI/180),
        .vu = vs
    };
    Command cmd = {.commandType = CommandType_e::VELOCITY_COMMAND};
    cmd.velocityCommand = velocity_command;
    state->cognitionCommands.push_back(cmd);
}

void SetGuidanceSpeedCmd(CognitionState_t* state,const std::string &planID,const double speed,const int hold){
    SpeedChange speed_change = {"",speed, hold };
    strcpy(speed_change.name,planID.c_str());
    Command cmd = {.commandType = CommandType_e::SPEED_CHANGE_COMMAND};
    cmd.speedChange = speed_change;
    state->cognitionCommands.push_back(cmd);
}

void SetGuidanceAltCmd(CognitionState_t* state,const std::string &planID,const double alt,const int hold){
    AltChange alt_change = {"",alt, hold };
    strcpy(alt_change.name,planID.c_str());
    Command cmd = {.commandType = CommandType_e::ALT_CHANGE_COMMAND};
    cmd.altChange = alt_change;
    state->cognitionCommands.push_back(cmd);
}

void SetGuidanceFlightPlan(CognitionState_t* state,const std::string &plan_id,const int wp_index){
    state->activePlan = GetPlan(&state->flightPlans,plan_id);
    state->nextWpId[plan_id] = wp_index;

    FpChange fp_change;
    std::memset(fp_change.name,0,25);
    strcpy(fp_change.name,plan_id.c_str());
    fp_change.wpIndex = wp_index;
    fp_change.nextFeasibleWp = state->nextFeasibleWpId;
    Command cmd = {.commandType = CommandType_e::FP_CHANGE};
    cmd.fpChange = fp_change;
    state->cognitionCommands.push_back(cmd);
    if(plan_id == "Plan0"){
        state->log << state->timeString + "| [WPREACHED] | Plan: " << plan_id<<", wp: "<<wp_index-1<<"\n";
    }
    state->log << state->timeString + "| [MODE] | Guidance Flightplan change, Plan: "<<plan_id<<", wp:"<<wp_index<<"\n";
}

void SetGuidanceP2P(CognitionState_t* state,const larcfm::Position &point,const double speed){
    P2PCommand p2p_command = {
        .point = {point.latitude(), point.longitude(), point.alt()},
        .speed = speed
    };
    Command cmd = {.commandType = CommandType_e::P2P_COMMAND};
    cmd.p2PCommand = p2p_command;
    state->cognitionCommands.push_back(cmd);
}

void SetDitchSiteRequestCmd(CognitionState_t* state){
    DitchCommand ditch_command;
    Command cmd = {.commandType=CommandType_e::DITCH_COMMAND};
    cmd.ditchCommand = ditch_command;
    state->cognitionCommands.push_back(cmd);
}

void SetLandCmd(CognitionState_t* state){
    LandCommand land_command;
    Command cmd = {.commandType=CommandType_e::LAND_COMMAND};
    cmd.landCommand = land_command;
    state->cognitionCommands.push_back(cmd);
}

void FindNewPath(CognitionState_t *state,const std::string &planID,const larcfm::Position &positionA,
                const larcfm::Velocity &velocityA,
                const larcfm::Position &positionB,
                const larcfm::Velocity &velocityB);

void SetGuidanceP2P(CognitionState_t *state,const larcfm::Position &point,const double speed);


void SendStatus(CognitionState_t* state,const char buffer[],const uint8_t severity){
    StatusMessage status_message;
    std::memset(status_message.buffer,0,250);
    strcpy(status_message.buffer,buffer);
    status_message.severity = severity;

    Command cmd = {.commandType = CommandType_e::STATUS_MESSAGE};
    cmd.statusMessage = status_message;
    state->cognitionCommands.push_back(cmd);
}

larcfm::Plan* GetPlan(std::list<larcfm::Plan>* flightPlans, const std::string &plan_id){
    for(auto &it : *flightPlans){
        if(it.getID() == plan_id){
            auto *fp = &it;
            return fp;
        }
    };
    return NULL;
}

int GetTotalWaypoints(std::list<larcfm::Plan>* flightPlans,const std::string &plan_id){
    larcfm::Plan *fp = GetPlan(flightPlans,plan_id);
    if (fp != NULL)
        return fp->size();
    else
        return 0;
}

larcfm::Position GetNextWP(larcfm::Plan*fp, int nextWP){
   if(nextWP >= fp->size()){
      nextWP = fp->size() - 1;
   }
   larcfm::Position next_wp_pos = fp->getPos(nextWP);
   return next_wp_pos;
}

larcfm::Velocity GetNextWPVelocity(larcfm::Plan*fp, int nextWP){
   if(nextWP >= fp->size()){
      nextWP = fp->size() - 1;
   }
   double gs = fp->gsOut(nextWP);
   double trk = fp->trkOut(nextWP);
   double vs = fp->vsOut(nextWP);
   return larcfm::Velocity::makeTrkGsVs(trk*180/M_PI,"degree",gs,"m/s",vs,"m/s");
}

larcfm::Position GetPrevWP(larcfm::Plan*fp, int nextWP){
   if(nextWP >= fp->size()){
      nextWP = fp->size() - 1;
   }
   int prev_wp_id = nextWP - 1;
   if(prev_wp_id < 0){
       prev_wp_id = 0;
   }
   larcfm::Position prev_wp_pos = fp->getPos(prev_wp_id);
   return prev_wp_pos;
}

void LogMessage(CognitionState_t* state,std::string message){
    state->log << state->timeString + "| " + message <<"\n";
}