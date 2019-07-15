/**
 * @file ardupilot.c
 * @brief function definitions for ardupilot app
 */

#define EXTERN

#include <msgdef/ardupilot_msg.h>
#include "ardupilot.h"
#include "ardupilot_version.h"
#include "intf_tbl.c"

/// Event filter definition for ardupilot
CFE_EVS_BinFilter_t  ARDUPILOT_EventFilters[] =
{  /* Event ID    mask */
        {ARDUPILOT_STARTUP_INF_EID,       0x0000},
        {ARDUPILOT_COMMAND_ERR_EID,       0x0000},
};

/* ARDUPILOT_AppMain() -- Application entry points */
void ARDUPILOT_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    ARDUPILOT_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE) {
        status = CFE_SB_RcvMsg(&appdataInt.Sch_MsgPtr, appdataInt.SchInterface_Pipe, CFE_SB_PEND_FOREVER);
        if (status == CFE_SUCCESS) {
            CFE_SB_MsgId_t MsgId;
            MsgId = CFE_SB_GetMsgId(appdataInt.Sch_MsgPtr);
            switch (MsgId) {
                case FREQ_50_WAKEUP_MID:
                    for (int i = 0; i < 10; i++)
                        GetMAVLinkMsgFromAP();
                    break;

                case FREQ_01_WAKEUP_MID:
                    //apSendHeartbeat();
                    break;
            }
        }

        status = CFE_SB_RcvMsg(&appdataInt.INTERFACEMsgPtr, appdataInt.INTERFACE_Pipe, 10);

        if (status == CFE_SUCCESS)
        {
            ARDUPILOT_ProcessPacket();
        }
    }

    ARDUPILOT_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void ARDUPILOT_AppInit(void){

    memset(&appdataInt,0,sizeof(appdataInt_t));
    appdataInt.runThreads = 1;

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(ARDUPILOT_EventFilters,
            sizeof(ARDUPILOT_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
            CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe( &appdataInt.INTERFACE_Pipe, /* Variable to hold Pipe ID */
                                ARDUPILOT_PIPE_DEPTH,    /* Depth of Pipe */
                                ARDUPILOT_PIPE_NAME);    /* Name of pipe */

    status = CFE_SB_CreatePipe( &appdataInt.SchInterface_Pipe, /* Variable to hold Pipe ID */
                                ARDUPILOT_PIPE_DEPTH,    /* Depth of Pipe */
                                SCH_ARDUPILOT_PIPE1_NAME);    /* Name of pipe */

    // Subscribe to wakeup messages from scheduler
    CFE_SB_Subscribe(FREQ_50_WAKEUP_MID,appdataInt.SchInterface_Pipe);
    CFE_SB_Subscribe(FREQ_01_WAKEUP_MID,appdataInt.SchInterface_Pipe);


    //Subscribe to command messages and kinematic band messages from the SB
    CFE_SB_Subscribe(ICAROUS_COMMANDS_MID, appdataInt.INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_FLIGHTPLAN_MID,appdataInt.INTERFACE_Pipe);

    // Initialize all messages that this App generates
    CFE_SB_InitMsg(&wpreached,ICAROUS_WPREACHED_MID,sizeof(missionItemReached_t),TRUE);
    CFE_SB_InitMsg(&traffic,ICAROUS_TRAFFIC_MID,sizeof(object_t),TRUE);
    CFE_SB_InitMsg(&position,ICAROUS_POSITION_MID,sizeof(position_t),TRUE);
    CFE_SB_InitMsg(&attitude,ICAROUS_ATTITUDE_MID,sizeof(attitude_t),TRUE);
    CFE_SB_InitMsg(&ack,ICAROUS_COMACK_MID,sizeof(cmdAck_t),TRUE);
    CFE_SB_InitMsg(&vfrhud,ICAROUS_VFRHUD_MID,sizeof(vfrhud_t),TRUE);
    CFE_SB_InitMsg(&battery_status,ICAROUS_BATTERY_STATUS_MID,sizeof(battery_status_t),TRUE);

    // Send event indicating app initialization
    CFE_EVS_SendEvent (ARDUPILOT_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                       "Ardupilot Interface initialized. Version %d.%d",
                       ARDUPILOT_MAJOR_VERSION,
                       ARDUPILOT_MINOR_VERSION);

    // Register table with table services
    status = CFE_TBL_Register(&appdataInt.INTERFACE_tblHandle,
                  "InterfaceTable",
                  sizeof(ArdupilotTable_t),
                  CFE_TBL_OPT_DEFAULT,
                  &ArdupilotTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(appdataInt.INTERFACE_tblHandle,CFE_TBL_SRC_ADDRESS,&Ardupilot_TblStruct);

    // Check which port to open from user defined parameters
    ArdupilotTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr,appdataInt.INTERFACE_tblHandle);

    char apName[50],gsName[50];

    appdataInt.ap.id = 0;
    appdataInt.waypointSeq = 0;
    appdataInt.nextWaypointIndex = 0;
    appdataInt.ap.portType = TblPtr->PortType;
    appdataInt.ap.baudrate = TblPtr->BaudRate;
    appdataInt.ap.portin   = TblPtr->Portin + 10 * CFE_PSP_GetSpacecraftId();
    appdataInt.ap.portout  = TblPtr->Portout;
    memcpy(appdataInt.ap.target,TblPtr->Address,50);


    //Set mission start flag to -1
    startMission.param1 = -1;

    // Free table pointer
    status = CFE_TBL_ReleaseAddress(appdataInt.INTERFACE_tblHandle);

    if (appdataInt.ap.portType == SOCKET){
        InitializeSocketPort(&appdataInt.ap);
    }else if(appdataInt.ap.portType == SERIAL){
        InitializeSerialPort(&appdataInt.ap,false);
    }

    appdataInt.waypoint_type = (int*)malloc(sizeof(int)*2);
    appdataInt.startWPUplink = false;
    appdataInt.startWPDownlink = false;
    appdataInt.downlinkRequestIndex = 0;
    appdataInt.foundUAV = 0;
}

void ARDUPILOT_AppCleanUp(){
    free((void*)appdataInt.waypoint_type);
}

int32_t ArdupilotTableValidationFunc(void *TblPtr){
  int32_t status = 0;
  return status;
}
