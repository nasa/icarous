/**
 * @file cognition.c
 * @brief function definitions for cognition app
 */
#define EXTERN

#include "cognition.h"
#include "cognition_version.h"
#include "cog_tbl.c"

/// Event filter definition for ardupilot
CFE_EVS_BinFilter_t  Cognition_EventFilters[] =
{  /* Event ID    mask */
		{COGNITION_STARTUP_INF_EID,       0x0000},
		{COGNITION_COMMAND_ERR_EID,       0x0000},
};

/* APINTERFACE_AppMain() -- Application entry points */
void COGNITION_AppMain(void){

	int32 status;
	uint32 RunStatus = CFE_ES_APP_RUN;

    // All functions to initialize app and open  connection to autopilot goes in here
    COGNITION_AppInit();

	while(CFE_ES_RunLoop(&RunStatus) == TRUE) {
		status = CFE_SB_RcvMsg(&appdataCog.SchMsgPtr, appdataCog.SchPipe, CFE_SB_POLL);
		if (status == CFE_SUCCESS) {
			CFE_SB_MsgId_t MsgId;
			MsgId = CFE_SB_GetMsgId(appdataCog.SchMsgPtr);
			switch (MsgId) {
				case FREQ_10_WAKEUP_MID:
                    COGNITION_DecisionProcess();
					break;
			}
		}

        // Get data from Software bus and send necessary commands to autopilot
        int32 status = CFE_SB_RcvMsg(&appdataCog.CogMsgPtr, appdataCog.CognitionPipe, 10);

        if (status == CFE_SUCCESS)
        {
            COGNITION_ProcessSBData();
        }

	}

    COGNITION_AppCleanUp();

	CFE_ES_ExitApp(RunStatus);
}

