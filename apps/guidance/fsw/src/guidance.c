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
                              sizeof(GuidanceParams_t),
                              CFE_TBL_OPT_DEFAULT,
                              &GuidanceTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(tblHandle,CFE_TBL_SRC_ADDRESS,&guidance_TblStruct);

    guidanceTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr, tblHandle);

    memcpy(&guidanceAppData.guidance_params,TblPtr,sizeof(guidanceTable_t));

    GUIDANCE_AppInitData();


    if(status == CFE_SUCCESS){
        // Send event indicating app initialization
        CFE_EVS_SendEvent(GUIDANCE_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                        "guidance App Initialized. Version %d.%d",
                        GUIDANCE_MAJOR_VERSION,
                        GUIDANCE_MINOR_VERSION);
    }
}

void GUIDANCE_AppInitData(void){

    guidanceAppData.Guidance = InitGuidance((GuidanceParams_t*)&guidanceAppData.guidance_params);
    guidanceAppData.takeoffComplete = false;
}

void GUIDANCE_AppCleanUp(void){
    // Do clean up here
}

void GUIDANCE_ProcessPacket(void){
    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(guidanceAppData.guidance_MsgPtr);

    switch(MsgId){
        case ICAROUS_POSITION_MID:{
            position_t *position = (position_t*) guidanceAppData.guidance_MsgPtr;
            memcpy(&guidanceAppData.pos,position,sizeof(position_t));
            if(position->aircraft_id == CFE_PSP_GetSpacecraftId()){
                double posLLA[3] = {position->latitude,
                                    position->longitude,
                                    position->altitude_rel}; 
                double TrkGsVs[3] = {0.0,0.0,0.0};
                ConvertVnedToTrkGsVs(position->vn,position->ve, position->vd,
                                     TrkGsVs, TrkGsVs+1, TrkGsVs+2);
                guidSetAircraftState(guidanceAppData.Guidance,posLLA,TrkGsVs);
            }
            break;
        }

        case ICAROUS_FLIGHTPLAN_MID:
        case ICAROUS_TRAJECTORY_MID:{
            flightplan_t *fp = (flightplan_t*)guidanceAppData.guidance_MsgPtr;
            for (int i=0;i<fp->num_waypoints;++i){
                double wp[3] = {fp->waypoints[i].latitude,
                                fp->waypoints[i].longitude,
                                fp->waypoints[i].altitude};
                bool eta = fp->waypoints[i].wp_metric == WP_METRIC_ETA?true:false;
                double value = fp->waypoints[i].value;
                guidInputFlightplanData(guidanceAppData.Guidance,
                                    fp->id,fp->scenario_time,i,wp,eta,value);
            }
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

            guidanceAppData.guidance_params.defaultWpSpeed = msg->defaultWpSpeed;
            guidanceAppData.guidance_params.captureRadiusScaling = msg->captureRadiusScaling;
            guidanceAppData.guidance_params.guidanceRadiusScaling = msg->guidanceRadiusScaling;
            guidanceAppData.guidance_params.xtrkDev = msg->xtrkDev;
            guidanceAppData.guidance_params.climbFpAngle = climbAngle;
            guidanceAppData.guidance_params.climbAngleVRange = msg->climbAngleVRange;
            guidanceAppData.guidance_params.climbAngleHRange = msg->climbAngleHRange;
            guidanceAppData.guidance_params.climbRateGain = msg->climbRateGain;
            guidanceAppData.guidance_params.maxClimbRate = msg->maxClimbRate;
            guidanceAppData.guidance_params.minClimbRate = msg->minClimbRate;
            guidanceAppData.guidance_params.maxCap = msg->maxCap;
            guidanceAppData.guidance_params.minCap = msg->minCap;
            guidanceAppData.guidance_params.maxSpeed = msg->maxSpeed;
            guidanceAppData.guidance_params.minSpeed = msg->minSpeed;
            guidanceAppData.guidance_params.yawForward = msg->yawForward;
            guidSetParams(guidanceAppData.Guidance,&guidanceAppData.guidance_params);
            break;
        }
    }
}

