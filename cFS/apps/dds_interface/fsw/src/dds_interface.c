/*******************************************************************************
 ** File: Interface.c
 **
 ** Purpose:
 **   App to interface mavlink compatible autpilot (ardupilot), ground station
 **   and Icarous.
 **
 *******************************************************************************/
#define EXTERN

#include <Icarous_msg.h>
#include "dds_interface.h"
#include "dds_interface_version.h"
#include "dds_interface_events.h"
#include "extra/dds.h"


CFE_EVS_BinFilter_t  DDS_INTERFACE_EventFilters[] =
{  /* Event ID    mask */
		{DDS_INTERFACE_STARTUP_INF_EID,       0x0000},
		{DDS_INTERFACE_COMMAND_ERR_EID,       0x0000},
}; /// Event ID definitions

/* DDS_Interface_AppMain() -- Application entry points */
void DDS_AppMain(void){

	int32 status;
	uint32 RunStatus = CFE_ES_APP_RUN;

	DDS_INTERFACE_AppInit();


	while(CFE_ES_RunLoop(&RunStatus) == TRUE){
		status = CFE_SB_RcvMsg(&appdataDDSInt.DDS_INTERFACEMsgPtr, appdataDDSInt.DDS_INTERFACE_Pipe, CFE_SB_PEND_FOREVER);

		if (status == CFE_SUCCESS)
		{
			DDS_INTERFACE_ProcessPacket();
		}
	}

	CFE_ES_RunLoop(&RunStatus);
	DDS_INTERFACE_AppCleanUp();

	CFE_ES_ExitApp(RunStatus);
}

void DDS_INTERFACE_AppInit(void){

	memset(&appdataDDSInt,0,sizeof(appdataDDSInt_t));


	int32 status;

	// Register the app with executive services
	CFE_ES_RegisterApp();

	// Register the events
	CFE_EVS_Register(DDS_INTERFACE_EventFilters,
			sizeof(DDS_INTERFACE_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
			CFE_EVS_BINARY_FILTER);

	// Create pipe to receive SB messages
	status = CFE_SB_CreatePipe( &appdataDDSInt.DDS_INTERFACE_Pipe, /* Variable to hold Pipe ID */
				    DDS_INTERFACE_PIPE_DEPTH,    /* Depth of Pipe */
				    DDS_INTERFACE_PIPE_NAME);    /* Name of pipe */


	//Subscribe to command messages and kinematic band messages from the SB
	CFE_SB_Subscribe(ICAROUS_POSITION_MID, appdataDDSInt.DDS_INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_TRAFFIC_MID, appdataDDSInt.DDS_INTERFACE_Pipe);

	// Initialize DDS
	GPSInfo_DDSInit();

	// Send event indicating app initialization
	CFE_EVS_SendEvent (DDS_INTERFACE_STARTUP_INF_EID, CFE_EVS_INFORMATION,
						"DDS Interface Initialized. Version %d.%d",
						DDS_INTERFACE_MAJOR_VERSION,
						DDS_INTERFACE_MINOR_VERSION);
}

void DDS_INTERFACE_AppCleanUp(){

}


void DDS_INTERFACE_ProcessPacket(){
	CFE_SB_MsgId_t  MsgId;

	MsgId = CFE_SB_GetMsgId(appdataDDSInt.DDS_INTERFACEMsgPtr);
	switch (MsgId)
	{
		case ICAROUS_POSITION_MID: {

            position_t* position;
            position = (position_t*) appdataDDSInt.DDS_INTERFACEMsgPtr;
            
            GPSInfo_Send(0,
                         position->time_gps,
                         position->latitude,
                         position->longitude,
                         position->altitude_abs,
                         position->altitude_rel,
                         position->vx,
                         position->vy,
                         position->vz);

            break;
		}

        case ICAROUS_TRAFFIC_MID:{

            object_t* traffic;
            traffic = (object_t*) appdataDDSInt.DDS_INTERFACEMsgPtr;

            GPSInfo_Send(traffic->index + 1,
                         0,
                         traffic->latitude,
                         traffic->longitude,
                         traffic->altiude,
                         traffic->altiude,
                         traffic->vx,
                         traffic->vy,
                         traffic->vz);

            break;


        }
	}

	return;
}

