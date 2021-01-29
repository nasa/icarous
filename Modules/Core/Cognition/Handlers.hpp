#include "EventHandler.hpp"
#include "Cognition.hpp"

#define MAKE_HANDLER(NAME) std::make_shared<NAME>()

class TakeoffPhaseHandler: public EventHandler<CognitionState_t>{
   retVal_e Initialize(CognitionState_t* state){
       TakeoffCommand takeoff_command;
       Command cmd = {.commandType=Command::TAKEOFF_COMMAND};
       cmd.takeoffCommand = takeoff_command;
       state->cognitionCommands.push_back(cmd);
       state->missionStart = -1;
       return SUCCESS;
   }

   retVal_e Terminate(CognitionState_t* state){
       if(state->takeoffComplete == 1){
           return SUCCESS;
       }else if(state->takeoffComplete != 0){
           LogMessage(state,"[WARNING] | Takeoff failed");
           return SUCCESS;
       }
       return INPROGRESS;
   }
};

class EngageNominalPlan: public EventHandler<CognitionState_t>{
    retVal_e Initialize(CognitionState_t* state){
        if(state->missionStart > 0){
            state->nextWpId["Plan0"] = state->missionStart;
            state->missionStart = -1;
        }

        if(state->takeoffComplete == 1){
            state->takeoffComplete = 0;
        }
        return SUCCESS;
    }

    retVal_e Execute(CognitionState_t* state){
       LogMessage(state,"[HANDLER] | Engage nominal plan");
       SetGuidanceFlightPlan(state,(char*)"Plan0",state->nextWpId["Plan0"]);
       return SUCCESS;
    }
};

class ReturnToMission: public EventHandler<CognitionState_t>{
   retVal_e Initialize(CognitionState_t* state){
        if(PrimaryPlanCompletionTrigger(state)){
            return SHUTDOWN;
        }
        LogMessage(state, "[STATUS] | " + eventName + " | Return to mission");
        state->numSecPaths++;
        std::string pathName = "Plan" + std::to_string(state->numSecPaths);
        larcfm::Plan* fp = GetPlan(&state->flightPlans,"Plan0");

        larcfm::Position positionA,positionB;
        larcfm::Velocity velocityA,velocityB;
        velocityA = state->velocity;
        positionA = state->position;
        if(state->parameters.return2NextWP == 0){
            int wpID = state->nextWpId[fp->getID()];
            double gs  = fp->gsIn(wpID);
            double trk = fp->trkIn(wpID)*180/M_PI;
            double vs  = fp->vsIn(wpID);
            positionB = state->clstPoint;
            velocityB = larcfm::Velocity::makeTrkGsVs(trk,"degree",gs,"m/s",vs,"m/s");
        }else{
            positionB = GetNextWP(fp,state->nextFeasibleWpId);
            velocityB = GetNextWPVelocity(fp,state->nextFeasibleWpId);
            state->nextWpId[state->activePlan->getID()] = state->nextFeasibleWpId;
            if(state->activePlan->getID() == "Plan0"){
                state->nextWpId["Plan0"] += 1;
            }
        }
        FindNewPath(state,pathName,positionA, velocityA, positionB, velocityB);
        SendStatus(state,(char*)"IC:Computing secondary path",6);
        state->request = REQUEST_PROCESSING;
        return SUCCESS;
   }

   retVal_e Execute(CognitionState_t* state){
        if(state->request == REQUEST_RESPONDED){
            state->request = REQUEST_NIL;
            std::string pathName = "Plan" + std::to_string(state->numSecPaths);
            SetGuidanceFlightPlan(state,(char*)pathName.c_str(),1);
           return SUCCESS;
        }else{
            std::string pathName = "Plan" + std::to_string(state->numSecPaths);
            larcfm::Plan *fp = GetPlan(&state->flightPlans,pathName);
            if(fp != nullptr){
                state->request = REQUEST_RESPONDED;
            } 
           return INPROGRESS;
        }
   }
}; 

