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

void COGNITION_AppInitData(){
    appdataCog.cog.returnSafe = true;
    appdataCog.cog.nextPrimaryWP = 1;
    appdataCog.cog.resolutionTypeCmd = -1;
    appdataCog.cog.request = REQUEST_NIL;
    appdataCog.cog.fpPhase = IDLE_PHASE;
    appdataCog.cog.missionStart = -1;
    appdataCog.cog.keepInConflict = false;
    appdataCog.cog.keepOutConflict = false;
    appdataCog.cog.p2pcomplete = false;
    appdataCog.cog.takeoffComplete = -1;

    appdataCog.cog.trafficConflictState = NOOPC;
    appdataCog.cog.geofenceConflictState = NOOPC;
    appdataCog.cog.trafficTrackConflict = false;
    appdataCog.cog.trafficSpeedConflict = false;
    appdataCog.cog.trafficAltConflict = false;
    appdataCog.cog.XtrackConflictState = NOOPC;
    appdataCog.cog.resolutionTypeCmd = TRACK_RESOLUTION;
    appdataCog.cog.requestGuidance2NextWP = -1;
    appdataCog.cog.searchType = _ASTAR;
    appdataCog.cog.topofdescent = false;
    appdataCog.cog.ditch = false;
    appdataCog.cog.endDitch = false;
    appdataCog.cog.resetDitch = false;
    appdataCog.cog.primaryFPReceived = false;
    appdataCog.cog.mergingActive = 0;

    appdataCog.cog.nextWPFeasibility1 = 1;
    appdataCog.cog.nextWPFeasibility2 = 1;
    appdataCog.cog.emergencyDescentState = SUCCESS;
    CFE_SB_InitMsg(&appdataCog.statustxt,ICAROUS_STATUS_MID,sizeof(status_t),TRUE);
    memset(appdataCog.cog.ditchsite,0,sizeof(double)*3);
}


