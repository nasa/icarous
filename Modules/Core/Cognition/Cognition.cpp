#include "Cognition.hpp"
#include "WP2Plan.hpp"

Cognition::Cognition(const std::string callsign){
    Initialize();
    parameters.resolutionType = TRACK_RESOLUTION;
    parameters.DTHR = 30;
    parameters.ZTHR = 1000;
    parameters.allowedXtrackDeviation = 1000;
    // Open a log file
    struct timespec  tv;
    clock_gettime(CLOCK_REALTIME,&tv);
    double localT = tv.tv_sec + static_cast<float>(tv.tv_nsec)/1E9;
    callSign = callsign;
    std::string filename("log/Cognition-" + callSign + "-" + std::to_string(localT) + ".log");
    log.open(filename);
}

void Cognition::Initialize(){
    returnSafe = true;
    request = REQUEST_NIL;
    fpPhase = IDLE_PHASE;
    missionStart = -1;
    keepInConflict = false;
    keepOutConflict = false;
    p2pComplete = false;
    takeoffComplete = -1;
    trafficConflictState = NOOPC;
    geofenceConflictState = NOOPC;
    allTrafficConflicts[0] = false;
    allTrafficConflicts[1] = false;
    allTrafficConflicts[2] = false;
    allTrafficConflicts[3] = false;
    XtrackConflictState = NOOPC;
    return2NextWPState = NOOPC;
    requestGuidance2NextWP = -1;
    topOfDescent = false;
    todAltitude = 0;
    ditch = false;
    endDitch = false;
    resetDitch = false;
    primaryFPReceived = false;
    mergingActive = 0;
    wpMetricTime = false;
    emergencyDescentState = SUCCESS;
    ditchSite = larcfm::Position::makeLatLonAlt(0,"deg",0,"deg",0,"m");
    activePlan = nullptr;
    numSecPaths = 0;
    scenarioTime = 0;
}

void Cognition::Reset(){
    Initialize();
    ResetFlightPhases();
}

void Cognition::InputVehicleState(const larcfm::Position &pos,const larcfm::Velocity &vel,const double heading){
    position = pos;
    velocity = vel;
    hdg = heading;
    speed = vel.gs();
}


void Cognition::InputFlightPlanData(const std::string &plan_id,const std::list<waypoint_t> &waypoints,const double initHeading,bool repair){

    larcfm::Plan* fp = GetPlan(plan_id);
    larcfm::Plan newPlan(plan_id); 
    if (fp != NULL){
        fp->clear();
        ConvertWPList2Plan(fp,plan_id,waypoints,initHeading,repair);
    }else{
        fp = &newPlan;
        ConvertWPList2Plan(fp,plan_id,waypoints,initHeading,repair);
        flightPlans.push_back(newPlan);
    }

    nextWpId[plan_id] = 1;
    if(plan_id == "Plan0"){
        primaryFPReceived = true;
        scenarioTime = fp->time(0);
    }
    
    resolutionStartSpeed = fp->gsIn(1);
}

void Cognition::InputTrajectoryMonitorData(const trajectoryMonitorData_t & tjMonData){
    timeToFenceViolation = tjMonData.timeToFenceViolation;
    timeToTrafficViolation3 = tjMonData.timeToTrafficViolation;
    planProjectedFenceConflict = tjMonData.fenceConflict;
    planProjectedTrafficConflict = tjMonData.trafficConflict;
    nextFeasibleWpId = tjMonData.nextFeasibleWP;

    if(tjMonData.offsets[0] > 10){
        planProjectedTrafficConflict = false;
        planProjectedFenceConflict = false;
    }
}

larcfm::Plan* Cognition::GetPlan(const std::string &plan_id){
    for(auto &it : flightPlans){
        if(it.getID() == plan_id){
            auto *fp = &it;
            return fp;
        }
    };
    return NULL;
}

int Cognition::GetTotalWaypoints(const std::string &plan_id){
    larcfm::Plan *fp = GetPlan(plan_id);
    if (fp != NULL)
        return fp->size();
    else
        return 0;
}

bool Cognition::CheckPlanComplete(const std::string &plan_id){
    larcfm::Plan *fp = GetPlan(plan_id);
    if (fp != NULL)
        return (nextWpId[plan_id] >= fp->size());
    else
        return false;
}

larcfm::Position Cognition::GetNextWP(){
   int next_wp_id = nextWpId[activePlan->getID()];
   if(next_wp_id >= activePlan->size()){
      next_wp_id = activePlan->size() - 1;
   }
   larcfm::Position next_wp_pos = activePlan->getPos(next_wp_id);
   return next_wp_pos;
}

larcfm::Velocity Cognition::GetNextWPVelocity(){
   int next_wp_id = nextWpId[activePlan->getID()];
   if(next_wp_id >= activePlan->size()){
      next_wp_id = activePlan->size() - 1;
   }
   double gs = activePlan->gsOut(next_wp_id);
   double trk = activePlan->trkOut(next_wp_id);
   double vs = activePlan->vsOut(next_wp_id);
   return larcfm::Velocity::makeTrkGsVs(trk*180/M_PI,"degree",gs,"m/s",vs,"m/s");
}

larcfm::Position Cognition::GetPrevWP(){
   int next_wp_id = nextWpId[activePlan->getID()];
   if(next_wp_id >= activePlan->size()){
      next_wp_id = activePlan->size() - 1;
   }
   int prev_wp_id = next_wp_id - 1;
   if(prev_wp_id < 0){
       prev_wp_id = 0;
   }
   larcfm::Position prev_wp_pos = activePlan->getPos(prev_wp_id);
   return prev_wp_pos;
}

