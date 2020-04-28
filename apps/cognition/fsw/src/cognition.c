/**
 * @file cognition.c
 * @brief function definitions for cognition app
 */
#define EXTERN 

#include "cognition.h"
#include "cognition_version.h"
#include "cog_tbl.c"
#include "cognition_core.h"

extern cognition_t cog;

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

void COGNITION_AppInitData(){
    cog.returnSafe = true;
    cog.nextPrimaryWP = 1;
    cog.resolutionTypeCmd = -1;
    cog.request = REQUEST_NIL;
    cog.fpPhase = IDLE_PHASE;
    cog.missionStart = -1;
    cog.keepInConflict = false;
    cog.keepOutConflict = false;
    cog.p2pcomplete = false;
    cog.takeoffComplete = -1;

    cog.trafficConflictState = NOOPC;
    cog.geofenceConflictState = NOOPC;
    cog.trafficTrackConflict = false;
    cog.trafficSpeedConflict = false;
    cog.trafficAltConflict = false;
    cog.XtrackConflictState = NOOPC;
    cog.resolutionTypeCmd = TRACK_RESOLUTION;
    cog.requestGuidance2NextWP = -1;
    cog.searchType = _ASTAR;
    cog.topofdescent = false;
    cog.ditch = false;
    cog.endDitch = false;
    cog.resetDitch = false;
    cog.primaryFPReceived = false;
    cog.mergingActive = 0;

    cog.nextWPFeasibility1 = 1;
    cog.nextWPFeasibility2 = 1;
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
            cog.primaryFPReceived = true;

            cog.wpPrev1[0] =fplan->waypoints[0].latitude;
            cog.wpPrev1[1] =fplan->waypoints[0].longitude;
            cog.wpPrev1[2] =fplan->waypoints[0].altitude;

            cog.wpNext1[0] =fplan->waypoints[1].latitude;
            cog.wpNext1[1] =fplan->waypoints[1].longitude;
            cog.wpNext1[2] =fplan->waypoints[1].altitude;
            cog.num_waypoints = fplan->num_waypoints;
            cog.scenarioTime = fplan->scenario_time;

            if(fplan->waypoints[1].wp_metric == WP_METRIC_ETA){
                cog.wpMetricTime = true;
                cog.refWPTime = fplan->waypoints[1].value;
            }else{
                cog.wpMetricTime = false;
                cog.refWPTime = 0;
            }
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
               cog.trafficTrackConflict = true;
               if(trk->resPreferred >= 0)
                   cog.preferredTrack = trk->resPreferred;
               else
                   cog.preferredTrack = -1000;
            }else{
               cog.trafficTrackConflict = false;
               cog.preferredTrack = -10000;
            }

            memcpy(cog.trkBandType,appdataCog.trkBands.type,sizeof(int)*20);
            memcpy(cog.trkBandMin,appdataCog.trkBands.min,sizeof(double)*20);
            memcpy(cog.trkBandMax,appdataCog.trkBands.max,sizeof(double)*20);
            cog.trkBandNum = appdataCog.trkBands.numBands;
            cog.nextWPFeasibility1 = appdataCog.trkBands.wpFeasibility1[cog.nextPrimaryWP];
            cog.nextWPFeasibility2 = appdataCog.trkBands.wpFeasibility1[cog.nextPrimaryWP];
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
               cog.trafficSpeedConflict = true;
               if(!isinf(gs->resPreferred)){
                   cog.preferredSpeed = gs->resPreferred * fac;
                   cog.prevResSpeed = cog.preferredSpeed;
               }
               else{
                   cog.preferredSpeed = -10000;
               }
            }else{
               int id1 = cog.nextPrimaryWP;
               int id2 = cog.nextSecondaryWP;
               if (id1 >= appdataCog.flightplan1.num_waypoints)
                   id1 = appdataCog.flightplan1.num_waypoints-1;

               if (id2 >= appdataCog.flightplan2.num_waypoints)
                   id2 = appdataCog.flightplan2.num_waypoints-1;


               if(cog.Plan0 && gs->wpFeasibility1[id1]) {
                   cog.trafficSpeedConflict = false;
               }else if(cog.Plan1 && gs->wpFeasibility2[id2]){
                   cog.trafficSpeedConflict = false;
               }
               else {
                   if(cog.trafficSpeedConflict){
                      cog.trafficSpeedConflict = true;
                   }
               }
               cog.preferredSpeed = -1000;
            }
            break;
        }

        case ICAROUS_BANDS_ALT_MID:{
            bands_t* alt = (bands_t*) appdataCog.CogMsgPtr;

            memcpy(&appdataCog.altBands,alt,sizeof(bands_t));
            if(alt->currentConflictBand == 1){
               cog.trafficAltConflict = true;
               if(!isinf(alt->resPreferred))
                   cog.preferredAlt = alt->resPreferred;
               else
                   cog.preferredAlt = -10000;
            }else{
               cog.trafficAltConflict = false;
               cog.preferredAlt = -1000;
            }
            break;
        }

        case ICAROUS_BANDS_VS_MID:{
            bands_t* vspeed = (bands_t*)appdataCog.CogMsgPtr;
            memcpy(&appdataCog.vsBands,vspeed,sizeof(bands_t));
            cog.resVUp = vspeed->resUp;
            cog.resVDown = vspeed->resDown;
            cog.vsBandsNum = vspeed->numBands;
            break;
        }

		case ICAROUS_POSITION_MID:{
            position_t* pos = (position_t*) appdataCog.CogMsgPtr;
			memcpy(&appdataCog.position,pos,sizeof(position_t));			
            cog.speed = sqrt(pow(pos->vn,2) + pow(pos->ve,2));
            cog.position[0] = pos->latitude;
            cog.position[1] = pos->longitude;
            cog.position[2] = pos->altitude_rel;

            cog.velocity[0] = pos->vn;
            cog.velocity[1] = pos->ve;
            cog.velocity[2] = pos->vd;

            cog.hdg         = pos->hdg;
			break;
		}

        case ICAROUS_COMMANDS_MID:{
            argsCmd_t* cmd = (argsCmd_t*) appdataCog.CogMsgPtr;

            switch(cmd->name){
                case _TRAFFIC_RES_:{
                    cog.resolutionTypeCmd = cmd->param1;
                    break;
                }

                default:
                    break;
            }
            break;
        }

        case ICAROUS_STARTMISSION_MID:{
            argsCmd_t* msg = (argsCmd_t*) appdataCog.CogMsgPtr;
            cog.missionStart = (int)msg->param1;            
            cog.scenarioTime += msg->param2;
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
            cog.resolutionSpeed = fpm->resolutionSpeed;
            break;
        }

        case TRAFFIC_PARAMETERS_MID:{
            traffic_parameters_t* msg = (traffic_parameters_t*) appdataCog.CogMsgPtr;
            cog.DTHR = msg->det_1_WCV_DTHR;
            cog.ZTHR = msg->det_1_WCV_ZTHR;
            cog.resolutionTypeCmd = msg->resType;
            break;
        }

        case TRAJECTORY_PARAMETERS_MID:{
            trajectory_parameters_t* msg = (trajectory_parameters_t*) appdataCog.CogMsgPtr;
            cog.allowedXtrackDev1 = msg->xtrkDev;
            cog.xtrkGain = msg->xtrkGain;
            cog.resolutionSpeed = msg->resSpeed;
            cog.searchType = msg->searchAlgorithm;
            break;
        }

        case ICAROUS_WPREACHED_MID:{
            missionItemReached_t* msg = (missionItemReached_t*) appdataCog.CogMsgPtr;
            if(!msg->feedback){
                break;
            }

            if(strcmp(msg->planID,"Plan0") == 0){
                cog.nextPrimaryWP = msg->reachedwaypoint + 1;
                cog.nextWP = cog.nextPrimaryWP;
                strcpy(cog.currentPlanID,"Plan0");
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s, %d/%d\n",msg->planID,msg->reachedwaypoint,appdataCog.flightplan1.num_waypoints);

                cog.wpPrev1[0] =appdataCog.flightplan1.waypoints[cog.nextWP - 1].latitude;
                cog.wpPrev1[1] =appdataCog.flightplan1.waypoints[cog.nextWP - 1].longitude;
                cog.wpPrev1[2] =appdataCog.flightplan1.waypoints[cog.nextWP - 1].altitude;

                cog.wpNext1[0] =appdataCog.flightplan1.waypoints[cog.nextWP].latitude;
                cog.wpNext1[1] =appdataCog.flightplan1.waypoints[cog.nextWP].longitude;
                cog.wpNext1[2] =appdataCog.flightplan1.waypoints[cog.nextWP].altitude;

                if(appdataCog.flightplan1.waypoints[cog.nextWP].wp_metric == WP_METRIC_ETA){
                    cog.wpMetricTime = true;
                    cog.refWPTime = appdataCog.flightplan1.waypoints[cog.nextWP].value;
                }else{
                    cog.refWPTime = 0;
                    cog.wpMetricTime = false;
                }

            }else if(strcmp(msg->planID,"Plan1") == 0){
                cog.nextSecondaryWP = msg->reachedwaypoint + 1;
                cog.nextWP = cog.nextSecondaryWP;
                strcpy(cog.currentPlanID,"Plan1");
                if(cog.nextSecondaryWP >= appdataCog.flightplan2.num_waypoints)
                    cog.fp2complete = true;
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s, %d/%d\n",msg->planID,msg->reachedwaypoint,appdataCog.flightplan2.num_waypoints);

                cog.wpPrev2[0] =appdataCog.flightplan2.waypoints[cog.nextWP - 1].latitude;
                cog.wpPrev2[1] =appdataCog.flightplan2.waypoints[cog.nextWP - 1].longitude;
                cog.wpPrev2[2] =appdataCog.flightplan2.waypoints[cog.nextWP - 1].altitude;

                cog.wpNext2[0] =appdataCog.flightplan2.waypoints[cog.nextWP].latitude;
                cog.wpNext2[1] =appdataCog.flightplan2.waypoints[cog.nextWP].longitude;
                cog.wpNext2[2] =appdataCog.flightplan2.waypoints[cog.nextWP].altitude;




            }else if(strcmp(msg->planID,"P2P") == 0){
                cog.p2pcomplete = true;
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s\n",msg->planID);
            }else if(strcmp(msg->planID,"Takeoff") == 0){
                cog.takeoffComplete = msg->reachedwaypoint;
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
            cog.request = REQUEST_RESPONDED;
            cog.num_waypoints2 = fp->num_waypoints;
            break;
        }

        case ICAROUS_GEOFENCE_MONITOR_MID:{
            geofenceConflict_t* gfConflct = (geofenceConflict_t*) appdataCog.CogMsgPtr;
            if(gfConflct->numConflicts > 0 ) {
                if (gfConflct->conflictTypes[0] == _KEEPIN_)
                    cog.keepInConflict = true;
                else if (gfConflct->conflictTypes[0] == _KEEPOUT_)
                    cog.keepOutConflict = true;
            }else{
                cog.keepOutConflict = false;
                cog.keepInConflict = false;
            }

            memcpy(cog.recoveryPosition,gfConflct->recoveryPosition, sizeof(double)*3);

            int totalWP;
            totalWP = appdataCog.flightplan1.num_waypoints;
            for (int i = cog.nextPrimaryWP; i < totalWP; ++i)
            {
                if (!gfConflct->waypointConflict1[i])
                {
                    // Check to see that the next waypoint is not too close when
                    // dealing with a traffic conflict
                    if(cog.resolutionTypeCmd == 4 && cog.trafficTrackConflict){
                        double nextWP[3] = {appdataCog.flightplan1.waypoints[i].latitude,
                                            appdataCog.flightplan1.waypoints[i].longitude,
                                            appdataCog.flightplan1.waypoints[i].altitude};
                        double dist = ComputeDistance(cog.position,nextWP);
                        // Consider a waypoint feasible if its greater than the 3*DTHR values.
                        // Note DTHR is in ft. Convert from ft to m before comparing with dist.
                        if (dist > 3 * (cog.DTHR/3)){
                            cog.nextFeasibleWP1 = i;
                            cog.nextPrimaryWP = cog.nextFeasibleWP1;
                            break;
                        }
                    }else{
                        cog.nextFeasibleWP1 = i;
                        break;
                    }
                }
            }
            cog.directPathToFeasibleWP1 = gfConflct->directPathToWaypoint1[cog.nextFeasibleWP1] && appdataCog.trkBands.wpFeasibility1[cog.nextFeasibleWP1];
            totalWP = appdataCog.flightplan2.num_waypoints;
            for (int i = cog.nextSecondaryWP; i < totalWP; ++i)
            {
                if (!gfConflct->waypointConflict2[i])
                {
                    cog.nextFeasibleWP2 = i;
                    break;
                }
            }
            cog.directPathToFeasibleWP2 = gfConflct->directPathToWaypoint2[cog.nextFeasibleWP2] && appdataCog.trkBands.wpFeasibility2[cog.nextFeasibleWP2];
            cog.wpNextFb1[0] = appdataCog.flightplan1.waypoints[cog.nextFeasibleWP1].latitude;
            cog.wpNextFb1[1] = appdataCog.flightplan1.waypoints[cog.nextFeasibleWP1].longitude;
            cog.wpNextFb1[2] = appdataCog.flightplan1.waypoints[cog.nextFeasibleWP1].altitude;

            cog.wpNextFb2[0] = appdataCog.flightplan2.waypoints[cog.nextFeasibleWP2].latitude;
            cog.wpNextFb2[1] = appdataCog.flightplan2.waypoints[cog.nextFeasibleWP2].longitude;
            cog.wpNextFb2[2] = appdataCog.flightplan2.waypoints[cog.nextFeasibleWP2].altitude;
            break;
        }

        #ifdef APPDEF_MERGER
        case MERGER_STATUS_MID:{
            argsCmd_t* cmd = (argsCmd_t*) appdataCog.CogMsgPtr;
            cog.mergingActive = (int) cmd->param1;
            break;
        }
        #endif

        #ifdef APPDEF_SAFE2DITCH
        case SAFE2DITCH_STATUS_MID:{
            OS_printf("Received ditch status\n");
            safe2ditchStatus_t* status = (safe2ditchStatus_t*) appdataCog.CogMsgPtr;
            cog.ditch = status->ditchRequested;
            memcpy(cog.ditchsite,status->ditchsite,sizeof(double)*3);
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
    if(cog.sendCommand){
        switch(cog.guidanceCommand){
            case PRIMARY_FLIGHTPLAN:
            case SECONDARY_FLIGHTPLAN:{
                COGNITION_SendGuidanceFlightPlan(); 
                break;
            }

            case VECTOR:{
                COGNITION_SendGuidanceVelCmd();
                break;
            }

            case POINT2POINT:{
                COGNITION_SendGuidanceP2P();
                break;
            }

            case SPEED_CHANGE:{
                COGNITION_SendSpeedChange();
                break;
            }

            case TAKEOFF:{
                COGNITION_SendTakeoff();
                break;
            }

            case LAND:{
                COGNITION_SendLand();
                break;
            }
        }
        cog.sendCommand = false;
    }

    if(cog.sendStatusTxt){
        SetStatus(appdataCog.statustxt,cog.statusBuf,cog.statusSeverity);
        cog.sendStatusTxt = false;
    }

    if(cog.pathRequest){
        COGNITION_FindNewPath();
        cog.pathRequest = false;
    }
}

void COGNITION_SendGuidanceVelCmd(){
      argsCmd_t cmd;
      CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(argsCmd_t),TRUE);
      cmd.name = VECTOR;
      cmd.param1 = (float) cog.cmdparams[0];
      cmd.param2 = (float) cog.cmdparams[1];
      cmd.param3 = (float) cog.cmdparams[2];
      SendSBMsg(cmd);
}

