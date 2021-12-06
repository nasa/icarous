/**
 * @file Handlers.hpp
 * @brief Defines all handlers used in Cognition
 * @details Handlers are inherited from the EventHandler base class.
 * The derived class should provide suitable implementations for the Initialize, Execute and Terminate member functions
 */
#include "EventHandler.hpp"
#include "Cognition.hpp"

#define MAKE_HANDLER(NAME) std::make_shared<NAME>()

/**
 * @brief Handler to activate nominal mission path 
 */
class EngageNominalPlan: public EventHandler<CognitionState_t>{
    public:
    retVal_e Execute(CognitionState_t* state){
       LogMessage(state,"[HANDLER] | Engage nominal plan");
       /// Enqueue the nominal mission plan and the next waypoint to fly to
       SetGuidanceFlightPlan(state,state->missionPlan,state->nextWpId[state->missionPlan]);
       state->icReady = true;
       return SUCCESS;
    }
};

/**
 * @brief Handler for takeoff trigger
 */
class TakeoffPhaseHandler: public EventHandler<CognitionState_t>{
   public:
   double takeoffStartTime; ///< Keep track of time since we sent the takeoff command
   
   retVal_e Initialize(CognitionState_t* state){
       LogMessage(state,"[HANDLER] | Takeoff Phase");
       /// Enqueue the takeoff command
       TakeoffCommand takeoff_command;
       Command cmd = {.commandType=CommandType_e::TAKEOFF_COMMAND};
       cmd.takeoffCommand = takeoff_command;
       state->cognitionCommands.push_back(cmd);
       state->missionStart = -1;
       state->takeoffState = TAKEOFF_INPROGESS;
       takeoffStartTime = state->utcTime;
       return SUCCESS;
   }

   retVal_e Execute(CognitionState_t* state){
       /// Wait till we've obtained confirmation of takeoff
       if(state->takeoffState == TAKEOFF_COMPLETE){
           return SUCCESS;
       }else if(state->utcTime - takeoffStartTime > 5){
           /// If no confirmation was received, let's try restarting takeoff.
           LogMessage(state,"[WARNING] | Takeoff failed. Restarting takeoff");
           return RESET;
       }
       return INPROGRESS;
   }

   retVal_e Terminate(CognitionState_t* state){
       /// If takeoff is successful, spawn the EngageNominalPlan handler
       if(state->takeoffState == TAKEOFF_COMPLETE){
           state->nextWpId[state->missionPlan] = 1;
           ExecuteHandler(MAKE_HANDLER(EngageNominalPlan),"Departure");
       }
       return SUCCESS;
   }
};

/**
 * @brief A return to mission handler using only vector commands
 * 
 */
class Vector2Mission: public EventHandler<CognitionState_t>{
   public:
   larcfm::Position target; ///< target position we are currently vectoring to
   double gs;               ///< ground speed
   retVal_e Initialize(CognitionState_t* state){
       LogMessage(state,"[HANDLER] | Vector to mission");
       LogMessage(state, "[STATUS] | " + eventName + " | Vectoring to mission");
       /// Extract current ground speed at initialization and maintain this speed
       gs = state->velocity.gs(); 
       /// Two possible targets based on user input
       if(state->parameters.return2NextWP){
           /// - Vector to next feasible waypoint in the mission plan
           larcfm::Plan* fp = GetPlan(&state->flightPlans,state->missionPlan);
           if(state->missionPlan == "Plan0"){
               state->nextWpId[state->missionPlan] =state->nextFeasibleWpId;
           }
           target = fp->getPos(state->nextWpId[state->missionPlan]);
       }else{
           /// - Vector to nearest point on the mission flightplan
           target = state->clstPoint;
           larcfm::Plan* fp = GetPlan(&state->flightPlans,state->missionPlan);
           int nextWP = state->nextWpId[state->missionPlan];
           GetNearestPositionOnPlan(fp, state->position, nextWP);
           state->nextWpId[state->missionPlan] =nextWP;
       }
       return SUCCESS;
   }

