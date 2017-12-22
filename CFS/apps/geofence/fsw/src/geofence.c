//
// Created by Swee Balachandran on 12/22/17.
//
#include "geofence.h"

CFE_EVS_BinFilter_t  Geofence_EventFilters[] =
        {  /* Event ID    mask */
                {GEOFENCE_STARTUP_INF_EID,       0x0000},
                {GEOFENCE_COMMAND_ERR_EID,       0x0000},
        }; /// Event ID definitions

/* Application entry points */
void Geofence_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    Geofence_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        status = CFE_SB_RcvMsg(&geofenceAppData.Geofence_MsgPtr, geofenceAppData.Geofence_Pipe, 10);

        if (status == CFE_SUCCESS)
        {
            Geofence_ProcessPacket();
        }
    }

    Geofence_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void Geofence_AppInit(void) {

    memset(&geofenceAppData, 0, sizeof(geofenceAppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(Geofence_EventFilters,
                     sizeof(Geofence_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe(&geofenceAppData.Geofence_Pipe, /* Variable to hold Pipe ID */
                               GEOFENCE_PIPE_DEPTH,       /* Depth of Pipe */
                               GEOFENCE_PIPE_NAME);       /* Name of pipe */

    //Subscribe to plexil output messages from the SB
    CFE_SB_Subscribe(PLEXIL_OUTPUT_MID, geofenceAppData.Geofence_Pipe);

    // Initialize all messages that this App generates
    CFE_SB_InitMsg(&gfPlexilMsg, PLEXIL_INPUT_MID, sizeof(plexil_interface_t), TRUE);

    // Send event indicating app initialization
    CFE_EVS_SendEvent(GEOFENCE_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "Geofence App Initialized. Version %d.%d",
                      GEOFENCE_MAJOR_VERSION,
                      GEOFENCE_MINOR_VERSION);

    geofenceAppData.fdata = new_FlightData("/ram/icarous.txt");
    geofenceAppData.gfMonitor = new_GeofenceMonitor(geofenceAppData.fdata);

}

void Geofence_AppCleanUp(){
    // Do clean up here
    delete_GeofenceMonitor(geofenceAppData.gfMonitor);
    delete_FlightData(geofenceAppData.fdata);
}

void Geofence_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(geofenceAppData.Geofence_MsgPtr);

    switch(MsgId){

    }
    return;
}