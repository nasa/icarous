#include "Cognition.hpp"
#include "StateReader.h"
#include "ParameterData.h"
#include "WP2Plan.hpp"


Cognition::Cognition(const std::string callsign,const std::string config){
    ReadParamsFromFile(config); 
    // Open a log file
    struct timespec  tv;
    clock_gettime(CLOCK_REALTIME,&tv);
    double localT = tv.tv_sec + static_cast<float>(tv.tv_nsec)/1E9;
    cogState.callSign = callsign;
    std::string filename("log/Cognition-" + cogState.callSign + "-" + std::to_string(localT) + ".log");
    cogState.log.open(filename);

    InitializeState();
    InitializeEventHandlers();
}


void Cognition::ReadParamsFromFile(const std::string config){
    
    larcfm::StateReader reader;
    larcfm::ParameterData parameters;
    reader.open(config);
    reader.updateParameterData(parameters);
    cogState.parameters.resolutionType = parameters.getInt("daa_resolution_type");
    cogState.parameters.allowedXtrackDeviation = parameters.getValue("allowed_xtrk_deviation");
    cogState.parameters.active = !parameters.getBool("passive_mode");
    cogState.parameters.persistenceTime = parameters.getValue("persistence_time");
    cogState.parameters.return2NextWP =parameters.getBool("return_nextwp");
    cogState.parameters.returnVector = parameters.getBool("return_vector");
    cogState.parameters.verifyPlanConflict = parameters.getValue("verify_conflict_with_plan");
    cogState.parameters.planLookaheadTime = parameters.getValue("plan_lookahead");

}

void Cognition::InitializeState(){
    cogState.nextWpId.clear();
    cogState.returnSafe = true;
    cogState.request = REQUEST_NIL;
    cogState.missionStart = -1;
    cogState.keepInConflict = false;
    cogState.keepOutConflict = false;
    cogState.p2pComplete = false;
    cogState.takeoffComplete = -1;
    cogState.trafficConflictState = NOOPC;
    cogState.geofenceConflictState = NOOPC;
    cogState.allTrafficConflicts[0] = false;
    cogState.allTrafficConflicts[1] = false;
    cogState.allTrafficConflicts[2] = false;
    cogState.allTrafficConflicts[3] = false;
    cogState.validResolution[0] = false;
    cogState.validResolution[1] = false;
    cogState.validResolution[2] = false;
    cogState.validResolution[3] = false;
    cogState.planProjectedFenceConflict = false;
    cogState.planProjectedTrafficConflict = false;
    cogState.XtrackConflictState = NOOPC;
    cogState.return2NextWPState = NOOPC;
    cogState.requestGuidance2NextWP = -1;
    cogState.topOfDescent = false;
    cogState.todAltitude = -1;
    cogState.ditch = false;
    cogState.endDitch = false;
    cogState.resetDitch = false;
    cogState.primaryFPReceived = false;
    cogState.mergingActive = 0;
    cogState.wpMetricTime = false;
    cogState.emergencyDescentState = SUCCESS;
    cogState.ditchSite = larcfm::Position::makeLatLonAlt(0,"deg",0,"deg",0,"m");
    cogState.activePlan = nullptr;
    cogState.numSecPaths = 0;
    cogState.scenarioTime = 0;
    cogState.xtrackDeviation = 0;
    cogState.missionPlan = "Plan0";
    cogState.icReady = false;
}

void Cognition::Reset(){
    InitializeState();
}

void Cognition::Run(double time){
    char buffer[25];
    std::sprintf(buffer,"%10.5f",time);
    cogState.timeString = std::string(buffer);
    cogState.utcTime = time;
    if(cogState.utcTime >= cogState.scenarioTime){
        eventMng.Run(&cogState);
    }
    cogState.log.flush();
}