class ReturnToNextFeasibleWP:public EventHandler<CognitionState_t>{
    retVal_e Initialize(CognitionState_t* state){
        if(PrimaryPlanCompletionTrigger(state)){
            return SHUTDOWN;
        }

        LogMessage(state, "[STATUS] | " + eventName + " | Return to next feasible waypoint");
        state->nextWpId["Plan0"] = state->nextFeasibleWpId;
        state->numSecPaths++;
        std::string pathName = "Plan" + std::to_string(state->numSecPaths);
        larcfm::Position positionB;
        state->nextWpId[state->activePlan->getID()] = state->nextFeasibleWpId;
        larcfm::Plan* fp = GetPlan(&state->flightPlans,"Plan0");
        positionB = GetNextWP(fp,state->nextFeasibleWpId);

        larcfm::Position positionA = state->position;
        larcfm::Velocity velocityA = state->velocity;
        larcfm::Velocity velocityB = GetNextWPVelocity(fp,state->nextFeasibleWpId);
        FindNewPath(state,pathName,positionA, velocityA, positionB, velocityB);
        SendStatus(state,(char*)"IC:Computing secondary path",6);
        if(state->activePlan->getID() == "Plan0"){
            state->nextWpId["Plan0"] += 1;
        }
        state->request = REQUEST_PROCESSING;
        return SUCCESS;
    }

    retVal_e Execute(CognitionState_t* state){
        if(state->request == REQUEST_RESPONDED){
            state->request = REQUEST_NIL;
            std::string pathName = "Plan" + std::to_string(state->numSecPaths);
            SetGuidanceFlightPlan(state,(char*)pathName.c_str(),1);
           return SUCCESS;
        }else{
            std::string pathName = "Plan" + std::to_string(state->numSecPaths);
            larcfm::Plan *fp = GetPlan(&state->flightPlans,pathName);
            if(fp != nullptr){
                state->request = REQUEST_RESPONDED;
            } 
           return INPROGRESS;
        }
    }
};

class LandPhaseHandler: public EventHandler<CognitionState_t>{
   retVal_e Execute(CognitionState_t* state){
       SendStatus(state, (char *)"IC: Landing", 6);
       LandCommand land_command;
       Command cmd = {.commandType = Command::LAND_COMMAND};
       cmd.landCommand = land_command;
       state->cognitionCommands.push_back(cmd);
       state->missionStart = -2;
       LogMessage(state,"[FLIGHT_PHASES] | LANDING -> IDLE");
       return SUCCESS;
   }
};

class TrafficConflictHandler: public EventHandler<CognitionState_t>{
 public:
    double startTime;

    retVal_e Initialize(CognitionState_t* state){
         state->resolutionStartSpeed = state->velocity.gs();
         state->prevResSpeed  = state->resolutionStartSpeed;
         state->prevResTrack  = state->hdg;
         state->prevResAlt    = larcfm::Units::to(larcfm::Units::m,state->position.alt());
         state->prevResVspeed = larcfm::Units::to(larcfm::Units::mps,state->velocity.vs());
         state->preferredSpeed = state->prevResSpeed;
         state->preferredTrack = state->prevResTrack;
         state->preferredAlt   = state->prevResAlt;
         state->trafficConflictStartTime = state->utcTime;

         state->resType = GetResolutionType(state);

         int ind = state->resType;
         if (state->validResolution[ind]){
             
             if (state->resType == SPEED_RESOLUTION) {
                 LogMessage(state, "[STATUS] | "+ eventName +" | Resolving traffic conflict with speed resolution");
             }
             else if (state->resType == ALTITUDE_RESOLUTION) {
                 LogMessage(state, "[STATUS] | "+ eventName +" | Resolving traffic conflict with altitude resolution");
             }
             else if (state->resType == TRACK_RESOLUTION) {
                 LogMessage(state, "[STATUS] | "+ eventName +" | Resolving traffic conflict with track resolution");
                 LogMessage(state, "[MODE] | Guidance Vector Request");
             }
             else if(state->resType == VERTICALSPEED_RESOLUTION ){
                 LogMessage(state, "[STATUS] | "+ eventName +" | Resolving traffic conflict with vertical speed resolution");
                 LogMessage(state, "[MODE] | Guidance Vector Request");
             }
         }else{
             return RESET;
         }

         return SUCCESS;
    }

