//
// Created by Swee Balachandran on 12/22/17.
//
#include <Icarous_msg.h>
#include <CWrapper/TrajectoryPlanner_proxy.h>
#include "trajectory.h"

CFE_EVS_BinFilter_t  TRAJECTORY_EventFilters[] =
        {  /* Event ID    mask */
                {TRAJECTORY_STARTUP_INF_EID,       0x0000},
                {TRAJECTORY_COMMAND_ERR_EID,       0x0000},
        }; /// Event ID definitions

/* Application entry points */
void TRAJECTORY_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    TRAJECTORY_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        status = CFE_SB_RcvMsg(&TrajectoryAppData.Trajectory_MsgPtr, TrajectoryAppData.Trajectory_Pipe, 10);

        if (status == CFE_SUCCESS)
        {
            TRAJECTORY_ProcessPacket();
        }
    }

    TRAJECTORY_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void TRAJECTORY_AppInit(void) {

    memset(&TrajectoryAppData, 0, sizeof(TrajectoryAppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(TRAJECTORY_EventFilters,
                     sizeof(TRAJECTORY_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe(&TrajectoryAppData.Trajectory_Pipe, /* Variable to hold Pipe ID */
                               TRAJECTORY_PIPE_DEPTH,       /* Depth of Pipe */
                               TRAJECTORY_PIPE_NAME);       /* Name of pipe */

    //Subscribe to plexil output messages from the SB
    CFE_SB_Subscribe(SERVICE_TRAJECTORY_MID, TrajectoryAppData.Trajectory_Pipe);
    CFE_SB_Subscribe(ICAROUS_WP_MID,TrajectoryAppData.Trajectory_Pipe);
    CFE_SB_Subscribe(ICAROUS_GEOFENCE_MID, TrajectoryAppData.Trajectory_Pipe);
    CFE_SB_Subscribe(ICAROUS_RESET_MID, TrajectoryAppData.Trajectory_Pipe);
    CFE_SB_Subscribe(ICAROUS_RESETFP_MID, TrajectoryAppData.Trajectory_Pipe);
    CFE_SB_Subscribe(ICAROUS_TRAFFIC_MID,TrajectoryAppData.Trajectory_Pipe);

    // Initialize all messages that this App generates
    CFE_SB_InitMsg(&trajServiceResponse, SERVICE_RESPONSE_MID, sizeof(service_t), TRUE);

    // Send event indicating app initialization
    CFE_EVS_SendEvent(TRAJECTORY_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "TRAJECTORY App Initialized. Version %d.%d",
                      TRAJECTORY_MAJOR_VERSION,
                      TRAJECTORY_MINOR_VERSION);

    TrajectoryAppData.fdata = new_FlightData("../ram/icarous.txt");
    TrajectoryAppData.pplanner = new_PathPlanner(TrajectoryAppData.fdata);

}

void TRAJECTORY_AppCleanUp(){
    // Do clean up here
    delete_PathPlanner(TrajectoryAppData.pplanner);
    delete_FlightData(TrajectoryAppData.fdata);
}

void TRAJECTORY_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(TrajectoryAppData.Trajectory_MsgPtr);

    switch(MsgId){
        case ICAROUS_WP_MID: {
            waypoint_t* wp;
            wp = (waypoint_t*) TrajectoryAppData.Trajectory_MsgPtr;

            double position[3] = {wp->latitude,wp->longitude,wp->altitude};
            double speed = wp->speed;
            double id = wp->wayPointIndex;
            char name[] = "Plan0";
            PathPlanner_InputFlightPlan(TrajectoryAppData.pplanner,name,(int)id,position,speed);
            break;
        }

        case ICAROUS_GEOFENCE_MID: {
            geofence_t *gf;
            gf = (geofence_t *) TrajectoryAppData.Trajectory_MsgPtr;
            SwigObj vertexWrapper;
            vertexWrapper.obj = (void*)gf;
            FlightData_InputGeofenceData(TrajectoryAppData.fdata,&vertexWrapper);
            break;
        }

        case ICAROUS_TRAFFIC_MID:{
            object_t* msg;
            msg = (object_t*) TrajectoryAppData.Trajectory_MsgPtr;

            FlightData_AddTraffic(TrajectoryAppData.fdata,msg->index,msg->latitude,msg->longitude,msg->altiude,msg->vx,msg->vy,msg->vz);
            break;
        }

        case ICAROUS_RESET_MID:{
            PathPlanner_ClearAllPlans(TrajectoryAppData.pplanner);
            FlightData_ClearFenceList(TrajectoryAppData.fdata);
        }

        case ICAROUS_RESETFP_MID:{
            PathPlanner_ClearAllPlans(TrajectoryAppData.pplanner);
        }

        case SERVICE_TRAJECTORY_MID: {
            service_t* msg;
            msg = (service_t*) TrajectoryAppData.Trajectory_MsgPtr;
            TrajServiceHandler(msg);
            break;
        }
    }
    return;
}