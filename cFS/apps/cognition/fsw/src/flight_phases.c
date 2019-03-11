#define EXTERN extern

#include "cognition.h"

void FlightPhases(void){

    // Handling nominal flight phases
    switch(appdataCog.fpPhase){

        case IDLE:{
            if(appdataCog.missionStart == 0){
                // If missionStart = 0 time to start the misison
                appdataCog.fpPhase = TAKEOFF; 
            }else if(appdataCog.missionStart > 0){
                // If missionStart > 0 goto climb state directly
                appdataCog.fpPhase = CRUISE;
            }
            break;
        }

        case TAKEOFF:{

            if(!appdataCog.takeoffStarted){
                argsCmd_t cmd;

                // Send ARM command
                CFE_SB_InitMsg(&cmd,ICAROUS_COMMANDS_MID, sizeof(argsCmd_t),TRUE);
                cmd.name = _ARM_;
                cmd.param1 = 1;
                SendSBMsg(cmd); 

                // Send Takeoff Command
                CFE_SB_InitMsg(&cmd,ICAROUS_COMMANDS_MID, sizeof(argsCmd_t),TRUE);
                cmd.name = _TAKEOFF_;
                cmd.param1 = appdataCog.takeoffAlt;
                SendSBMsg(cmd); 

                appdataCog.takeoffStarted = true;
            }

            if(appdataCog.takeoffStarted){
                if(appdataCog.takeoffStatus == 0){
                     // If takeoff successful, switch to CLIMB
                     appdataCog.fpPhase = CLIMB;
                }else if(appdataCog.takeoffStatus == -1){
                    // If status stays in -1 for too long, fall back to idle, 
                    // assume that takeoff failed.

                }else{
                    // If takeoff failed, fall back to idle
                    appdataCog.takeoffStarted = false;
                    appdataCog.fpPhase = IDLE;
                }
            }

            break;
        }

        case CLIMB:{

            // Once cruising altitude is reached
            if(fabs(appdataCog.position.altitude_rel - 5.0) < 1){
                 appdataCog.fpPhase = CRUISE;
            }
            break;
        }

        case CRUISE:{
            // Set next waypoint

            // Start performing conflict management
            break;
        }

        case DESCENT:{

            break;
        }

        case APPROACH:{

            break;
        }

        case LANDING:{

            break;
        }
    }


}