void HandleGuidanceCommands(argsCmd_t *cmd){
    switch((int)cmd->name){
        case FLIGHTPLAN:{
            strcpy(guidanceAppData.activePlan,cmd->buffer);
            SetGuidanceMode(guidanceAppData.Guidance,(int)cmd->name,cmd->buffer,(int)cmd->param1);
            char buffer[100];
            memset(buffer,0,sizeof(char)*100);
            sprintf(buffer,"Following flightplan %s to waypoint %d",cmd->buffer,(int)cmd->param1);
            SetStatus(guidanceAppData.statustxt,buffer,SEVERITY_INFO);
            break;
        }

        case VECTOR:{
            double trkgsvs[3] = {0.0,0.0,0.0};  
            ConvertVnedToTrkGsVs(cmd->param1,cmd->param2,cmd->param3,trkgsvs,trkgsvs+1,trkgsvs+2);
            guidInputVelocityCmd(guidanceAppData.Guidance,trkgsvs);
            char name[] = "";
            SetGuidanceMode(guidanceAppData.Guidance,(int)cmd->name,name,0);
            break;
        }
        
        case POINT2POINT:{
            double pointA[3] = {guidanceAppData.pos.latitude,
                                  guidanceAppData.pos.longitude,
                                  guidanceAppData.pos.altitude_abs};
            double pointB[3] = {cmd->param1,cmd->param2,cmd->param3};
            double speed = cmd->param4;
            char name[] = "P2P";
            for(int i=0;i<2;++i){
                if (i == 0)
                    guidInputFlightplanData(guidanceAppData.Guidance,name,0,i,pointA,false,speed);
                else
                    guidInputFlightplanData(guidanceAppData.Guidance,name,0,i,pointB,false,speed);

            }

            SetGuidanceMode(guidanceAppData.Guidance,(int)cmd->name,name,1);
            SetStatus(guidanceAppData.statustxt,"Point to point control",SEVERITY_INFO);
            break;
        }

        case TAKEOFF:{
            //OS_printf("Received takeoff command in guidance app\n");
            ComputeTakeoffGuidanceInput();
            missionItemReached_t msg;
            CFE_SB_InitMsg(&msg,ICAROUS_WPREACHED_MID,sizeof(missionItemReached_t),TRUE);
            msg.feedback = true;
            strcpy(msg.planID,"Takeoff");
            msg.reachedwaypoint = 0;
            SendSBMsg(msg);
            break;
        }

        case LAND:{

            SetGuidanceMode(guidanceAppData.Guidance,(int)cmd->name,(char*)"LAND",0);
            argsCmd_t cmd1;
            CFE_SB_InitMsg(&cmd1,ICAROUS_COMMANDS_MID,sizeof(argsCmd_t),TRUE);
            cmd1.name = _LAND_;
            SendSBMsg(cmd1);
            break;
        }

        case SPEED_CHANGE:{
            char planID[25];
            strcpy(planID,cmd->buffer);
            double speed = cmd->param1;
            bool hold = cmd->param2==1?true:false;
            ChangeWaypointSpeed(guidanceAppData.Guidance,planID,0,speed,hold);
            break;
        }

    }
}

void GUIDANCE_Run(void){

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME,&ts);
    double time = ts.tv_sec + (double)(ts.tv_nsec)/1E9;
    RunGuidance(guidanceAppData.Guidance,time);

    GuidanceOutput_t guidOutput;
    guidGetOutput(guidanceAppData.Guidance,&guidOutput);

    if(guidOutput.guidanceMode == TAKEOFF){
        return;
    }

    if (guidOutput.guidanceMode == FLIGHTPLAN ||
        guidOutput.guidanceMode == POINT2POINT) {
        if (guidOutput.nextWP >= 0 && guidOutput.wpReached)
        {
            missionItemReached_t msg;
            CFE_SB_InitMsg(&msg, ICAROUS_WPREACHED_MID, sizeof(missionItemReached_t), TRUE);
            msg.feedback = true;
            strcpy(msg.planID, guidOutput.activePlan);
            msg.reachedwaypoint = guidOutput.nextWP - 1;
            SendSBMsg(msg);

            argsCmd_t speedChange;
            CFE_SB_InitMsg(&speedChange, ICAROUS_COMMANDS_MID, sizeof(argsCmd_t), TRUE);
            speedChange.name = _SETSPEED_;
            speedChange.param1 = guidanceAppData.guidance_params.defaultWpSpeed;
            SendSBMsg(speedChange);
        }
    }

    if (guidOutput.guidanceMode != GUIDE_NOOP){
        argsCmd_t cmd;
        CFE_SB_InitMsg(&cmd, ICAROUS_COMMANDS_MID, sizeof(argsCmd_t), TRUE);
        cmd.name = _SETVEL_;
        double vn,ve,vd;
        ConvertTrkGsVsToVned(guidOutput.velCmd[0],guidOutput.velCmd[1],
                             guidOutput.velCmd[2],&vn,&ve,&vd);
        cmd.param1 = vn;
        cmd.param2 = ve;
        cmd.param3 = vd;
        cmd.param4 = guidanceAppData.guidance_params.yawForward;
        SendSBMsg(cmd);
    }
}

void ComputeTakeoffGuidanceInput(void){
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
}


int32_t GuidanceTableValidationFunc(void *TblPtr){
    return 0;
}