void Cognition::ReachedWaypoint(const std::string &plan_id, const int reached_wp_id){

    log<<timeString + "| [WPREACHED] | Plan: " << plan_id<<", wp: "<<reached_wp_id<<"\n";
    if(plan_id == "P2P"){
        p2pComplete = true;
    }else if(plan_id == "Takeoff"){
        takeoffComplete = true;

    }else if(plan_id == "PlanM"){
        nextWpId["PlanM"] = reached_wp_id + 1;
        if(nextWpId["PlanM"] >= GetTotalWaypoints("PlanM")){
            mergingActive = 2;
        }

    }else{
        int next_wp_id = reached_wp_id + 1;
        larcfm::Plan* fp = GetPlan(plan_id);
        if (fp != nullptr) {
            activePlan  = fp;
            int total_waypoints = GetTotalWaypoints(plan_id);
            nextWpId[plan_id] = next_wp_id;
            resolutionStartSpeed = activePlan->gsIn(next_wp_id);
            if (next_wp_id < total_waypoints) {
                refWpTime = activePlan->time(next_wp_id);
            }
        }
    }

}

void Cognition::InputParameters(const cognition_params_t &new_params){
    parameters = new_params;
    
}

void Cognition::InputDitchStatus(const larcfm::Position &ditch_site,const double todAlt,const bool ditch_requested){
    ditchSite = ditch_site;
    todAltitude = todAlt;
    ditch = ditch_requested;
}

void Cognition::InputMergeStatus(const int merge_status){
    mergingActive = merge_status;
}

void Cognition::InputTrackBands(const bands_t &track_bands){
    utcTime = track_bands.time;
    recovery[TRACK_RESOLUTION] = (bool) track_bands.recovery;
    if (track_bands.currentConflictBand == 1) {
        allTrafficConflicts[TRACK_RESOLUTION] = true;
        if (!std::isnan(track_bands.resPreferred) && !std::isinf(track_bands.resPreferred))
            preferredTrack = track_bands.resPreferred;
        else
            preferredTrack = prevResTrack;
    }
    else
    {
        allTrafficConflicts[TRACK_RESOLUTION] = false;
        preferredTrack = prevResTrack;
    }

    std::memcpy(trkBandType,track_bands.type,sizeof(int)*20);
    std::memcpy(trkBandMin,track_bands.min,sizeof(double)*20);
    std::memcpy(trkBandMax,track_bands.max,sizeof(double)*20);
    trkBandNum = track_bands.numBands;
}

void Cognition::InputSpeedBands(const bands_t &speed_bands){

    recovery[SPEED_RESOLUTION] = (bool)speed_bands.recovery; 
    if(speed_bands.currentConflictBand == 1){
        // Factor to increment or decrement the provided speed resolution by
        // This will eliminate oscillatory behavior due to numerical errors
        allTrafficConflicts[SPEED_RESOLUTION] = true;
        double fac;
        if (fabs(speed_bands.resPreferred - speed_bands.resDown) < 1e-3) {
            // Preferred resolution is to slow down
            fac = 0.99;
        } else {
            // Preferred resolution is to speed up
            fac = 1.01;
        }

        // Use the provided resolution if it is valid
        if (!std::isinf(speed_bands.resPreferred) && !std::isnan(speed_bands.resPreferred)) {
            preferredSpeed = speed_bands.resPreferred * fac;
        }
    }else{
        allTrafficConflicts[SPEED_RESOLUTION] = false;
        preferredSpeed = prevResSpeed;
    }
    std::memcpy(gsBandType,speed_bands.type,sizeof(int)*20);
    std::memcpy(gsBandMin,speed_bands.min,sizeof(double)*20);
    std::memcpy(gsBandMax,speed_bands.max,sizeof(double)*20);
    gsBandNum = speed_bands.numBands;
}

void Cognition::InputAltBands(const bands_t &alt_bands){
    recovery[ALTITUDE_RESOLUTION] = (bool) alt_bands.recovery;
    if(alt_bands.currentConflictBand == 1){
        allTrafficConflicts[ALTITUDE_RESOLUTION] = true;
        if(!std::isinf(alt_bands.resPreferred) && !std::isnan(alt_bands.resPreferred))
            preferredAlt = alt_bands.resPreferred;
        else
            preferredAlt = prevResAlt;
    }else{
        allTrafficConflicts[ALTITUDE_RESOLUTION] = false;
        preferredAlt = prevResAlt;
    }
    timeToTrafficViolation1 = alt_bands.timeToViolation[0];
    timeToTrafficViolation2 = alt_bands.timeToViolation[1];
    std::memcpy(altBandType,alt_bands.type,sizeof(int)*20);
    std::memcpy(altBandMin,alt_bands.min,sizeof(double)*20);
    std::memcpy(altBandMax,alt_bands.max,sizeof(double)*20);
    altBandNum = alt_bands.numBands;
}

void Cognition::InputVSBands(const bands_t &vs_bands){
    recovery[VERTICALSPEED_RESOLUTION] = (bool) vs_bands.recovery;
    resVUp = vs_bands.resUp;
    resVDown = vs_bands.resDown;
    preferredVSpeed = vs_bands.resPreferred;
    vsBandsNum = vs_bands.numBands;

    if(!std::isinf(preferredVSpeed) && !std::isnan(preferredVSpeed)){
       prevResVspeed = preferredVSpeed;
    }else{
        preferredVSpeed = prevResVspeed;
    }
}

