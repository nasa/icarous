#include "Cognition.hpp"
#include "Cognition.h"
#include "UtilFunctions.h"

void* CognitionInit(const char callsign[],const char config[]){
    Cognition* cog = new Cognition(std::string(callsign),std::string(config));
    return (void*)cog;
}

void ReadParamFromFile(void* obj,char config[]){
    Cognition* cog = (Cognition*)obj;
    cog->ReadParamsFromFile(std::string(config));
}

void Reset(void *obj){
    Cognition* cog = (Cognition*)obj;
    cog->Reset();
}

void InputVehicleState(void *obj,const double pos[3],const double vel[3],const double heading){
    Cognition* cog = (Cognition*)obj;
    larcfm::Position position = larcfm::Position::makeLatLonAlt(pos[0],"deg",pos[1],"deg",pos[2],"m");
    larcfm::Velocity velocity = larcfm::Velocity::makeTrkGsVs(vel[0],"degree",vel[1],"m/s",vel[2],"m/s");
    cog->InputVehicleState(position,velocity,heading);
}

void InputParmetersFromFile(void* obj,const char config[]){
    Cognition* cog = (Cognition*)obj;
    cog->ReadParamsFromFile(std::string(config));
}

void InputFlightPlanData(void* obj,char planid[],waypoint_t wpts[],int totalwp,
                         double initHeading,bool kinematize,double repairTurnRate){
    Cognition* cog = (Cognition*)obj;
    std::list<waypoint_t> waypoints(wpts,wpts+totalwp);
    cog->InputFlightPlanData(planid,waypoints,initHeading,kinematize,repairTurnRate);
}

void InputParameters(void *obj,const cognition_params_t *new_params){
    Cognition* cog = (Cognition*)obj;
    cog->InputParameters(*new_params);
}

void InputTrafficAlert(void* obj,const char callsign[20],int alert){
    Cognition* cog = (Cognition*)obj;
    cog->InputTrafficAlert(std::string(callsign),alert);
}


void InputDitchStatus(void *obj,const double ditch_pos[3],const double todAltitude,const bool ditch_requested){
    Cognition* cog = (Cognition*)obj;
    larcfm::Position ditch_location = larcfm::Position::makeLatLonAlt(ditch_pos[0],"deg",
                                                                      ditch_pos[1],"deg",
                                                                      ditch_pos[2],"m");
    cog->InputDitchStatus(ditch_location,todAltitude, ditch_requested);
}

void InputMergeStatus(void *obj,const int merge_status){
    Cognition* cog = (Cognition*)obj;
    cog->InputMergeStatus(merge_status);
}

void InputTrackBands(void *obj,const bands_t *track_bands){
    Cognition* cog = (Cognition*)obj;
    cog->InputTrackBands(*track_bands);
}

void InputSpeedBands(void *obj,const bands_t *speed_bands){
    Cognition* cog = (Cognition*)obj;
    cog->InputSpeedBands(*speed_bands);
}

void InputAltBands(void *obj,const bands_t *alt_bands){
    Cognition* cog = (Cognition*)obj;
    cog->InputAltBands(*alt_bands);
}

void InputVSBands(void *obj,const bands_t *vs_bands){
    Cognition* cog = (Cognition*)obj;
    cog->InputVSBands(*vs_bands);
}

void InputGeofenceConflictData(void *obj,const geofenceConflict_t *gf_conflict){
    Cognition* cog = (Cognition*)obj;
    cog->InputGeofenceConflictData(*gf_conflict);
}

void ReachedWaypoint(void *obj, const char plan_id[], const int wp_reached_id){
    Cognition* cog = (Cognition*)obj;
    std::string plan_name = std::string(plan_id);
    cog->ReachedWaypoint(plan_name,wp_reached_id);
}

int GetCognitionOutput(void *obj,Command *command){
    Cognition* cog = (Cognition*)obj;
    return cog->GetCognitionOutput(*command);
}

void StartMission(void *obj,const int mission_start_value,const double delay){
    Cognition* cog = (Cognition*)obj;
    cog->StartMission(mission_start_value,delay);
}

int RunCognition(void *obj,double time){
    Cognition* cog = (Cognition*)obj;
    cog->Run(time);
    return cog->cogState.missionStart;
}

void InputTrajectoryMonitorData(void* obj,const trajectoryMonitorData_t* tjMonData){
    Cognition* cog = (Cognition*)obj;
    return cog->InputTrajectoryMonitorData(*tjMonData);
}