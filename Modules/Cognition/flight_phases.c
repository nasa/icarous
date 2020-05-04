
#include "cognition_core.h"

void ResetFlightPhases(cognition_t *cog) {
    cog->takeoffState = INITIALIZING;
    cog->cruiseState = INITIALIZING;
}

void InitializeCognition(cognition_t *cog){
    cog->returnSafe = true;
    cog->nextPrimaryWP = 1;
    cog->resolutionTypeCmd = -1;
    cog->request = REQUEST_NIL;
    cog->fpPhase = IDLE_PHASE;
    cog->missionStart = -1;
    cog->keepInConflict = false;
    cog->keepOutConflict = false;
    cog->p2pcomplete = false;
    cog->takeoffComplete = -1;
    cog->trafficConflictState = NOOPC;
    cog->geofenceConflictState = NOOPC;
    cog->trafficTrackConflict = false;
    cog->trafficSpeedConflict = false;
    cog->trafficAltConflict = false;
    cog->XtrackConflictState = NOOPC;
    cog->resolutionTypeCmd = TRACK_RESOLUTION;
    cog->requestGuidance2NextWP = -1;
    cog->searchType = 1;
    cog->topofdescent = false;
    cog->ditch = false;
    cog->endDitch = false;
    cog->resetDitch = false;
    cog->primaryFPReceived = false;
    cog->mergingActive = 0;
    cog->nextWPFeasibility1 = 1;
    cog->nextWPFeasibility2 = 1;
}

