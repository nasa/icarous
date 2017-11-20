/*******************************************************************************
** File: icarous.c
**
*******************************************************************************/

#include "icarous.h"

CFE_EVS_BinFilter_t  ICAROUS_EventFilters[] =
{  /* Event ID    mask */
		{ICAROUS_STARTUP_INF_EID,       0x0000},
		{ICAROUS_COMMAND_ERR_EID,       0x0000},
};


void ICAROUS_AppMain( void )
{

    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    ICAROUS_AppInit();

    appdataIC.ic = icarous_create_init(&appdataIC.TblPtrCpy);

    /*
    ** ICAROUS Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&appdataIC.ICAROUSMsgPtr, appdataIC.ICAROUS_Pipe, 1);
        
        if (status == CFE_SUCCESS)
        {
        	ICAROUS_ProcessPacket();
        }

        icarous_run(appdataIC.ic);

        ICAROUS_Output();
    }

    CFE_ES_ExitApp(RunStatus);

}

void ICAROUS_AppInit(void)
{

	int32 status;
    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp() ;


    /*
     ** Register the events
     */
    CFE_EVS_Register(ICAROUS_EventFilters,
    		sizeof(ICAROUS_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
			CFE_EVS_BINARY_FILTER);

    /// Initialize SB messages
    CFE_SB_InitMsg(&appdataIC.cmd,ICAROUS_COMMANDS_MID,sizeof(ArgsCmd_t),TRUE);
    CFE_SB_InitMsg(&appdataIC.bands,ICAROUS_VISBAND_MID,sizeof(visbands_t),TRUE);

    /*
    ** Create the Software Bus command pipe and subscribe to housekeeping
    **  messages
    */
    CFE_SB_CreatePipe(&appdataIC.ICAROUS_Pipe, ICAROUS_PIPE_DEPTH,"ICAROUS_PIPE");
    CFE_SB_Subscribe(ICAROUS_WP_MID, appdataIC.ICAROUS_Pipe);
    CFE_SB_Subscribe(ICAROUS_GEOFENCE_MID, appdataIC.ICAROUS_Pipe);
    CFE_SB_Subscribe(ICAROUS_STARTMISSION_MID, appdataIC.ICAROUS_Pipe);
    CFE_SB_Subscribe(ICAROUS_COMACK_MID, appdataIC.ICAROUS_Pipe);
    CFE_SB_Subscribe(ICAROUS_POSITION_MID, appdataIC.ICAROUS_Pipe);
    CFE_SB_Subscribe(ICAROUS_WPREACHED_MID, appdataIC.ICAROUS_Pipe);
    CFE_SB_SubscribeEx(ICAROUS_TRAFFIC_MID, appdataIC.ICAROUS_Pipe,CFE_SB_Default_Qos,100);

    // Register table with table services
    status = CFE_TBL_Register(&appdataIC.ICAROUS_tblHandle,
    		"IcarousTable",
			sizeof(icarous_table_t),
			CFE_TBL_OPT_DEFAULT,
			&IcarousTableValidationFunc);

    status = CFE_TBL_Load(appdataIC.ICAROUS_tblHandle,CFE_TBL_SRC_FILE,"/cf/apps/icarous_tbl.tbl");

    // Check which port to open from user defined parameters
    icarous_table_t *TblPtr;
    status = CFE_TBL_GetAddress(&TblPtr,appdataIC.ICAROUS_tblHandle);
    memcpy(&appdataIC.TblPtrCpy,TblPtr,sizeof(icarous_table_t));

    // Free table pointer
    status = CFE_TBL_ReleaseAddress(appdataIC.ICAROUS_tblHandle);

    CFE_EVS_SendEvent (ICAROUS_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               	      "ICAROUS Initialized. Version %d.%d",
					  ICAROUS_MAJOR_VERSION,
					  ICAROUS_MINOR_VERSION);
}


void ICAROUS_ProcessPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(appdataIC.ICAROUSMsgPtr);

    switch (MsgId)
    {
        case ICAROUS_WP_MID:
        {
        	waypoint_t *wp;
        	wp = (waypoint_t*)appdataIC.ICAROUSMsgPtr;
        	icarous_setFlightData(appdataIC.ic,wp);
            break;
        }

        case ICAROUS_GEOFENCE_MID:
        {	
        	geofence_t *gf;
        	gf = (geofence_t*)appdataIC.ICAROUSMsgPtr;		
        	icarous_setGeofenceData(appdataIC.ic,gf);
        	break;
        }

        case ICAROUS_STARTMISSION_MID:
        {	  
        	ArgsCmd_t *cmd;
        	cmd = (ArgsCmd_t*)appdataIC.ICAROUSMsgPtr;
        	icarous_StartMission(appdataIC.ic,cmd->param1);
        	break;
        }

        case ICAROUS_COMACK_MID:
        {
        	CmdAck_t* ack;
        	ack = (CmdAck_t*)appdataIC.ICAROUSMsgPtr;
        	icarous_inputAck(appdataIC.ic,ack);
        	break;
        }

        case ICAROUS_POSITION_MID:
        {
        	position_t* pos;
        	pos = (position_t*)appdataIC.ICAROUSMsgPtr;
        	icarous_inputPosition(appdataIC.ic,pos);
        	break;
        }

        case ICAROUS_WPREACHED_MID:
        {
        	missionItemReached_t* msnItem;
        	msnItem = (missionItemReached_t*)appdataIC.ICAROUSMsgPtr;
        	icarous_inputMissionItemReached(appdataIC.ic,msnItem);
        	break;
        }

        case ICAROUS_TRAFFIC_MID:
        {
        	object_t* traffic;
        	traffic = (object_t*)appdataIC.ICAROUSMsgPtr;
        	icarous_inputTraffic(appdataIC.ic,traffic);
        	break;
        }

    }

    return;
}

void ICAROUS_Output(){

	int n=1;
	while(n>=0){
		n = icarous_GetCommand(appdataIC.ic,&appdataIC.cmd);
		if(n >= 0){
			CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &appdataIC.cmd);
			CFE_SB_SendMsg((CFE_SB_Msg_t *) &appdataIC.cmd);
		}
	}


	n = icarous_sendKinematicBands(appdataIC.ic,&appdataIC.bands);
	if(n>0){
		CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &appdataIC.bands);
		CFE_SB_SendMsg((CFE_SB_Msg_t *) &appdataIC.bands);
	}
}

int32_t IcarousTableValidationFunc(void *TblPtr){

  int32_t status = 0;

  return status;
}