void Cognition::InputGeofenceConflictData(const geofenceConflict_t &gf_conflict){

    if(gf_conflict.numConflicts > 0){
        if (gf_conflict.conflictTypes[0] == 0)
            keepInConflict = true;
        else if (gf_conflict.conflictTypes[0] == 1)
            keepOutConflict = true;

        recoveryPosition = larcfm::Position::makeLatLonAlt(gf_conflict.recoveryPosition[0],"deg",
                                                       gf_conflict.recoveryPosition[1],"deg",
                                                       gf_conflict.recoveryPosition[2],"m");
    }else{
        keepOutConflict = false;
        keepInConflict = false;
    }
}

void Cognition::StartMission(const int mission_start_value,const double delay){
    missionStart = mission_start_value;
    scenarioTime += delay;
    activePlan = GetPlan("Plan0");
    if (activePlan != nullptr)
    {
        if (delay > 0)
        {
            activePlan->timeShiftPlan(0, delay);
        }

        
    }
}

int Cognition::GetCognitionOutput(Command &command){
    size_t commands_remaining = cognitionCommands.size();
    if (commands_remaining > 0){
        command = cognitionCommands.front();
        cognitionCommands.pop_front();
    }
    return commands_remaining;
}

void Cognition::ResetFlightPhases() {
    takeoffState = INITIALIZING;
    cruiseState = INITIALIZING;
}