void COGNITION_SendGuidanceFlightPlan(){
  argsCmd_t cmd;
  CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(argsCmd_t),TRUE);
  if(cog.Plan0){
     cmd.name = PRIMARY_FLIGHTPLAN;
     cmd.param1 = cog.nextPrimaryWP;
  }else if(cog.Plan1){
     cmd.name = SECONDARY_FLIGHTPLAN;
     cmd.param1 = cog.nextSecondaryWP;

     missionItemReached_t itemReached;
     CFE_SB_InitMsg(&itemReached,ICAROUS_WPREACHED_MID,sizeof(itemReached),TRUE);
     itemReached.feedback = false;
     strcpy(itemReached.planID,"Plan0");
     itemReached.reachedwaypoint = cog.nextFeasibleWP1-1;
     SendSBMsg(itemReached);
  }
  cmd.param2 =  cog.nextFeasibleWP1; 
  SendSBMsg(cmd);
}

void COGNITION_SendGuidanceP2P(){
   //Send Goto position
   argsCmd_t guidanceCmd; CFE_SB_InitMsg(&guidanceCmd, GUIDANCE_COMMAND_MID, sizeof(argsCmd_t), TRUE);
   guidanceCmd.name = POINT2POINT;
   guidanceCmd.param1 = cog.cmdparams[0];
   guidanceCmd.param2 = cog.cmdparams[1];
   guidanceCmd.param3 = cog.cmdparams[2];
   guidanceCmd.param4 = cog.cmdparams[3]; 
   SendSBMsg(guidanceCmd);
}

