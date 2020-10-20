#include "Cognition.hpp"

bool TakeoffTrigger(CognitionState_t* state){
    return state->missionStart == 0 && 
           !state->keepInConflict &&
           !state->keepOutConflict;
}

bool NominalDepartureTrigger(CognitionState_t* state){
    return state->takeoffComplete == 1 || state->missionStart > 0;
}

bool FenceConflictTrigger(CognitionState_t* state){
    return state->planProjectedFenceConflict && state->timeToFenceViolation < 10 &&
           !state->trafficConflict;
}

bool SecondaryPlanCompletionTrigger(CognitionState_t* state){
    if(state->activePlan != nullptr){
        std::string planID = state->activePlan->getID();
        return (planID != "Plan0" &&
                planID != "DitchPath" &&
                state->nextWpId[planID] >= state->activePlan->size());
    }else{
        return false;
    }
}

bool PrimaryPlanCompletionTrigger(CognitionState_t* state){
    if(state->activePlan != nullptr){
        std::string planID = state->activePlan->getID();
        return (planID == "Plan0" && state->nextWpId[planID] >= state->activePlan->size());
    } else {
        return false;
    }
}

bool FlightPlanDeviationTrigger(CognitionState_t* state){
    if(state->activePlan == nullptr){
        return false;
    }
    state->XtrackConflict = (state->xtrackDeviation > state->parameters.allowedXtrackDeviation) &&
                            !state->trafficConflict;
    return state->XtrackConflict;

}

bool TrafficConflictVectorResTrigger(CognitionState_t* state){
    bool conflict = false;
    if(state->parameters.resolutionType == SEARCH_RESOLUTION){
        return false;
    }
    conflict = conflict || state->allTrafficConflicts[SPEED_RESOLUTION];
    conflict = conflict || state->allTrafficConflicts[TRACK_RESOLUTION];
    conflict = conflict || state->allTrafficConflicts[ALTITUDE_RESOLUTION];
    conflict = conflict || state->allTrafficConflicts[VERTICALSPEED_RESOLUTION];

    conflict = conflict && state->planProjectedTrafficConflict;

    state->trafficConflict = conflict;
    return conflict;
}

bool TrafficConflictPathResTrigger(CognitionState_t* state){
    bool conflict = false;
    if(state->parameters.resolutionType != SEARCH_RESOLUTION){
        return false;
    }
    conflict = conflict || state->allTrafficConflicts[SPEED_RESOLUTION];
    conflict = conflict || state->allTrafficConflicts[TRACK_RESOLUTION];
    conflict = conflict || state->allTrafficConflicts[ALTITUDE_RESOLUTION];
    conflict = conflict || state->allTrafficConflicts[VERTICALSPEED_RESOLUTION];
    conflict = conflict && state->planProjectedTrafficConflict;

    state->trafficConflict = conflict;
    if(!conflict && state->planProjectedTrafficConflict){
       if(state->timeToTrafficViolation3 < state->parameters.lookaheadTime){ 
          state->trafficConflict = true; 
       }
    }
    return state->trafficConflict;

}

bool TrafficConflictDitchTrigger(CognitionState_t* state){
    return state->trafficConflict && state->parameters.resolutionType == DITCH_RESOLUTION;
}

bool MergingActivityTrigger(CognitionState_t* state){
    return state->mergingActive == 1;
}

bool DitchingTrigger(CognitionState_t* state){
    return state->ditch;
}

bool DitchSiteTODTrigger(CognitionState_t* state){
    if(state->activePlan != nullptr){
        std::string planID = state->activePlan->getID();
        return (planID == "DitchPath" &&
                state->nextWpId[planID] >= state->activePlan->size());
    }else{
        return false;
    }
}