void FlightPhases(cognition_t *cog){

    // Handling nominal flight phases
    status_e status;
    switch(cog->fpPhase){

        case IDLE_PHASE:{
            ResetFlightPhases(cog);
            if(cog->missionStart == 0){
                // If missionStart = 0 time to start the misison
                if(cog->primaryFPReceived){
                    time_t fpTime = cog->scenarioTime;
                    time_t currTime = time(NULL);
                    if( currTime >= fpTime ){
                        cog->fpPhase = TAKEOFF_PHASE; 
                        cog->missionStart = -1;
                    }else{
                        time_t timeRemanining = (fpTime - currTime);
                        if(timeRemanining%5 == 0){
                            SendStatus(cog,"FP counting down",6);
                        }
                    }
                }else{
                    SendStatus(cog,"No flightplan loaded",4);
                    cog->missionStart = -1;
                }
            }else if(cog->missionStart > 0){
                // If missionStart > 0 goto climb state directly
                if(cog->primaryFPReceived){
                    cog->fpPhase = CRUISE_PHASE;
                }else{
                    SendStatus(cog,"No flightplan loaded",4);
                }
                cog->missionStart = -1;
            }

            if(cog->ditch){
                cog->emergencyDescentState = INITIALIZING;
                cog->fpPhase = EMERGENCY_DESCENT_PHASE;
            }
            break;
        }

        case TAXI_PHASE:{
            break;
        }

        case TAKEOFF_PHASE:{
            //printf("Takeoff phase\n");
            status = Takeoff(cog);
            if (status == SUCCESS){
                cog->fpPhase = CLIMB_PHASE;
            }else if(status == FAILED){
                cog->fpPhase = IDLE_PHASE;
            }
            break;
        }

        case CLIMB_PHASE:{
            status = Climb(cog);
            if (status == SUCCESS){
                cog->fpPhase = CRUISE_PHASE;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case CRUISE_PHASE:{
            status = Cruise(cog);
            if (status == SUCCESS){
                cog->fpPhase = DESCENT_PHASE;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }

            if( cog->mergingActive == 1){
                cog->fpPhase = MERGING_PHASE;
            }

            if(cog->ditch){
                cog->emergencyDescentState = INITIALIZING;
                cog->fpPhase = EMERGENCY_DESCENT_PHASE;
            }

            break;
        }

        case DESCENT_PHASE:{
            status = Descent(cog);
            if (status == SUCCESS){
                cog->fpPhase = APPROACH_PHASE;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case EMERGENCY_DESCENT_PHASE:{
            status = EmergencyDescent(cog);

            // Check if there is another request for ditching
            // and initialize ditching again
            if(cog->ditch){
                cog->emergencyDescentState = INITIALIZING;
            }
            
            // If end ditch is requested, kill ditching
            if(cog->endDitch){
                cog->emergencyDescentState = SUCCESS;
            }
            break;
        }

        case APPROACH_PHASE:{
            status = Approach(cog);
            if (status == SUCCESS){
                cog->fpPhase = LANDING_PHASE;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case LANDING_PHASE:{
            status = Landing(cog);
            //printf("Landing\n");
            if (status == SUCCESS){
                cog->fpPhase = IDLE_PHASE;
                cog->missionStart = -2;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case MERGING_PHASE:{
            if(cog->mergingActive == 2 || cog->mergingActive == 0){
                cog->fpPhase = CRUISE_PHASE;
                SetGuidanceFlightPlan(cog,"Plan0",cog->nextFeasibleWP1);
            }else if(cog->mergingActive == 3){
                //printf("Setting guidance flightplan\n");
                //SetGuidanceFlightPlan(cog,"PlanM",1);
                //cog->mergingActive = 1;
                //cog->nextPrimaryWP += 1;
            }
            break;
        }

    
    }


}

status_e Takeoff(cognition_t *cog){
    switch(cog->takeoffState){
       case INITIALIZING:{
           // Send takeoff command to guidance application
           if(!cog->keepInConflict && !cog->keepOutConflict){
               cog->guidanceCommand = cTAKEOFF;
               cog->sendCommand = true;
               cog->takeoffState = RUNNING;
               cog->takeoffComplete = 0;
           }else{
               cog->takeoffState = FAILED;
           }
           break;
       } 

       case RUNNING:{
           // Wait for confirmation status from guidance application

           if(cog->takeoffComplete == 1){
               cog->takeoffState = SUCCESS;
           }else if(cog->takeoffComplete == 0){
               cog->takeoffState = RUNNING;
           }else{
               cog->takeoffState = FAILED;
               printf("Takeoff failed\n");
           }
           break;
       }

       case SUCCESS: break;
       case FAILED: break;
    }

    return cog->takeoffState;
}

status_e Climb(cognition_t *cog){
    // Currently only a place holder for rotrocraft
    return SUCCESS;
}

status_e Cruise(cognition_t* cog){

   switch(cog->cruiseState){

       case INITIALIZING:{
            SetGuidanceFlightPlan(cog,"Plan0",cog->nextPrimaryWP);
            cog->cruiseState = RUNNING;
            break;
       }

       case RUNNING:{

            bool status = false;

            if(cog->mergingActive == 0){
                status |= TrafficConflictManagement(cog);

                status |= GeofenceConflictManagement(cog);

                status |= XtrackManagement(cog); 

                status |= ReturnToNextWP(cog);

                // Perform time management only when following the primary flightplan
                //if(!status){
                //    TimeManagement();
                //}
            }

            if(cog->nextPrimaryWP >= cog->num_waypoints){
                cog->cruiseState = SUCCESS;
                printf("Completing cruise\n");  
            }
            break;
       }

       case SUCCESS:break;
       case FAILED: break;
   }

   return cog->cruiseState;
}

status_e Descent(cognition_t *cog){
    // Currently only a place holder for rotorcraft
    return SUCCESS;
}

status_e Approach(cognition_t *cog){
    // Currently only a place holder for rotorcraft
    return SUCCESS;
}

status_e Landing(cognition_t *cog){
    SendStatus(cog,"IC: Landing",6);
    cog->sendCommand = true;
    cog->guidanceCommand = cLAND;
    return SUCCESS;
}

status_e EmergencyDescent(cognition_t *cog){
    static bool command_sent;
    double positionA[3] = {cog->position[0],
                           cog->position[1],
                           cog->position[2]};
   
    double positionB[3] = {cog->ditchsite[0],
                           cog->ditchsite[1],
                           positionA[2]};

    double Trk, Gs, Vs;
    ConvertVnedToTrkGsVs(cog->velocity[0], 
                         cog->velocity[1],
                         cog->velocity[2], 
                         &Trk, &Gs, &Vs);

    double velocityA[3] = {Trk,Gs,Vs};


    switch(cog->emergencyDescentState){

        case INITIALIZING:{
            SendStatus(cog,"IC:Starting to ditch",6);
            command_sent = false;
            cog->request = REQUEST_NIL;

            // Find a new path to the ditch site 
            // Note that if a direct path is not availabe, this will find
            // a possible detour. 
            FindNewPath(cog,cog->searchType, positionA, velocityA, positionB);

            cog->request = REQUEST_PROCESSING;
            cog->emergencyDescentState = RUNNING;
            cog->ditch = false;
            break;
        }

        case RUNNING:{
            double dist2Target = ComputeDistance(positionA,positionB);

            // Proceeed to the top of descent by following the 
            // computed flight plan
            if(cog->request == REQUEST_RESPONDED && !cog->topofdescent){
                SetGuidanceFlightPlan(cog,"Plan1",1);  
                cog->request = REQUEST_NIL;
            }else if(!cog->topofdescent){
                // Check if top of descent (TOD) has been reached
                // TOP is calculated assuming a 45 degree flight path angle
                // while descending
                if(dist2Target < positionA[2]){
                    cog->topofdescent = true;
                    SendStatus(cog,"IC:Reached TOD",6);
                }
            }else{
                //TODO: Add parameter for final leg of ditching
                if(!command_sent){
                    SetGuidanceP2P(cog,cog->ditchsite[0],cog->ditchsite[1],cog->ditchsite[2],1.5);
                    command_sent = true;
                }else{
                   if(cog->p2pcomplete){
                      cog->emergencyDescentState = SUCCESS;
                   }      
                }
            }
            break;
        }

        case SUCCESS:{
            // Once vehicle has reached the final ditch site, we can land
            cog->fpPhase = LANDING_PHASE;
            break;
        }

        case FAILED:{
            break;
        }
    }
    return cog->emergencyDescentState;
}
