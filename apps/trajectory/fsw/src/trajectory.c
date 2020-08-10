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
    CFE_SB_SubscribeLocal(TRAJECTORY_PARAMETERS_MID, TrajectoryAppData.TrajData_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
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
    TrajectoryAppData.pplanner = new_PathPlanner(callsign);
    PathPlanner_InitializeAstarParameters(TrajectoryAppData.pplanner,
                                          TblPtr->astar_enable3D,
                                          TblPtr->astar_gridSize,
                                          TblPtr->astar_resSpeed,
                                          TblPtr->astar_lookahead,
                                          TblPtr->astar_daaConfigFile);

    PathPlanner_InitializeRRTParameters(TrajectoryAppData.pplanner,
                                        TblPtr->rrt_resSpeed,
                                        TblPtr->rrt_numIterations,
                                        TblPtr->rrt_dt,
                                        TblPtr->rrt_macroSteps,
                                        TblPtr->rrt_capR,
                                        TblPtr->rrt_daaConfigFile);

    TrajectoryAppData.flightplan2.num_waypoints = 0;
    TrajectoryAppData.numPlansComputed = 0;
    TrajectoryAppData.nextWP1 = 1;
    TrajectoryAppData.nextWP2 = 1;
    TrajectoryAppData.xtrkDev = TblPtr->xtrkDev;
    TrajectoryAppData.xtrkGain = TblPtr->xtrkGain;
    TrajectoryAppData.resSpeed = TblPtr->resSpeed;
    TrajectoryAppData.searchType = TblPtr->searchAlgorithm;
    TrajectoryAppData.updateDAAParams = TblPtr->updateDaaParams;
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
            //sprintf(planID,"Plan%d",TrajectoryAppData.numPlansComputed);
            strcpy(planID,msg->planID);
            int val = PathPlanner_FindPath(TrajectoryAppData.pplanner, TrajectoryAppData.searchType, planID, msg->initialPosition, msg->finalPosition, msg->initialVelocity);

            if(val <= 0){
                OS_printf("*** No path found ***\n");
            }else{
                flightplan_t result;
                CFE_SB_InitMsg(&result, ICAROUS_TRAJECTORY_MID, sizeof(flightplan_t), TRUE);
                memcpy(result.id, planID, 10);
                result.num_waypoints = (uint16_t)PathPlanner_GetTotalWaypoints(TrajectoryAppData.pplanner, planID);
                for (int i = 0; i < result.num_waypoints; ++i)
                {
                    double wp[4];
                    PathPlanner_GetWaypoint(TrajectoryAppData.pplanner, planID, i, wp);
                    if (i >= MAX_WAYPOINTS)
                    {
                        OS_printf("Trajectory: more than MAX_WAYPOINTS");
                        break;
                    }
                    result.waypoints[i].wp_metric = WP_METRIC_SPEED;
                    result.waypoints[i].value = TrajectoryAppData.trajParams.resSpeed;
                    result.waypoints[i].latitude = (float)wp[0];
                    result.waypoints[i].longitude = (float)wp[1];
                    result.waypoints[i].altitude = (float)wp[2];
                }

                SendSBMsg(result);

                memcpy(&TrajectoryAppData.flightplan2, &result, sizeof(flightplan_t));

                // Check that the nextWP is the destination for the new plan.
                // If not increment nextWP until it is the destination WP
                int destinationWP = TrajectoryAppData.nextWP1;
                while (destinationWP < TrajectoryAppData.flightplan1.num_waypoints)
                {
                    double wp[3] = {TrajectoryAppData.flightplan1.waypoints[destinationWP].latitude,
                                    TrajectoryAppData.flightplan1.waypoints[destinationWP].longitude,
                                    TrajectoryAppData.flightplan1.waypoints[destinationWP].altitude};
                    if ((fabs(wp[0] - msg->finalPosition[0]) < 1e-8) &&
                        (fabs(wp[1] - msg->finalPosition[1]) < 1e-8) &&
                        (fabs(wp[2] - msg->finalPosition[2]) < 1e-8))
                    {
                        break;
                    }
                    else
                    {
                        destinationWP++;
                    }
                }

                if (destinationWP < TrajectoryAppData.flightplan1.num_waypoints)
                {
                    char missionPlan[] = "Plan0";
                    PathPlanner_CombinePlan(TrajectoryAppData.pplanner, planID, missionPlan, destinationWP + 1);

                    // Publish EUTIL trajectory information
                    char buffer[MAX_DATABUFFER_SIZE] = {0};
                    time_t timeNow = time(NULL);
                    char combinePlanId[] = "Plan+";
                    PathPlanner_PlanToString(TrajectoryAppData.pplanner, combinePlanId, buffer, true, timeNow);

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
                PathPlanner_ClearAllPlans(TrajectoryAppData.pplanner);
                memcpy(&TrajectoryAppData.flightplan1, fp, sizeof(flightplan_t));
                for (int i = 0; i < fp->num_waypoints; ++i)
                {
                    double position[3] = {fp->waypoints[i].latitude, fp->waypoints[i].longitude, fp->waypoints[i].altitude};
                    int id = i;
                    double time;
                    if (fp->waypoints[i].wp_metric == WP_METRIC_ETA){
                        time = fp->waypoints[i].value;
                    }else{
                        time = id;
                    }
                    char name[] = "Plan0";
                    PathPlanner_InputFlightPlan(TrajectoryAppData.pplanner, name, (int)id, position, time);
                    OS_MutSemTake(TrajectoryAppData.mutexAcState);
                    TrajectoryAppData.monitor = true;
                    OS_MutSemGive(TrajectoryAppData.mutexAcState);
                }

                CFE_ES_WriteToSysLog("Trajectory:Received flight plan with %d waypoints\n", fp->num_waypoints);
                break;
            }

            case EUTL1_TRAJECTORY_MID:{
                TrajectoryAppData.eutlReceived = true;
                stringdata_t *strdata = (stringdata_t*)TrajectoryAppData.Traj_MsgPtr;
                char buffer[MAX_DATABUFFER_SIZE] = {0};    // TODO: Read this from the  message
                strcpy(buffer,strdata->buffer);
                char planID[] = "Plan0";
                PathPlanner_StringToPlan(TrajectoryAppData.pplanner,planID,buffer);  

                flightplan_t fp;
                CFE_SB_InitMsg(&fp,ICAROUS_FLIGHTPLAN_MID,sizeof(flightplan_t),TRUE);
                fp.num_waypoints = (uint16_t)PathPlanner_GetTotalWaypoints(TrajectoryAppData.pplanner, planID);
                strcpy(fp.id,"Plan0");
                for (int i = 0; i < fp.num_waypoints; ++i)
                {
                    double wp[4];
                    PathPlanner_GetWaypoint(TrajectoryAppData.pplanner, planID, i, wp);

                    if(i==0){
                        fp.scenario_time = wp[3];
                    }

                    if (i >= MAX_WAYPOINTS)
                    {
                        OS_printf("Trajectory: more than MAX_WAYPOINTS");
                        break;
                    }
                    if (wp[3] > 0){
                        fp.waypoints[i].wp_metric = WP_METRIC_ETA;
                        fp.waypoints[i].value =  wp[3] - fp.scenario_time;
                    }else{
                        fp.waypoints[i].wp_metric = WP_METRIC_NONE;
                    }
                    fp.waypoints[i].latitude = (float)wp[0];
                    fp.waypoints[i].longitude = (float)wp[1];
                    fp.waypoints[i].altitude = (float)wp[2];
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

                PathPlanner_InputGeofenceData(TrajectoryAppData.pplanner, gf->type, gf->index, gf->totalvertices, gf->floor, gf->ceiling, vertices);

                break;
            }

            case ICAROUS_POSITION_MID:
            {

                position_t *pos;
                pos = (position_t *)TrajectoryAppData.Traj_MsgPtr;

                if (pos->aircraft_id != CFE_PSP_GetSpacecraftId())
                {
                    double _pos[3] = {pos->latitude, pos->longitude, pos->altitude_rel};
                    double _vel[3] = {pos->vn, pos->ve, pos->vd};
                    int val = PathPlanner_InputTraffic(TrajectoryAppData.pplanner, pos->aircraft_id, _pos, _vel);
                    if (val)
                        CFE_ES_WriteToSysLog("Trajectory:Received intruder:%d\n", pos->aircraft_id);
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
                double vel[3] = {msg->vn, msg->ve, msg->vd};
                int val = PathPlanner_InputTraffic(TrajectoryAppData.pplanner, msg->index, pos, vel);
                if (val)
                    CFE_ES_WriteToSysLog("Trajectory:Received intruder: %d\n", msg->index);
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
                }
                else if (strcmp(msg->planID, "Plan1") == 0)
                {
                    TrajectoryAppData.nextWP2 = msg->reachedwaypoint + 1;
                }
                OS_MutSemGive(TrajectoryAppData.mutexAcState);
                break;
            }

            case ICAROUS_RESET_MID:
            {
                PathPlanner_ClearAllPlans(TrajectoryAppData.pplanner);
                break;
            }

            case ICAROUS_RESETFP_MID:
            {
                PathPlanner_ClearAllPlans(TrajectoryAppData.pplanner);
                break;
            }

            case FREQ_30_WAKEUP_MID:
            {

                double position[3];
                double velocity[3];
                int newWP1;
                int newWP2;
                bool monitor;

                OS_MutSemTake(TrajectoryAppData.mutexAcState);
                memcpy(position, TrajectoryAppData.position, sizeof(double) * 3);
                memcpy(velocity, TrajectoryAppData.velocity, sizeof(double) * 3);
                newWP1 = TrajectoryAppData.nextWP1;
                newWP2 = TrajectoryAppData.nextWP2;
                monitor = TrajectoryAppData.monitor;
                OS_MutSemGive(TrajectoryAppData.mutexAcState);

                if (!monitor)
                    break;

                // Compute distance to next waypoint.
                double nextWP1[3] = {TrajectoryAppData.flightplan1.waypoints[newWP1].latitude,
                                     TrajectoryAppData.flightplan1.waypoints[newWP1].longitude,
                                     TrajectoryAppData.flightplan1.waypoints[newWP1].altitude};

                double dist2NextWP1 = ComputeDistance(position, nextWP1);


                double prevWP1[3] = {TrajectoryAppData.flightplan1.waypoints[newWP1-1].latitude,
                                     TrajectoryAppData.flightplan1.waypoints[newWP1-1].longitude,
                                     TrajectoryAppData.flightplan1.waypoints[newWP1-1].altitude};

                // Compute xtrack deviation for current leg.
                double offset[2];
                ComputeXtrackDistance(prevWP1,nextWP1,position,offset);
                // Maneuver to intercept plan
                double maneuver[3];
                ManueverToIntercept(prevWP1,nextWP1,position,maneuver,TrajectoryAppData.xtrkGain,TrajectoryAppData.resSpeed,TrajectoryAppData.xtrkDev);

                double interceptHeadingToPlan;
                interceptHeadingToPlan = GetInterceptHeadingToPlan(prevWP1,nextWP1,position);

                CFE_SB_InitMsg(&TrajectoryAppData.fpMonitor, FLIGHTPLAN_MONITOR_MID, sizeof(flightplan_monitor_t), TRUE);
                strcpy(TrajectoryAppData.fpMonitor.planID,"Plan0\0");
                TrajectoryAppData.fpMonitor.nextWP = TrajectoryAppData.nextWP1;
                TrajectoryAppData.fpMonitor.dist2NextWP = dist2NextWP1;
                TrajectoryAppData.fpMonitor.crossTrackDeviation = offset[0];
                memcpy(TrajectoryAppData.fpMonitor.interceptManeuver, maneuver, sizeof(double) * 3);
                TrajectoryAppData.fpMonitor.interceptHeadingToPlan = interceptHeadingToPlan;
                TrajectoryAppData.fpMonitor.allowedXtrackError = TrajectoryAppData.xtrkDev;
                TrajectoryAppData.fpMonitor.resolutionSpeed = TrajectoryAppData.resSpeed;
                TrajectoryAppData.fpMonitor.searchType = TrajectoryAppData.searchType;
                SendSBMsg(TrajectoryAppData.fpMonitor);

                // Compute monitoring information for flight plan 2
                if(newWP2 >= TrajectoryAppData.flightplan2.num_waypoints || TrajectoryAppData.flightplan2.num_waypoints > 0){
                    break;
                }
                 // Compute distance to next waypoint.
                double nextWP2[3] = {TrajectoryAppData.flightplan2.waypoints[newWP2].latitude,
                                     TrajectoryAppData.flightplan2.waypoints[newWP2].longitude,
                                     TrajectoryAppData.flightplan2.waypoints[newWP2].altitude};
               
                double prevWP2[3] = {TrajectoryAppData.flightplan2.waypoints[newWP2-1].latitude,
                                     TrajectoryAppData.flightplan2.waypoints[newWP2-1].longitude,
                                     TrajectoryAppData.flightplan2.waypoints[newWP2-1].altitude};
               
                double dist2NextWP2 = ComputeDistance(position, nextWP2);


                // Compute xtrack deviation for current leg.
                ComputeXtrackDistance(prevWP2,nextWP2,position,offset);

                // Maneuver to intercept plan
                ManueverToIntercept(prevWP2,nextWP2,position,maneuver,TrajectoryAppData.xtrkGain,TrajectoryAppData.resSpeed,TrajectoryAppData.xtrkDev);

                interceptHeadingToPlan = GetInterceptHeadingToPlan(prevWP2,nextWP2,position);

                CFE_SB_InitMsg(&TrajectoryAppData.fpMonitor, FLIGHTPLAN_MONITOR_MID, sizeof(flightplan_monitor_t), TRUE);
                strcpy(TrajectoryAppData.fpMonitor.planID,"Plan1\0");
                TrajectoryAppData.fpMonitor.nextWP = TrajectoryAppData.nextWP2;
                TrajectoryAppData.fpMonitor.dist2NextWP = dist2NextWP2;
                TrajectoryAppData.fpMonitor.crossTrackDeviation = offset[0];
                memcpy(TrajectoryAppData.fpMonitor.interceptManeuver, maneuver, sizeof(double) * 3);
                TrajectoryAppData.fpMonitor.interceptHeadingToPlan = interceptHeadingToPlan;
                TrajectoryAppData.fpMonitor.allowedXtrackError = TrajectoryAppData.xtrkDev;
                TrajectoryAppData.fpMonitor.resolutionSpeed = TrajectoryAppData.resSpeed;
                TrajectoryAppData.fpMonitor.searchType = TrajectoryAppData.searchType;
                SendSBMsg(TrajectoryAppData.fpMonitor);

                break;
            }

            case TRAFFIC_PARAMETERS_MID:
            {
                    char params[2000];

                    traffic_parameters_t* msg;
                    msg = (traffic_parameters_t*) TrajectoryAppData.Traj_MsgPtr;

                    ConstructDAAParamString(msg,params);

                    if(TrajectoryAppData.updateDAAParams){
                        PathPlanner_UpdateDAAParameters(TrajectoryAppData.pplanner,params);
                    }
		    break;
            }

            case TRAJECTORY_PARAMETERS_MID:
            {

                trajectory_parameters_t *msg;
                msg = (trajectory_parameters_t *)TrajectoryAppData.Traj_MsgPtr;
                memcpy(&TrajectoryAppData.trajParams,msg,sizeof(trajectory_parameters_t));

                PathPlanner_UpdateAstarParameters(TrajectoryAppData.pplanner,
                                                  msg->astar_enable3D,
                                                  msg->astar_gridSize,
                                                  msg->astar_resSpeed,
                                                  msg->astar_lookahead,
                                                  msg->astar_daaConfigFile);

                PathPlanner_UpdateRRTParameters(TrajectoryAppData.pplanner,
                                                msg->rrt_resSpeed,
                                                msg->rrt_numIterations,
                                                msg->rrt_dt,
                                                msg->rrt_macroSteps,
                                                msg->rrt_capR,
                                                msg->rrt_daaConfigFile);

                TrajectoryAppData.xtrkDev = msg->xtrkDev;
                TrajectoryAppData.xtrkGain = msg->xtrkGain;
                TrajectoryAppData.resSpeed = msg->resSpeed;
                TrajectoryAppData.searchType = msg->searchAlgorithm;
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