   retVal_e Execute(CognitionState_t* state){
       /// get heading to target
       double trkRef = state->position.track(target) * 180/M_PI; 
       /// compute proportial control to reach target altitude
       double vs = 0.1*(target.alt() - state->position.alt());   
       double dist = state->position.distanceH(target);         
       double trkCurrent = state->velocity.track("degree");
       double trkCmd;
       int direction = 1;
       double diff = fmod(360 + fabs(trkCurrent - trkRef),360);
       /// Determine turn direction based on turn conflicts
       if(diff >= 45){
           if (state->rightTurnConflict) {
               /// - left turn if right turn conflict
               trkCmd = trkCurrent -2;
           }else if(state->leftTurnConflict){
               /// - right turn if left turn conflict
               trkCmd = trkCurrent + 2;
           }else{
               trkCmd = trkRef;
           }

       }else{
           trkCmd = trkRef;
       }
       
       /// speed reduction when near target to facilitate capture
       if (dist < 200) {
          gs = fmin(gs, dist * 0.25);
       }

       /// Limit climb rate within [-2.5,2.5] m/s. TODO: Use parameters here
       vs = std::max(-2.5,std::min(vs,2.5));                    

       /// Enqueue the computed velocity commands
       SetGuidanceVelCmd(state,trkCmd,gs,vs);

       /// Finish vectoring if in the vicinity
       if ( dist < fmax(10,2.5*gs)){
           return SUCCESS;
       }else{
           return INPROGRESS;
       }
   }

   retVal_e Terminate(CognitionState_t* state){
       /// Engage nominal mission plan upon termination
       ExecuteHandler(MAKE_HANDLER(EngageNominalPlan),"");
       return SUCCESS;
   }
};

/**
 * @brief Return to mission using a flightplan 
 * 
 */
class ReturnToMission: public EventHandler<CognitionState_t>{
   public:
   retVal_e Initialize(CognitionState_t* state){
        /// Only execute this handler if the nominal plan was not completed
        if(PrimaryPlanCompletionTrigger(state)){
            return SHUTDOWN;
        } 

        /// Only execute if cognition is active
        if(!state->parameters.active){
             return SHUTDOWN;
        }
  
        LogMessage(state,"[HANDLER] | Return to mission");
        LogMessage(state, "[STATUS] | " + eventName + " | Return to mission");
        state->numSecPaths++;
        std::string pathName = "Plan" + std::to_string(state->numSecPaths);
        larcfm::Plan* fp = GetPlan(&state->flightPlans,state->missionPlan);

        /// Extract current position and velocity
        larcfm::Position positionA,positionB;
        larcfm::Velocity velocityA,velocityB;
        velocityA = state->velocity;
        positionA = state->position;

        /// Two possible options for goals
        if(!state->parameters.return2NextWP){
            /// - Plan to the closest point
            int wpID = state->nextWpId[fp->getID()];
            double gs  = fp->gsIn(wpID);
            double trk = fp->trkIn(wpID)*180/M_PI;
            double vs  = fp->vsIn(wpID);
            positionB = state->clstPoint;
            velocityB = larcfm::Velocity::makeTrkGsVs(trk,"degree",gs,"m/s",vs,"m/s");
        }else{
            /// - Plan to the next feasible waypoint
            if(state->missionPlan == "Plan0"){
                state->nextWpId[state->missionPlan] = state->nextFeasibleWpId;
            }
            int index = state->nextWpId[state->missionPlan];
            positionB = GetNextWP(fp,index);
            velocityB = GetNextWPVelocity(fp,index);
        }

        /// Send out path planning request
        FindNewPath(state,pathName,positionA, velocityA, positionB, velocityB);
        SendStatus(state,(char*)"IC:Computing secondary path",6);
        state->pathRequest = REQUEST_PROCESSING;
        return SUCCESS;
   }

