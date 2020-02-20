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

void COGNITION_AppInitData(){
    appdataCog.returnSafe = true;
    appdataCog.nextPrimaryWP = 1;
    appdataCog.resolutionTypeCmd = -1;
    appdataCog.request = REQUEST_NIL;
    appdataCog.fpPhase = IDLE_PHASE;
    appdataCog.missionStart = -1;
    appdataCog.keepInConflict = false;
    appdataCog.keepOutConflict = false;
    appdataCog.p2pcomplete = false;
    appdataCog.takeoffComplete = -1;

    appdataCog.trafficConflictState = NOOPC;
    appdataCog.geofenceConflictState = NOOPC;
    appdataCog.trafficTrackConflict = false;
    appdataCog.trafficSpeedConflict = false;
    appdataCog.trafficAltConflict = false;
    appdataCog.XtrackConflictState = NOOPC;
    appdataCog.resolutionTypeCmd = TRACK_RESOLUTION;
    appdataCog.requestGuidance2NextWP = -1;
    appdataCog.searchAlgType = _ASTAR;
    appdataCog.topofdescent = false;
    appdataCog.ditch = false;
    appdataCog.endDitch = false;
    appdataCog.resetDitch = false;
    memset(appdataCog.trkBands.wpFeasibility1,1,sizeof(bool)*50);
    memset(appdataCog.trkBands.wpFeasibility2,1,sizeof(bool)*50);
    CFE_SB_InitMsg(&appdataCog.statustxt,ICAROUS_STATUS_MID,sizeof(status_t),TRUE);
        
}


