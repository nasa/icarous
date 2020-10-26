//
// Created by Swee Balachandran on 12/22/17.
//
#include <time.h>
#include <math.h>
#include "trajectory.h"
#include "UtilFunctions.h"
#include "trajectory_tbl.h"
#include "trajectory_tbl.c"

CFE_EVS_BinFilter_t TRAJECTORY_EventFilters[] =
    {/* Event ID    mask */
     {TRAJECTORY_STARTUP_INF_EID, 0x0000},
     {TRAJECTORY_COMMAND_ERR_EID, 0x0000},
     {TRAJECTORY_RECEIVED_INTRUDER_EID, 0x0000}}; /// Event ID definitions

/* Application entry points */
void TRAJECTORY_AppMain(void)
{

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    TRAJECTORY_AppInit();

    status = CFE_ES_CreateChildTask(&traj_monitor_task_id, "Traj Monitor", TRAJECTORY_Monitor, traj_monitor_stack, TRAJ_MONITOR_TASK_STACK_SIZE, TRAJ_MONITOR_PRIORITY, 0);
    if (status != OS_SUCCESS)
    {
        OS_printf("Error creating TRAJECTORY_Monitor task \n");
    }

    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        
        status = CFE_SB_RcvMsg(&TrajectoryAppData.TrajRequest_MsgPtr, TrajectoryAppData.TrajRequest_Pipe, CFE_SB_PEND_FOREVER);

        if (status == CFE_SUCCESS)
        {
            struct timespec ts;
            timespec_get(&ts,TIME_UTC);
            TrajectoryAppData.timeNow = ts.tv_sec + (double)(ts.tv_nsec)/1E9;
            TRAJECTORY_ProcessPacket();
        }
    }

    TrajectoryAppData.runThreads = CFE_ES_RunLoop(&RunStatus);

    TRAJECTORY_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void TRAJECTORY_AppInit(void)
{

    memset(&TrajectoryAppData, 0, sizeof(TrajectoryAppData_t));
    TrajectoryAppData.runThreads = 1;

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(TRAJECTORY_EventFilters,
                     sizeof(TRAJECTORY_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe(&TrajectoryAppData.TrajRequest_Pipe, /* Variable to hold Pipe ID */
                               TRAJECTORY_PIPE_DEPTH,               /* Depth of Pipe */
                               TRAJECTORY_PIPE_NAME);               /* Name of pipe */

    status = CFE_SB_CreatePipe(&TrajectoryAppData.TrajData_Pipe, /* Variable to hold Pipe ID */
                               TRAJECTORY_SCH_PIPE_DEPTH,        /* Depth of Pipe */
                               TRAJECTORY_SCH_PIPE_NAME);        /* Name of pipe */

    //Subscribe to plexil output messages from the SB

    CFE_SB_SubscribeLocal(FREQ_30_WAKEUP_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_POSITION_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_FLIGHTPLAN_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_GEOFENCE_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_RESET_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_RESETFP_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_TRAFFIC_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_TRAJECTORY_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_WPREACHED_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(TRAFFIC_PARAMETERS_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_STARTMISSION_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(TRAJECTORY_PARAMETERS_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(GUIDANCE_COMMAND_MID,TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_TRAJECTORY_REQUEST_MID, TrajectoryAppData.TrajRequest_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(EUTL1_TRAJECTORY_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);

    // Register table with table services
    status = CFE_TBL_Register(&TrajectoryAppData.Trajectory_TblHandle,
                              "TrajectoryTable",
                              sizeof(TrajectoryTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &TrajectoryTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(TrajectoryAppData.Trajectory_TblHandle, CFE_TBL_SRC_ADDRESS, &Trajectory_TblStruct);

    TrajectoryTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void **)&TblPtr, TrajectoryAppData.Trajectory_TblHandle);

    TRAJECTORY_AppInitData(TblPtr);

    // Send event indicating app initialization
    if(status == CFE_SUCCESS){
        CFE_EVS_SendEvent(TRAJECTORY_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                         "TRAJECTORY App Initialized. Version %d.%d",
                         TRAJECTORY_MAJOR_VERSION,
                         TRAJECTORY_MINOR_VERSION);
    }

}

void TRAJECTORY_AppInitData(TrajectoryTable_t* TblPtr){

    char callsign[30];
    memset(callsign,0,30);
    sprintf(callsign,"aircraft%d",CFE_PSP_GetSpacecraftId());
    TrajectoryAppData.pplanner = new_TrajManager(callsign);
        

    TrajectoryAppData.flightplan2.num_waypoints = 0;
    TrajectoryAppData.numPlansComputed = 0;
    TrajectoryAppData.nextWP1 = 0;
    TrajectoryAppData.nextWP2 = 1;
    TrajectoryAppData.eutlReceived = false;
    strcpy(TrajectoryAppData.planID, "Plan0\0");

    int32 status = OS_MutSemCreate(&TrajectoryAppData.mutexAcState, "ACstate", 0);
    if (status != OS_SUCCESS)
        OS_printf("Error creating ACState mutex\n");

}

void TRAJECTORY_AppCleanUp(void)
{
    // Do clean up here
}

void TRAJECTORY_ProcessPacket(void)
{

    CFE_SB_MsgId_t MsgId;
    MsgId = CFE_SB_GetMsgId(TrajectoryAppData.TrajRequest_MsgPtr);

    switch (MsgId)
    {
        case ICAROUS_TRAJECTORY_REQUEST_MID:
        {
            trajectory_request_t *msg;
            msg = (trajectory_request_t *)TrajectoryAppData.TrajRequest_MsgPtr;
            TrajectoryAppData.numPlansComputed++;

            // Plan
            char planID[25];
            strcpy(planID,msg->planID);
            int val = TrajManager_FindPath(TrajectoryAppData.pplanner, planID, msg->initialPosition, msg->finalPosition, msg->initialVelocity, msg->finalVelocity);

            if(val <= 0){
                OS_printf("*** No path found ***\n");
            }else{
                
                TrajManager_SetPlanOffset(TrajectoryAppData.pplanner,planID,0,TrajectoryAppData.timeNow);

                flightplan_t result;
                CFE_SB_InitMsg(&result, ICAROUS_TRAJECTORY_MID, sizeof(flightplan_t), TRUE);
                memcpy(result.id, planID, 10);
                result.num_waypoints = (uint16_t)TrajManager_GetTotalWaypoints(TrajectoryAppData.pplanner, planID);
                for (int i = 0; i < result.num_waypoints; ++i)
                {
                    TrajManager_GetWaypoint(TrajectoryAppData.pplanner, planID, i, result.waypoints+i);
                    if (i >= MAX_WAYPOINTS)
                    {
                        OS_printf("Trajectory: more than MAX_WAYPOINTS");
                        break;
                    }
                }

                SendSBMsg(result);

                memcpy(&TrajectoryAppData.flightplan2, &result, sizeof(flightplan_t));

                
                char missionPlan[] = "Plan0";
                TrajManager_CombinePlan(TrajectoryAppData.pplanner, planID, missionPlan, -1);


                // Publish EUTIL trajectory information
                char buffer[MAX_DATABUFFER_SIZE] = {0};
                char combinePlanId[] = "Plan+";
                TrajManager_PlanToString(TrajectoryAppData.pplanner, combinePlanId, buffer, true, (int)TrajectoryAppData.timeNow);

                CFE_SB_ZeroCopyHandle_t cpyhandle;
                stringdata_t *bigdataptr = (stringdata_t *)CFE_SB_ZeroCopyGetPtr(sizeof(stringdata_t), &cpyhandle);
                CFE_SB_InitMsg(bigdataptr, EUTL2_TRAJECTORY_MID, sizeof(stringdata_t), TRUE);
                strcpy(bigdataptr->buffer, buffer);
                CFE_SB_TimeStampMsg((CFE_SB_Msg_t *)bigdataptr);
                int32 status = CFE_SB_ZeroCopySend((CFE_SB_Msg_t *)bigdataptr, cpyhandle);
                if (status != CFE_SUCCESS)
                {
                    OS_printf("Error sending EUTL trajectory\n");
                }
            }
            break;
        }
    }
    return;
}

void TRAJECTORY_Monitor(void)
{
    OS_TaskRegister();
    while (TrajectoryAppData.runThreads)
    {
        int32 status = CFE_SB_RcvMsg(&TrajectoryAppData.Traj_MsgPtr, TrajectoryAppData.TrajData_Pipe, CFE_SB_PEND_FOREVER);
        struct timespec ts;
        timespec_get(&ts,TIME_UTC);
        TrajectoryAppData.timeNow = ts.tv_sec + (double)(ts.tv_nsec)/1E9;
        if (status == CFE_SUCCESS)
        {
            CFE_SB_MsgId_t MsgId;
            MsgId = CFE_SB_GetMsgId(TrajectoryAppData.Traj_MsgPtr);
            switch (MsgId)
            {

            case ICAROUS_FLIGHTPLAN_MID:
            {
                if(TrajectoryAppData.eutlReceived){
                    break;
                }

                flightplan_t *fp;
                fp = (flightplan_t *)TrajectoryAppData.Traj_MsgPtr;
                TrajManager_ClearAllPlans(TrajectoryAppData.pplanner);
                memcpy(&TrajectoryAppData.flightplan1, fp, sizeof(flightplan_t));
                waypoint_t wp[MAX_WAYPOINTS];
                for (int i = 0; i < fp->num_waypoints; ++i)
                {
                    wp[i] = fp->waypoints[i];
                } 
                char name[] = "Plan0";
                TrajManager_InputFlightPlan(TrajectoryAppData.pplanner, name, wp, fp->num_waypoints,0,false,0);
                OS_MutSemTake(TrajectoryAppData.mutexAcState);
                TrajectoryAppData.monitor = true;
                OS_MutSemGive(TrajectoryAppData.mutexAcState);
                CFE_ES_WriteToSysLog("Trajectory:Received flight plan with %d waypoints\n", fp->num_waypoints);
                TrajectoryAppData.nextWP1 = 1;
                break;
            }

            case EUTL1_TRAJECTORY_MID:{
                TrajectoryAppData.eutlReceived = true;
                stringdata_t *strdata = (stringdata_t*)TrajectoryAppData.Traj_MsgPtr;
                char buffer[MAX_DATABUFFER_SIZE] = {0};    // TODO: Read this from the  message
                strcpy(buffer,strdata->buffer);
                char planID[] = "Plan0";
                TrajManager_StringToPlan(TrajectoryAppData.pplanner,planID,buffer);  

                flightplan_t fp;
                CFE_SB_InitMsg(&fp,ICAROUS_FLIGHTPLAN_MID,sizeof(flightplan_t),TRUE);
                fp.num_waypoints = (uint16_t)TrajManager_GetTotalWaypoints(TrajectoryAppData.pplanner, planID);
                strcpy(fp.id,"Plan0");
                for (int i = 0; i < fp.num_waypoints; ++i)
                {
                    waypoint_t wp;
                    TrajManager_GetWaypoint(TrajectoryAppData.pplanner, planID, i, &wp);


                    if (i >= MAX_WAYPOINTS)
                    {
                        OS_printf("Trajectory: more than MAX_WAYPOINTS");
                        break;
                    }

                    fp.waypoints[i] = wp; 
                }
                memcpy(&TrajectoryAppData.flightplan1, &fp, sizeof(flightplan_t));
                TrajectoryAppData.monitor = true;
                SendSBMsg(fp);
                break;
            }

            case ICAROUS_GEOFENCE_MID:
            {
                geofence_t *gf;
                gf = (geofence_t *)TrajectoryAppData.Traj_MsgPtr;

                double vertices[MAX_VERTICES][2];

                for (int i = 0; i < gf->totalvertices; ++i)
                {
                    vertices[i][0] = gf->vertices[i][0];
                    vertices[i][1] = gf->vertices[i][1];
                }

                TrajManager_InputGeofenceData(TrajectoryAppData.pplanner, gf->type, gf->index, gf->totalvertices, gf->floor, gf->ceiling, vertices);

                break;
            }

            case ICAROUS_POSITION_MID:
            {

                position_t *pos;
                pos = (position_t *)TrajectoryAppData.Traj_MsgPtr;

                if (pos->aircraft_id != CFE_PSP_GetSpacecraftId())
                {
                    double _pos[3] = {pos->latitude, pos->longitude, pos->altitude_rel};
                    double trkGsVs[3] = {0.0,0.0,0.0};
                    ConvertVnedToTrkGsVs(pos->vn,pos->ve,pos->vd,trkGsVs,trkGsVs+1,trkGsVs+2);
                    TrajManager_InputTraffic(TrajectoryAppData.pplanner, pos->aircraft_id, _pos, trkGsVs, TrajectoryAppData.timeNow);
                }
                else
                {
                    OS_MutSemTake(TrajectoryAppData.mutexAcState);
                    TrajectoryAppData.position[0] = pos->latitude;
                    TrajectoryAppData.position[1] = pos->longitude;
                    TrajectoryAppData.position[2] = pos->altitude_rel;

                    double trk, gs, vs;
                    ConvertVnedToTrkGsVs(pos->vn, pos->ve, pos->vd, &trk, &gs, &vs);

                    TrajectoryAppData.velocity[0] = trk;
                    TrajectoryAppData.velocity[1] = gs;
                    TrajectoryAppData.velocity[2] = vs;
                    OS_MutSemGive(TrajectoryAppData.mutexAcState);
                }

                break;
            }

            case ICAROUS_TRAFFIC_MID:
            {
                object_t *msg;
                msg = (object_t *)TrajectoryAppData.Traj_MsgPtr;

                double pos[3] = {msg->latitude, msg->longitude, msg->altitude};
                double trkGsVs[3] = {0.0,0.0,0.0};
                ConvertVnedToTrkGsVs(msg->vn,msg->ve,msg->vd,trkGsVs,trkGsVs+1,trkGsVs+2);
                TrajManager_InputTraffic(TrajectoryAppData.pplanner, msg->index, pos, trkGsVs, TrajectoryAppData.timeNow);
                break;
            }

            case ICAROUS_WPREACHED_MID:
            {
                missionItemReached_t *msg;
                msg = (missionItemReached_t *)TrajectoryAppData.Traj_MsgPtr;
                OS_MutSemTake(TrajectoryAppData.mutexAcState);
                if (strcmp(msg->planID, "Plan0") == 0)
                {
                    TrajectoryAppData.nextWP1 = msg->reachedwaypoint + 1;
                }else
                {
                    TrajectoryAppData.nextWP2 = msg->reachedwaypoint + 1;
                }
                OS_MutSemGive(TrajectoryAppData.mutexAcState);
                break;
            }

            case ICAROUS_RESET_MID:
            {
                TrajManager_ClearAllPlans(TrajectoryAppData.pplanner);
                break;
            }

            case ICAROUS_RESETFP_MID:
            {
                TrajManager_ClearAllPlans(TrajectoryAppData.pplanner);
                break;
            }

            case ICAROUS_STARTMISSION_MID:{
                double diff = TrajectoryAppData.timeNow - TrajectoryAppData.flightplan1.waypoints[0].time;
                TrajManager_SetPlanOffset(TrajectoryAppData.pplanner,(char*)"Plan0",0,diff);
                break;
            }

            case FREQ_30_WAKEUP_MID:
            {

                if(TrajectoryAppData.nextWP1 == 0){
                    break;
                } 
                double position[3];
                double velocity[3];
                int nextWP;

                OS_MutSemTake(TrajectoryAppData.mutexAcState);
                memcpy(position, TrajectoryAppData.position, sizeof(double) * 3);
                memcpy(velocity, TrajectoryAppData.velocity, sizeof(double) * 3);
                OS_MutSemGive(TrajectoryAppData.mutexAcState);
                if(strcmp(TrajectoryAppData.planID,"Plan0") == 0){
                    nextWP = TrajectoryAppData.nextWP1;
                }else{
                    nextWP = TrajectoryAppData.nextWP2;
                }
                cfsTrajectoryMonitorData_t tjMonData;
                CFE_SB_InitMsg(&tjMonData, FLIGHTPLAN_MONITOR_MID, sizeof(flightplan_t), TRUE);
                trajectoryMonitorData_t monData;
                monData = TrajManager_MonitorTrajectory(TrajectoryAppData.pplanner,TrajectoryAppData.timeNow,TrajectoryAppData.planID,position,velocity,TrajectoryAppData.nextWP1,nextWP);
                memcpy(tjMonData.databuffer,(char*)&monData,sizeof(trajectoryMonitorData_t));
                SendSBMsg(tjMonData);

                break;
            }

            case TRAJECTORY_PARAMETERS_MID:
            {
                trajectory_parameters_t *tjparams = (trajectory_parameters_t*)TrajectoryAppData.Traj_MsgPtr; 
                TrajManager_UpdateDubinsPlannerParameters(TrajectoryAppData.pplanner,&tjparams->dbparams);
                break;
            }

            case GUIDANCE_COMMAND_MID:{
                argsCmd_t* cmd = (argsCmd_t*) TrajectoryAppData.Traj_MsgPtr;
                if((int) cmd->name == 0){
                    if(strcmp(cmd->buffer,"Plan0") == 0){
                        TrajectoryAppData.nextWP1 = (int)cmd->param1;
                    }else{
                        TrajectoryAppData.nextWP2 = (int)cmd->param1;
                    }
                    strcpy(TrajectoryAppData.planID,cmd->buffer);
                }
                break;
            }

            default:
                break;
            }
        }
    }
}

int32_t TrajectoryTableValidationFunc(void *TblPtr)
{
    return 0;
}