    retVal_e Execute(CognitionState_t *state) {

        int resolutionType = state->resType;
      
        // If track, gs and vs are in recovery, don't allow combined resolution
        if (state->recovery[TRACK_RESOLUTION] && 
            state->recovery[SPEED_RESOLUTION] && 
            state->recovery[VERTICALSPEED_RESOLUTION]) {
            if (resolutionType == TRACK_SPEED_VS_RESOLUTION) {
                resolutionType = TRACK_RESOLUTION;
            }
        }
        larcfm::Position target = GetPlan(&state->flightPlans,"Plan0")->getPos(state->nextFeasibleWpId);

        switch (resolutionType) {

        case SPEED_RESOLUTION: {
            if (fabs(state->preferredSpeed - state->prevResSpeed) >= 0.1) {
                SetGuidanceSpeedCmd(state,state->activePlan->getID(), state->preferredSpeed);
                state->prevResSpeed = state->preferredSpeed;
            }
            state->returnSafe = ComputeTargetFeasibility(state,target);
            break;
        }

        case ALTITUDE_RESOLUTION:
        {
            double climb_rate = 0.0;
            double diff;

            double current_alt = state->position.alt();
            double alt_pref = state->preferredAlt;
            bool newTargetAlt = fabs(alt_pref - state->prevResAlt) > 1e-3;
            bool prevTargetReached = fabs(current_alt - state->prevResAlt) < 2;
            /* 
             * Implement an altitude resolution with the following criteria:
             * Check if the previous target altitude has already been reached 
             * before implementing a new updated resolution.
             */
            if ((newTargetAlt && prevTargetReached) || state->newAltConflict)
            {
                state->newAltConflict = false;
                SetGuidanceAltCmd(state,state->activePlan->getID(), alt_pref, 1);
                state->prevResAlt = alt_pref;
            }

            state->returnSafe = ComputeTargetFeasibility(state,target);

            break;
        }

        case TRACK_RESOLUTION:
        {
            double speed = state->resolutionStartSpeed;
            double climb_rate = 0;
            SetGuidanceVelCmd(state,state->preferredTrack, speed, climb_rate);
            state->prevResTrack = state->preferredTrack;
            state->returnSafe = ComputeTargetFeasibility(state,state->clstPoint);
            state->closestPointFeasible = state->returnSafe;
            state->returnSafe &= ComputeTargetFeasibility(state,target);
            break;
        }

        case VERTICALSPEED_RESOLUTION:
        {
            double speed = state->resolutionStartSpeed;
            double res_up = state->resVUp;
            double res_down = state->resVDown;
            // If there is a valid up resolution, execute up resolution.
            // else execute the down resolution. If 0 vertical speed is possible,
            // that is preferred over the up or down resolutions.
            if (!std::isinf(res_up) && !std::isnan(res_up))
            {
                if (res_up >= 1e-3)
                {
                    SetGuidanceVelCmd(state,state->hdg, speed, -res_up);
                    state->prevResVspeed = res_up;
                }
                else
                {
                    SetGuidanceVelCmd(state,state->hdg, speed, 0.0);
                    state->prevResVspeed = 0.0;
                }
            }
            else if (!std::isinf(res_down) && !std::isnan(res_down))
            {
                if (res_down <= -1e-3)
                {
                    SetGuidanceVelCmd(state,state->hdg, speed, -res_down);
                    state->prevResVspeed = res_down;
                }
                else
                {
                    SetGuidanceVelCmd(state,state->hdg, speed, 0.0);
                    state->prevResVspeed = 0.0;
                }
            }
            else
            {
                SetGuidanceVelCmd(state,state->hdg, speed, state->prevResVspeed);
            }

            uint8_t val;
            if (state->vsBandsNum > 0)
            {
                val = 0;
            }
            else
            {
                val = 1;
            }

            state->returnSafe = (bool)val;
            break;
        }

        case TRACK_SPEED_VS_RESOLUTION:
        {
            double outTrack = state->preferredTrack;
            double outSpeed = state->preferredSpeed;
            double outVS = state->preferredVSpeed;
            if (state->recovery[TRACK_RESOLUTION])
            {
                outTrack = std::fmod(360 + state->velocity.trk() * 180 / M_PI, 360);
            }

            if (state->recovery[SPEED_RESOLUTION])
            {
                outSpeed = state->velocity.gs();
            }

            if (state->recovery[VERTICALSPEED_RESOLUTION])
            {
                outVS = state->velocity.vs();
            }

            SetGuidanceVelCmd(state,outTrack, outSpeed, outVS);
            state->returnSafe = ComputeTargetFeasibility(state,target);
        }

        default:
        {

            break;
        }
        }

        bool conflict = state->trafficConflict || (!state->returnSafe);
        if(conflict){
            return INPROGRESS;
        }else{
            startTime = state->utcTime;
            return SUCCESS;
        }
    }

