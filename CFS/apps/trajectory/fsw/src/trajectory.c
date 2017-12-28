//
// Created by Swee Balachandran on 12/22/17.
//
#include "trajectory.h"

CFE_EVS_BinFilter_t  Trajectory_EventFilters[] =
        {  /* Event ID    mask */
                {Trajectory_STARTUP_INF_EID,       0x0000},
                {Trajectory_COMMAND_ERR_EID,       0x0000},
        }; /// Event ID definitions

/* Application entry points */
void Trajectory_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    Trajectory_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        status = CFE_SB_RcvMsg(&TrajectoryAppData.Trajectory_MsgPtr, TrajectoryAppData.Trajectory_Pipe, 10);

        if (status == CFE_SUCCESS)
        {
            Trajectory_ProcessPacket();
        }
    }

    Trajectory_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void Trajectory_AppInit(void) {

    memset(&TrajectoryAppData, 0, sizeof(TrajectoryAppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(Trajectory_EventFilters,
                     sizeof(Trajectory_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe(&TrajectoryAppData.Trajectory_Pipe, /* Variable to hold Pipe ID */
                               Trajectory_PIPE_DEPTH,       /* Depth of Pipe */
                               Trajectory_PIPE_NAME);       /* Name of pipe */

    //Subscribe to plexil output messages from the SB
    CFE_SB_Subscribe(PLEXIL_OUTPUT_MID, TrajectoryAppData.Trajectory_Pipe);
    //CFE_SB_Subscribe(ICAROUS_GEOFENCE_MID,geofenceAppData.Geofence_Pipe);

    // Initialize all messages that this App generates
    CFE_SB_InitMsg(&gfPlexilMsg, PLEXIL_INPUT_MID, sizeof(plexil_interface_t), TRUE);

    // Send event indicating app initialization
    CFE_EVS_SendEvent(Trajectory_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "Trajectory App Initialized. Version %d.%d",
                      Trajectory_MAJOR_VERSION,
                      Trajectory_MINOR_VERSION);

    TrajectoryAppData.fdata = new_FlightData("/ram/icarous.txt");
    //TrajectoryAppData.gfMonitor = new_TrajectoryMonitor(TrajectoryAppData.fdata);

}

void Trajectory_AppCleanUp(){
    // Do clean up here
    delete_TrajectoryMonitor(TrajectoryAppData.gfMonitor);
    delete_FlightData(TrajectoryAppData.fdata);
}

void Trajectory_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(TrajectoryAppData.Trajectory_MsgPtr);

    switch(MsgId){
        case ICAROUS_Trajectory_MID:

            break;

        case PLEXIL_OUTPUT_MID:

            break;
    }
    return;
}