void COGNITION_ProcessSBData() {


    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(appdataCog.CogMsgPtr);
    switch (MsgId)
    {
        case ICAROUS_FLIGHTPLAN_MID:{
            flightplan_t* fplan = (flightplan_t*)appdataCog.CogMsgPtr;
            memcpy(&appdataCog.flightplan1,fplan,sizeof(flightplan_t));
            appdataCog.cog.primaryFPReceived = true;

            appdataCog.cog.wpPrev1[0] =fplan->waypoints[0].latitude;
            appdataCog.cog.wpPrev1[1] =fplan->waypoints[0].longitude;
            appdataCog.cog.wpPrev1[2] =fplan->waypoints[0].altitude;

            appdataCog.cog.wpNext1[0] =fplan->waypoints[1].latitude;
            appdataCog.cog.wpNext1[1] =fplan->waypoints[1].longitude;
            appdataCog.cog.wpNext1[2] =fplan->waypoints[1].altitude;
            appdataCog.cog.num_waypoints = fplan->num_waypoints;
            appdataCog.cog.scenarioTime = fplan->scenario_time;

            if(fplan->waypoints[1].wp_metric == WP_METRIC_ETA){
                appdataCog.cog.wpMetricTime = true;
                appdataCog.cog.refWPTime = fplan->waypoints[1].value;
            }else{
                appdataCog.cog.wpMetricTime = false;
                appdataCog.cog.refWPTime = 0;
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
               appdataCog.cog.trafficTrackConflict = true;
               if(trk->resPreferred >= 0)
                   appdataCog.cog.preferredTrack = trk->resPreferred;
               else
                   appdataCog.cog.preferredTrack = -1000;
            }else{
               appdataCog.cog.trafficTrackConflict = false;
               appdataCog.cog.preferredTrack = -10000;
            }

            memcpy(appdataCog.cog.trkBandType,appdataCog.trkBands.type,sizeof(int)*20);
            memcpy(appdataCog.cog.trkBandMin,appdataCog.trkBands.min,sizeof(double)*20);
            memcpy(appdataCog.cog.trkBandMax,appdataCog.trkBands.max,sizeof(double)*20);
            appdataCog.cog.trkBandNum = appdataCog.trkBands.numBands;
            appdataCog.cog.nextWPFeasibility1 = appdataCog.trkBands.wpFeasibility1[appdataCog.cog.nextPrimaryWP];
            appdataCog.cog.nextWPFeasibility2 = appdataCog.trkBands.wpFeasibility1[appdataCog.cog.nextPrimaryWP];
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
               appdataCog.cog.trafficSpeedConflict = true;
               if(!isinf(gs->resPreferred) && !isnan(gs->resPreferred)){
                   appdataCog.cog.preferredSpeed = gs->resPreferred * fac;
                   appdataCog.cog.prevResSpeed = appdataCog.cog.preferredSpeed;
               }
               else{
                   appdataCog.cog.preferredSpeed = -10000;
               }
            }else{
               int id1 = appdataCog.cog.nextPrimaryWP;
               int id2 = appdataCog.cog.nextSecondaryWP;
               if (id1 >= appdataCog.flightplan1.num_waypoints)
                   id1 = appdataCog.flightplan1.num_waypoints-1;

               if (id2 >= appdataCog.flightplan2.num_waypoints)
                   id2 = appdataCog.flightplan2.num_waypoints-1;


               if(appdataCog.cog.Plan0 && gs->wpFeasibility1[id1]) {
                   appdataCog.cog.trafficSpeedConflict = false;
               }else if(appdataCog.cog.Plan1 && gs->wpFeasibility2[id2]){
                   appdataCog.cog.trafficSpeedConflict = false;
               }
               else {
                   if(appdataCog.cog.trafficSpeedConflict){
                      appdataCog.cog.trafficSpeedConflict = true;
                   }
               }
               appdataCog.cog.preferredSpeed = -1000;
            }
            break;
        }

        case ICAROUS_BANDS_ALT_MID:{
            bands_t* alt = (bands_t*) appdataCog.CogMsgPtr;

            memcpy(&appdataCog.altBands,alt,sizeof(bands_t));
            if(alt->currentConflictBand == 1){
               appdataCog.cog.trafficAltConflict = true;
               if(!isinf(alt->resPreferred))
                   appdataCog.cog.preferredAlt = alt->resPreferred;
               else
                   appdataCog.cog.preferredAlt = -10000;
            }else{
               appdataCog.cog.trafficAltConflict = false;
               appdataCog.cog.preferredAlt = -1000;
            }
            break;
        }

        case ICAROUS_BANDS_VS_MID:{
            bands_t* vspeed = (bands_t*)appdataCog.CogMsgPtr;
            memcpy(&appdataCog.vsBands,vspeed,sizeof(bands_t));
            appdataCog.cog.resVUp = vspeed->resUp;
            appdataCog.cog.resVDown = vspeed->resDown;
            appdataCog.cog.vsBandsNum = vspeed->numBands;
            break;
        }

		case ICAROUS_POSITION_MID:{
            position_t* pos = (position_t*) appdataCog.CogMsgPtr;
			memcpy(&appdataCog.position,pos,sizeof(position_t));			
            appdataCog.cog.speed = sqrt(pow(pos->vn,2) + pow(pos->ve,2));
            appdataCog.cog.position[0] = pos->latitude;
            appdataCog.cog.position[1] = pos->longitude;
            appdataCog.cog.position[2] = pos->altitude_rel;

            appdataCog.cog.velocity[0] = pos->vn;
            appdataCog.cog.velocity[1] = pos->ve;
            appdataCog.cog.velocity[2] = pos->vd;

            appdataCog.cog.hdg         = pos->hdg;
			break;
		}

        case ICAROUS_COMMANDS_MID:{
            argsCmd_t* cmd = (argsCmd_t*) appdataCog.CogMsgPtr;

            switch(cmd->name){
                case _TRAFFIC_RES_:{
                    appdataCog.cog.resolutionTypeCmd = cmd->param1;
                    break;
                }

                default:
                    break;
            }
            break;
        }

        case ICAROUS_STARTMISSION_MID:{
            argsCmd_t* msg = (argsCmd_t*) appdataCog.CogMsgPtr;
            appdataCog.cog.missionStart = (int)msg->param1;
            appdataCog.cog.scenarioTime += msg->param2;
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
            appdataCog.cog.resolutionSpeed = fpm->resolutionSpeed;
            break;
        }

        case TRAFFIC_PARAMETERS_MID:{
            traffic_parameters_t* msg = (traffic_parameters_t*) appdataCog.CogMsgPtr;
            appdataCog.cog.DTHR = msg->det_1_WCV_DTHR;
            appdataCog.cog.ZTHR = msg->det_1_WCV_ZTHR;
            appdataCog.cog.resolutionTypeCmd = msg->resType;
            break;
        }

        case TRAJECTORY_PARAMETERS_MID:{
            trajectory_parameters_t* msg = (trajectory_parameters_t*) appdataCog.CogMsgPtr;
            appdataCog.cog.allowedXtrackDev1 = msg->xtrkDev;
            appdataCog.cog.xtrkGain = msg->xtrkGain;
            appdataCog.cog.resolutionSpeed = msg->resSpeed;
            appdataCog.cog.searchType = msg->searchAlgorithm;
            break;
        }

        case ICAROUS_WPREACHED_MID:{
            missionItemReached_t* msg = (missionItemReached_t*) appdataCog.CogMsgPtr;
            if(!msg->feedback){
                break;
            }

            if(strcmp(msg->planID,"Plan0") == 0){
                appdataCog.cog.nextPrimaryWP = msg->reachedwaypoint + 1;
                appdataCog.cog.nextWP = appdataCog.cog.nextPrimaryWP;
                strcpy(appdataCog.cog.currentPlanID,"Plan0");
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s, %d/%d\n",msg->planID,msg->reachedwaypoint,appdataCog.flightplan1.num_waypoints);

                appdataCog.cog.wpPrev1[0] =appdataCog.flightplan1.waypoints[appdataCog.cog.nextWP - 1].latitude;
                appdataCog.cog.wpPrev1[1] =appdataCog.flightplan1.waypoints[appdataCog.cog.nextWP - 1].longitude;
                appdataCog.cog.wpPrev1[2] =appdataCog.flightplan1.waypoints[appdataCog.cog.nextWP - 1].altitude;

                appdataCog.cog.wpNext1[0] =appdataCog.flightplan1.waypoints[appdataCog.cog.nextWP].latitude;
                appdataCog.cog.wpNext1[1] =appdataCog.flightplan1.waypoints[appdataCog.cog.nextWP].longitude;
                appdataCog.cog.wpNext1[2] =appdataCog.flightplan1.waypoints[appdataCog.cog.nextWP].altitude;

                if(appdataCog.flightplan1.waypoints[appdataCog.cog.nextWP].wp_metric == WP_METRIC_ETA){
                    appdataCog.cog.wpMetricTime = true;
                    appdataCog.cog.refWPTime = appdataCog.flightplan1.waypoints[appdataCog.cog.nextWP].value;
                }else{
                    appdataCog.cog.refWPTime = 0;
                    appdataCog.cog.wpMetricTime = false;
                }

            }else if(strcmp(msg->planID,"Plan1") == 0){
                appdataCog.cog.nextSecondaryWP = msg->reachedwaypoint + 1;
                appdataCog.cog.nextWP = appdataCog.cog.nextSecondaryWP;
                strcpy(appdataCog.cog.currentPlanID,"Plan1");
                if(appdataCog.cog.nextSecondaryWP >= appdataCog.flightplan2.num_waypoints)
                    appdataCog.cog.fp2complete = true;
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s, %d/%d\n",msg->planID,msg->reachedwaypoint,appdataCog.flightplan2.num_waypoints);

                appdataCog.cog.wpPrev2[0] =appdataCog.flightplan2.waypoints[appdataCog.cog.nextWP - 1].latitude;
                appdataCog.cog.wpPrev2[1] =appdataCog.flightplan2.waypoints[appdataCog.cog.nextWP - 1].longitude;
                appdataCog.cog.wpPrev2[2] =appdataCog.flightplan2.waypoints[appdataCog.cog.nextWP - 1].altitude;

                appdataCog.cog.wpNext2[0] =appdataCog.flightplan2.waypoints[appdataCog.cog.nextWP].latitude;
                appdataCog.cog.wpNext2[1] =appdataCog.flightplan2.waypoints[appdataCog.cog.nextWP].longitude;
                appdataCog.cog.wpNext2[2] =appdataCog.flightplan2.waypoints[appdataCog.cog.nextWP].altitude;




            }else if(strcmp(msg->planID,"P2P") == 0){
                appdataCog.cog.p2pcomplete = true;
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s\n",msg->planID);
            }else if(strcmp(msg->planID,"Takeoff") == 0){
                appdataCog.cog.takeoffComplete = msg->reachedwaypoint;
                CFE_ES_WriteToSysLog("Cognition:Received wp reached %s\n",msg->planID);
            }else if(strcmp(msg->planID,"PlanM") == 0){
                appdataCog.cog.nextSecondaryWP = msg->reachedwaypoint + 1;
                if(appdataCog.cog.nextSecondaryWP >= appdataCog.cog.num_waypoints2){
                    appdataCog.cog.mergingActive = 2; 
                }
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
            appdataCog.cog.request = REQUEST_RESPONDED;
            appdataCog.cog.num_waypoints2 = fp->num_waypoints;
            break;
        }

        case ICAROUS_GEOFENCE_MONITOR_MID:{
            geofenceConflict_t* gfConflct = (geofenceConflict_t*) appdataCog.CogMsgPtr;
            if(gfConflct->numConflicts > 0 ) {
                if (gfConflct->conflictTypes[0] == _KEEPIN_)
                    appdataCog.cog.keepInConflict = true;
                else if (gfConflct->conflictTypes[0] == _KEEPOUT_)
                    appdataCog.cog.keepOutConflict = true;
            }else{
                appdataCog.cog.keepOutConflict = false;
                appdataCog.cog.keepInConflict = false;
            }

            memcpy(appdataCog.cog.recoveryPosition,gfConflct->recoveryPosition, sizeof(double)*3);

            int totalWP;
            totalWP = appdataCog.flightplan1.num_waypoints;
            for (int i = appdataCog.cog.nextPrimaryWP; i < totalWP; ++i)
            {
                if (!gfConflct->waypointConflict1[i])
                {
                    // Check to see that the next waypoint is not too close when
                    // dealing with a traffic conflict
                    if(appdataCog.cog.resolutionTypeCmd == 4 && appdataCog.cog.trafficTrackConflict){
                        double nextWP[3] = {appdataCog.flightplan1.waypoints[i].latitude,
                                            appdataCog.flightplan1.waypoints[i].longitude,
                                            appdataCog.flightplan1.waypoints[i].altitude};
                        double dist = ComputeDistance(appdataCog.cog.position,nextWP);
                        // Consider a waypoint feasible if its greater than the 3*DTHR values.
                        // Note DTHR is in ft. Convert from ft to m before comparing with dist.
                        if (dist > 3 * (appdataCog.cog.DTHR/3)){
                            appdataCog.cog.nextFeasibleWP1 = i;
                            appdataCog.cog.nextPrimaryWP = appdataCog.cog.nextFeasibleWP1;
                            break;
                        }
                    }else{
                        appdataCog.cog.nextFeasibleWP1 = i;
                        break;
                    }
                }
            }
            appdataCog.cog.directPathToFeasibleWP1 = gfConflct->directPathToWaypoint1[appdataCog.cog.nextFeasibleWP1] && appdataCog.trkBands.wpFeasibility1[appdataCog.cog.nextFeasibleWP1];
            totalWP = appdataCog.flightplan2.num_waypoints;
            for (int i = appdataCog.cog.nextSecondaryWP; i < totalWP; ++i)
            {
                if (!gfConflct->waypointConflict2[i])
                {
                    appdataCog.cog.nextFeasibleWP2 = i;
                    break;
                }
            }
            appdataCog.cog.directPathToFeasibleWP2 = gfConflct->directPathToWaypoint2[appdataCog.cog.nextFeasibleWP2] && appdataCog.trkBands.wpFeasibility2[appdataCog.cog.nextFeasibleWP2];
            appdataCog.cog.wpNextFb1[0] = appdataCog.flightplan1.waypoints[appdataCog.cog.nextFeasibleWP1].latitude;
            appdataCog.cog.wpNextFb1[1] = appdataCog.flightplan1.waypoints[appdataCog.cog.nextFeasibleWP1].longitude;
            appdataCog.cog.wpNextFb1[2] = appdataCog.flightplan1.waypoints[appdataCog.cog.nextFeasibleWP1].altitude;

            appdataCog.cog.wpNextFb2[0] = appdataCog.flightplan2.waypoints[appdataCog.cog.nextFeasibleWP2].latitude;
            appdataCog.cog.wpNextFb2[1] = appdataCog.flightplan2.waypoints[appdataCog.cog.nextFeasibleWP2].longitude;
            appdataCog.cog.wpNextFb2[2] = appdataCog.flightplan2.waypoints[appdataCog.cog.nextFeasibleWP2].altitude;
            break;
        }

        #ifdef APPDEF_MERGER
        case MERGER_STATUS_MID:{
            argsCmd_t* cmd = (argsCmd_t*) appdataCog.CogMsgPtr;
            appdataCog.cog.mergingActive = (int) cmd->param1;
            break;
        }
        #endif

        #ifdef APPDEF_SAFE2DITCH
        case SAFE2DITCH_STATUS_MID:{
            OS_printf("Received ditch status\n");
            safe2ditchStatus_t* status = (safe2ditchStatus_t*) appdataCog.CogMsgPtr;
            appdataCog.cog.ditch = status->ditchRequested;
            memcpy(appdataCog.cog.ditchsite,status->ditchsite,sizeof(double)*3);
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
    FlightPhases(&appdataCog.cog);
    if(appdataCog.cog.sendCommand){
        switch(appdataCog.cog.guidanceCommand){
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
        appdataCog.cog.sendCommand = false;
    }

    if(appdataCog.cog.sendStatusTxt){
        SetStatus(appdataCog.statustxt,appdataCog.cog.statusBuf,appdataCog.cog.statusSeverity);
        appdataCog.cog.sendStatusTxt = false;
    }

    if(appdataCog.cog.pathRequest){
        COGNITION_FindNewPath();
        appdataCog.cog.pathRequest = false;
    }

    if(appdataCog.cog.emergencyDescentState == NOOPS){
       COGNITION_SendDitchRequest();
    }
}

void COGNITION_SendGuidanceVelCmd(){
      argsCmd_t cmd;
      CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(argsCmd_t),TRUE);
      cmd.name = VECTOR;
      cmd.param1 = (float) appdataCog.cog.cmdparams[0];
      cmd.param2 = (float) appdataCog.cog.cmdparams[1];
      cmd.param3 = (float) appdataCog.cog.cmdparams[2];
      SendSBMsg(cmd);
}

void COGNITION_SendGuidanceFlightPlan(){
  argsCmd_t cmd;
  CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(argsCmd_t),TRUE);
  if(appdataCog.cog.Plan0){
     cmd.name = PRIMARY_FLIGHTPLAN;
     cmd.param1 = appdataCog.cog.nextPrimaryWP;
  }else if(appdataCog.cog.Plan1){
     cmd.name = SECONDARY_FLIGHTPLAN;
     cmd.param1 = appdataCog.cog.nextSecondaryWP;

     missionItemReached_t itemReached;
     CFE_SB_InitMsg(&itemReached,ICAROUS_WPREACHED_MID,sizeof(itemReached),TRUE);
     itemReached.feedback = false;
     strcpy(itemReached.planID,"Plan0");
     itemReached.reachedwaypoint = appdataCog.cog.nextFeasibleWP1-1;
     SendSBMsg(itemReached);
  }
  cmd.param2 =  appdataCog.cog.nextFeasibleWP1; 
  SendSBMsg(cmd);
}

void COGNITION_SendGuidanceP2P(){
   //Send Goto position
   argsCmd_t guidanceCmd; CFE_SB_InitMsg(&guidanceCmd, GUIDANCE_COMMAND_MID, sizeof(argsCmd_t), TRUE);
   guidanceCmd.name = POINT2POINT;
   guidanceCmd.param1 = appdataCog.cog.cmdparams[0];
   guidanceCmd.param2 = appdataCog.cog.cmdparams[1];
   guidanceCmd.param3 = appdataCog.cog.cmdparams[2];
   guidanceCmd.param4 = appdataCog.cog.cmdparams[3]; 
   SendSBMsg(guidanceCmd);
}

void COGNITION_SendSpeedChange(){
    // Send speed change command
    argsCmd_t cmd;
    CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(argsCmd_t),TRUE);
    cmd.name = SPEED_CHANGE;
    cmd.param1 = appdataCog.cog.cmdparams[0];
    cmd.param2 = appdataCog.cog.cmdparams[1];
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

   pathRequest.algorithm = appdataCog.cog.searchType;

   memcpy(pathRequest.initialPosition, appdataCog.cog.startPosition, sizeof(double) * 3);
   memcpy(pathRequest.initialVelocity, appdataCog.cog.startVelocity, sizeof(double) * 3);
   memcpy(pathRequest.finalPosition, appdataCog.cog.stopPosition, sizeof(double) * 3);

   SendSBMsg(pathRequest);
}

void COGNITION_SendDitchRequest(){
   noArgsCmd_t cmd;
   CFE_SB_InitMsg(&cmd,ICAROUS_DITCH_MID,sizeof(noArgsCmd_t),TRUE);
   cmd.name = _DITCH_;
   SendSBMsg(cmd);
}