void Cognition::InputVehicleState(const larcfm::Position &pos,const larcfm::Velocity &vel,const double heading){
    cogState.position = pos;
    cogState.velocity = vel;
    cogState.hdg = heading;
    cogState.speed = vel.gs();

    //Update nearest point with respect to active flightplan
    if(!cogState.parameters.return2NextWP){
        larcfm::Plan *fp = cogState.activePlan;
        if (fp != nullptr && fp->getID() == "Plan0") {
            int nextWP = cogState.nextWpId[fp->getID()];
            cogState.clstPoint = GetNearestPositionOnPlan(fp, cogState.position, nextWP);
        }
    }
}


void Cognition::InputFlightPlanData(const std::string &plan_id,const std::list<waypoint_t> &waypoints,
                                    const double initHeading,bool repair,double repairTurnRate){

    larcfm::Plan* fp = GetPlan(&cogState.flightPlans,plan_id);
    larcfm::Plan newPlan(plan_id); 
    if (fp != NULL){
        fp->clear();
        ConvertWPList2Plan(fp,plan_id,waypoints,initHeading,repair,repairTurnRate);
    }else{
        fp = &newPlan;
        ConvertWPList2Plan(fp,plan_id,waypoints,initHeading,repair,repairTurnRate);
        cogState.flightPlans.push_back(newPlan);
    }

    cogState.nextWpId[plan_id] = 1;
    if(plan_id == "Plan0"){
        cogState.primaryFPReceived = true;
        cogState.scenarioTime = fp->time(0);
    }

    if(cogState.activePlan != nullptr){
        if (cogState.activePlan->getID() == "Plan0" && plan_id == "Plan0") {
            cogState.nextWpId["Plan0"] = 1;
            SetGuidanceFlightPlan(&cogState, "Plan0", 1);
        }
    }
    
    cogState.resolutionStartSpeed = fp->gsIn(1);
}

void Cognition::InputTrajectoryMonitorData(const trajectoryMonitorData_t & tjMonData){
    cogState.timeToFenceViolation = tjMonData.timeToFenceViolation;
    cogState.timeToTrafficViolation3 = tjMonData.timeToTrafficViolation;
    cogState.planProjectedFenceConflict = tjMonData.fenceConflict;
    cogState.planProjectedTrafficConflict = tjMonData.trafficConflict;
    cogState.nextFeasibleWpId = tjMonData.nextFeasibleWP;
    cogState.xtrackDeviation = tjMonData.offsets1[0];
    if(tjMonData.offsets1[0] > 10){
        cogState.planProjectedTrafficConflict = false;
        cogState.planProjectedFenceConflict = false;
    }
}



void Cognition::ReachedWaypoint(const std::string &plan_id, const int reached_wp_id){

    cogState.log<<cogState.timeString + "| [WPREACHED] | Plan: " << plan_id<<", wp: "<<reached_wp_id<<"\n";
    if(plan_id == "P2P" && reached_wp_id == 1){
        // Note P2P mode uses flightplan with 2 waypoints. 
        // 0th point is the initial point and 1st point is the target point
        // hence the check of reach_wp_id == 1 in the above if.
        cogState.p2pComplete = true;
    }else if(plan_id == "Takeoff"){
        cogState.takeoffComplete = 1;
    }else if(plan_id == "PlanM"){
        cogState.nextWpId["PlanM"] = reached_wp_id + 1;
        if(cogState.nextWpId["PlanM"] >= GetTotalWaypoints(&cogState.flightPlans,"PlanM")){
            cogState.mergingActive = 2;
        }

    }else{
        int next_wp_id = reached_wp_id + 1;
        larcfm::Plan* fp = GetPlan(&cogState.flightPlans,plan_id);
        if(fp->getInfo(next_wp_id-1) == "<BOD>"){
            next_wp_id = fp->size()+1;
        }
        if (fp != nullptr) {
            cogState.activePlan  = fp;
            int total_waypoints = GetTotalWaypoints(&cogState.flightPlans,plan_id);
            cogState.nextWpId[plan_id] = next_wp_id;
            cogState.resolutionStartSpeed = cogState.activePlan->gsIn(next_wp_id);
            if (next_wp_id < total_waypoints) {
                cogState.refWpTime = cogState.activePlan->time(next_wp_id);
            }
        }
    }

}