void COGNITION_ProcessSBData() {


    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(appdataCog.CogMsgPtr);
    switch (MsgId)
    {
        case ICAROUS_FLIGHTPLAN_MID:{
            flightplan_t* fplan = (flightplan_t*)appdataCog.CogMsgPtr;
            memcpy(&appdataCog.flightplan1,fplan,sizeof(flightplan_t));

            //TODO: The flight plan icarous uses is stored in appdataCog.flightplan
            break;
        }

        case ICAROUS_RESET_MID:{
            COGNITION_AppInitData();  
            break;
        }

         case ICAROUS_BANDS_TRACK_MID:{
            bands_t* trk = (bands_t*) appdataCog.CogMsgPtr;

            memcpy(&appdataCog.trkBands,trk,sizeof(bands_t));

            if(trk->currentConflictBand == 1){
               appdataCog.trafficTrackConflict = true;
               if(trk->resPreferred >= 0)
                   appdataCog.preferredTrack = trk->resPreferred;
               else
                   appdataCog.preferredTrack = -1000;
            }else{
               appdataCog.trafficTrackConflict = false;
               appdataCog.preferredTrack = -10000;
            }

            break;
        }

        case ICAROUS_BANDS_SPEED_MID:{
            bands_t* gs = (bands_t*) appdataCog.CogMsgPtr;

            memcpy(&appdataCog.gsBands,gs,sizeof(bands_t));
            
            double fac;
            if( fabs(gs->resPreferred - gs->resDown) < 1e-3){
                 fac = 0.8;                
            }else{
                 fac = 1.2;
            }


            if(gs->currentConflictBand == 1){
               appdataCog.trafficSpeedConflict = true;
               if(!isinf(gs->resPreferred)){
                   appdataCog.preferredSpeed = gs->resPreferred * fac;
               }
               else
                   appdataCog.preferredSpeed = -10000;
            }else{
               int id = appdataCog.nextPrimaryWP;
               if (id >= appdataCog.flightplan1.num_waypoints)
                   id = appdataCog.flightplan1.num_waypoints-1;

               if(appdataCog.Plan0 && gs->wpFeasibility1[id]) {
                   appdataCog.trafficSpeedConflict = false;
               }else if(appdataCog.Plan1 && gs->wpFeasibility2[id]){
                   appdataCog.trafficSpeedConflict = false;
               }
               else {
                   if(appdataCog.trafficSpeedConflict){
                      appdataCog.trafficSpeedConflict = true;
                   }
               }
               appdataCog.preferredSpeed = -1000;
            }
            break;
        }

        case ICAROUS_BANDS_ALT_MID:{
            bands_t* alt = (bands_t*) appdataCog.CogMsgPtr;

            memcpy(&appdataCog.altBands,alt,sizeof(bands_t));
            if(alt->currentConflictBand == 1){
               appdataCog.trafficAltConflict = true;
               if(!isinf(alt->resPreferred))
                   appdataCog.preferredAlt = alt->resPreferred;
               else
                   appdataCog.preferredAlt = -10000;
            }else{
               appdataCog.trafficAltConflict = false;
               appdataCog.preferredAlt = -1000;
            }
            break;
        }

        case ICAROUS_BANDS_VS_MID:{
            bands_t* vspeed = (bands_t*)appdataCog.CogMsgPtr;
            memcpy(&appdataCog.vsBands,vspeed,sizeof(bands_t));
            break;
        }

		case ICAROUS_POSITION_MID:{
            position_t* pos = (position_t*) appdataCog.CogMsgPtr;
			memcpy(&appdataCog.position,pos,sizeof(position_t));			
            appdataCog.speed = sqrt(pow(pos->vn,2) + pow(pos->ve,2));
			break;
		}

        case ICAROUS_COMMANDS_MID:{
            argsCmd_t* cmd = (argsCmd_t*) appdataCog.CogMsgPtr;

            switch(cmd->name){
                case _TRAFFIC_RES_:{
                    appdataCog.resolutionTypeCmd = cmd->param1;
                    break;
                }

                default:
                    break;
            }
            break;
        }

        case ICAROUS_STARTMISSION_MID:{
            argsCmd_t* msg = (argsCmd_t*) appdataCog.CogMsgPtr;
            appdataCog.missionStart = (int)msg->param1;            
            CFE_ES_WriteToSysLog("Cognition:Received start mission command\n");
            break;
        }


        case FLIGHTPLAN_MONITOR_MID:{
            flightplan_monitor_t* fpm = (flightplan_monitor_t*) appdataCog.CogMsgPtr;
            if (strcmp(fpm->planID,"Plan0") == 0){
                memcpy(&appdataCog.fp1monitor,fpm,sizeof(flightplan_monitor_t));
                //OS_printf("xtrack dev %f, allowd dev %f\n",fpm->crossTrackDeviation,fpm->allowedXtrackError);
            }else if(strcmp(fpm->planID,"Plan1") == 0){
                memcpy(&appdataCog.fp2monitor,fpm,sizeof(flightplan_monitor_t));
            }
            appdataCog.resolutionSpeed = fpm->resolutionSpeed;
            break;
        }

        case TRAFFIC_PARAMETERS_MID:{
            traffic_parameters_t* msg = (traffic_parameters_t*) appdataCog.CogMsgPtr;
            appdataCog.DTHR = msg->det_1_WCV_DTHR;
            appdataCog.ZTHR = msg->det_1_WCV_ZTHR;
            appdataCog.resolutionTypeCmd = msg->resType;
            break;
        }

        case ICAROUS_WPREACHED_MID:{
            missionItemReached_t* msg = (missionItemReached_t*) appdataCog.CogMsgPtr;
            if(!msg->feedback){
                break;
            }

            if(strcmp(msg->planID,"Plan0") == 0){
                appdataCog.nextPrimaryWP = msg->reachedwaypoint + 1;
                appdataCog.nextWP = appdataCog.nextPrimaryWP;
                strcpy(appdataCog.currentPlanID,"Plan0");
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s, %d/%d\n",msg->planID,msg->reachedwaypoint,appdataCog.flightplan1.num_waypoints);
            }else if(strcmp(msg->planID,"Plan1") == 0){
                appdataCog.nextSecondaryWP = msg->reachedwaypoint + 1;
                appdataCog.nextWP = appdataCog.nextSecondaryWP;
                strcpy(appdataCog.currentPlanID,"Plan1");
                if(appdataCog.nextSecondaryWP >= appdataCog.flightplan2.num_waypoints)
                    appdataCog.fp2complete = true;
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s, %d/%d\n",msg->planID,msg->reachedwaypoint,appdataCog.flightplan2.num_waypoints);
            }else if(strcmp(msg->planID,"P2P") == 0){
                appdataCog.p2pcomplete = true;
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s\n",msg->planID);
            }else if(strcmp(msg->planID,"Takeoff") == 0){
                appdataCog.takeoffComplete = msg->reachedwaypoint;
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s\n",msg->planID);
            }else{
            }
            break;
        }

        case ICAROUS_TRAJECTORY_MID:
        {
            flightplan_t *fp;
            fp = (flightplan_t *)appdataCog.CogMsgPtr;
            memcpy(&appdataCog.flightplan2, fp, sizeof(flightplan_t));
            appdataCog.request = REQUEST_RESPONDED;
            break;
        }

        case ICAROUS_GEOFENCE_MONITOR_MID:{
            geofenceConflict_t* gfConflct = (geofenceConflict_t*) appdataCog.CogMsgPtr;
            if(gfConflct->numConflicts > 0 ) {
                if (gfConflct->conflictTypes[0] == _KEEPIN_)
                    appdataCog.keepInConflict = true;
                else if (gfConflct->conflictTypes[0] == _KEEPOUT_)
                    appdataCog.keepOutConflict = true;
            }else{
                appdataCog.keepOutConflict = false;
                appdataCog.keepInConflict = false;
            }

            memcpy(appdataCog.recoveryPosition,gfConflct->recoveryPosition, sizeof(double)*3);

            int totalWP;
            totalWP = appdataCog.flightplan1.num_waypoints;
            for (int i = appdataCog.nextPrimaryWP; i < totalWP; ++i)
            {
                if (!gfConflct->waypointConflict1[i])
                {
                    appdataCog.nextFeasibleWP1 = i;
                    break;
                }
            }
            appdataCog.directPathToFeasibleWP1 = gfConflct->directPathToWaypoint1[appdataCog.nextFeasibleWP1] && appdataCog.trkBands.wpFeasibility1[appdataCog.nextFeasibleWP1];
            totalWP = appdataCog.flightplan2.num_waypoints;
            for (int i = appdataCog.nextSecondaryWP; i < totalWP; ++i)
            {
                if (!gfConflct->waypointConflict2[i])
                {
                    appdataCog.nextFeasibleWP2 = i;
                    break;
                }
            }
            appdataCog.directPathToFeasibleWP2 = gfConflct->directPathToWaypoint2[appdataCog.nextFeasibleWP2] && appdataCog.trkBands.wpFeasibility2[appdataCog.nextFeasibleWP2];
            break;
        }

        #ifdef APPDEF_MERGER
        case MERGER_STATUS_MID:{
            argsCmd_t* cmd = (argsCmd_t*) appdataCog.CogMsgPtr;
            if((int)cmd->param1 == MERGING_ACTIVE){
                appdataCog.mergingActive = true;
            }else{
                appdataCog.mergingActive = false;
            }

            break;
        }
        #endif

        #ifdef APPDEF_SAFE2DITCH
        case SAFE2DITCH_STATUS_MID:{
            OS_printf("Received ditch status\n");
            safe2ditchStatus_t* status = (safe2ditchStatus_t*) appdataCog.CogMsgPtr;
            appdataCog.ditch = status->ditchRequested;
            memcpy(appdataCog.ditchsite,status->ditchsite,sizeof(double)*3);
            break;
        }
        #endif
    }
    return;
}

void COGNITION_AppCleanUp(){
    //TODO: clean up memory allocation here if necessary
}

int32_t cognitionTableValidationFunc(void *TblPtr){
  int32_t status = 0;
  return status;
}

void COGNITION_DecisionProcess(){
    FlightPhases();
}
