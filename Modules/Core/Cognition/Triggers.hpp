#include "Cognition.hpp"

bool TakeoffTrigger(CognitionState_t* state){
    return state->missionStart == 0 && 
           !state->keepInConflict &&
           !state->keepOutConflict;
}

bool FenceConflictTrigger(CognitionState_t* state){
    return state->planProjectedFenceConflict && state->timeToFenceViolation < 10 &&
           !state->trafficConflict && state->parameters.active;
}

bool SecondaryPlanCompletionTrigger(CognitionState_t* state){
    if(state->activePlan != nullptr){
        std::string planID = state->activePlan->getID();
        return (planID != "Plan0" &&
                planID != "DitchPath" &&
                state->nextWpId[planID] >= state->activePlan->size() && state->parameters.active);
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

    if(state->activePlan->getID() == "Plan0"){
        if(state->nextWpId["Plan0"] >= state->activePlan->size()){
            return false;
        }
    }
    state->XtrackConflict = (state->xtrackDeviation > state->parameters.allowedXtrackDeviation) &&
                            !state->trafficConflict && state->parameters.active;
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

    if(state->parameters.return2NextWP < 2){
        conflict = conflict && state->planProjectedTrafficConflict;
    }

    state->trafficConflict = conflict && state->parameters.active;
    return state->trafficConflict;
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
    return state->trafficConflict && state->parameters.active;

}

bool TrafficConflictDitchTrigger(CognitionState_t* state){
    return state->trafficConflict && state->parameters.resolutionType == DITCH_RESOLUTION && state->parameters.active;
}

bool MergingActivityTrigger(CognitionState_t* state){
    return state->mergingActive == 1 && state->parameters.active;
}

bool DitchingTrigger(CognitionState_t* state){
    return state->ditch && state->parameters.active;
}

bool DitchSiteTODTrigger(CognitionState_t* state){
    if(state->activePlan != nullptr){
        std::string planID = state->activePlan->getID();
        return (planID == "DitchPath" &&
                state->nextWpId[planID] >= state->activePlan->size()) && state->parameters.active;
    }else{
        return false;
    }
}