void Cognition::InputParameters(const cognition_params_t &new_params){
    cogState.parameters = new_params;
}

void Cognition::InputDitchStatus(const larcfm::Position &ditch_site,const double todAlt,const bool ditch_requested){
    cogState.ditchSite = ditch_site;
    cogState.todAltitude = todAlt;
    cogState.ditch = ditch_requested;
}

void Cognition::InputMergeStatus(const int merge_status){
    cogState.mergingActive = merge_status;
}

void Cognition::InputTrackBands(const bands_t &track_bands){
    cogState.utcTime = track_bands.time;
    cogState.recovery[TRACK_RESOLUTION] = track_bands.recovery >= 0;
    if (track_bands.currentConflictBand == 1) {
        cogState.allTrafficConflicts[TRACK_RESOLUTION] = true;
        if (!std::isnan(track_bands.resPreferred) && !std::isinf(track_bands.resPreferred)){
            cogState.preferredTrack = track_bands.resPreferred;
            cogState.validResolution[TRACK_RESOLUTION] = true;
        }
        else{
            cogState.preferredTrack = cogState.prevResTrack;
            cogState.validResolution[TRACK_RESOLUTION] = false;
        }
    }
    else
    {
        cogState.allTrafficConflicts[TRACK_RESOLUTION] = false;
        cogState.preferredTrack = cogState.prevResTrack;
    }

    std::memcpy(cogState.trkBandType,track_bands.type,sizeof(int)*20);
    std::memcpy(cogState.trkBandMin,track_bands.min,sizeof(double)*20);
    std::memcpy(cogState.trkBandMax,track_bands.max,sizeof(double)*20);
    cogState.trkBandNum = track_bands.numBands;
}

void Cognition::InputSpeedBands(const bands_t &speed_bands){

    cogState.recovery[SPEED_RESOLUTION] = speed_bands.recovery >= 0; 
    if(speed_bands.currentConflictBand == 1){
        // Factor to increment or decrement the provided speed resolution by
        // This will eliminate oscillatory behavior due to numerical errors
        cogState.allTrafficConflicts[SPEED_RESOLUTION] = true;
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
            cogState.preferredSpeed = speed_bands.resPreferred * fac;
            cogState.validResolution[SPEED_RESOLUTION] = true;
        }else{
            cogState.validResolution[SPEED_RESOLUTION] = false;
        }
    }else{
        cogState.allTrafficConflicts[SPEED_RESOLUTION] = false;
        cogState.preferredSpeed = cogState.prevResSpeed;
    }
    std::memcpy(cogState.gsBandType,speed_bands.type,sizeof(int)*20);
    std::memcpy(cogState.gsBandMin,speed_bands.min,sizeof(double)*20);
    std::memcpy(cogState.gsBandMax,speed_bands.max,sizeof(double)*20);
    cogState.gsBandNum = speed_bands.numBands;
}

void Cognition::InputAltBands(const bands_t &alt_bands){
    cogState.recovery[ALTITUDE_RESOLUTION] = alt_bands.recovery >= 0;
    if(alt_bands.currentConflictBand == 1){
        cogState.allTrafficConflicts[ALTITUDE_RESOLUTION] = true;
        if(!std::isinf(alt_bands.resPreferred) && !std::isnan(alt_bands.resPreferred)){
            cogState.preferredAlt = alt_bands.resPreferred;
            cogState.validResolution[ALTITUDE_RESOLUTION] = true;
        }
        else{
            cogState.preferredAlt = cogState.prevResAlt;
            cogState.validResolution[ALTITUDE_RESOLUTION] = false;
        }
    }else{
        cogState.allTrafficConflicts[ALTITUDE_RESOLUTION] = false;
        cogState.preferredAlt = cogState.prevResAlt;
    }
    cogState.timeToTrafficViolation1 = alt_bands.timeToViolation[0];
    cogState.timeToTrafficViolation2 = alt_bands.timeToViolation[1];
    std::memcpy(cogState.altBandType,alt_bands.type,sizeof(int)*20);
    std::memcpy(cogState.altBandMin,alt_bands.min,sizeof(double)*20);
    std::memcpy(cogState.altBandMax,alt_bands.max,sizeof(double)*20);
    cogState.altBandNum = alt_bands.numBands;
}

