//
// Created by Swee Balachandran on 12/22/17.
//
#include <CWrapper/TrajectoryPlanner_proxy.h>
#include <trajectory_msg.h>
#include <ardupilot_msg.h>
#include "trajectory.h"
#include "UtilFunctions.h"
#include "trajectory_tbl.h"

CFE_EVS_BinFilter_t  TRAJECTORY_EventFilters[] =
        {  /* Event ID    mask */
                {TRAJECTORY_STARTUP_INF_EID,       0x0000},
                {TRAJECTORY_COMMAND_ERR_EID,       0x0000},
                {TRAJECTORY_RECEIVED_INTRUDER_EID, 0x0000}
        }; /// Event ID definitions

/* Application entry points */
void TRAJECTORY_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    TRAJECTORY_AppInit();


    status = CFE_ES_CreateChildTask( &traj_monitor_task_id, "Traj Monitor", TRAJECTORY_Monitor, traj_monitor_stack, TRAJ_MONITOR_TASK_STACK_SIZE, TRAJ_MONITOR_PRIORITY, 0);
    if ( status != OS_SUCCESS ){
        OS_printf("Error creating TRAJECTORY_Monitor task \n");
    }


    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        status = CFE_SB_RcvMsg(&TrajectoryAppData.TrajRequest_MsgPtr, TrajectoryAppData.TrajRequest_Pipe, 10);

        if (status == CFE_SUCCESS)
        {
            TRAJECTORY_ProcessPacket();
        }
    }

    TrajectoryAppData.runThreads = CFE_ES_RunLoop(&RunStatus);

    TRAJECTORY_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void TRAJECTORY_AppInit(void) {

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
                               TRAJECTORY_PIPE_DEPTH,       /* Depth of Pipe */
                               TRAJECTORY_PIPE_NAME);       /* Name of pipe */


    status = CFE_SB_CreatePipe(&TrajectoryAppData.TrajData_Pipe, /* Variable to hold Pipe ID */
                               TRAJECTORY_SCH_PIPE_DEPTH,       /* Depth of Pipe */
                               TRAJECTORY_SCH_PIPE_NAME);       /* Name of pipe */

    //Subscribe to plexil output messages from the SB
    CFE_SB_Subscribe(ICAROUS_FLIGHTPLAN_MID,TrajectoryAppData.TrajData_Pipe);
    CFE_SB_Subscribe(ICAROUS_GEOFENCE_MID, TrajectoryAppData.TrajData_Pipe);
    CFE_SB_Subscribe(ICAROUS_RESET_MID, TrajectoryAppData.TrajData_Pipe);
    CFE_SB_Subscribe(ICAROUS_RESETFP_MID, TrajectoryAppData.TrajData_Pipe);
    CFE_SB_Subscribe(ICAROUS_POSITION_MID, TrajectoryAppData.TrajData_Pipe);
    CFE_SB_Subscribe(ICAROUS_TRAFFIC_MID,TrajectoryAppData.TrajData_Pipe);
    CFE_SB_Subscribe(ICAROUS_TRAJECTORY_MID, TrajectoryAppData.TrajData_Pipe);
    CFE_SB_Subscribe(ICAROUS_WPREACHED_MID,TrajectoryAppData.TrajData_Pipe);
    CFE_SB_Subscribe(TRAJECTORY_WAKEUP_MID, TrajectoryAppData.TrajData_Pipe);

    CFE_SB_Subscribe(ICAROUS_TRAJECTORY_REQUEST_MID, TrajectoryAppData.TrajRequest_Pipe);
    // Register table with table services
    status = CFE_TBL_Register(&TrajectoryAppData.Trajectory_TblHandle,
                              "TrajectoryTable",
                              sizeof(TrajectoryTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &TrajectoryTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(TrajectoryAppData.Trajectory_TblHandle, CFE_TBL_SRC_FILE, "/cf/trajectory_tbl.tbl");

    TrajectoryTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr, TrajectoryAppData.Trajectory_TblHandle);

    // Send event indicating app initialization
    CFE_EVS_SendEvent(TRAJECTORY_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "TRAJECTORY App Initialized. Version %d.%d",
                      TRAJECTORY_MAJOR_VERSION,
                      TRAJECTORY_MINOR_VERSION);

    TrajectoryAppData.pplanner = new_PathPlanner(TblPtr->obsbuffer,TblPtr->maxCeiling);
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

    TrajectoryAppData.numPlansComputed = 0;
    TrajectoryAppData.nextWP = 1;
    TrajectoryAppData.xtrkDev = TblPtr->xtrkDev;
    TrajectoryAppData.xtrkGain = TblPtr->xtrkGain;
    TrajectoryAppData.resSpeed = TblPtr->resSpeed;
    TrajectoryAppData.searchType = TblPtr->searchAlgorithm;

    status = OS_MutSemCreate( &TrajectoryAppData.mutexAcState, "ACstate", 0);
	if ( status != OS_SUCCESS )
		OS_printf("Error creating ACState mutex\n");

}