   retVal_e Execute(CognitionState_t* state){
        if(state->pathRequest == REQUEST_RESPONDED){
            /// Activate new plan if the new plan was received
            state->pathRequest = REQUEST_NIL;
            std::string pathName = "Plan" + std::to_string(state->numSecPaths);
            SetGuidanceFlightPlan(state,pathName,1);
           return SUCCESS;
        }else{
            /// Check for status of path request if request hasn't been responded yet. TODO: Perhaps a timeout is necessary here
            std::string pathName = "Plan" + std::to_string(state->numSecPaths);
            larcfm::Plan *fp = GetPlan(&state->flightPlans,pathName);
            if(fp != nullptr){
                state->pathRequest = REQUEST_RESPONDED;
            } 
           return INPROGRESS;
        }
   }
}; 

/**
 * @brief Handler to plan a path to the next feasible waypoint
 * 
 */
class ReturnToNextFeasibleWP:public EventHandler<CognitionState_t>{
    retVal_e Initialize(CognitionState_t* state){
        LogMessage(state,"[HANDLER] | Return to next feasible WP");
        if(PrimaryPlanCompletionTrigger(state)){
            return SHUTDOWN;
        }

        /// Only execute when cognition is active
        if(!state->parameters.active){
             return SHUTDOWN;
        }

        /// Extract the next feasible waypoint and velocity at next feasible waypoit in the nominal path
        if(state->missionPlan == "Plan0"){
            state->nextWpId[state->missionPlan] = state->nextFeasibleWpId;
        }
        int index = state->nextWpId[state->missionPlan];

        LogMessage(state, "[STATUS] | " + eventName + " | Return to next feasible waypoint " + std::to_string(index));
        state->numSecPaths++;
        std::string pathName = "Plan" + std::to_string(state->numSecPaths);
        larcfm::Position positionB;
        larcfm::Plan* fp = GetPlan(&state->flightPlans,state->missionPlan);
        positionB = GetNextWP(fp,index);
        larcfm::Velocity velocityB = GetNextWPVelocity(fp,index);

        /// Extract current position and velocity 
        larcfm::Position positionA = state->position;
        larcfm::Velocity velocityA = state->velocity;

        /// Send a path request
        FindNewPath(state,pathName,positionA, velocityA, positionB, velocityB);
        SendStatus(state,(char*)"IC:Computing secondary path",6);
        state->pathRequest = REQUEST_PROCESSING;
        return SUCCESS;
    }

    retVal_e Execute(CognitionState_t* state){
        /// Check if the path request was resoponded
        if(state->pathRequest == REQUEST_RESPONDED){
            state->pathRequest = REQUEST_NIL;
            std::string pathName = "Plan" + std::to_string(state->numSecPaths);
            /// - Activate new plan when received
            SetGuidanceFlightPlan(state,pathName,1);
            /// - Reset previous line of sight status. This will be updated by data from trajectory monitoring
            state->lineOfSight2GoalPrev = true;
           return SUCCESS;
        }else{
            std::string pathName = "Plan" + std::to_string(state->numSecPaths);
            larcfm::Plan *fp = GetPlan(&state->flightPlans,pathName);
            if(fp != nullptr){
                state->pathRequest = REQUEST_RESPONDED;
            } 
           return INPROGRESS;
        }
    }
};

/**
 * @brief Landing handler 
 * 
 */
class LandPhaseHandler: public EventHandler<CognitionState_t>{
   retVal_e Initialize(CognitionState_t* state){
       LogMessage(state,"[HANDLER] | Land phase");
       return SUCCESS;
   }

   retVal_e Execute(CognitionState_t* state){
       /// Enqueue the land command
       SendStatus(state, (char *)"IC: Landing", 6);
       LandCommand land_command;
       Command cmd = {.commandType = CommandType_e::LAND_COMMAND};
       cmd.landCommand = land_command;
       state->cognitionCommands.push_back(cmd);
       state->missionStart = -2;
       state->activePlan = nullptr;
       LogMessage(state,"[FLIGHT_PHASES] | LANDING -> IDLE");
       return SUCCESS;
   }
};

/**
 * @brief Traffic conflict handler (for daidalus guidance) 
 * 
 */
class TrafficConflictHandler: public EventHandler<CognitionState_t>{
 public:
    double startTime;

