
#include "cognition_core.h"

cognition_t cog;

void ResetFlightPhases(void) {
    cog.takeoffState = INITIALIZING;
    cog.cruiseState = INITIALIZING;
}

void FlightPhases(void){

    // Handling nominal flight phases
    status_e status;
    switch(cog.fpPhase){

        case IDLE_PHASE:{
            ResetFlightPhases();
            if(cog.missionStart == 0){
                // If missionStart = 0 time to start the misison
                if(cog.primaryFPReceived){
                    time_t fpTime = cog.scenarioTime;
                    time_t currTime = time(NULL);
                    if( currTime >= fpTime ){
                        cog.fpPhase = TAKEOFF_PHASE; 
                        cog.missionStart = -1;
                    }else{
                        time_t timeRemanining = (fpTime - currTime);
                        if(timeRemanining%5 == 0){
                            SendStatus("FP counting down",6);
                        }
                    }
                }else{
                    SendStatus("No flightplan loaded",4);
                    cog.missionStart = -1;
                }
            }else if(cog.missionStart > 0){
                // If missionStart > 0 goto climb state directly
                if(cog.primaryFPReceived){
                    cog.fpPhase = CRUISE_PHASE;
                }else{
                    SendStatus("No flightplan loaded",4);
                }
                cog.missionStart = -1;
            }

            if(cog.ditch){
                cog.emergencyDescentState = INITIALIZING;
                cog.fpPhase = EMERGENCY_DESCENT_PHASE;
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
                cog.fpPhase = CLIMB_PHASE;
            }else if(status == FAILED){
                cog.fpPhase = IDLE_PHASE;
            }
            break;
        }

        case CLIMB_PHASE:{
            status = Climb();
            if (status == SUCCESS){
                cog.fpPhase = CRUISE_PHASE;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case CRUISE_PHASE:{
            status = Cruise();
            if (status == SUCCESS){
                cog.fpPhase = DESCENT_PHASE;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }

            if( cog.mergingActive){
                cog.fpPhase = MERGING_PHASE;
            }

            if(cog.ditch){
                cog.emergencyDescentState = INITIALIZING;
                cog.fpPhase = EMERGENCY_DESCENT_PHASE;
            }

            break;
        }

        case DESCENT_PHASE:{
            status = Descent();
            if (status == SUCCESS){
                cog.fpPhase = APPROACH_PHASE;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case EMERGENCY_DESCENT_PHASE:{
            status = EmergencyDescent();

            // Check if there is another request for ditching
            // and initialize ditching again
            if(cog.ditch){
                cog.emergencyDescentState = INITIALIZING;
            }
            
            // If end ditch is requested, kill ditching
            if(cog.endDitch){
                cog.emergencyDescentState = SUCCESS;
            }
            break;
        }

        case APPROACH_PHASE:{
            status = Approach();
            if (status == SUCCESS){
                cog.fpPhase = LANDING_PHASE;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case LANDING_PHASE:{
            status = Landing();
            //printf("Landing\n");
            if (status == SUCCESS){
                cog.fpPhase = IDLE_PHASE;
                cog.missionStart = -2;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case MERGING_PHASE:{
            if(!cog.mergingActive){
                cog.fpPhase = CRUISE_PHASE;
                SetGuidanceFlightPlan("Plan0",cog.nextFeasibleWP1);
            }
            break;
        }

    
    }


}

status_e Takeoff(){
    switch(cog.takeoffState){
       case INITIALIZING:{
           // Send takeoff command to guidance application
           if(!cog.keepInConflict && !cog.keepOutConflict){
               cog.guidanceCommand = cTAKEOFF;
               cog.sendCommand = true;
               cog.takeoffState = RUNNING;
               cog.takeoffComplete = 0;
           }else{
               cog.takeoffState = FAILED;
           }
           break;
       } 

       case RUNNING:{
           // Wait for confirmation status from guidance application

           if(cog.takeoffComplete == 1){
               cog.takeoffState = SUCCESS;
           }else if(cog.takeoffComplete == 0){
               cog.takeoffState = RUNNING;
           }else{
               cog.takeoffState = FAILED;
               printf("Takeoff failed\n");
           }
           break;
       }

       case SUCCESS: break;
       case FAILED: break;
    }

    return cog.takeoffState;
}

status_e Climb(){
    // Currently only a place holder for rotrocraft
    return SUCCESS;
}

status_e Cruise(){

   switch(cog.cruiseState){

       case INITIALIZING:{
            SetGuidanceFlightPlan("Plan0",cog.nextPrimaryWP);
            cog.cruiseState = RUNNING;
            break;
       }

       case RUNNING:{

            bool status = false;

            status |= TrafficConflictManagement();

            status |= GeofenceConflictManagement();

            status |= XtrackManagement(); 

            status |= ReturnToNextWP();

            // Perform time management only when following the primary flightplan
            if(!status){
                TimeManagement();
            }

            if(cog.nextPrimaryWP >= cog.num_waypoints){
                cog.cruiseState = SUCCESS;
                printf("Completing cruise\n");  
            }
            break;
       }

       case SUCCESS:break;
       case FAILED: break;
   }

   return cog.cruiseState;
}

status_e Descent(){
    // Currently only a place holder for rotorcraft
    return SUCCESS;
}

status_e Approach(){
    // Currently only a place holder for rotorcraft
    return SUCCESS;
}

status_e Landing(){
    SendStatus("IC: Landing",6);
    cog.sendCommand = true;
    cog.guidanceCommand = cLAND;
    return SUCCESS;
}

status_e EmergencyDescent(){
    static bool command_sent;
    double positionA[3] = {cog.position[0],
                           cog.position[1],
                           cog.position[2]};
   
    double positionB[3] = {cog.ditchsite[0],
                           cog.ditchsite[1],
                           positionA[2]};

    double Trk, Gs, Vs;
    ConvertVnedToTrkGsVs(cog.velocity[0], 
                         cog.velocity[1],
                         cog.velocity[2], 
                         &Trk, &Gs, &Vs);

    double velocityA[3] = {Trk,Gs,Vs};


    switch(cog.emergencyDescentState){

        case INITIALIZING:{
            SendStatus("IC:Starting to ditch",6);
            command_sent = false;
            cog.request = REQUEST_NIL;

            // Find a new path to the ditch site 
            // Note that if a direct path is not availabe, this will find
            // a possible detour. 
            FindNewPath(cog.searchType, positionA, velocityA, positionB);

            cog.request = REQUEST_PROCESSING;
            cog.emergencyDescentState = RUNNING;
            cog.ditch = false;
            break;
        }

        case RUNNING:{
            double dist2Target = ComputeDistance(positionA,positionB);

            // Proceeed to the top of descent by following the 
            // computed flight plan
            if(cog.request == REQUEST_RESPONDED && !cog.topofdescent){
                SetGuidanceFlightPlan("Plan1",1);  
                cog.request = REQUEST_NIL;
            }else if(!cog.topofdescent){
                // Check if top of descent (TOD) has been reached
                // TOP is calculated assuming a 45 degree flight path angle
                // while descending
                if(dist2Target < positionA[2]){
                    cog.topofdescent = true;
                    SendStatus("IC:Reached TOD",6);
                }
            }else{
                //TODO: Add parameter for final leg of ditching
                if(!command_sent){
                    SetGuidanceP2P(cog.ditchsite[0],cog.ditchsite[1],cog.ditchsite[2],1.5);
                    command_sent = true;
                }else{
                   if(cog.p2pcomplete){
                      cog.emergencyDescentState = SUCCESS;
                   }      
                }
            }
            break;
        }

        case SUCCESS:{
            // Once vehicle has reached the final ditch site, we can land
            cog.fpPhase = LANDING_PHASE;
            break;
        }

        case FAILED:{
            break;
        }
    }
    return cog.emergencyDescentState;
}