void COGNITION_SendSpeedChange(){
    // Send speed change command
    argsCmd_t cmd;
    CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(argsCmd_t),TRUE);
    cmd.name = SPEED_CHANGE;
    cmd.param1 = cog.cmdparams[0];
    SendSBMsg(cmd);
}

void COGNITION_SendTakeoff(){
    argsCmd_t cmd;
    CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(cmd),TRUE);
    cmd.name = TAKEOFF;
    SendSBMsg(cmd);
}

void COGNITION_SendLand(){
    argsCmd_t cmd;
    CFE_SB_InitMsg(&cmd, GUIDANCE_COMMAND_MID, sizeof(cmd), TRUE);
    cmd.name = LAND;
    SendSBMsg(cmd);
}

void COGNITION_FindNewPath(){
   trajectory_request_t pathRequest;
   CFE_SB_InitMsg(&pathRequest, ICAROUS_TRAJECTORY_REQUEST_MID, sizeof(trajectory_request_t), TRUE);

   pathRequest.algorithm = cog.searchType;

   memcpy(pathRequest.initialPosition, cog.startPosition, sizeof(double) * 3);
   memcpy(pathRequest.initialVelocity, cog.startVelocity, sizeof(double) * 3);
   memcpy(pathRequest.finalPosition, cog.stopPosition, sizeof(double) * 3);

   SendSBMsg(pathRequest);
}