void TRAJECTORY_AppCleanUp(){
    // Do clean up here
    delete_PathPlanner(TrajectoryAppData.pplanner);
}

void TRAJECTORY_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(TrajectoryAppData.TrajRequest_MsgPtr);

    switch(MsgId){
            case ICAROUS_TRAJECTORY_REQUEST_MID:{
                    trajectory_request_t* msg;
                    msg = (trajectory_request_t*) TrajectoryAppData.TrajRequest_MsgPtr;
                    TrajectoryAppData.numPlansComputed++;

                    // Plan
                    char planID[10];
                    sprintf(planID,"plan%d",TrajectoryAppData.numPlansComputed);
                    int val = PathPlanner_FindPath(TrajectoryAppData.pplanner,msg->algorithm,planID,msg->initialPosition,msg->finalPosition,msg->initialVelocity);

                    flightplan_t result;
                    CFE_SB_InitMsg(&result,ICAROUS_TRAJECTORY_MID,sizeof(flightplan_t),TRUE);
                    memcpy(result.planID,planID,10);
                    result.totalWayPoints = (uint16_t) PathPlanner_GetTotalWaypoints(TrajectoryAppData.pplanner,planID);
                    for(int i=0;i<result.totalWayPoints;++i){
                        double wp[3];
                        PathPlanner_GetWaypoint(TrajectoryAppData.pplanner,planID,i,wp);
                        if(i > 49){
                            OS_printf("Trajectory: more than 50 waypoints");
                            break;
                        }
                        result.position[i][0] = (float)wp[0];
                        result.position[i][1] = (float)wp[1];
                        result.position[i][2] = (float)wp[2];

                        //OS_printf("%f, %f , %f\n",wp[0],wp[1],wp[2]);
                    }
                    SendSBMsg(result);
                    break;
            }
    }
    return;
}

