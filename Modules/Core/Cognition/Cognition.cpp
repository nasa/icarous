#include "Cognition.hpp"

Cognition::Cognition(const std::string callsign){
    Initialize();
    parameters.resolutionType = TRACK_RESOLUTION;
    parameters.DTHR = 30;
    parameters.ZTHR = 1000;
    parameters.allowedXtrackDeviation = 1000;
    parameters.XtrackGain = 0.6;
    parameters.resolutionSpeed = 1;
    parameters.searchType = 1;
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
    trafficTrackConflict = false;
    trafficSpeedConflict = false;
    trafficAltConflict = false;
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

void Cognition::InputFlightPlanData(const std::string &plan_id,
                                    const double scenario_time,
                                    const int wp_id,
                                    const larcfm::Position &wp_position,
                                    const int wp_metric,
                                    const double wp_value){
    larcfm::Plan* fp = GetPlan(plan_id);

    // Initialize the feasibility of this waypoint to true
    if(fp != NULL) {
        // Existing flight plan

        double wp_time;
        if(wp_id == 0){
            fp->clear();
            wp_time = scenario_time;
        }else if(wp_metric == 1){
            // wp metric is ETA
            wp_time = wp_value;
            wpMetricEta[plan_id].push_back(true);
            wpFeasibility[plan_id].push_back(true);
        }else if(wp_metric == 2){
            // wp metric is speed
            wpMetricEta[plan_id].push_back(false);
            wpFeasibility[plan_id].push_back(true);
            double speed = wp_value;
            int prev_wp_id = wp_id - 1;
            if(prev_wp_id < 0){
                prev_wp_id = 0;
            }
            larcfm::Position prev_position = fp->getPos(prev_wp_id);
            double start_time = fp->time(prev_wp_id);
            double dist = prev_position.distanceH(wp_position);
            wp_time = start_time + dist/speed;
        }
        fp->add(wp_position,wp_time);
        return;
    }else{
        // New flight plan
        larcfm::Plan newPlan(plan_id);
        newPlan.add(wp_position,scenario_time);
        flightPlans.push_back(newPlan);
        std::vector<bool> init(1,true);
        wpFeasibility[plan_id] = std::move(init);
        nextWpId[plan_id] = 1;
        if(plan_id == "Plan0"){
            primaryFPReceived = true;
            scenarioTime = scenario_time;
        }
        return;
    }

    wpMetricEta[plan_id] = {(wp_metric == 1)};

    
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

bool Cognition::NextWpFeasible(){
    int next_wp_id = nextWpId[activePlan->getID()];
    bool next_wp_feas = wpFeasibility[activePlan->getID()].at(next_wp_id);
    return next_wp_feas;
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
        activePlan = GetPlan(plan_id);
        if (activePlan != nullptr) {
            int total_waypoints = GetTotalWaypoints(plan_id);
            nextWpId[plan_id] = next_wp_id;
            if (next_wp_id < total_waypoints) {
                wpMetricTime = wpMetricEta[plan_id][next_wp_id];
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
    if (track_bands.currentConflictBand == 1) {
        trafficTrackConflict = true;
        if (!std::isnan(track_bands.resPreferred) && !std::isinf(track_bands.resPreferred))
            preferredTrack = track_bands.resPreferred;
        else
            preferredTrack = prevResTrack;
    }
    else
    {
        trafficTrackConflict = false;
        preferredTrack = prevResTrack;
    }

    std::memcpy(trkBandType,track_bands.type,sizeof(int)*20);
    std::memcpy(trkBandMin,track_bands.min,sizeof(double)*20);
    std::memcpy(trkBandMax,track_bands.max,sizeof(double)*20);
    trkBandNum = track_bands.numBands;
    trackRecovery = (bool) track_bands.recovery;
    if (activePlan != nullptr)
    {
        std::string plan_id = activePlan->getID();
        bool wpF[50];
        if (plan_id == "Plan0") {
            std::memcpy(wpF, track_bands.wpFeasibility1, sizeof(bool) * 50);
            closestPointFeasible = track_bands.fp1ClosestPointFeasible;
        } else {
            std::memcpy(wpF, track_bands.wpFeasibility2, sizeof(bool) * 50);
        }
        std::vector<bool> v(wpF, wpF + 50);
        wpFeasibility[plan_id] = std::move(v);
    }
}

void Cognition::InputSpeedBands(const bands_t &speed_bands){

    gsRecovery = (bool)speed_bands.recovery; 
    if(speed_bands.currentConflictBand == 1){
        // Factor to increment or decrement the provided speed resolution by
        // This will eliminate oscillatory behavior due to numerical errors
        trafficSpeedConflict = true;
        double fac;
        if (fabs(speed_bands.resPreferred - speed_bands.resDown) < 1e-3) {
            // Preferred resolution is to slow down
            fac = 0.8;
        } else {
            // Preferred resolution is to speed up
            fac = 1.2;
        }

        // Use the provided resolution if it is valid
        if (!std::isinf(speed_bands.resPreferred) && !std::isnan(speed_bands.resPreferred)) {
            preferredSpeed = speed_bands.resPreferred * fac;
            prevResSpeed = preferredSpeed;
        }
    }else{
        // If there is no current speed conflict,
        // check to make sure it is feasible to turn to the next waypoint
        if (activePlan != nullptr) {
            int next_wp_id = nextWpId[activePlan->getID()];
            int num_waypoints = GetTotalWaypoints(activePlan->getID());
            if (next_wp_id >= num_waypoints)
                next_wp_id = num_waypoints - 1;
            bool feasibility;
            if (activePlan->getID() == "Plan0"){
                feasibility = speed_bands.wpFeasibility1[next_wp_id];
            }else{
                feasibility = speed_bands.wpFeasibility2[next_wp_id];
            }
                
            if (feasibility){
                trafficSpeedConflict = false;
            }           
        }
    }
}

void Cognition::InputAltBands(const bands_t &alt_bands){
    altRecovery = (bool) alt_bands.recovery;
    if(alt_bands.currentConflictBand == 1){
        trafficAltConflict = true;
        if(!std::isinf(alt_bands.resPreferred) && !std::isnan(alt_bands.resPreferred))
            preferredAlt = alt_bands.resPreferred;
        else
            preferredAlt = prevResAlt;
    }else{
        trafficAltConflict = false;
        preferredAlt = prevResAlt;
    }
    //TODO: Use preferred resolution only if preferred resolution is not
    //close to the ground.
}

void Cognition::InputVSBands(const bands_t &vs_bands){
    vsRecovery = (bool) vs_bands.recovery;
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

    
   
    std::string primary_plan_id = "Plan0";
    if (nextWpId.count(primary_plan_id) > 0)
    {
        larcfm::Plan* fp = GetPlan("Plan0");
        int next_wp_id = nextWpId[primary_plan_id];
        int num_waypoints = GetTotalWaypoints(primary_plan_id);
        for (int i = next_wp_id; i < num_waypoints; ++i)
        {
            if (!gf_conflict.waypointConflict1[i])
            {
                // Check to see that the next waypoint is not too close when
                // dealing with a traffic conflict
                if (parameters.resolutionType == 4 && trafficTrackConflict)
                {
                    double dist = position.distanceH(fp->getPos(i));
                    // Consider a waypoint feasible if its greater than the 3*DTHR values.
                    // Note DTHR is in ft. Convert from ft to m before comparing with dist.
                    if (dist > 3 * (parameters.DTHR / 3))
                    {
                        nextFeasibleWpId[primary_plan_id] = i;
                        break;
                    }
                }
                else
                {
                    nextFeasibleWpId[primary_plan_id] = i;
                    break;
                }
            }
        }
    }
    if(primaryFPReceived){
        int next_feasible_wp_id = nextFeasibleWpId[primary_plan_id];
        bool direct_path_geofence = gf_conflict.directPathToWaypoint1[next_feasible_wp_id];
        bool direct_path_traffic = wpFeasibility[primary_plan_id][next_feasible_wp_id];
        directPathToFeasibleWP1 = direct_path_geofence && direct_path_traffic;
    }else{
        directPathToFeasibleWP1 = false;
    }

    if (activePlan != nullptr)
    {
        std::string plan_id = activePlan->getID();
        // If secondary flight plan is active
        if (plan_id != "Plan0")
        {
            int num_waypoints_secondary = GetTotalWaypoints(plan_id);
            int secondary_wp_id = nextWpId[plan_id];
            for (int i = secondary_wp_id; i < num_waypoints_secondary; ++i)
            {
                if (!gf_conflict.waypointConflict2[i])
                {
                    nextFeasibleWpId[plan_id] = i;
                    break;
                }
            }
            int next_feasible_wp_id = nextFeasibleWpId[plan_id];
            bool direct_path_geofence = gf_conflict.directPathToWaypoint2[next_feasible_wp_id];
            bool direct_path_traffic = wpFeasibility[plan_id].at(next_feasible_wp_id);
            directPathToFeasibleWP2 = direct_path_geofence && direct_path_traffic;
        }
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
                SetGuidanceFlightPlan((char*)"Plan0",nextFeasibleWpId["Plan0"]);
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
                //printf("Completing cruise\n");
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
    larcfm::Velocity velocityA = velocity;

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
            FindNewPath("DitchPath",positionA, velocityA, positionB);

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
                if(nextWpId["DitchPath"] >= GetPlan("DitchPath")->size()){
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
                trafficConflict = trafficSpeedConflict | trafficAltConflict | trafficTrackConflict;
                if(trafficConflict){
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

void Cognition::SetGuidanceFlightPlan(const std::string &plan_id,const int wp_index){
    activePlan = GetPlan(plan_id);
    nextWpId[plan_id] = wp_index;

    FpChange fp_change;
    std::memset(fp_change.name,0,25);
    strcpy(fp_change.name,plan_id.c_str());
    fp_change.wpIndex = wp_index;
    fp_change.nextFeasibleWp = nextFeasibleWpId["Plan0"];
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

bool Cognition::GeofenceConflictManagement(){
   bool geofence_conflict = (keepInConflict || keepOutConflict) && !trafficConflict;
   switch(geofenceConflictState){

      case NOOPC:{
         if(geofence_conflict){
            SendStatus((char*)"IC: Geofence conflict detected",1);

            log << timeString + "| [CONFLICT] | Geofence conflict detected" <<"\n";
            geofenceConflictState = INITIALIZE;
            requestGuidance2NextWP = -1;
            return2NextWPState = NOOPC;
           break;
         }
         return false;
      }

      case INITIALIZE:{
         //TODO: Get recovery position
         SetGuidanceP2P(recoveryPosition,parameters.resolutionSpeed);
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
         return2NextWPState = NOOPC;
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
   ManeuverToIntercept(prev_wp_pos,
                       next_wp_pos,
                       position,parameters.XtrackGain,
                       parameters.resolutionSpeed,
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
         SetGuidanceVelCmd(interceptHeading, parameters.resolutionSpeed,0.0);

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
   trafficConflict = trafficSpeedConflict | trafficAltConflict | trafficTrackConflict;
  
   switch(trafficConflictState){
      case NOOPC:{
         if(trafficConflict){
            SendStatus((char*)"IC:traffic conflict",1);
            log << timeString + "| [CONFLICT] | Traffic conflict detected" <<"\n";
            trafficConflictState = INITIALIZE;
            requestGuidance2NextWP = -1;
            break;
         }
         return false;
      }

      case INITIALIZE:{
         trafficConflictState = RESOLVE;
         prevResSpeed  = parameters.resolutionSpeed;
         prevResTrack  = hdg;
         prevResAlt    = larcfm::Units::to(larcfm::Units::m,position.alt());
         prevResVspeed = larcfm::Units::to(larcfm::Units::mps,velocity.vs());
         preferredSpeed = prevResSpeed;
         preferredTrack = prevResTrack;
         preferredAlt    = prevResAlt;
         requestGuidance2NextWP = 1;
         GetResolutionType();
         

         // Use this only for search based resolution
         if(parameters.resolutionType == SEARCH_RESOLUTION){
            return2NextWPState = INITIALIZE;
            log << timeString + "| [STATUS] | Resolving conflict with search resolution" <<"\n";
         }else if(parameters.resolutionType == SPEED_RESOLUTION){
            log << timeString + "| [STATUS] | Resolving conflict with speed resolution" <<"\n";
            log << timeString + "| [MODE] | Guidance Speed Request"<<"\n";
         }else if(parameters.resolutionType == ALTITUDE_RESOLUTION){
            log << timeString + "| [STATUS] | Resolving conflict with altitude resolution" <<"\n";
            log << timeString + "| [MODE] | Guidance Vector Request"<<"\n";
         }else if(parameters.resolutionType == TRACK_RESOLUTION ||
                   parameters.resolutionType == TRACK_RESOLUTION2){
            log << timeString + "| [STATUS] | Resolving conflict with track resolution" <<"\n";
            log << timeString + "| [MODE] | Guidance Vector Request"<<"\n";
         }else if(parameters.resolutionType == DITCH_RESOLUTION){
            // The top level statemachine should catch this
            // and transition to the emergency descent state.
            log << timeString + "| [STATUS] | Resolving conflict with ditch resolution" <<"\n";
         }
         break;
      }

      case RESOLVE:{
         if(parameters.resolutionType == SEARCH_RESOLUTION){
            // Only for search based resolution
            if(return2NextWPState != NOOPC){
               ReturnToNextWP();
            }else{
               trafficConflictState = NOOPC;
            }
         }else{

            if(parameters.resolutionType == TRACK_RESOLUTION2){
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

         if(parameters.resolutionType == SPEED_RESOLUTION){
            requestGuidance2NextWP = -1;
            trafficConflictState = NOOPC;
            SetGuidanceSpeedCmd(activePlan->getID(),parameters.resolutionSpeed);
            sendStatus = true;
         }

         if(sendStatus){
            SendStatus((char*)"IC:traffic conflict resolved",6);
            log << timeString + "| [RESOLVED] | Traffic conflict resolved" <<"\n";
         }
         return2NextWPState = NOOPC;
         break;
      }

      case COMPUTE:{
         break;
      }
   }

   return true;
}

bool Cognition::RunTrafficResolution(){

   int resolutionType = parameters.resolutionType;

   // If track, gs and vs are in recovery, don't allow combined resolution 
   if(trackRecovery && gsRecovery && vsRecovery){
       if(resolutionType == TRACK_SPEED_VS_RESOLUTION){
            resolutionType = TRACK_RESOLUTION;
       }
   }

   switch(parameters.resolutionType){

      case SPEED_RESOLUTION:{
         //printf("resolution speed = %f\n",preferredSpeed);
         if(preferredSpeed >= 0){
            SetGuidanceSpeedCmd(activePlan->getID(),preferredSpeed);
            prevResSpeed = preferredSpeed;
         }else{
            SetGuidanceSpeedCmd(activePlan->getID(),prevResSpeed);
         }

         returnSafe = NextWpFeasible();
         break;
      }

      case ALTITUDE_RESOLUTION:{
         double climb_rate = 0.0;
         double diff;

         double current_alt = position.alt();
         double alt_pref = preferredAlt;

         diff = alt_pref - current_alt;

         if(diff > 50){
             diff = 50;
         }else if(diff < -50){
             diff = -50;
         }

         climb_rate = -0.2*diff;
         double speed = parameters.resolutionSpeed;
         SetGuidanceVelCmd(hdg,speed,climb_rate);
         prevResVspeed = climb_rate;
         prevResAlt = alt_pref;

         returnSafe = NextWpFeasible();
         break;
      }

      case TRACK_RESOLUTION:
      case TRACK_RESOLUTION2:{
         //printf("executing traffic resolution\n");
         double speed = parameters.resolutionSpeed;
         double climb_rate = 0;
         SetGuidanceVelCmd(preferredTrack,speed,climb_rate);
         prevResTrack = preferredTrack;

         double intercept_heading_to_plan = GetInterceptHeadingToPlan(GetPrevWP(),GetNextWP(),position);
         returnSafe = CheckSafeToTurn(hdg,intercept_heading_to_plan);

         if(returnSafe){
            returnSafe = CheckSafeToTurn(hdg,intercept_heading_to_plan);   
         }

         break;
      }

      case VERTICALSPEED_RESOLUTION:{
         double speed = parameters.resolutionSpeed;
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
         if(trackRecovery){
            outTrack =std::fmod(360 + velocity.trk()*180/M_PI,360);
         }

         if(gsRecovery){
            outSpeed = velocity.gs();
         }

         if(vsRecovery){
             outVS = velocity.vs();
         }

         SetGuidanceVelCmd(outTrack,outSpeed,outVS);
         double intercept_heading_to_plan = GetInterceptHeadingToPlan(GetPrevWP(),GetNextWP(),position);
         returnSafe = CheckSafeToTurn(hdg,intercept_heading_to_plan);

         if(returnSafe){
            returnSafe = CheckSafeToTurn(hdg,intercept_heading_to_plan);
         }
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

         if (keepInConflict || keepOutConflict || trafficConflict){
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
         nextWpId[returningPlan] = nextFeasibleWpId[returningPlan];
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
            nextWpId[activePlan->getID()] = nextFeasibleWpId[activePlan->getID()];
            positionB = GetNextWP();


            larcfm::Position positionA = position;
            larcfm::Velocity velocityA = velocity;
            FindNewPath(pathName,positionA, velocityA, positionB);
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
            if(next_secondary_wp_id > (ceil((float)(num_secondary_wps-1))/2)){
               if (keepInConflict || keepOutConflict || trafficConflict){
                  log << timeString + "| [STATUS] | Incomplete termination of return to path: " <<next_secondary_wp_id<<"/"<<num_secondary_wps<<"\n";
                  return2NextWPState = NOOPC;
                  requestGuidance2NextWP = 1;
                  return false;
               }
            }
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

bool Cognition::CheckSafeToTurn(const double from_heading,const double to_heading) {
   bool conflict = false;
   for (int i = 0; i < trkBandNum; i++)
   {
      bool val;
      val = CheckTurnConflict(trkBandMin[i],
                              trkBandMax[i],
                              from_heading, to_heading);
      conflict |= val;
   }

   conflict |= !NextWpFeasible();

   return !conflict;
}

void Cognition::FindNewPath(const std::string &PlanID,
                            const larcfm::Position &positionA,
                            const larcfm::Velocity &velocityA,
                            const larcfm::Position &positionB){
    FpRequest fp_request = {
         parameters.searchType,
        "",
        {positionA.latitude(), positionA.longitude(), positionA.alt()},
        {positionB.latitude(), positionB.longitude(), positionB.alt()},
        {larcfm::Units::to(larcfm::Units::deg,velocityA.trk()), 
         larcfm::Units::to(larcfm::Units::mps,velocityA.gs()), 
         larcfm::Units::to(larcfm::Units::mps,velocityA.vs())}
    };
    strcpy(fp_request.name,PlanID.c_str());
    Command cmd = {.commandType=Command::FP_REQUEST};
    cmd.fpRequest = fp_request;
    cognitionCommands.push_back(cmd);

    log << timeString + "| [PATH_REQUEST] | Computing secondary path: " <<PlanID<<"\n";
}

void Cognition::GetResolutionType(){
   //
}

bool Cognition::TimeManagement(){
   if(!wpMetricTime){
      return false;
   }
   larcfm::Position currPosition = position;
   larcfm::Position nextPosition = GetNextWP();

   double dist_to_next_wp = currPosition.distanceH(nextPosition);
   double current_speed = velocity.norm();
   time_t curr_time = time(NULL);
   time_t scenario_time = scenarioTime;
   time_t next_wp_sta = scenario_time + (long)refWpTime;
   time_t next_wp_eta = curr_time + dist_to_next_wp/current_speed;
   double arr_tolerance = 3; //TODO: Make this a user defined parameter
   double max_speed = 7;     //TODO: Make parameter
   double min_speed = 0.5;   //TODO: Make parameter
   double new_speed;

   if (labs(next_wp_sta - next_wp_eta) > arr_tolerance){
      new_speed = dist_to_next_wp/(next_wp_sta - curr_time);
      if(new_speed > max_speed){
         new_speed = max_speed;
      }else{
         if(new_speed < min_speed){
            new_speed = min_speed;
         }
      }


      SetGuidanceSpeedCmd(activePlan->getID(),new_speed);
      return true;
   }
   return false;
}