int Cognition::FlightPhases(double time){

    utcTime = time;
    char buffer[25];
    std::sprintf(buffer,"%10.5f",time);
    timeString = std::string(buffer);
    // Handling nominal flight phases
    status_e status;
    switch(fpPhase){

        case IDLE_PHASE:{
            ResetFlightPhases();
            if(missionStart == 0){
                // If missionStart = 0 time to start the misison
  
                if(primaryFPReceived){
                    double fp_time = scenarioTime;
                    if( utcTime >= fp_time ){
                        fpPhase = TAKEOFF_PHASE;
                        missionStart = -1;
                        log << timeString + "| [FLIGHT_PHASES] | IDLE -> TAKEOFF" <<"\n";
                    }else{
                        double time_remanining = (fp_time - utcTime);
                        if(fmod(time_remanining,5) == 0){
                            SendStatus((char*)"FP counting down",6);
                        }
                    }
                }else{

                    log << timeString + "| [WARNING] | Flighplan not loaded" <<"\n";
                    SendStatus((char*)"No flightplan loaded",4);
                    missionStart = -1;
                }
            }else if(missionStart > 0){
                // If missionStart > 0 goto climb state directly
                if(primaryFPReceived){
                    fpPhase = CRUISE_PHASE;
                    log << timeString + "| [FLIGHT_PHASES] | IDLE -> CRUISE" <<"\n";
                }else{
                    SendStatus((char*)"No flightplan loaded",4);
                    log << timeString + "| [WARNING] | Flightplan not loaded" <<"\n";
                }
                missionStart = -1;
            }

            if(ditch){
                emergencyDescentState = INITIALIZING;
                fpPhase = EMERGENCY_DESCENT_PHASE;
                log << timeString + "| [FLIGHT_PHASES] | IDLE -> EMERGENCY_DESCENT" <<"\n";
            }
            break;
        }

        case TAXI_PHASE:{
            break;
        }

        case TAKEOFF_PHASE:{
            //printf("Takeoff phase\n");
            status = Takeoff();
            if (status == SUCCESS){
                fpPhase = CLIMB_PHASE;

                log << timeString + "| [FLIGHT_PHASES] | TAKEOFF -> CLIMB" <<"\n";
            }else if(status == FAILED){
                fpPhase = IDLE_PHASE;
            }
            break;
        }

        case CLIMB_PHASE:{
            status = Climb();
            if (status == SUCCESS){
                fpPhase = CRUISE_PHASE;
                log << timeString + "| [FLIGHT_PHASES] | CLIMB -> CRUISE" <<"\n";
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case CRUISE_PHASE:{
            status = Cruise();
            if (status == SUCCESS){
                fpPhase = DESCENT_PHASE;
                log << timeString + "| [FLIGHT_PHASES] | CRUISE -> DESCENT" <<"\n";
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }

            if( mergingActive == 1){
                fpPhase = MERGING_PHASE;
                log << timeString + "| [FLIGHT_PHASES] | CRUISE -> MERGING" <<"\n";
            }

            if(ditch){
                emergencyDescentState = INITIALIZING;
                fpPhase = EMERGENCY_DESCENT_PHASE;
                log << timeString + "| [FLIGHT_PHASES] | CRUISE -> EMERGENCY_DESCENT" <<"\n";
            }

            if(trafficConflictState == RESOLVE && parameters.resolutionType == DITCH_RESOLUTION){
                trafficConflictState = NOOPC;
                fpPhase = EMERGENCY_DESCENT_PHASE;
                emergencyDescentState = NOOPS;
                DitchCommand ditch_command;
                Command cmd = {.commandType=Command::DITCH_COMMAND};
                cmd.ditchCommand = ditch_command;
                cognitionCommands.push_back(cmd);
                log << timeString + "| [STATUS] | Sending Ditch Request Signal" <<"\n";
            }

            break;
        }

        case DESCENT_PHASE:{
            status = Descent();
            if (status == SUCCESS){
                fpPhase = APPROACH_PHASE;
                log << timeString + "| [FLIGHT_PHASES] | DESCENT -> APPROACH" <<"\n";
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case EMERGENCY_DESCENT_PHASE:{
            status = EmergencyDescent();

            // Check if there is another request for ditching
            // and initialize ditching again
            if(ditch){
                emergencyDescentState = INITIALIZING;
            }

            // If end ditch is requested, kill ditching
            if(endDitch){
                emergencyDescentState = SUCCESS;
                log << timeString + "| [STATUS] | END DITCHING" <<"\n";
            }
            break;
        }

        case APPROACH_PHASE:{
            status = Approach();
            if (status == SUCCESS){
                fpPhase = LANDING_PHASE;
                log << timeString + "| [FLIGHT_PHASES] | APPROACH -> LANDING" <<"\n";
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case LANDING_PHASE:{
            status = Landing();
            //printf("Landing\n");
            if (status == SUCCESS){
                fpPhase = IDLE_PHASE;
                missionStart = -2;
                log << timeString + "| [FLIGHT_PHASES] | LANDING -> IDLE" <<"\n";
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case MERGING_PHASE:{
            if(mergingActive == 2 || mergingActive == 0){
                fpPhase = CRUISE_PHASE;
                SetGuidanceFlightPlan((char*)"Plan0",nextFeasibleWpId);
                log << timeString + "| [FLIGHT_PHASES] | MERGING -> CRUISE" <<"\n";
            }else if(mergingActive == 3){
                //printf("Setting guidance flightplan\n");
                //SetGuidanceFlightPlan((char*)"PlanM",1);
                //mergingActive = 1;
                //nextPrimaryWP += 1;
            }
            break;
        }
    }
    log.flush();
    return fpPhase;
}

status_e Cognition::Takeoff(){
    switch(takeoffState){
       case INITIALIZING:{
           // Send takeoff command to guidance application
           if(!keepInConflict && !keepOutConflict){
               TakeoffCommand takeoff_command;
               Command cmd = {.commandType=Command::TAKEOFF_COMMAND};
               cmd.takeoffCommand = takeoff_command;
               cognitionCommands.push_back(cmd);
               takeoffState = RUNNING;
               takeoffComplete = 0;
           }else{
               takeoffState = FAILED;
               log << timeString + "| [CONFLICT] | Geofence constraint violated before takeoff" <<"\n";
           }
           break;
       }

       case RUNNING:{
           // Wait for confirmation status from guidance application
           if(takeoffComplete == 1){
               takeoffState = SUCCESS;
           }else if(takeoffComplete == 0){
               takeoffState = RUNNING;
           }else{
               takeoffState = FAILED;
               log << timeString + "| [WARNING] | Takeoff failed" <<"\n";
           }
           break;
       }

       case SUCCESS: break;
       case FAILED: break;
       case NOOPS: break;
    }

    return takeoffState;
}

status_e Cognition::Climb(){
    // Currently only a place holder for rotrocraft
    return SUCCESS;
}

status_e Cognition::Cruise(){
   switch(cruiseState){

       case INITIALIZING:{
            SetGuidanceFlightPlan((char*)"Plan0",nextWpId["Plan0"]);
            cruiseState = RUNNING;
            break;
       }

       case RUNNING:{

            bool status = false;

            if(mergingActive == 0){
                status |= TrafficConflictManagement();

                status |= GeofenceConflictManagement();

                status |= XtrackManagement();

                status |= ReturnToNextWP();

                // Perform time management only when following the primary flightplan
                //if(!status){
                //    TimeManagement();
                //}
            }

            if(nextWpId["Plan0"] >= GetTotalWaypoints("Plan0")){
                cruiseState = SUCCESS;
            }
            break;
       }

       case SUCCESS:break;
       case FAILED: break;
       case NOOPS: break;
   }

   return cruiseState;
}

status_e Cognition::Descent(){
    // Currently only a place holder for rotorcraft
    return SUCCESS;
}

status_e Cognition::Approach(){
    // Currently only a place holder for rotorcraft
    return SUCCESS;
}

status_e Cognition::Landing(){
    SendStatus((char*)"IC: Landing",6);

    LandCommand land_command;
    Command cmd = {.commandType=Command::LAND_COMMAND};
    cmd.landCommand = land_command;
    cognitionCommands.push_back(cmd);
    return SUCCESS;
}

status_e Cognition::EmergencyDescent(){
    static bool command_sent;
    larcfm::Position positionA = position.mkAlt(todAltitude);

    double trk = std::fmod(2*M_PI + ditchSite.track(positionA),2*M_PI);
    larcfm::Position positionB = ditchSite.linearDist2D(trk,todAltitude).mkAlt(todAltitude);
    double trkGoal = positionA.track(positionB)*180/M_PI;
    larcfm::Velocity velocityA = velocity;
    larcfm::Velocity velocityB = larcfm::Velocity::makeTrkGsVs(trkGoal,"degree",velocityA.gs(),"m/s",0,"m/s");

    switch(emergencyDescentState){

        case NOOPS:{
            break;
        }

        case INITIALIZING:{
            SendStatus((char*)"IC:Starting to ditch",6);
            command_sent = false;
            request = REQUEST_NIL;

            // Find a new path to the ditch site
            // Note that if a direct path is not availabe, this will find
            // a possible detour.
            FindNewPath("DitchPath",positionA, velocityA, positionB, velocityB);

            request = REQUEST_PROCESSING;
            emergencyDescentState = RUNNING;
            ditch = false;
            break;
        }

        case RUNNING:{
            double dist_to_target = positionA.distanceH(positionB);

            // Proceeed to the top of descent by following the
            // computed flight plan
            if(request == REQUEST_RESPONDED && !topOfDescent){
                SetGuidanceFlightPlan("DitchPath",1);
                request = REQUEST_NIL;
            }else if(request == REQUEST_PROCESSING){
                larcfm::Plan* fp = GetPlan("DitchPath");
                if(fp != nullptr){
                    request = REQUEST_RESPONDED;
                }
            }else if(!topOfDescent){
                // Check if top of descent (TOD) has been reached
                // TOP is calculated assuming a 45 degree flight path angle
                // while descending
                int totalWP = GetPlan("DitchPath")->size();
                if(nextWpId["DitchPath"] >= totalWP){
                    topOfDescent = true;
                    SendStatus((char*)"IC:Reached TOD",6);
                    log << timeString + "| [STATUS] | Reached TOD" <<"\n";
                    command_sent = false;
                }else if(dist_to_target > positionA.alt() && dist_to_target < 2.5 * positionA.alt()){
                    if(!command_sent){
                        SetGuidanceSpeedCmd("DitchPath",3,1);
                        command_sent = true;
                    }
                }

                // Continuously check for conflicts with intruders when proceeding to ditch site
                trafficConflict = allTrafficConflicts[SPEED_RESOLUTION] || 
                                  allTrafficConflicts[TRACK_RESOLUTION] || 
                                  allTrafficConflicts[ALTITUDE_RESOLUTION];
                if(trafficConflict && nextWpId["DitchPath"] >= 2){
                    emergencyDescentState = INITIALIZING;
                }
            }else{
                //TODO: Add parameter for final leg of ditching
                if(!command_sent){
                    SetGuidanceP2P(ditchSite,1.5);
                    command_sent = true;
                }else{
                   if(p2pComplete){
                      emergencyDescentState = SUCCESS;
                   }
                }
            }
            break;
        }

        case SUCCESS:{
            // Once vehicle has reached the final ditch site, we can land
            fpPhase = LANDING_PHASE;
            break;
        }

        case FAILED:{
            break;
        }
    }
    return emergencyDescentState;
}



bool Cognition::GeofenceConflictManagement(){
   bool geofence_conflict = (keepInConflict || keepOutConflict) && !trafficConflict && planProjectedFenceConflict;
   switch(geofenceConflictState){

      case NOOPC:{
         if(planProjectedFenceConflict){
             // Consider this as a conflict if projected time is within the time to turn around
             if(timeToFenceViolation < 10){
                 geofence_conflict = true;
             }else{
                 geofence_conflict = false;
             }
         }

         if(geofence_conflict && (return2NextWPState == NOOPC || return2NextWPState == RESOLVE)){
            SendStatus((char*)"IC: Geofence conflict detected",1);
            log << timeString + "| [CONFLICT] | Geofence conflict detected" <<"\n";
            std::cout<<"Time to violation:"<<timeToFenceViolation<<std::endl;
            return2NextWPState = NOOPC;
            geofenceConflictState = RESOLVE;
            p2pComplete = true;
            requestGuidance2NextWP = 1;
           break;
         }
         return false;
      }

      case INITIALIZE:{
         //TODO: Get recovery position
         SetGuidanceP2P(recoveryPosition,resolutionStartSpeed);
         geofenceConflictState = RESOLVE;
         p2pComplete = false;
         break;
      }

      case RESOLVE:{
         // Wait for confirmation
         bool val = p2pComplete;

         if(val){
            geofenceConflictState = COMPLETE;
         }
         break;
      }

      case COMPLETE:{
         // TODO: Fly to next waypoint
         requestGuidance2NextWP = 1;
         geofenceConflictState = NOOPC;
         SendStatus((char*)"IC:Geofence resolution complete",6);
         log << timeString + "| [RESOLVED] | Geofence conflict resolved" <<"\n";
         break;
      }

      case COMPUTE:{
         break;
      }
   }

   return true;
}

bool Cognition::XtrackManagement(){
   larcfm::Position next_wp_pos = GetNextWP();
   larcfm::Position prev_wp_pos = GetPrevWP();

   double offset[2];
   xtrackDeviation = ComputeXtrackDistance(prev_wp_pos,next_wp_pos,position,offset);
   XtrackConflict = (fabs(xtrackDeviation) > parameters.allowedXtrackDeviation);

   larcfm::Velocity interceptManeuver;
   double interceptHeading = GetInterceptHeadingToPlan(prev_wp_pos,next_wp_pos,position);
   double gain = 0.6;
   ManeuverToIntercept(prev_wp_pos,
                       next_wp_pos,
                       position,gain,
                       resolutionStartSpeed,
                       parameters.allowedXtrackDeviation,
                       interceptManeuver);

   bool conflict = XtrackConflict && (trafficConflictState == NOOPC) && (requestGuidance2NextWP == 0);

   switch (XtrackConflictState) {
      case NOOPC:{
         if(conflict){
            SendStatus((char*)"IC:xtrack conflict",1);
            XtrackConflictState = INITIALIZE;
            log << timeString + "| [CONFLICT] | Xtrack conflict detected" <<"\n";
            break;
         }
         return false;
      }

      case INITIALIZE:{
         // Send appropriate mode change command to guidance module
         XtrackConflictState = RESOLVE;
         break;
      }

      case RESOLVE:{
         // Send velocity command to guidance
         //SetGuidanceVelCmd(larcfm::Units::to(larcfm::Units::deg,interceptManeuver.compassAngle()),
          //                 interceptManeuver.groundSpeed("m/s"),
           //                interceptManeuver.verticalSpeed("m/s"));
         SetGuidanceVelCmd(interceptHeading, resolutionStartSpeed,0.0);

         if(!XtrackConflict){
            XtrackConflictState = COMPLETE;
         }

         break;
      }

      case COMPLETE:{
         //Continue on with current mission
         XtrackConflictState = NOOPC;
         return2NextWPState = NOOPC;
         int next_wp_id = nextWpId[activePlan->getID()];
         SetGuidanceFlightPlan((char*)"Plan0",next_wp_id);
         SendStatus((char*)"IC:xtrack conflict resolved",6);
         log << timeString + "| [RESOLVED] | Xtrack conflict resolved" <<"\n";
         break;
      }

      case COMPUTE:{
         break;
      }
   }

   return true;
}

bool Cognition::TrafficConflictManagement(){

   // Check for traffic conflict
   trafficConflict = (allTrafficConflicts[SPEED_RESOLUTION] || 
                      allTrafficConflicts[ALTITUDE_RESOLUTION] || 
                      allTrafficConflicts[TRACK_RESOLUTION]) && planProjectedTrafficConflict;
   if(!trafficConflict && planProjectedTrafficConflict){
       if(timeToTrafficViolation3 < parameters.lookaheadTime && parameters.resolutionType == SEARCH_RESOLUTION){ 
          trafficConflict = true; 
       }
   }
   switch(trafficConflictState){
      case NOOPC:{
          
         if(trafficConflict){
             SendStatus((char *)"IC:traffic conflict", 1);
             log << timeString + "| [CONFLICT] | Traffic conflict detected"
                 << "\n";
             trafficConflictState = INITIALIZE;
             requestGuidance2NextWP = -1;
             newAltConflict = true;
             trafficConflictStartTime = utcTime;
             break;
         }
         return false;
      }

      case INITIALIZE:{
         trafficConflictState = RESOLVE;
         prevResSpeed  = resolutionStartSpeed;
         prevResTrack  = hdg;
         prevResAlt    = larcfm::Units::to(larcfm::Units::m,position.alt());
         prevResVspeed = larcfm::Units::to(larcfm::Units::mps,velocity.vs());
         preferredSpeed = prevResSpeed;
         preferredTrack = prevResTrack;
         preferredAlt    = prevResAlt;
         requestGuidance2NextWP = 1;

         resType = GetResolutionType();
         
         // Use this only for search based resolution
         if(resType == SEARCH_RESOLUTION){
            return2NextWPState = INITIALIZE;
            log << timeString + "| [STATUS] | Resolving conflict with search resolution" <<"\n";
         }else if(resType == SPEED_RESOLUTION){
            log << timeString + "| [STATUS] | Resolving conflict with speed resolution" <<"\n";
            log << timeString + "| [MODE] | Guidance Speed Request"<<"\n";
         }else if(resType == ALTITUDE_RESOLUTION){
            log << timeString + "| [STATUS] | Resolving conflict with altitude resolution" <<"\n";
            log << timeString + "| [MODE] | Guidance Vector Request"<<"\n";
         }else if(resType == TRACK_RESOLUTION ||
                   parameters.resolutionType == TRACK_RESOLUTION2){
            log << timeString + "| [STATUS] | Resolving conflict with track resolution" <<"\n";
            log << timeString + "| [MODE] | Guidance Vector Request"<<"\n";
         }else if(resType == DITCH_RESOLUTION){
            // The top level statemachine should catch this
            // and transition to the emergency descent state.
            log << timeString + "| [STATUS] | Resolving conflict with ditch resolution" <<"\n";
         }
         break;
      }

      case RESOLVE:{
         if(resType == SEARCH_RESOLUTION){
            // Only for search based resolution
            if(return2NextWPState != NOOPC){
               ReturnToNextWP();
            }else{
               trafficConflictState = NOOPC;
            }
         }else{

            if(resType == TRACK_RESOLUTION2){
                requestGuidance2NextWP = 0;
            }
            if(!RunTrafficResolution()){
               if( !(requestGuidance2NextWP == 0 && !closestPointFeasible) ){
                    trafficConflictState = COMPLETE;
               }
            }
         }

         break;
      }

      case COMPLETE:{
         bool sendStatus = false;
         trafficConflictState = NOOPC;
         if(requestGuidance2NextWP == 0 && activePlan->getID() == "Plan0" && closestPointFeasible){
            sendStatus = true;
         }else if(requestGuidance2NextWP == 1){
            sendStatus = true;
         }

         if(resType == SPEED_RESOLUTION){
            requestGuidance2NextWP = -1;
            trafficConflictState = NOOPC;
            SetGuidanceSpeedCmd(activePlan->getID(),resolutionStartSpeed);
            sendStatus = true;
         }

         if(resType == ALTITUDE_RESOLUTION){
            requestGuidance2NextWP = -1;
            trafficConflictState = NOOPC;
            std::string planid = activePlan->getID();
            double alt = activePlan->getPos(nextWpId[planid]).alt();
            if(fabs(prevResAlt - alt) > 1e-3){
                SetGuidanceAltCmd(planid,alt,1);
                prevResAlt = alt;
            }
            sendStatus = true;
         }

         if(sendStatus){
            SendStatus((char*)"IC:traffic conflict resolved",6);
            log << timeString + "| [RESOLVED] | Traffic conflict resolved" <<"\n";
         }
         return2NextWPState = NOOPC;

         if(requestGuidance2NextWP == 0){
             std::string planid = activePlan->getID();
             SetGuidanceFlightPlan(planid,nextWpId[planid]);
         }
         break;
      }

      case COMPUTE:{
         break;
      }
   }

   return true;
}

bool Cognition::RunTrafficResolution(){

   int resolutionType = resType;

   // If track, gs and vs are in recovery, don't allow combined resolution 
   if(recovery[TRACK_RESOLUTION] && recovery[SPEED_RESOLUTION] && recovery[VERTICALSPEED_RESOLUTION]){
       if(resolutionType == TRACK_SPEED_VS_RESOLUTION){
            resolutionType = TRACK_RESOLUTION;
       }
   }

   switch(resolutionType){

      case SPEED_RESOLUTION:{
         if(fabs(preferredSpeed - prevResSpeed) >= 0.1){
            SetGuidanceSpeedCmd(activePlan->getID(),preferredSpeed);
            prevResSpeed = preferredSpeed;
         }
         returnSafe = ComputeTargetFeasibility(GetPlan("Plan0")->getPos(nextFeasibleWpId));
         break;
      }

      case ALTITUDE_RESOLUTION:{
         double climb_rate = 0.0;
         double diff;

         double current_alt = position.alt();
         double alt_pref = preferredAlt;
         bool newTargetAlt = fabs(alt_pref - prevResAlt) > 1e-3;
         bool prevTargetReached = fabs(current_alt - prevResAlt) < 2;
         /* 
          * Implement an altitude resolution with the following criteria:
          * Check if the previous target altitude has already been reached 
          * before implementing a new updated resolution.
          */
         if(  (newTargetAlt && prevTargetReached) || newAltConflict ){
            newAltConflict = false;
            SetGuidanceAltCmd(activePlan->getID(),alt_pref,1);
            prevResAlt = alt_pref;
         }

         returnSafe = ComputeTargetFeasibility(GetPlan("Plan0")->getPos(nextFeasibleWpId));

         break;
      }

      case TRACK_RESOLUTION:
      case TRACK_RESOLUTION2:{
         //printf("executing traffic resolution\n");
         double speed = resolutionStartSpeed;
         double climb_rate = 0;
         SetGuidanceVelCmd(preferredTrack,speed,climb_rate);
         prevResTrack = preferredTrack;

         
         if(parameters.resolutionType == TRACK_RESOLUTION){
            returnSafe  = ComputeTargetFeasibility(GetPlan("Plan0")->getPos(nextFeasibleWpId));
         }else{
            larcfm::Position clstPoint = GetNearestPositionOnPlan(GetPrevWP(),GetNextWP(),position);
            returnSafe  = ComputeTargetFeasibility(clstPoint);
            closestPointFeasible = returnSafe;
         }
         break;
      }

      case VERTICALSPEED_RESOLUTION:{
         double speed = resolutionStartSpeed;
         double res_up = resVUp;
         double res_down = resVDown;
         // If there is a valid up resolution, execute up resolution.
         // else execute the down resolution. If 0 vertical speed is possible,
         // that is preferred over the up or down resolutions.
         if(!std::isinf(res_up) && !std::isnan(res_up)){
            if(res_up >= 1e-3){
                  SetGuidanceVelCmd(hdg,speed,-res_up);
                  prevResVspeed = res_up;
            }else{
               SetGuidanceVelCmd(hdg,speed,0.0);
               prevResVspeed = 0.0;
            }
         }else if(!std::isinf(res_down) && !std::isnan(res_down)){
            if(res_down <= -1e-3){
                  SetGuidanceVelCmd(hdg,speed,-res_down);
                  prevResVspeed = res_down;
            }else{
               SetGuidanceVelCmd(hdg,speed,0.0);
               prevResVspeed = 0.0;
            }
         }else{
            SetGuidanceVelCmd(hdg,speed,prevResVspeed);
         }

         uint8_t val;
         if(vsBandsNum > 0){
            val = 0;
         }else{
            val = 1;
         }

         //printf("Return safe: %d\n\n",val);
         returnSafe = (bool) val;
         break;
      }

      case TRACK_SPEED_VS_RESOLUTION:{
         double outTrack = preferredTrack; 
         double outSpeed = preferredSpeed; 
         double outVS = preferredVSpeed;
         if(recovery[TRACK_RESOLUTION]){
            outTrack =std::fmod(360 + velocity.trk()*180/M_PI,360);
         }

         if(recovery[SPEED_RESOLUTION]){
            outSpeed = velocity.gs();
         }

         if(recovery[VERTICALSPEED_RESOLUTION]){
             outVS = velocity.vs();
         }

         SetGuidanceVelCmd(outTrack,outSpeed,outVS);
         returnSafe = ComputeTargetFeasibility(GetPlan("Plan0")->getPos(nextFeasibleWpId));
      }

      default:{

         break;
      }
   }

   trafficConflict |= (!returnSafe);
   return trafficConflict;
}

bool Cognition::ReturnToNextWP(){

   //Set this as the current mission wp
   switch(return2NextWPState){

      case NOOPC:{

         if (trafficConflict){
            return false;
         }

         if(requestGuidance2NextWP == 1){
            return2NextWPState = INITIALIZE;
            break;
         }
         return false;
      }

      case INITIALIZE:{

         std::string returningPlan("Plan0");
         // Currently only consider return to nominal mission plan
         //returningPlan = activePlan->getID();
         nextWpId[returningPlan] = nextFeasibleWpId;
         return2NextWPState = COMPUTE;
         request = REQUEST_NIL;
         log << timeString + "| [CONFLICT] | Returning to "<<returningPlan<<" wp:"<<nextWpId[returningPlan]<<"\n";
         break;
      }

      case COMPUTE:
      {

         if (request == REQUEST_NIL)
         {

            numSecPaths++;
            std::string pathName = "Plan" + std::to_string(numSecPaths);
            larcfm::Position positionB;
            nextWpId[activePlan->getID()] = nextFeasibleWpId;
            positionB = GetNextWP();


            larcfm::Position positionA = position;
            larcfm::Velocity velocityA = velocity;
            larcfm::Velocity velocityB = GetNextWPVelocity();
            FindNewPath(pathName,positionA, velocityA, positionB, velocityB);
            request = REQUEST_PROCESSING;
            SendStatus((char*)"IC:Computing secondary path",6);

         }
         else if (request == REQUEST_RESPONDED)
         {
            request = REQUEST_NIL;
            return2NextWPState = RESOLVE;
            std::string pathName = "Plan" + std::to_string(numSecPaths);
            SetGuidanceFlightPlan((char*)pathName.c_str(),1);
         }else{

            std::string pathName = "Plan" + std::to_string(numSecPaths);
            larcfm::Plan *fp = GetPlan(pathName);
            if(fp != nullptr){
                request = REQUEST_RESPONDED;
            } 


         }
         break;
      }

      case RESOLVE:{
         // Execute the secondary path

         std::string plan_id = activePlan->getID();
         if(CheckPlanComplete(plan_id)){
            return2NextWPState = COMPLETE;
         }else{
            int next_secondary_wp_id = nextWpId[plan_id];
            int num_secondary_wps = GetTotalWaypoints(plan_id);
            /*
            //if(next_secondary_wp_id > (ceil((float)(num_secondary_wps-1))/2)){
               if (trafficConflict){
                  log << timeString + "| [STATUS] | Incomplete termination of return to path: " <<next_secondary_wp_id<<"/"<<num_secondary_wps<<"\n";
                  return2NextWPState = NOOPC;
                  requestGuidance2NextWP = 1;
                  return false;
               }
            //}*/
         }
         break;
      }

      case COMPLETE:{
  
         return2NextWPState = NOOPC;
         requestGuidance2NextWP = -1;
         log << timeString + "| [RESOLVED] | Resuming Plan0"<<"\n";
         if(nextWpId["Plan0"] < GetTotalWaypoints("Plan0")){
            SetGuidanceFlightPlan((char*)"Plan0",nextWpId["Plan0"]);
         }
         SendStatus((char*)"IC:Resuming mission",6);
         break;
      }
   }
   return true;
}

void Cognition::FindNewPath(const std::string &PlanID,
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
    Command cmd = {.commandType=Command::FP_REQUEST};
    cmd.fpRequest = fp_request;
    cognitionCommands.push_back(cmd);

    log << timeString + "| [PATH_REQUEST] | Computing secondary path: " <<PlanID<<"\n";
}

resolutionType_e Cognition::GetResolutionType(){
   int resType = parameters.resolutionType;
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
      if(!allTrafficConflicts[resPriority[i]]){
          continue;
      }
      if(!recovery[resPriority[i]]){
          if(resPriority[i] == TRACK_RESOLUTION){
            if(!(trkBandNum == 1 && trkBandMin[0] < 1e-3 && trkBandMax[0] > 359.999)){
               return (resolutionType_e) resPriority[i];
            }
          }else{
            return (resolutionType_e) resPriority[i];
          }
      }
   }

   // If all the dimensions are in recovery, 
   // return the first resolution in the priority.
   return (resolutionType_e) resPriority[0];
}

void Cognition::SetGuidanceVelCmd(const double track,const double gs,const double vs){
    VelocityCommand velocity_command = {
        .vn = gs*cos(track* M_PI/180),
        .ve = gs*sin(track* M_PI/180),
        .vu = vs
    };
    Command cmd = {.commandType = Command::VELOCITY_COMMAND};
    cmd.velocityCommand = velocity_command;
    cognitionCommands.push_back(cmd);
}

void Cognition::SetGuidanceSpeedCmd(const std::string &planID,const double speed,const int hold){
    SpeedChange speed_change = {"",speed, hold };
    strcpy(speed_change.name,planID.c_str());
    Command cmd = {.commandType = Command::SPEED_CHANGE_COMMAND};
    cmd.speedChange = speed_change;
    cognitionCommands.push_back(cmd);
}

void Cognition::SetGuidanceAltCmd(const std::string &planID,const double alt,const int hold){
    AltChange alt_change = {"",alt, hold };
    strcpy(alt_change.name,planID.c_str());
    Command cmd = {.commandType = Command::ALT_CHANGE_COMMAND};
    cmd.altChange = alt_change;
    cognitionCommands.push_back(cmd);
}

void Cognition::SetGuidanceFlightPlan(const std::string &plan_id,const int wp_index){
    activePlan = GetPlan(plan_id);
    nextWpId[plan_id] = wp_index;

    FpChange fp_change;
    std::memset(fp_change.name,0,25);
    strcpy(fp_change.name,plan_id.c_str());
    fp_change.wpIndex = wp_index;
    fp_change.nextFeasibleWp = nextFeasibleWpId;
    Command cmd = {.commandType = Command::FP_CHANGE};
    cmd.fpChange = fp_change;
    cognitionCommands.push_back(cmd);
    log << timeString + "| [MODE] | Guidance Flightplan change, Plan: "<<plan_id<<", wp:"<<wp_index<<"\n";
}

void Cognition::SetGuidanceP2P(const larcfm::Position &point,const double speed){
    P2PCommand p2p_command = {
        .point = {point.latitude(), point.longitude(), point.alt()},
        .speed = speed
    };
    Command cmd = {.commandType = Command::P2P_COMMAND};
    cmd.p2PCommand = p2p_command;
    cognitionCommands.push_back(cmd);
}

void Cognition::SendStatus(const char buffer[],const uint8_t severity){
    StatusMessage status_message;
    std::memset(status_message.buffer,0,250);
    strcpy(status_message.buffer,buffer);
    status_message.severity = severity;

    Command cmd = {.commandType = Command::STATUS_MESSAGE};
    cmd.statusMessage = status_message;
    cognitionCommands.push_back(cmd);
}