void TRAJECTORY_Monitor(void){
    OS_TaskRegister();
	while(TrajectoryAppData.runThreads){
        int32 status = CFE_SB_RcvMsg(&TrajectoryAppData.Traj_MsgPtr, TrajectoryAppData.TrajData_Pipe,CFE_SB_PEND_FOREVER);
        if (status == CFE_SUCCESS)
        {
            CFE_SB_MsgId_t  MsgId;
            MsgId = CFE_SB_GetMsgId(TrajectoryAppData.Traj_MsgPtr);
            switch (MsgId){

                case ICAROUS_FLIGHTPLAN_MID: {
                    flightplan_t* wp;
                    wp = (flightplan_t*) TrajectoryAppData.Traj_MsgPtr;
                    PathPlanner_ClearAllPlans(TrajectoryAppData.pplanner);
                    memcpy(&TrajectoryAppData.mission, wp, sizeof(flightplan_t));
                    for(int i=0;i<wp->totalWayPoints;++i) {
                        double position[3] = {wp->position[i][0], wp->position[i][1], wp->position[i][2]};
                        double speed = wp->speed[i];
                        int id = i;
                        char name[] = "Plan0";
                        PathPlanner_InputFlightPlan(TrajectoryAppData.pplanner, name, (int) id, position, speed);
                        OS_MutSemTake(TrajectoryAppData.mutexAcState);
                        TrajectoryAppData.monitor = true;
                        OS_MutSemGive(TrajectoryAppData.mutexAcState);
                    }

                    CFE_EVS_SendEvent(TRAJECTORY_RECEIVED_FLIGHTPLAN_EID,CFE_EVS_INFORMATION,"Received flight plan with %d waypoints",wp->totalWayPoints);
                    break;
                }

                case ICAROUS_GEOFENCE_MID: {
                    geofence_t *gf;
                    gf = (geofence_t *) TrajectoryAppData.Traj_MsgPtr;

                    double vertices[50][2];

                    for(int i=0;i<gf->totalvertices;++i) {
                        vertices[i][0] = gf->vertices[i][0];
                        vertices[i][1] = gf->vertices[i][1];
                    }

                    PathPlanner_InputGeofenceData(TrajectoryAppData.pplanner,gf->type,gf->index,gf->totalvertices,gf->floor,gf->ceiling,vertices);

                    break;
                }

                case ICAROUS_POSITION_MID:{

                    position_t* pos;
                    pos = (position_t*) TrajectoryAppData.Traj_MsgPtr;

                    if(pos->aircraft_id != CFE_PSP_GetSpacecraftId()) {
                        double _pos[3] = {pos->latitude,pos->longitude,pos->altitude_rel};
                        double _vel[3] = {pos->vx,pos->vy,pos->vz};
                        int val = PathPlanner_InputTraffic(TrajectoryAppData.pplanner,pos->aircraft_id,_pos,_vel);
                        if(val)
                            CFE_EVS_SendEvent(TRAJECTORY_RECEIVED_INTRUDER_EID, CFE_EVS_INFORMATION,"Received intruder:%d",pos->aircraft_id);
                    }
                    else {
                        OS_MutSemTake(TrajectoryAppData.mutexAcState);
                        TrajectoryAppData.position[0] = pos->latitude;
                        TrajectoryAppData.position[1] = pos->longitude;
                        TrajectoryAppData.position[2] = pos->altitude_rel;

                        double trk,gs,vs;
                        ConvertVnedToTrkGsVs(pos->vx,pos->vy,pos->vz,&trk,&gs,&vs);

                        TrajectoryAppData.velocity[0] = trk;
                        TrajectoryAppData.velocity[1] = gs;
                        TrajectoryAppData.velocity[2] = vs;
                        OS_MutSemGive(TrajectoryAppData.mutexAcState);
                    }

                    break;
                }

                case ICAROUS_TRAFFIC_MID:{
                    object_t* msg;
                    msg = (object_t*) TrajectoryAppData.Traj_MsgPtr;

                    double pos[3] = {msg->latitude,msg->longitude,msg->altitude};
                    double vel[3] = {msg->vx,msg->vy,msg->vz};
                    int val = PathPlanner_InputTraffic(TrajectoryAppData.pplanner,msg->index,pos,vel);
                    if(val)
                        CFE_EVS_SendEvent(TRAJECTORY_RECEIVED_INTRUDER_EID, CFE_EVS_INFORMATION,"Received intruder: %d",msg->index);
                    break;
                }

                case ICAROUS_WPREACHED_MID:{
                    missionItemReached_t* msg;
                    msg = (missionItemReached_t*) TrajectoryAppData.Traj_MsgPtr;
                    OS_MutSemTake(TrajectoryAppData.mutexAcState);
                    TrajectoryAppData.nextWP = msg->reachedwaypoint + 1;
                    OS_MutSemGive(TrajectoryAppData.mutexAcState);
                    break;
                }

                case ICAROUS_RESET_MID:{
                    PathPlanner_ClearAllPlans(TrajectoryAppData.pplanner);
                    break;
                }

                case ICAROUS_RESETFP_MID:{
                    PathPlanner_ClearAllPlans(TrajectoryAppData.pplanner);
                    break;
                }



                case TRAJECTORY_WAKEUP_MID: {

                    double position[3];
                    double velocity[3];
                    int nextWP;
                    bool monitor;

                    OS_MutSemTake(TrajectoryAppData.mutexAcState);
                    memcpy(position,TrajectoryAppData.position,sizeof(double)*3);
                    memcpy(velocity,TrajectoryAppData.velocity,sizeof(double)*3);
                    nextWP = TrajectoryAppData.nextWP;
                    monitor = TrajectoryAppData.monitor;
                    OS_MutSemGive(TrajectoryAppData.mutexAcState);

                    if(!monitor)
                        break;

                    // Compute distance to next waypoint.
                    double nextWPPosition[3] = {TrajectoryAppData.mission.position[nextWP][0],
                                                TrajectoryAppData.mission.position[nextWP][1],
                                                TrajectoryAppData.mission.position[nextWP][2]};

                    double dist2NextWP = PathPlanner_Dist2Waypoint(TrajectoryAppData.pplanner,position,nextWPPosition);

                    // Compute xtrack deviation for current leg.
                    double offset[2];
                    PathPlanner_ComputeXtrackDistance_c(TrajectoryAppData.pplanner,"Plan0",nextWP,position,offset);

                    // Maneuver to intercept plan
                    double maneuver[3];
                    PathPlanner_ManueverToIntercept_c(TrajectoryAppData.pplanner,"Plan0",nextWP,position,maneuver,
                                                      TrajectoryAppData.xtrkGain,TrajectoryAppData.resSpeed,TrajectoryAppData.xtrkDev);

                    double interceptHeadingToPlan;
                    interceptHeadingToPlan = PathPlanner_GetInterceptHeadingToPlan_c(TrajectoryAppData.pplanner,"Plan0",nextWP,position);

                    CFE_SB_InitMsg(&TrajectoryAppData.fpMonitor,FLIGHTPLAN_MONITOR_MID,sizeof(flightplan_monitor_t),TRUE);
                    TrajectoryAppData.fpMonitor.nextWP = TrajectoryAppData.nextWP;
                    TrajectoryAppData.fpMonitor.dist2NextWP = dist2NextWP;
                    TrajectoryAppData.fpMonitor.crossTrackDeviation = offset[0];
                    memcpy(TrajectoryAppData.fpMonitor.interceptManeuver,maneuver,sizeof(double)*3);
                    TrajectoryAppData.fpMonitor.interceptHeadingToPlan = interceptHeadingToPlan;
                    TrajectoryAppData.fpMonitor.allowedXtrackError = TrajectoryAppData.xtrkDev;
                    TrajectoryAppData.fpMonitor.resolutionSpeed = TrajectoryAppData.resSpeed;
                    TrajectoryAppData.fpMonitor.searchType = TrajectoryAppData.searchType;
                    SendSBMsg(TrajectoryAppData.fpMonitor);
                    break;
                }
                default:
                    break;
            }
        }
    }
}

int32_t TrajectoryTableValidationFunc(void *TblPtr){
    return 0;
}