    retVal_e Initialize(CognitionState_t* state){
         LogMessage(state,"[HANDLER] | Traffic Conflict");
         /// Initialize required variables
         state->resolutionStartSpeed = state->velocity.gs();
         state->prevResSpeed  = state->resolutionStartSpeed;
         state->prevResTrack  = state->hdg;
         state->prevResAlt    = larcfm::Units::to(larcfm::Units::m,state->position.alt());
         state->prevResVspeed = larcfm::Units::to(larcfm::Units::mps,state->velocity.vs());
         state->preferredSpeed = state->prevResSpeed;
         state->preferredTrack = state->prevResTrack;
         state->preferredAlt   = state->prevResAlt;
         state->trafficConflictStartTime = state->utcTime;

         /// Determine the resolution type
         state->resType = GetResolutionType(state);

         /// Check if we have a valid resolution
         int ind = state->resType;
         if (state->validResolution[ind]){

             std::string message = " against ";
             for(auto id: state->conflictTraffics){
                 if (id != ""){
                     message += id + ", ";
                 }
             }


             if (state->resType == SPEED_RESOLUTION) {
                 LogMessage(state, "[STATUS] | "+ eventName +" | Resolving traffic conflict with speed resolution" + message);
             }
             else if (state->resType == ALTITUDE_RESOLUTION) {
                 LogMessage(state, "[STATUS] | "+ eventName +" | Resolving traffic conflict with altitude resolution" + message);
             }
             else if (state->resType == TRACK_RESOLUTION) {
                 LogMessage(state, "[STATUS] | "+ eventName +" | Resolving traffic conflict with track resolution" + message);
                 LogMessage(state, "[MODE] | Guidance Vector Request");
             }
             else if(state->resType == VERTICALSPEED_RESOLUTION ){
                 LogMessage(state, "[STATUS] | "+ eventName +" | Resolving traffic conflict with vertical speed resolution" + message);
                 LogMessage(state, "[MODE] | Guidance Vector Request");
             }
         }else{
             /// Shutdown handler if no valid resolution
             return SHUTDOWN;
         }

         /// Shutdown handler if cognition is required to remain passive
         if(!state->parameters.active){
             return SHUTDOWN;
         }

         return SUCCESS;
    }