    retVal_e Terminate(CognitionState_t* state){
        
        if(state->utcTime - startTime < state->parameters.persistenceTime){
            return INPROGRESS;
        }
        std::string planid = state->activePlan->getID();
        if(state->resType == SPEED_RESOLUTION){
            SetGuidanceSpeedCmd(state,state->activePlan->getID(),state->resolutionStartSpeed);
            SetGuidanceFlightPlan(state,planid,state->nextWpId[planid]);
        }else if(state->resType == ALTITUDE_RESOLUTION){
            double alt = state->activePlan->getPos(state->nextWpId[planid]).alt();
            if(fabs(state->prevResAlt - alt) > 1e-3){
                SetGuidanceAltCmd(state,planid,alt,1);
                state->prevResAlt = alt;
                SetGuidanceFlightPlan(state,planid,state->nextWpId[planid]);
            }
        }else{
            ExecuteHandler(MAKE_HANDLER(ReturnToMission),"PostTrafficConflict");
        }
        std::string message = " against ";
        for(auto id: state->conflictTraffics){
            if (id != ""){
                message += id + ", ";
            }
        }
        SendStatus(state,(char*)"IC:traffic conflict resolved",6);
        LogMessage(state,"[RESOLVED] | Traffic conflict resolved"+message);
        state->conflictTraffics.clear();
        return SUCCESS;
    }

};


class MergingHandler: public EventHandler<CognitionState_t>{
    retVal_e Execute(CognitionState_t* state){
        if(state->mergingActive == 2 || state->mergingActive == 0){
            return SUCCESS;
        }else{
            return INPROGRESS;
        }
    }

    retVal_e Terminate(CognitionState_t* state){
        SetGuidanceFlightPlan(state,(char*)"Plan0",state->nextFeasibleWpId);
        LogMessage(state,"[FLIGHT_PHASES] | MERGING -> CRUISE");
        return SUCCESS;
    }

};

class RequestDitchSite: public EventHandler<CognitionState_t>{
    retVal_e Execute(CognitionState_t* state){
        SetDitchSiteRequestCmd(state);
        LogMessage(state,"[STATUS] | Sending Ditch Request Signal");
        return SUCCESS;
    }
};

class ProceedToDitchSite: public EventHandler<CognitionState_t>{
   
    retVal_e Initialize(CognitionState_t* state){
        // Find a path to TOD
        larcfm::Position positionA = state->position.mkAlt(state->todAltitude);
        double trk = std::fmod(2*M_PI + state->ditchSite.track(positionA),2*M_PI);
        larcfm::Position positionB = state->ditchSite.linearDist2D(trk,state->todAltitude).mkAlt(state->todAltitude);
        double trkGoal = positionA.track(positionB)*180/M_PI;
        larcfm::Velocity velocityA = state->velocity;
        larcfm::Velocity velocityB = larcfm::Velocity::makeTrkGsVs(trkGoal,"degree",3,"m/s",0,"m/s");
        FindNewPath(state,"DitchPath",positionA, velocityA, positionB, velocityB);
        SendStatus(state,(char*)"IC:Computing ditch path",6);
        state->request = REQUEST_PROCESSING;
        state->ditch = false;
        return SUCCESS;
    }

    retVal_e Execute(CognitionState_t* state){
        if(state->request == REQUEST_RESPONDED){
            state->request = REQUEST_NIL;
            SetGuidanceFlightPlan(state,"DitchPath",1);
           return SUCCESS;
        }else{
            larcfm::Plan *fp = GetPlan(&state->flightPlans,"DitchPath");
            if(fp != nullptr){
                state->request = REQUEST_RESPONDED;
            } 
           return INPROGRESS;
        }
        
    }

};

class ProceedFromTODtoLand: public EventHandler<CognitionState_t>{
   retVal_e Initialize(CognitionState_t* state){
       LogMessage(state,"[STATUS] | Reached TOD, proceeding to land");
       SetGuidanceP2P(state,state->ditchSite,1.5);
       return SUCCESS;
   }

   retVal_e Execute(CognitionState_t* state){
       if(state->ditch){
           // if we receive the ditch signal again while tryin to land
           // assume current ditch site is infeasible.
           return SUCCESS; 
       }else if(state->p2pComplete){
           state->activePlan = nullptr;
           return SUCCESS;
       }else{
           return INPROGRESS;
       }
   }

   retVal_e Terminate(CognitionState_t* state){
       LogMessage(state,"[STATUS] | Execute land handler");
       ExecuteHandler(MAKE_HANDLER(LandPhaseHandler),"PostTODtoLand");
       return SUCCESS;
   }

};