void Cognition::InputVSBands(const bands_t &vs_bands){
    cogState.recovery[VERTICALSPEED_RESOLUTION] = vs_bands.recovery >= 0;
    cogState.resVUp = vs_bands.resUp;
    cogState.resVDown = vs_bands.resDown;
    cogState.preferredVSpeed = vs_bands.resPreferred;
    cogState.vsBandsNum = vs_bands.numBands;

    if(!std::isinf(cogState.preferredVSpeed) && !std::isnan(cogState.preferredVSpeed)){
       cogState.prevResVspeed = cogState.preferredVSpeed;
       cogState.validResolution[VERTICALSPEED_RESOLUTION] = true;
    }else{
       cogState.preferredVSpeed = cogState.prevResVspeed;
       cogState.validResolution[VERTICALSPEED_RESOLUTION] = false;
    }
}

void Cognition::InputGeofenceConflictData(const geofenceConflict_t &gf_conflict){

    if(gf_conflict.numConflicts > 0){
        if (gf_conflict.conflictTypes[0] == 0)
            cogState.keepInConflict = true;
        else if (gf_conflict.conflictTypes[0] == 1)
            cogState.keepOutConflict = true;

        cogState.recoveryPosition = larcfm::Position::makeLatLonAlt(gf_conflict.recoveryPosition[0],"deg",
                                                       gf_conflict.recoveryPosition[1],"deg",
                                                       gf_conflict.recoveryPosition[2],"m");
    }else{
        cogState.keepOutConflict = false;
        cogState.keepInConflict = false;
    }
}

void Cognition::StartMission(const int mission_start_value,const double delay){
    cogState.missionStart = mission_start_value;
    cogState.scenarioTime += delay;
    cogState.activePlan = GetPlan(&cogState.flightPlans,"Plan0");
    if (cogState.activePlan != nullptr)
    {
        if (delay > 0)
        {
            cogState.activePlan->timeShiftPlan(0, delay);
        }

        
    }
}

int Cognition::GetCognitionOutput(Command &command){
    size_t commands_remaining = cogState.cognitionCommands.size();
    if (commands_remaining > 0){
        command = cogState.cognitionCommands.front();
        cogState.cognitionCommands.pop_front();
    }
    return commands_remaining;
}

std::map<std::string,int> GetPriorityValues(){
    std::ifstream priorityFile;
    priorityFile.open("Priority.txt");
    std::map<std::string,int> inputPriorities;
    // Set default priority values 
    inputPriorities["Takeoff"] = 1;
    inputPriorities["NominalDeparture"] = 6;
    inputPriorities["PrimaryPlanComplete"] = 1;
    inputPriorities["SecondaryPlanComplete"] = 1;
    inputPriorities["Merging"] = 3;
    inputPriorities["FenceConflict"] = 1;
    inputPriorities["TrafficConflict1"] = 2;
    inputPriorities["TrafficConflict2"] = 2;
    inputPriorities["FlightPlanDeviation"] = 1;
    inputPriorities["TrafficConflict3"] = 4;
    inputPriorities["Ditching"] = 5;
    inputPriorities["TODReached"] = 1;

    if (priorityFile.is_open()) {
        std::string name;
        int val;
        while (priorityFile >> name) {
            priorityFile >> val;
            inputPriorities[name] = (int)val;
        }
    }

    return inputPriorities;
}

void Cognition::InputTrafficAlert(std::string callsign, int alert){
    if(alert > 0){
        cogState.conflictTraffics.insert(callsign);
    }
}