void COGNITION_AppInit(void){

	memset(&appdataCog,0,sizeof(appdataCog));

	int32 status;

	// Register the app with executive services
	CFE_ES_RegisterApp();

	// Register the events
	CFE_EVS_Register(Cognition_EventFilters,
			sizeof(Cognition_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
			CFE_EVS_BINARY_FILTER);

	// Create pipe to receive SB messages
	status = CFE_SB_CreatePipe( &appdataCog.CognitionPipe,     /* Variable to hold Pipe ID */
								COGNITION_PIPE_DEPTH,          /* Depth of Pipe */
								COGNITION_PIPE_NAME);          /* Name of pipe */

	// Create pipe to receive scheduler messages
	status = CFE_SB_CreatePipe( &appdataCog.SchPipe,            /* Variable to hold Pipe ID */
								COGNITION_PIPE_DEPTH,           /* Depth of Pipe */
								SCH_COGNITION_PIPE_NAME);       /* Name of pipe */

	// Subscribe to wakeup messages from scheduler
	CFE_SB_SubscribeLocal(FREQ_10_WAKEUP_MID,appdataCog.SchPipe,CFE_SB_DEFAULT_MSG_LIMIT);

	// Subscribe to messages from the software bus
	//Subscribe to command messages from the SB to command the autopilot
    CFE_SB_Subscribe(ICAROUS_POSITION_MID, appdataCog.CognitionPipe);
	CFE_SB_SubscribeLocal(ICAROUS_COMMANDS_MID, appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_RESET_MID, appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_FLIGHTPLAN_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_BANDS_TRACK_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_BANDS_SPEED_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_BANDS_ALT_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_BANDS_VS_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(TRAFFIC_PARAMETERS_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(TRAJECTORY_PARAMETERS_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_TRAJECTORY_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_STARTMISSION_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(FLIGHTPLAN_MONITOR_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_GEOFENCE_MONITOR_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_WPREACHED_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);

    #ifdef APPDEF_MERGER
    CFE_SB_SubscribeLocal(MERGER_STATUS_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    #endif

    #ifdef APPDEF_SAFE2DITCH
    CFE_SB_SubscribeLocal(SAFE2DITCH_STATUS_MID,appdataCog.CognitionPipe,CFE_SB_DEFAULT_MSG_LIMIT);
    #endif
	// Initialize all messages that this App generates.
	// To perfrom sense and avoid, as a minimum, the following messages must be generated

	// Send event indicating app initialization
    if(status == CFE_SUCCESS){
        CFE_EVS_SendEvent (COGNITION_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                        "Cognition Interface initialized. Version %d.%d",
                        COGNITION_MAJOR_VERSION,
                        COGNITION_MINOR_VERSION);
    }

    COGNITION_AppInitData();

}

void COGNITION_AppInitData(void){
    appdataCog.cog = CognitionInit();
    CFE_SB_InitMsg(&appdataCog.statustxt,ICAROUS_STATUS_MID,sizeof(status_t),TRUE);
}


void COGNITION_ProcessSBData(void) {


    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(appdataCog.CogMsgPtr);
    switch (MsgId)
    {
        case ICAROUS_FLIGHTPLAN_MID:{
            flightplan_t* fplan = (flightplan_t*)appdataCog.CogMsgPtr;
            memcpy(&appdataCog.flightplan1,fplan,sizeof(flightplan_t));
            for(int i = 0; i<fplan->num_waypoints; ++i){
                waypoint_t wp = fplan->waypoints[i];
                double wp_pos[3] = {wp.latitude, wp.longitude, wp.altitude};
                InputFlightPlanData(appdataCog.cog,fplan->id,fplan->scenario_time,i,wp_pos,wp.wp_metric,wp.value);
            }
            break;
        }

        case ICAROUS_RESET_MID:{
            COGNITION_AppInitData();
            break;
        }

         case ICAROUS_BANDS_TRACK_MID:{
            bands_t* trk = (bands_t*) CFE_SB_GetUserData(appdataCog.CogMsgPtr);
            memcpy(&appdataCog.trkBands,trk,sizeof(bands_t));
            InputTrackBands(appdataCog.cog,trk);
            break;
        }

        case ICAROUS_BANDS_SPEED_MID:{
            bands_t* gs = (bands_t*) CFE_SB_GetUserData(appdataCog.CogMsgPtr);
            memcpy(&appdataCog.gsBands,gs,sizeof(bands_t));
            InputSpeedBands(appdataCog.cog,gs);
            break;
        }

        case ICAROUS_BANDS_ALT_MID:{
            bands_t* alt = (bands_t*) CFE_SB_GetUserData(appdataCog.CogMsgPtr);
            memcpy(&appdataCog.altBands,alt,sizeof(bands_t));
            InputAltBands(appdataCog.cog,alt);
            break;
        }

        case ICAROUS_BANDS_VS_MID:{
            bands_t* vspeed = (bands_t*) CFE_SB_GetUserData(appdataCog.CogMsgPtr);
            memcpy(&appdataCog.vsBands,vspeed,sizeof(bands_t));
            InputVSBands(appdataCog.cog,vspeed);
            break;
        }

		case ICAROUS_POSITION_MID:{
            position_t* pos = (position_t*) appdataCog.CogMsgPtr;
			memcpy(&appdataCog.position,pos,sizeof(position_t));			
            double position[3] = {pos->latitude,pos->longitude,pos->altitude_rel};
            double velocity[3] = {pos->vn,pos->ve,pos->vd};
            double velTrkGsVs[3];
            ConvertVnedToTrkGsVs(velocity[0],velocity[1],velocity[2],velTrkGsVs,velTrkGsVs+1,velTrkGsVs+2);
            InputVehicleState(appdataCog.cog,position,velTrkGsVs,pos->hdg);
			break;
		}

        case ICAROUS_COMMANDS_MID:{
            argsCmd_t* cmd = (argsCmd_t*) appdataCog.CogMsgPtr;

            switch(cmd->name){
                case _TRAFFIC_RES_:{
                    appdataCog.parameters.resolutionType = cmd->param1;
                    InputParameters(appdataCog.cog,&appdataCog.parameters);
                    break;
                }

                default:
                    break;
            }
            break;
        }

        case ICAROUS_STARTMISSION_MID:{
            argsCmd_t* msg = (argsCmd_t*) appdataCog.CogMsgPtr;
            int mission_start = msg->param1;
            double scenario_time = msg->param2;
            StartMission(appdataCog.cog,mission_start,scenario_time);
            CFE_ES_WriteToSysLog("Cognition:Received start mission command\n");
            break;
        }


        // This may not be required
        case FLIGHTPLAN_MONITOR_MID:{
            flightplan_monitor_t* fpm = (flightplan_monitor_t*) appdataCog.CogMsgPtr;
            if (strcmp(fpm->planID,"Plan0") == 0){
                memcpy(&appdataCog.fp1monitor,fpm,sizeof(flightplan_monitor_t));
                //OS_printf("xtrack dev %f, allowd dev %f\n",fpm->crossTrackDeviation,fpm->allowedXtrackError);
            }else if(strcmp(fpm->planID,"Plan1") == 0){
                memcpy(&appdataCog.fp2monitor,fpm,sizeof(flightplan_monitor_t));
            }

            appdataCog.parameters.resolutionSpeed = fpm->resolutionSpeed;
            InputParameters(appdataCog.cog,&appdataCog.parameters);
            break;
        }

        case TRAFFIC_PARAMETERS_MID:{
            traffic_parameters_t* msg = (traffic_parameters_t*) appdataCog.CogMsgPtr;
            appdataCog.parameters.DTHR = msg->det_1_WCV_DTHR;
            appdataCog.parameters.ZTHR = msg->det_1_WCV_ZTHR;
            appdataCog.parameters.resolutionType = msg->resType;
            InputParameters(appdataCog.cog,&appdataCog.parameters);
            break;
        }

        case TRAJECTORY_PARAMETERS_MID:{
            trajectory_parameters_t* msg = (trajectory_parameters_t*) appdataCog.CogMsgPtr;
            appdataCog.parameters.allowedXtrackDeviation = msg->xtrkDev;
            appdataCog.parameters.XtrackGain = msg->xtrkGain;
            appdataCog.parameters.resolutionSpeed = msg->resSpeed;
            appdataCog.parameters.searchType = msg->searchAlgorithm;
            InputParameters(appdataCog.cog,&appdataCog.parameters);
            break;
        }

        case ICAROUS_WPREACHED_MID:{
            missionItemReached_t* msg = (missionItemReached_t*) appdataCog.CogMsgPtr;
            if(!msg->feedback){
                break;
            }
            ReachedWaypoint(appdataCog.cog,msg->planID,msg->reachedwaypoint);

            if(strcmp(msg->planID,"Plan0") == 0){
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s, %d/%d\n",msg->planID,msg->reachedwaypoint,appdataCog.flightplan1.num_waypoints);
            }else if(strcmp(msg->planID,"Plan1") == 0){
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s, %d/%d\n",msg->planID,msg->reachedwaypoint,appdataCog.flightplan2.num_waypoints);
            }else if(strcmp(msg->planID,"P2P") == 0){
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s\n",msg->planID);
            }else if(strcmp(msg->planID,"Takeoff") == 0){
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s\n",msg->planID);
            }else if(strcmp(msg->planID,"PlanM") == 0){
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s, %d/%d\n",msg->planID,msg->reachedwaypoint,appdataCog.flightplan2.num_waypoints);
            }else{
            }
            break;
        }

        case ICAROUS_TRAJECTORY_MID:
        {
            flightplan_t *fp;
            fp = (flightplan_t *)appdataCog.CogMsgPtr;
            memcpy(&appdataCog.flightplan2, fp, sizeof(flightplan_t));
            for(int i = 0;i < fp->num_waypoints; i++){
                waypoint_t wp = fp->waypoints[i];
                double wp_pos[3] = {wp.latitude, wp.longitude, wp.altitude};
                InputFlightPlanData(appdataCog.cog,fp->id,fp->scenario_time,i,wp_pos,wp.wp_metric,wp.value);
            }
            break;
        }

        case ICAROUS_GEOFENCE_MONITOR_MID:{
            geofenceConflict_t* gf_conflict = (geofenceConflict_t*) CFE_SB_GetUserData(appdataCog.CogMsgPtr);
            InputGeofenceConflictData(appdataCog.cog,gf_conflict);
            break;
        }

        #ifdef APPDEF_MERGER
        case MERGER_STATUS_MID:{
            argsCmd_t* cmd = (argsCmd_t*) appdataCog.CogMsgPtr;
            int merge_status = cmd->param1;
            InputMergeStatus(appdataCog.cog,merge_status);
            break;
        }
        #endif

        #ifdef APPDEF_SAFE2DITCH
        case SAFE2DITCH_STATUS_MID:{
            OS_printf("Received ditch status\n");
            safe2ditchStatus_t* status = (safe2ditchStatus_t*) appdataCog.CogMsgPtr;
            InputDitchStatus(appdataCog.cog,status->ditchsite,status->ditchRequested);
            break;
        }
        #endif
    }
    return;
}

void COGNITION_AppCleanUp(void){
    //TODO: clean up memory allocation here if necessary
}

int32_t cognitionTableValidationFunc(void *TblPtr){
  int32_t status = 0;
  return status;
}

void COGNITION_DecisionProcess(void){
    FlightPhases(appdataCog.cog);

    Command command;
    int commands_remaining = 1;
    while(commands_remaining > 0){
        commands_remaining = GetCognitionOutput(appdataCog.cog,&command);
        if(commands_remaining > 0){
            switch(command.commandType){
                case STATUS_MESSAGE:{
                    SetStatus(appdataCog.statustxt,command.statusMessage.buffer,command.statusMessage.severity);
                    break;
                }

                case VELOCITY_COMMAND:{
                    COGNITION_SendGuidanceVelCmd(command.velocityCommand);
                    break;
                }

                case P2P_COMMAND:{
                    COGNITION_SendGuidanceP2P(command.p2PCommand);
                    break;
                }

                case SPEED_CHANGE_COMMAND:{
                    COGNITION_SendSpeedChange(command.speedChange);
                    break;
                }

                case TAKEOFF_COMMAND:{
                    COGNITION_SendTakeoff(command.takeoffCommand);
                    break;
                }

                case LAND_COMMAND:{
                    COGNITION_SendLand(command.landCommand);
                    break;
                }

                case DITCH_COMMAND:{
                    COGNITION_SendDitchRequest(command.ditchCommand);
                    break;
                }

                case FP_CHANGE:{
                    COGNITION_SendGuidanceFlightPlan(command.fpChange);
                    break;
                }

                case FP_REQUEST:{
                    COGNITION_FindNewPath(command.fpRequest);
                    break;
                }
            }
        }
    }
}

void COGNITION_SendGuidanceVelCmd(VelocityCommand velocity_command){
    argsCmd_t cmd;
    CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(argsCmd_t),TRUE);
    cmd.name = VECTOR;
    cmd.param1 = (float) velocity_command.vn;
    cmd.param2 = (float) velocity_command.ve;
    cmd.param3 = (float) velocity_command.vu;
    SendSBMsg(cmd);
}

void COGNITION_SendGuidanceFlightPlan(FpChange fp_change_command){
    argsCmd_t cmd;
    CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(argsCmd_t),TRUE);

    strcpy(cmd.buffer,fp_change_command.name);
    if(strcmp(fp_change_command.name,"Plan0") == 0){
       cmd.name = FLIGHTPLAN;
       cmd.param1 = fp_change_command.wpIndex;
    }else{
       cmd.name = FLIGHTPLAN;
       cmd.param1 = fp_change_command.wpIndex;

       missionItemReached_t itemReached;
       CFE_SB_InitMsg(&itemReached,ICAROUS_WPREACHED_MID,sizeof(itemReached),TRUE);
       itemReached.feedback = false;
       strcpy(itemReached.planID,"Plan0");
       itemReached.reachedwaypoint = fp_change_command.nextFeasibleWp-1;
       SendSBMsg(itemReached);
    }
    cmd.param2 = fp_change_command.nextFeasibleWp;
    SendSBMsg(cmd);
}

void COGNITION_SendGuidanceP2P(P2PCommand p2p_command){
   //Send Goto position
   argsCmd_t guidanceCmd; CFE_SB_InitMsg(&guidanceCmd, GUIDANCE_COMMAND_MID, sizeof(argsCmd_t), TRUE);
   guidanceCmd.name = POINT2POINT;
   guidanceCmd.param1 = p2p_command.point[0];
   guidanceCmd.param2 = p2p_command.point[1];
   guidanceCmd.param3 = p2p_command.point[2];
   guidanceCmd.param4 = p2p_command.speed;
   SendSBMsg(guidanceCmd);
}

void COGNITION_SendSpeedChange(SpeedChange speed_command){
    // Send speed change command
    argsCmd_t cmd;
    CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(argsCmd_t),TRUE);
    cmd.name = SPEED_CHANGE;
    strcpy(cmd.buffer,speed_command.name);
    cmd.param1 = speed_command.speed;
    cmd.param2 = speed_command.hold;
    SendSBMsg(cmd);
}

void COGNITION_SendTakeoff(TakeoffCommand takeoff_command){
    argsCmd_t cmd;
    CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(cmd),TRUE);
    cmd.name = TAKEOFF;
    SendSBMsg(cmd);
}

void COGNITION_SendLand(LandCommand land_command){
    argsCmd_t cmd;
    CFE_SB_InitMsg(&cmd, GUIDANCE_COMMAND_MID, sizeof(cmd), TRUE);
    cmd.name = LAND;
    SendSBMsg(cmd);
}

void COGNITION_FindNewPath(FpRequest fp_request_command){
   trajectory_request_t pathRequest;
   CFE_SB_InitMsg(&pathRequest, ICAROUS_TRAJECTORY_REQUEST_MID, sizeof(trajectory_request_t), TRUE);

   pathRequest.algorithm = fp_request_command.searchType;
   strcpy(pathRequest.planID,fp_request_command.name);
   memcpy(pathRequest.initialPosition, fp_request_command.fromPosition, sizeof(double) * 3);
   memcpy(pathRequest.initialVelocity, fp_request_command.startVelocity, sizeof(double) * 3);
   memcpy(pathRequest.finalPosition, fp_request_command.toPosition, sizeof(double) * 3);

   SendSBMsg(pathRequest);
}

void COGNITION_SendDitchRequest(DitchCommand ditch_command){
   noArgsCmd_t cmd;
   CFE_SB_InitMsg(&cmd,ICAROUS_DITCH_MID,sizeof(noArgsCmd_t),TRUE);
   cmd.name = _DITCH_;
   SendSBMsg(cmd);
}
