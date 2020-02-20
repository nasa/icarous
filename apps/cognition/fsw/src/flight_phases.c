#define EXTERN extern

#include "cognition.h"

void ResetFlightPhases(void) {
    appdataCog.takeoffState = INITIALIZING;
    appdataCog.cruiseState = INITIALIZING;
}

void FlightPhases(void){

    // Handling nominal flight phases
    status_e status;
    switch(appdataCog.fpPhase){

        case IDLE_PHASE:{
            ResetFlightPhases();
            if(appdataCog.missionStart == 0){
                // If missionStart = 0 time to start the misison
                appdataCog.fpPhase = TAKEOFF_PHASE; 
                appdataCog.missionStart = -1;
            }else if(appdataCog.missionStart > 0){
                // If missionStart > 0 goto climb state directly
                appdataCog.fpPhase = CRUISE_PHASE;
                appdataCog.missionStart = -1;
            }

            if(appdataCog.ditch){
                appdataCog.emergencyDescentState = INITIALIZING;
                appdataCog.fpPhase = EMERGENCY_DESCENT_PHASE;
            }
            break;
        }

        case TAXI_PHASE:{
            break;
        }

        case TAKEOFF_PHASE:{
            //OS_printf("Takeoff phase\n");
            status = Takeoff();
            if (status == SUCCESS){
                appdataCog.fpPhase = CLIMB_PHASE;
            }else if(status == FAILED){
                appdataCog.fpPhase = IDLE_PHASE;
            }
            break;
        }

        case CLIMB_PHASE:{
            status = Climb();
            if (status == SUCCESS){
                appdataCog.fpPhase = CRUISE_PHASE;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case CRUISE_PHASE:{
            status = Cruise();
            if (status == SUCCESS){
                appdataCog.fpPhase = DESCENT_PHASE;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }

            if( appdataCog.mergingActive){
                appdataCog.fpPhase = MERGING_PHASE;
            }

            if(appdataCog.ditch){
                appdataCog.emergencyDescentState = INITIALIZING;
                appdataCog.fpPhase = EMERGENCY_DESCENT_PHASE;
            }

            break;
        }

        case DESCENT_PHASE:{
            status = Descent();
            if (status == SUCCESS){
                appdataCog.fpPhase = APPROACH_PHASE;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case EMERGENCY_DESCENT_PHASE:{
            status = EmergencyDescent();

            // Check if there is another request for ditching
            // and initialize ditching again
            if(appdataCog.ditch){
                appdataCog.emergencyDescentState = INITIALIZING;
            }
            
            // If end ditch is requested, kill ditching
            if(appdataCog.endDitch){
                appdataCog.emergencyDescentState = SUCCESS;
            }
            break;
        }

        case APPROACH_PHASE:{
            status = Approach();
            if (status == SUCCESS){
                appdataCog.fpPhase = LANDING_PHASE;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case LANDING_PHASE:{
            status = Landing();
            //OS_printf("Landing\n");
            if (status == SUCCESS){
                appdataCog.fpPhase = IDLE_PHASE;
            }else if(status == FAILED){
                // TODO: Figure out a fall back plan
            }
            break;
        }

        case MERGING_PHASE:{
            if(!appdataCog.mergingActive){
                appdataCog.fpPhase = CRUISE_PHASE;
                SetGuidanceFlightPlan("Plan0",appdataCog.nextFeasibleWP1);
            }
            break;
        }

    
    }


}

status_e Takeoff(){
    switch(appdataCog.takeoffState){
       case INITIALIZING:{
           // Send takeoff command to guidance application
           if(!appdataCog.keepInConflict && !appdataCog.keepOutConflict){
               argsCmd_t cmd;
               CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(cmd),TRUE);
               cmd.name = TAKEOFF;
               SendSBMsg(cmd);
               appdataCog.takeoffState = RUNNING;
               appdataCog.takeoffComplete = 0;
           }else{
               appdataCog.takeoffState = FAILED;
           }
           break;
       } 

       case RUNNING:{
           // Wait for confirmation status from guidance application

           if(appdataCog.takeoffComplete == 1){
               appdataCog.takeoffState = SUCCESS;
           }else if(appdataCog.takeoffComplete == 0){
               appdataCog.takeoffState = RUNNING;
           }else{
               appdataCog.takeoffState = FAILED;
               OS_printf("Takeoff failed\n");
           }
           break;
       }

       case SUCCESS: break;
       case FAILED: break;
    }

    return appdataCog.takeoffState;
}

status_e Climb(){
    // Currently only a place holder for rotrocraft
    return SUCCESS;
}

status_e Cruise(){

   switch(appdataCog.cruiseState){

       case INITIALIZING:{
            SetGuidanceFlightPlan("Plan0",appdataCog.nextPrimaryWP);
            appdataCog.cruiseState = RUNNING;
            break;
       }

       case RUNNING:{
            TrafficConflictManagement();

            GeofenceConflictManagement();

            XtrackManagement(); 

            ReturnToNextWP();

            if(appdataCog.nextPrimaryWP >= appdataCog.flightplan1.num_waypoints){
                appdataCog.cruiseState = SUCCESS;
                OS_printf("Completing cruise\n");  
            }
            break;
       }

       case SUCCESS:break;
       case FAILED: break;
   }

   return appdataCog.cruiseState;
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
    SetStatus(appdataCog.statustxt,"IC: Landing",SEVERITY_NOTICE);
    argsCmd_t cmd;
    CFE_SB_InitMsg(&cmd, GUIDANCE_COMMAND_MID, sizeof(cmd), TRUE);
    cmd.name = LAND;
    SendSBMsg(cmd);
    return SUCCESS;
}

status_e EmergencyDescent(){
    static bool command_sent;
    double positionA[3] = {appdataCog.position.latitude,
                           appdataCog.position.longitude,
                           appdataCog.position.altitude_abs};
   
    double positionB[3] = {appdataCog.ditchsite[0],
                           appdataCog.ditchsite[1],
                           positionA[2]};

    double Trk, Gs, Vs;
    ConvertVnedToTrkGsVs(appdataCog.position.vn, 
                         appdataCog.position.ve,
                         appdataCog.position.vd, 
                         &Trk, &Gs, &Vs);

    double velocityA[3] = {Trk,Gs,Vs};


    switch(appdataCog.emergencyDescentState){

        case INITIALIZING:{
            SetStatus(appdataCog.statustxt,"IC:Starging to ditch",SEVERITY_NOTICE);
            command_sent = false;
            appdataCog.request = REQUEST_NIL;

            // Find a new path to the ditch site 
            // Note that if a direct path is not availabe, this will find
            // a possible detour. 
            FindNewPath(appdataCog.searchAlgType, positionA, velocityA, positionB);

            appdataCog.request = REQUEST_PROCESSING;
            appdataCog.emergencyDescentState = RUNNING;
            appdataCog.ditch = false;
            break;
        }

        case RUNNING:{
            double dist2Target = ComputeDistance(positionA,positionB);

            // Proceeed to the top of descent by following the 
            // computed flight plan
            if(appdataCog.request == REQUEST_RESPONDED && !appdataCog.topofdescent){
                SetGuidanceFlightPlan("Plan1",1);  
                appdataCog.request = REQUEST_NIL;
            }else if(!appdataCog.topofdescent){
                // Check if top of descent (TOD) has been reached
                // TOP is calculated assuming a 45 degree flight path angle
                // while descending
                if(dist2Target < positionA[2]){
                    appdataCog.topofdescent = true;
                    SetStatus(appdataCog.statustxt,"IC:Reached TOP",SEVERITY_NOTICE);
                }
            }else{
                //TODO: Add parameter for final leg of ditching
                if(!command_sent){
                    SetGuidanceP2P(appdataCog.ditchsite[0],appdataCog.ditchsite[1],appdataCog.ditchsite[2],1.5);
                    command_sent = true;
                }else{
                   if(appdataCog.p2pcomplete){
                      appdataCog.emergencyDescentState = SUCCESS;
                   }      
                }
            }
            break;
        }

        case SUCCESS:{
            // Once vehicle has reached the final ditch site, we can land
            appdataCog.fpPhase = LANDING_PHASE;
            break;
        }

        case FAILED:{
            break;
        }
    }
    return appdataCog.emergencyDescentState;
}