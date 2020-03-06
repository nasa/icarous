//
// Created by Swee Balachandran on 4/17/2019.
//
#define EXTERN
#include "guidance.h"
#include "guidance_tbl.c"

CFE_EVS_BinFilter_t  guidance_EventFilters[] =
{  /* Event ID    mask */
        { GUIDANCE_STARTUP_INF_EID,       0x0000},
        { GUIDANCE_COMMAND_ERR_EID,       0x0000},
}; /// Event ID definitions

/* Application entry points */
void GUIDANCE_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    GUIDANCE_AppInit();
    
    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        status = CFE_SB_RcvMsg(&guidanceAppData.guidance_MsgPtr, guidanceAppData.guidance_Pipe, CFE_SB_PEND_FOREVER);

        if (status == CFE_SUCCESS)
        {
		    GUIDANCE_ProcessPacket();
        }
    }

    GUIDANCE_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void GUIDANCE_AppInit(void) {

    memset(&guidanceAppData, 0, sizeof(guidanceAppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(guidance_EventFilters,
                     sizeof(guidance_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe(&guidanceAppData.guidance_Pipe, /* Variable to hold Pipe ID */
		    GUIDANCE_PIPE_DEPTH,       /* Depth of Pipe */
		    GUIDANCE_PIPE_NAME);       /* Name of pipe */

    //Subscribe to plexil output messages from the SB
    CFE_SB_SubscribeLocal(ICAROUS_POSITION_MID,guidanceAppData.guidance_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(FREQ_30_WAKEUP_MID,guidanceAppData.guidance_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_FLIGHTPLAN_MID,guidanceAppData.guidance_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_TRAJECTORY_MID,guidanceAppData.guidance_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(GUIDANCE_COMMAND_MID,guidanceAppData.guidance_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(GUIDANCE_PARAMETERS_MID,guidanceAppData.guidance_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);

    // Register table with table services
    CFE_TBL_Handle_t tblHandle;
    status = CFE_TBL_Register(&tblHandle,
                              "GuidanceTable",
                              sizeof(guidanceTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &GuidanceTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(tblHandle,CFE_TBL_SRC_ADDRESS,&guidance_TblStruct);

    guidanceTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr, tblHandle);

    memcpy(&guidanceAppData.guidance_tbl,TblPtr,sizeof(guidanceTable_t));

    GUIDANCE_AppInitData();


    if(status == CFE_SUCCESS){
        // Send event indicating app initialization
        CFE_EVS_SendEvent(GUIDANCE_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                        "guidance App Initialized. Version %d.%d",
                        GUIDANCE_MAJOR_VERSION,
                        GUIDANCE_MINOR_VERSION);
    }
}

void GUIDANCE_AppInitData(){

    guidanceAppData.guidanceMode = NOOP;
    guidanceAppData.nextPrimaryWP = 1;

    CFE_SB_InitMsg(&guidanceAppData.velCmd,ICAROUS_COMMANDS_MID,sizeof(argsCmd_t),TRUE);
    guidanceAppData.velCmd.name = _SETVEL_;
    guidanceAppData.refSpeed = guidanceAppData.guidance_tbl.defaultWpSpeed;
    CFE_SB_InitMsg(&guidanceAppData.statustxt,ICAROUS_STATUS_MID,sizeof(status_t),TRUE);

}

void GUIDANCE_AppCleanUp(){
    // Do clean up here
}

void GUIDANCE_ProcessPacket(){
    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(guidanceAppData.guidance_MsgPtr);

    switch(MsgId){
        case ICAROUS_POSITION_MID:{
            position_t *msg = (position_t*) guidanceAppData.guidance_MsgPtr;
            if(msg->aircraft_id == CFE_PSP_GetSpacecraftId()){
                memcpy(&guidanceAppData.position,msg,sizeof(position_t));
            }
            break;
        }

        case ICAROUS_FLIGHTPLAN_MID:{
            flightplan_t *msg = (flightplan_t*)guidanceAppData.guidance_MsgPtr;
            memcpy(&guidanceAppData.primaryFlightPlan,msg,sizeof(flightplan_t));
            //OS_printf("Received primary flight plan %s\n",msg->id);
            break;
        }

        
        case ICAROUS_TRAJECTORY_MID:{
            flightplan_t *msg = (flightplan_t*)guidanceAppData.guidance_MsgPtr;
            memcpy(&guidanceAppData.secondaryFlightPlan,msg,sizeof(flightplan_t));
            break;
        }

        case GUIDANCE_COMMAND_MID:{
            argsCmd_t* msg = (argsCmd_t*)guidanceAppData.guidance_MsgPtr;
            HandleGuidanceCommands(msg);
            break;
        }

        case FREQ_30_WAKEUP_MID:{
            GUIDANCE_Run();
            break;
        }

        case GUIDANCE_PARAMETERS_MID:{
            guidance_parameters_t* msg = (guidance_parameters_t*) guidanceAppData.guidance_MsgPtr;

            double climbAngle = msg->climbAngle;
            if (climbAngle >= 75){
                climbAngle = 75;
            } else if (climbAngle <= 30){
                climbAngle = 30;
            }

            guidanceAppData.guidance_tbl.defaultWpSpeed = msg->defaultWpSpeed;
            guidanceAppData.guidance_tbl.captureRadiusScaling = msg->captureRadiusScaling;
            guidanceAppData.guidance_tbl.climbFpAngle = climbAngle;
            guidanceAppData.guidance_tbl.climbAngleVRange = msg->climbAngleVRange;
            guidanceAppData.guidance_tbl.climbAngleHRange = msg->climbAngleHRange;
            guidanceAppData.guidance_tbl.climbRateGain = msg->climbRateGain;
            guidanceAppData.guidance_tbl.maxClimbRate = msg->maxClimbRate;
            guidanceAppData.guidance_tbl.minClimbRate = msg->minClimbRate;
            guidanceAppData.guidance_tbl.yawForward = msg->yawForward;

            guidanceAppData.refSpeed = guidanceAppData.guidance_tbl.defaultWpSpeed;
            break;
        }
    }
}

void HandleGuidanceCommands(argsCmd_t *cmd){
    //OS_printf("received guidance command %d\n",(int)cmd->name);
    switch((int)cmd->name){
        case PRIMARY_FLIGHTPLAN:{
            guidanceAppData.reachedStatusUpdated = false;
            guidanceAppData.guidanceMode = PRIMARY_FLIGHTPLAN;
            guidanceAppData.nextPrimaryWP = (int)cmd->param1;
            //OS_printf("received primary waypoint following command to %d\n",(int)cmd->param1);
            char buffer[100];
            memset(buffer,0,sizeof(char)*100);
            sprintf(buffer,"Following primary flightplan to waypoint %d",(int)cmd->param1);
            SetStatus(guidanceAppData.statustxt,buffer,SEVERITY_INFO);
            break;
        }

        case SECONDARY_FLIGHTPLAN:{
            guidanceAppData.reachedStatusUpdated = false;
            guidanceAppData.guidanceMode = SECONDARY_FLIGHTPLAN;
            guidanceAppData.nextSecondaryWP = (int)cmd->param1;
            SetStatus(guidanceAppData.statustxt,"Following secondary flightplan",SEVERITY_INFO);
            break;
        }

        case VECTOR:{
            guidanceAppData.guidanceMode = VECTOR;
            guidanceAppData.velCmd.param1 = cmd->param1;
            guidanceAppData.velCmd.param2 = cmd->param2;
            guidanceAppData.velCmd.param3 = cmd->param3;
            guidanceAppData.velCmd.param4 = guidanceAppData.guidance_tbl.yawForward;
            break;
        }
        
        case POINT2POINT:{
            guidanceAppData.guidanceMode = POINT2POINT;
            guidanceAppData.reachedStatusUpdated = false;
            guidanceAppData.point[0] = cmd->param1;
            guidanceAppData.point[1] = cmd->param2;
            guidanceAppData.point[2] = cmd->param3; 
            guidanceAppData.pointSpeed = cmd->param4;
            SetStatus(guidanceAppData.statustxt,"Point to point control",SEVERITY_INFO);
            break;
        }

        case TAKEOFF:{
            //OS_printf("Received takeoff command in guidance app\n");
            argsCmd_t cmd1;              
            CFE_SB_InitMsg(&cmd1,ICAROUS_COMMANDS_MID,sizeof(argsCmd_t),TRUE);
            cmd1.name = _TAKEOFF_;
            cmd1.param1 = guidanceAppData.primaryFlightPlan.waypoints[1].altitude;
            SendSBMsg(cmd1);
            guidanceAppData.takeoffComplete = false;
            guidanceAppData.guidanceMode = TAKEOFF;
            break;
        }

        case LAND:{
            argsCmd_t cmd1;
            CFE_SB_InitMsg(&cmd1,ICAROUS_COMMANDS_MID,sizeof(argsCmd_t),TRUE);
            cmd1.name = _LAND_;
            guidanceAppData.guidanceMode = POINT2POINT;
            int wp = guidanceAppData.primaryFlightPlan.num_waypoints;
            cmd1.param5 = guidanceAppData.primaryFlightPlan.waypoints[wp-1].latitude;
            cmd1.param6 = guidanceAppData.primaryFlightPlan.waypoints[wp-1].longitude;
            cmd1.param7 = guidanceAppData.primaryFlightPlan.waypoints[wp-1].altitude;
            SendSBMsg(cmd1);
            break;
        }

        case SPEED_CHANGE:{
            guidanceAppData.refSpeed = cmd->param1;
            break;
        }

    }
}

void GUIDANCE_Run(){
    double position[3] = {guidanceAppData.position.latitude,
                            guidanceAppData.position.longitude,
                            guidanceAppData.position.altitude_rel};
    double velCmd[3] = {guidanceAppData.velCmd.param1,
                        guidanceAppData.velCmd.param2,
                        guidanceAppData.velCmd.param3};

    switch(guidanceAppData.guidanceMode){
        case NOOP:{
            break;
        }

        case PRIMARY_FLIGHTPLAN:{
            int nextWP  = ComputeFlightplanGuidanceInput(&guidanceAppData.primaryFlightPlan,
                                                        position,
                                                        guidanceAppData.nextPrimaryWP,
                                                        &guidanceAppData.reachedStatusUpdated,
                                                        &guidanceAppData.guidance_tbl,
                                                        velCmd,
                                                        &guidanceAppData.refSpeed);
            HandleFlightplanGuidance(&guidanceAppData.primaryFlightPlan, velCmd, guidanceAppData.nextPrimaryWP, nextWP);
            guidanceAppData.nextPrimaryWP = nextWP;
            break;
        }

        case SECONDARY_FLIGHTPLAN:{
            int nextWP  = ComputeFlightplanGuidanceInput(&guidanceAppData.secondaryFlightPlan,
                                                        position,
                                                        guidanceAppData.nextSecondaryWP,
                                                        &guidanceAppData.reachedStatusUpdated,
                                                        &guidanceAppData.guidance_tbl,
                                                        velCmd,
                                                        &guidanceAppData.refSpeed);
            HandleFlightplanGuidance(&guidanceAppData.secondaryFlightPlan, velCmd, guidanceAppData.nextSecondaryWP, nextWP);
            guidanceAppData.nextSecondaryWP = nextWP;

            break;
        }

        case POINT2POINT:{
            flightplan_t fp;
            fp.num_waypoints = 2;
            memset(fp.id,0,sizeof(char)*20);
            strcpy(fp.id,"P2P");
            // Starting position is the first waypoint
            fp.waypoints[0].latitude = guidanceAppData.position.latitude;
            fp.waypoints[0].longitude = guidanceAppData.position.longitude;
            fp.waypoints[0].altitude = guidanceAppData.position.altitude_abs;

            // Target position is second waypoint
            fp.waypoints[1].latitude = guidanceAppData.point[0];
            fp.waypoints[1].longitude = guidanceAppData.point[1];
            fp.waypoints[1].altitude = guidanceAppData.point[2];

            fp.waypoints[0].wp_metric = WP_METRIC_SPEED;
            fp.waypoints[0].value = guidanceAppData.pointSpeed;
            int nextWP  = ComputeFlightplanGuidanceInput(&fp,
                                                        position,
                                                        1,
                                                        &guidanceAppData.reachedStatusUpdated,
                                                        &guidanceAppData.guidance_tbl,
                                                        velCmd,
                                                        &guidanceAppData.refSpeed);
            HandleFlightplanGuidance(&fp, velCmd, 1, nextWP);

            /*
            bool status = Point2PointControl(position,
                                             guidanceAppData.point,
                                             guidanceAppData.pointSpeed,
                                             &guidanceAppData.guidance_tbl,
                                             velCmd); 
                                             
            if(status){
                missionItemReached_t wpReached;
                CFE_SB_InitMsg(&wpReached, ICAROUS_WPREACHED_MID, sizeof(wpReached), TRUE);
                strcpy(wpReached.planID, "P2P\0");
                wpReached.reachedwaypoint = 1;
                wpReached.feedback = true;
                SendSBMsg(wpReached);
                SetStatus(guidanceAppData.statustxt,"Reached position",SEVERITY_INFO);
                guidanceAppData.guidanceMode = NOOP;
            }
            SendVelocityCommand(velCmd);
            */
            break;
        }

        case VECTOR:{
            SendSBMsg(guidanceAppData.velCmd);
            break;
        }

        case ORBIT:{
            //TODO:
            break;
        }

        case HELIX:{
            //TODO:
            break;
        }

        case TAKEOFF:{
            ComputeTakeoffGuidanceInput();
            break;
        }

        case LAND:{
            //TODO:
            break;
        }

        case SPEED_CHANGE:{
            break;
        }

    }

    PublishGuidanceStatus();
}

void HandleFlightplanGuidance(flightplan_t* fp, double velCmd[3], int nextWP, int newNextWP){
    bool reachedWP = (nextWP != newNextWP);
    if(reachedWP){
        missionItemReached_t wpReached;
        CFE_SB_InitMsg(&wpReached, ICAROUS_WPREACHED_MID, sizeof(wpReached), TRUE);
        strcpy(wpReached.planID, fp->id);
        wpReached.reachedwaypoint = nextWP;
        wpReached.feedback = true;
        SendSBMsg(wpReached);
    }

    SendVelocityCommand(velCmd);
}

void SendVelocityCommand(double velCmd[3]){
    guidanceAppData.velCmd.param1 = velCmd[0];
    guidanceAppData.velCmd.param2 = velCmd[1];
    guidanceAppData.velCmd.param3 = velCmd[2];
    guidanceAppData.velCmd.param4 = guidanceAppData.guidance_tbl.yawForward;
    SendSBMsg(guidanceAppData.velCmd);
}

void ComputeTakeoffGuidanceInput(){
   if(!guidanceAppData.takeoffComplete){
       // Send the takeoff command and let the autopilot interfaces takeoff
       // care of the various operations involved in takeoff
       argsCmd_t cmd;
       CFE_SB_InitMsg(&cmd, ICAROUS_COMMANDS_MID, sizeof(cmd), TRUE);
       cmd.name = _TAKEOFF_;
       cmd.param1 = 10;
       SendSBMsg(cmd);
       guidanceAppData.takeoffComplete = true;
   }
   else
   {
   }
}

void PublishGuidanceStatus(){

    guidance_status_t status;
    CFE_SB_InitMsg(&status,GUIDANCE_STATUS_MID,sizeof(guidance_status_t),TRUE);
    status.mode = guidanceAppData.guidanceMode;
    if(status.mode == PRIMARY_FLIGHTPLAN){
        status.nextWP = guidanceAppData.nextPrimaryWP;
        status.totalWP = guidanceAppData.primaryFlightPlan.num_waypoints;
    }
    else{
        status.nextWP = guidanceAppData.nextSecondaryWP;
        status.totalWP = guidanceAppData.secondaryFlightPlan.num_waypoints;
    }

    status.velCmd[0] = guidanceAppData.velCmd.param1;
    status.velCmd[1] = guidanceAppData.velCmd.param2;
    status.velCmd[2] = guidanceAppData.velCmd.param3;
    SendSBMsg(status);
}

int32_t GuidanceTableValidationFunc(void *TblPtr){
    return 0;
}
