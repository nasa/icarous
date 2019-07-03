/**
 * @file ardupilot.c
 * @brief function definitions for ardupilot app
 */

#define EXTERN

#include <msgdef/ardupilot_msg.h>
#include "gsInterface.h"
#include "gsInterface_version.h"
#include "gsInterface_events.h"
#include "gsIntf_tbl.c"

/// Event filter definition for ardupilot
CFE_EVS_BinFilter_t  gsInterface_EventFilters[] =
{  /* Event ID    mask */
        {GSINTERFACE_STARTUP_INF_EID,       0x0000},
        {GSINTERFACE_COMMAND_ERR_EID,       0x0000},
};

/* gsInterface_AppMain() -- Application entry points */
void gsInterface_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    gsInterface_AppInit();
    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        int32 status = CFE_SB_RcvMsg(&appdataIntGS.Sch_MsgPtr, appdataIntGS.SchInterface_Pipe, CFE_SB_PEND_FOREVER);
        if (status == CFE_SUCCESS)
        {
            CFE_SB_MsgId_t  MsgId;
            MsgId = CFE_SB_GetMsgId(appdataIntGS.Sch_MsgPtr);
            switch (MsgId){
                case FREQ_50_WAKEUP_MID:
                    for(int i=0;i<10;++i)
                        GetMAVLinkMsgFromGS();
                    break;

                case FREQ_01_WAKEUP_MID:
                    appdataIntGS.hbeatFreqCount++;
                    int interval = 1;
                    if(appdataIntGS.hbeatFreqCount%interval == 0){
                        appdataIntGS.hbeatFreqCount = 0;
                        gsSendHeartbeat();
                    }
                    break;
            }
        }

        status = CFE_SB_RcvMsg(&appdataIntGS.INTERFACEMsgPtr, appdataIntGS.INTERFACE_Pipe, 10);

        if (status == CFE_SUCCESS)
        {
            gsInterface_ProcessPacket();
        }
    }

    gsInterface_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void gsInterface_AppInit(void){

    memset(&appdataIntGS,0,sizeof(appdataIntGS_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(gsInterface_EventFilters,
            sizeof(gsInterface_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
            CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe( &appdataIntGS.INTERFACE_Pipe, /* Variable to hold Pipe ID */
                                GSINTERFACE_PIPE_DEPTH,    /* Depth of Pipe */
                                GSINTERFACE_PIPE_NAME);    /* Name of pipe */

    status = CFE_SB_CreatePipe( &appdataIntGS.SchInterface_Pipe, /* Variable to hold Pipe ID */
                                GSINTERFACE_PIPE_DEPTH,    /* Depth of Pipe */
                                SCH_GSINTERFACE_PIPE1_NAME);    /* Name of pipe */


    // Subscribe to wakeup messages from scheduler
    CFE_SB_Subscribe(FREQ_50_WAKEUP_MID,appdataIntGS.SchInterface_Pipe);
    CFE_SB_Subscribe(FREQ_01_WAKEUP_MID,appdataIntGS.SchInterface_Pipe);

    //Subscribe to command messages and kinematic band messages from the SB
    CFE_SB_Subscribe(ICAROUS_POSITION_MID,appdataIntGS.INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_ATTITUDE_MID,appdataIntGS.INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_COMMANDS_MID, appdataIntGS.INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_BANDS_TRACK_MID, appdataIntGS.INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_STATUS_MID, appdataIntGS.INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_VFRHUD_MID, appdataIntGS.INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_TRAFFIC_MID, appdataIntGS.INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_ATTITUDE_MID, appdataIntGS.INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_BATTERY_STATUS_MID, appdataIntGS.INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_TRAJECTORY_MID, appdataIntGS.INTERFACE_Pipe);

    CFE_SB_InitMsg(&appdataIntGS.startMission,ICAROUS_STARTMISSION_MID,sizeof(argsCmd_t),TRUE);
    CFE_SB_InitMsg(&appdataIntGS.gfData,ICAROUS_GEOFENCE_MID,sizeof(geofence_t),TRUE);
    CFE_SB_InitMsg(&appdataIntGS.traffic,ICAROUS_TRAFFIC_MID,sizeof(object_t),TRUE);
    CFE_SB_InitMsg(&appdataIntGS.fpData,ICAROUS_FLIGHTPLAN_MID,sizeof(flightplan_t),TRUE);

    // Send event indicating app initialization
    CFE_EVS_SendEvent (GSINTERFACE_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                       "GS Interface initialized. Version %d.%d",
                       GSINTERFACE_INTERFACE_MAJOR_VERSION,
                       GSINTERFACE_INTERFACE_MINOR_VERSION);

    // Register table with table services
    status = CFE_TBL_Register(&appdataIntGS.INTERFACE_tblHandle,
                  "GSIntfTable",
                  sizeof(gsInterfaceTable_t),
                  CFE_TBL_OPT_DEFAULT,
                  &gsInterfaceTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(appdataIntGS.INTERFACE_tblHandle,CFE_TBL_SRC_ADDRESS,&gsIntf_TblStruct);

    // Initialize array of parameter ids
    gsInterface_InitializeParamIds();

    // Check which port to open from user defined parameters
    gsInterfaceTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr,appdataIntGS.INTERFACE_tblHandle);

    char apName[50],gsName[50];

    appdataIntGS.gs.id = 1;
    appdataIntGS.gs.portType = TblPtr->PortType;
    appdataIntGS.gs.baudrate = TblPtr->BaudRate;
    appdataIntGS.gs.portin   = TblPtr->Portin + 10 * CFE_PSP_GetSpacecraftId();
    appdataIntGS.gs.portout  = TblPtr->Portout + 10 * CFE_PSP_GetSpacecraftId();
    memcpy(appdataIntGS.gs.target,TblPtr->Address,50);

    //Set mission start flag to -1
    appdataIntGS.startMission.param1 = -1;
    appdataIntGS.hbeatFreqCount = 0;

    // Free table pointer
    status = CFE_TBL_ReleaseAddress(appdataIntGS.INTERFACE_tblHandle);


    if (appdataIntGS.gs.portType == SOCKET){
        InitializeSocketPort(&appdataIntGS.gs);
    }else if(appdataIntGS.gs.portType == SERIAL){
        InitializeSerialPort(&appdataIntGS.gs,false);
    }

    appdataIntGS.currentIcarousMode = 0;
    appdataIntGS.numGeofences = 0;
}

void gsInterface_AppCleanUp(){

}



int32_t gsInterfaceTableValidationFunc(void *TblPtr){

  int32_t status = 0;

  return status;
}