    retVal_e Execute(CognitionState_t *state) {

        int resolutionType = state->resType;
      
        /// If track, gs and vs are in recovery, don't allow combined resolution
        if (state->recovery[TRACK_RESOLUTION] && 
            state->recovery[SPEED_RESOLUTION] && 
            state->recovery[VERTICALSPEED_RESOLUTION]) {
            if (resolutionType == TRACK_SPEED_VS_RESOLUTION) {
                resolutionType = TRACK_RESOLUTION;
            }
        }

        /// Extract next waypoint we are flying to
        if(state->missionPlan == "Plan0"){
            state->nextWpId[state->missionPlan] = state->nextFeasibleWpId;
        }
        int index = state->nextWpId[state->missionPlan];
        larcfm::Position target = GetPlan(&state->flightPlans,state->missionPlan)->getPos(index);

        /**
         * Execute the specified resolution. Note that all resolution are constantly check if it safe
         * to return to target position 
         */
        switch (resolutionType) {
        case SPEED_RESOLUTION: {
            if (fabs(state->preferredSpeed - state->prevResSpeed) >= 0.1) {
                /// - Send speed command
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
            if(state->todAltitude > 0){
                alt_pref = state->todAltitude;
            }
            bool newTargetAlt = fabs(alt_pref - state->prevResAlt) > 1e-3;
            bool prevTargetReached = fabs(current_alt - state->prevResAlt) < 10;
            /**
             * Implement an altitude resolution with the following criteria:
             * Check if the previous target altitude has already been reached 
             * before implementing a new updated resolution.
             */
            if ((newTargetAlt && prevTargetReached) || state->newAltConflict)
            {
                state->newAltConflict = false;
                /// - Send altitude command
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
            /// Send velocity command for track resolution
            /// - Use heading provided by daidalus.
            /// - Maintain same speed at beginning of encounter
            /// - No climb rate
            SetGuidanceVelCmd(state,state->preferredTrack, speed, climb_rate);
            state->prevResTrack = state->preferredTrack;
            state->returnSafe = ComputeTargetFeasibility(state,target);
            /// For track resolutions, also check if is safe to return to closest point if that is requested
            if(!state->parameters.return2NextWP){
                state->returnSafe &= ComputeTargetFeasibility(state,state->clstPoint);
                state->closestPointFeasible = state->returnSafe;
            }
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
        /// Combined resolution. TODO: This is the least used resolution. Needs more testing
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

        /// Conflict is resolved only when there are no more conflicts and it is safe to return to target
        bool conflict = state->trafficConflict || (!state->returnSafe);
        if(conflict){
            return INPROGRESS;
        }else{
            startTime = state->utcTime;
            return SUCCESS;
        }
    }

    retVal_e Terminate(CognitionState_t* state){

        /// Do not terminate until the persistence time has elapsed. TODO: Not sure if this is required.
        if(state->utcTime - startTime < state->parameters.persistenceTime){
            return INPROGRESS;
        }
        std::string planid = state->activePlan->getID();
        if(state->resType == SPEED_RESOLUTION){
            /** 
             * For speed resolutions, just activate the nominal path
             * Note that we first send the nominal speed for current leg before activating the nominal path 
             */

            SetGuidanceSpeedCmd(state,state->activePlan->getID(),state->resolutionStartSpeed);
            SetGuidanceFlightPlan(state,planid,state->nextWpId[planid]);
            LogMessage(state,"[HANDLER] | Engage nominal plan");
        }else if(state->resType == ALTITUDE_RESOLUTION){
            /// For altitude resolutions, just activate the nominal path. First, send the nominal altitude for current leg
            double alt = state->activePlan->getPos(state->nextWpId[planid]).alt();
            if(fabs(state->prevResAlt - alt) > 1e-3){
                SetGuidanceAltCmd(state,planid,alt,1);
                state->prevResAlt = alt;
                SetGuidanceFlightPlan(state,planid,state->nextWpId[planid]);
                LogMessage(state,"[HANDLER] | Engage nominal plan");
            }
        }else{
            /// For all other resolutions, Use the ReturnToMission or Vector2Mission handlers as requested by user
            if(!state->parameters.returnVector){
                ExecuteHandler(MAKE_HANDLER(ReturnToMission),"PostTrafficConflict");
            }else{
                ExecuteHandler(MAKE_HANDLER(Vector2Mission),"PostTrafficConflict",priority-0.6);
            }
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

/**
 * @brief Merging handler
 * 
 */
class MergingHandler: public EventHandler<CognitionState_t>{
    public:
    bool mergingSpeedChange = false;
    retVal_e Initialize(CognitionState_t* state){
         LogMessage(state,"[HANDLER] | Merging");
         return SUCCESS;
    }

    retVal_e Execute(CognitionState_t* state){
        /// We just keep track of merging. This ensures we don't execute unnecessary handlers during merging.
        if(state->mergingActive == 2 || state->mergingActive == 0){
            return SUCCESS;
        }else{
            if(state->mergingActive == 3){
                mergingSpeedChange = true;
            }
            return INPROGRESS;
        }
    }

    retVal_e Terminate(CognitionState_t* state){
        /// Upon completion of merging, switch to the nominal path if we've changed speeds
        if(mergingSpeedChange){
            SetGuidanceFlightPlan(state,state->missionPlan,state->nextWpId[state->missionPlan]);
        }
        LogMessage(state,"[FLIGHT_PHASES] | MERGING -> CRUISE");
        return SUCCESS;
    }

};

/**
 * @brief Handler to initiate a ditch request (for Safe2Ditch)
 * 
 */
class RequestDitchSite: public EventHandler<CognitionState_t>{
    public:
    retVal_e Initialize(CognitionState_t* state){
         LogMessage(state,"[HANDLER] | Request ditch site");
         return SUCCESS;
    }

    retVal_e Execute(CognitionState_t* state){
        /// Send ditch request
        SetDitchSiteRequestCmd(state);
        LogMessage(state,"[STATUS] | Sending Ditch Request Signal");
        return SUCCESS;
    }
};

/**
 * @brief Ditching handler
 * 
 */
class ProceedToDitchSite: public EventHandler<CognitionState_t>{
    public:
    retVal_e Initialize(CognitionState_t* state){
        /// Find a path to TOD
        LogMessage(state,"[HANDLER] | Proceed to ditch site");
        larcfm::Position positionA = state->position.mkAlt(state->todAltitude);
        double trk = std::fmod(2*M_PI + state->ditchSite.track(positionA),2*M_PI);
        larcfm::Position positionB = state->ditchSite.linearDist2D(trk,state->todAltitude).mkAlt(state->todAltitude);
        double trkGoal = positionA.track(positionB)*180/M_PI;
        larcfm::Velocity velocityA = state->velocity;
        larcfm::Velocity velocityB = larcfm::Velocity::makeTrkGsVs(trkGoal,"degree",3,"m/s",0,"m/s");
        FindNewPath(state,"DitchPath",positionA, velocityA, positionB, velocityB);
        SendStatus(state,(char*)"IC:Computing ditch path",6);
        state->pathRequest = REQUEST_PROCESSING;
        state->ditch = false;
        return SUCCESS;
    }

    retVal_e Execute(CognitionState_t* state){
        if(state->pathRequest == REQUEST_RESPONDED){
            state->pathRequest = REQUEST_NIL;
            /// Execute path to TOD
            SetGuidanceFlightPlan(state,"DitchPath",1);
            state->missionPlan = "DitchPath";
           return SUCCESS;
        }else{
            larcfm::Plan *fp = GetPlan(&state->flightPlans,"DitchPath");
            if(fp != nullptr){
                state->pathRequest = REQUEST_RESPONDED;
            } 
           return INPROGRESS;
        }
        
    }

};

/**
 * @brief Post Ditch TOD handler
 * 
 */
class ProceedFromTODtoLand: public EventHandler<CognitionState_t>{
   public:
   retVal_e Initialize(CognitionState_t* state){
       /// Upon reaching TOD, proceed to landing site
       LogMessage(state,"[HANDLER] | Proceed from TOD to land");
       LogMessage(state,"[STATUS] | Reached TOD, proceeding to land");
       SetGuidanceP2P(state,state->ditchSite,1.5);
       return SUCCESS;
   }

   retVal_e Execute(CognitionState_t* state){
       if(state->ditch){
           /// if we receive the ditch signal again while tryin to land
           /// assume current ditch site is infeasible.
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

// Handler to return to launch point
class ReturnToLaunch: public EventHandler<CognitionState_t>{
    retVal_e Initialize(CognitionState_t* state){
        LogMessage(state,"[HANDLER] | Return to launch");
        larcfm::Position positionA = state->position;
        double trk = std::fmod(2*M_PI + state->launchPoint.track(positionA),2*M_PI);
        larcfm::Position positionB = state->launchPoint.mkAlt(state->position.alt());
        double trkGoal = positionA.track(positionB)*180/M_PI;
        larcfm::Velocity velocityA = state->velocity;
        larcfm::Velocity velocityB = larcfm::Velocity::makeTrkGsVs(trkGoal,"degree",3,"m/s",0,"m/s");
        FindNewPath(state,"RtlPath",positionA, velocityA, positionB, velocityB);
        SendStatus(state,(char*)"IC:Computing RTL path",6);
        state->pathRequest = REQUEST_PROCESSING;
        return SUCCESS;
    }

    retVal_e Execute(CognitionState_t* state){
        if(state->pathRequest == REQUEST_RESPONDED){
            state->pathRequest = REQUEST_NIL;
            SetGuidanceFlightPlan(state,"RtlPath",1);
            state->missionPlan = "RtlPath";
            return SUCCESS;
        }else{
            larcfm::Plan *fp = GetPlan(&state->flightPlans,"RtlPath");
            if(fp != nullptr){
                state->pathRequest = REQUEST_RESPONDED;
            }
            return INPROGRESS;
        }
    }
};
