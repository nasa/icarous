//
// Created by Swee Balachandran on 12/22/17.
//
#include <Icarous_msg.h>
#include "geofence.h"

CFE_EVS_BinFilter_t  GEOFENCE_EventFilters[] =
        {  /* Event ID    mask */
                {GEOFENCE_STARTUP_INF_EID,       0x0000},
                {GEOFENCE_COMMAND_ERR_EID,       0x0000},
        }; /// Event ID definitions

/* Application entry points */
void GEOFENCE_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    GEOFENCE_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        status = CFE_SB_RcvMsg(&geofenceAppData.Geofence_MsgPtr, geofenceAppData.Geofence_Pipe, 10);

        if (status == CFE_SUCCESS)
        {
            GEOFENCE_ProcessPacket();
        }
    }

    GEOFENCE_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void GEOFENCE_AppInit(void) {

    memset(&geofenceAppData, 0, sizeof(geofenceAppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(GEOFENCE_EventFilters,
                     sizeof(GEOFENCE_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe(&geofenceAppData.Geofence_Pipe, /* Variable to hold Pipe ID */
                               GEOFENCE_PIPE_DEPTH,       /* Depth of Pipe */
                               GEOFENCE_PIPE_NAME);       /* Name of pipe */

    //Subscribe to plexil output messages from the SB
    CFE_SB_Subscribe(SERVICE_GEOFENCE_MID, geofenceAppData.Geofence_Pipe);
    CFE_SB_Subscribe(ICAROUS_GEOFENCE_MID,geofenceAppData.Geofence_Pipe);
    CFE_SB_Subscribe(ICAROUS_RESET_MID,geofenceAppData.Geofence_Pipe);

    // Initialize all messages that this App generates
    CFE_SB_InitMsg(&gfServiceResponse, SERVICE_RESPONSE_MID, sizeof(service_t), TRUE);

    // Send event indicating app initialization
    CFE_EVS_SendEvent(GEOFENCE_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "Geofence App Initialized. Version %d.%d",
                      GEOFENCE_MAJOR_VERSION,
                      GEOFENCE_MINOR_VERSION);

    geofenceAppData.fdata = new_FlightData("../ram/icarous.txt");
    geofenceAppData.gfMonitor = new_GeofenceMonitor(geofenceAppData.fdata);

}

void GEOFENCE_AppCleanUp(){
    // Do clean up here
    delete_GeofenceMonitor(geofenceAppData.gfMonitor);
    delete_FlightData(geofenceAppData.fdata);
}

void GEOFENCE_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(geofenceAppData.Geofence_MsgPtr);

    switch(MsgId){
        case ICAROUS_GEOFENCE_MID: {
            geofence_t *gf;
            gf = (geofence_t *) geofenceAppData.Geofence_MsgPtr;
            SwigObj vertexWrapper;
            vertexWrapper.obj = (void*)gf;
            FlightData_InputGeofenceData(geofenceAppData.fdata,&vertexWrapper);
            break;
        }
        case SERVICE_GEOFENCE_MID: {
            service_t* msg = (service_t*)geofenceAppData.Geofence_MsgPtr;
            GeoPlxMsgHandler(msg);
            break;
        }

        case ICAROUS_RESET_MID:{
            NoArgsCmd_t *resetIcarous;
            resetIcarous = (NoArgsCmd_t*) geofenceAppData.Geofence_MsgPtr;
            FlightData_ClearFenceList(geofenceAppData.fdata);
        }
    }
    return;
}