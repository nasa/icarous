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
            //TrafficConflictManagement();

            GeofenceConflictManagement();

            XtrackManagement(); 

            ReturnToNextWP();

            if(appdataCog.nextPrimaryWP >= appdataCog.flightplan1.num_waypoints){
                appdataCog.cruiseState = SUCCESS;
                OS_printf("Completing cruise\n");  
            }
            break;
       }
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
    argsCmd_t cmd;
    CFE_SB_InitMsg(&cmd, GUIDANCE_COMMAND_MID, sizeof(cmd), TRUE);
    cmd.name = LAND;
    SendSBMsg(cmd);
    return SUCCESS;
}