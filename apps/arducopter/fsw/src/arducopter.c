/**
 * @file arducopter.c
 * @brief function definitions for arducopter app
 */

#define EXTERN

#define INIT_PARAM

#include "arducopter.h"
#include "arducopter_version.h"
#include "intf_tbl.c"

/// Event filter definition for arducopter
CFE_EVS_BinFilter_t  ARDUCOPTER_EventFilters[] =
{  /* Event ID    mask */
        {ARDUCOPTER_STARTUP_INF_EID,       0x0000},
        {ARDUCOPTER_COMMAND_ERR_EID,       0x0000},
};

/* ARDUCOPTER_AppMain() -- Application entry points */
void ARDUCOPTER_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    ARDUCOPTER_AppInit();

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
                    apSendHeartbeat();
                    break;
            }
        }

        status = CFE_SB_RcvMsg(&appdataInt.INTERFACEMsgPtr, appdataInt.INTERFACE_Pipe, 10);

        if (status == CFE_SUCCESS)
        {
            ARDUCOPTER_ProcessPacket();
        }

        // Stop parameter timer if parameter was sent
        if(appdataInt.paramSent)
            ap_stopTimer(&appdataInt.pmtimer);
    }

    ARDUCOPTER_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void ARDUCOPTER_AppInit(void){

    memset(&appdataInt,0,sizeof(appdataInt_t));
    appdataInt.runThreads = 1;

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(ARDUCOPTER_EventFilters,
            sizeof(ARDUCOPTER_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
            CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe( &appdataInt.INTERFACE_Pipe, /* Variable to hold Pipe ID */
                                ARDUCOPTER_PIPE_DEPTH,    /* Depth of Pipe */
                                ARDUCOPTER_PIPE_NAME);    /* Name of pipe */

    status = CFE_SB_CreatePipe( &appdataInt.SchInterface_Pipe, /* Variable to hold Pipe ID */
                                ARDUCOPTER_PIPE_DEPTH,    /* Depth of Pipe */
                                SCH_ARDUCOPTER_PIPE1_NAME);    /* Name of pipe */

    // Subscribe to wakeup messages from scheduler
    CFE_SB_SubscribeLocal(FREQ_50_WAKEUP_MID,appdataInt.SchInterface_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(FREQ_01_WAKEUP_MID,appdataInt.SchInterface_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);


    //Subscribe to command messages and kinematic band messages from the SB
    CFE_SB_SubscribeLocal(ICAROUS_COMMANDS_MID, appdataInt.INTERFACE_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_FLIGHTPLAN_MID,appdataInt.INTERFACE_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_STATUS_MID,appdataInt.INTERFACE_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_TRAFFIC_MID,appdataInt.INTERFACE_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_BANDS_TRACK_MID, appdataInt.INTERFACE_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_TRAJECTORY_MID, appdataInt.INTERFACE_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(UPLINK_FLIGHTPLAN_MID,appdataInt.INTERFACE_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(GUIDANCE_COMMAND_MID,appdataInt.INTERFACE_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);

    // Initialize all messages that this App generates
    CFE_SB_InitMsg(&wpreached,ICAROUS_WPREACHED_MID,sizeof(missionItemReached_t),TRUE);
    CFE_SB_InitMsg(&traffic,ICAROUS_TRAFFIC_MID,sizeof(object_t),TRUE);
    CFE_SB_InitMsg(&position,ICAROUS_POSITION_MID,sizeof(position_t),TRUE);
    CFE_SB_InitMsg(&attitude,ICAROUS_ATTITUDE_MID,sizeof(attitude_t),TRUE);
    CFE_SB_InitMsg(&ack,ICAROUS_COMACK_MID,sizeof(cmdAck_t),TRUE);
    CFE_SB_InitMsg(&vfrhud,ICAROUS_VFRHUD_MID,sizeof(vfrhud_t),TRUE);
    CFE_SB_InitMsg(&battery_status,ICAROUS_BATTERY_STATUS_MID,sizeof(battery_status_t),TRUE);
    CFE_SB_InitMsg(&rc_channels,ICAROUS_RC_CHANNELS_MID,sizeof(rc_channels_t),TRUE);
    CFE_SB_InitMsg(&local_position,ICAROUS_LOCAL_POSITION_MID,sizeof(local_position_t),TRUE);
    CFE_SB_InitMsg(&startMission,ICAROUS_STARTMISSION_MID,sizeof(argsCmd_t),TRUE);


    // Register table with table services
    status = CFE_TBL_Register(&appdataInt.INTERFACE_tblHandle,
                  "InterfaceTable",
                  sizeof(ArducopterTable_t),
                  CFE_TBL_OPT_DEFAULT,
                  &ArducopterTableValidationFunc);

    // Load app table data 
    
    status = CFE_TBL_Load(appdataInt.INTERFACE_tblHandle,CFE_TBL_SRC_ADDRESS,&Arducopter_TblStruct);

    ArducopterTable_t *TblPtr;

    // Check which port to open from user defined parameters ArducopterTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr,appdataInt.INTERFACE_tblHandle);

    memcpy(&appdataInt.Table,TblPtr,sizeof(ArducopterTable_t));

    // Free table pointer
    status = CFE_TBL_ReleaseAddress(appdataInt.INTERFACE_tblHandle);

    ARDUCOPTER_AppInitializeData();

    if(status == CFE_SUCCESS){
        // Send event indicating app initialization
        CFE_EVS_SendEvent (ARDUCOPTER_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                       "Arducopter Interface initialized. Version %d.%d",
                       ARDUCOPTER_MAJOR_VERSION,
                       ARDUCOPTER_MINOR_VERSION);

    }

}

void ARDUCOPTER_AppInitializeData(){

    InitializePortConfig("arducopter",&appdataInt.ap);

    InitializeAircraftCallSign(appdataInt.callsign.value);

    ArducopterTable_t *TblPtr = &appdataInt.Table;

    appdataInt.sentDefaultParams = false;
    appdataInt.ap.id = 0;
    appdataInt.waypointSeq = 0;
    appdataInt.nextWaypointIndex = 0;
    appdataInt.icRcChannel = TblPtr->icRcChannel;
    appdataInt.pwmStart = TblPtr->pwmStart;
    appdataInt.pwmReset = TblPtr->pwmReset; 

    //Set mission start flag to -1
    startMission.param1 = -1;

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
    appdataInt.startMission = false;
    appdataInt.restartMission = true;
    appdataInt.fpread = false;
    appdataInt.takeoff = false;

    appdataInt.wptimer = 0xffff;
    appdataInt.gftimer = 0xffff;
    appdataInt.pmtimer = 0xffff;
    appdataInt.tjtimer = 0xffff;
    appdataInt.fenceSent = false;

    bool status = InitializeParams("../ram/icarous_default.parm",appdataInt.storedparams,PARAM_COUNT);
    if(!status){
        OS_printf("Error loading parameters\n");
        exit(0);
    }
}
    
void ARDUCOPTER_AppCleanUp(){
    free((void*)appdataInt.waypoint_type);
}

int32_t ArducopterTableValidationFunc(void *TblPtr){
  int32_t status = 0;
  return status;
}
