/**
 * @file cognition.c
 * @brief function definitions for cognition app
 */
#define EXTERN 

#include "cognition.h"
#include "cognition_version.h"
#include "cog_tbl.c"

/// Event filter definition for ardupilot
CFE_EVS_BinFilter_t  Cognition_EventFilters[] =
{  /* Event ID    mask */
		{COGNITION_STARTUP_INF_EID,       0x0000},
		{COGNITION_COMMAND_ERR_EID,       0x0000},
};

/* APINTERFACE_AppMain() -- Application entry points */
void COGNITION_AppMain(void){

	int32 status;
	uint32 RunStatus = CFE_ES_APP_RUN;

    // All functions to initialize app and open  connection to autopilot goes in here
    COGNITION_AppInit();

	while(CFE_ES_RunLoop(&RunStatus) == TRUE) {
		status = CFE_SB_RcvMsg(&appdataCog.SchMsgPtr, appdataCog.SchPipe, CFE_SB_POLL);
		if (status == CFE_SUCCESS) {
			CFE_SB_MsgId_t MsgId;
			MsgId = CFE_SB_GetMsgId(appdataCog.SchMsgPtr);
			switch (MsgId) {

				case FREQ_01_WAKEUP_MID:
                    COGNITION_DecisionProcess();
					break;
			}
		}

        // Get data from Software bus and send necessary commands to autopilot
		status = CFE_SB_RcvMsg(&appdataCog.CogMsgPtr, appdataCog.CognitionPipe, 10);

		if (status == CFE_SUCCESS)
		{
			COGNITION_ProcessSBData();
		}
	}

    COGNITION_AppCleanUp();

	CFE_ES_ExitApp(RunStatus);
}

void COGNITION_AppInit(void){

	memset(&appdataCog,0,sizeof(appdataCog));

	int32 status;

	// Register the app with executive services
	CFE_ES_RegisterApp();

	// Register the events
	CFE_EVS_Register(Cognition_EventFilters,
			sizeof(Cognition_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
			CFE_EVS_BINARY_FILTER);

	// Create pipe to receive SB messages
	status = CFE_SB_CreatePipe( &appdataCog.CognitionPipe,     /* Variable to hold Pipe ID */
								COGNITION_PIPE_DEPTH,          /* Depth of Pipe */
								COGNITION_PIPE_NAME);          /* Name of pipe */

	// Create pipe to receive scheduler messages
	status = CFE_SB_CreatePipe( &appdataCog.SchPipe,            /* Variable to hold Pipe ID */
								COGNITION_PIPE_DEPTH,           /* Depth of Pipe */
								SCH_COGNITION_PIPE_NAME);       /* Name of pipe */

	// Subscribe to wakeup messages from scheduler
	CFE_SB_Subscribe(FREQ_01_WAKEUP_MID,appdataCog.SchPipe);

	// Subscribe to messages from the software bus
	//Subscribe to command messages from the SB to command the autopilot
    CFE_SB_Subscribe(ICAROUS_POSITION_MID, appdataCog.CognitionPipe);
	CFE_SB_Subscribe(ICAROUS_COMMANDS_MID, appdataCog.CognitionPipe);
    CFE_SB_Subscribe(ICAROUS_FLIGHTPLAN_MID,appdataCog.CognitionPipe);
    CFE_SB_Subscribe(ICAROUS_BANDS_TRACK_MID,appdataCog.CognitionPipe);
    CFE_SB_Subscribe(ICAROUS_BANDS_SPEED_MID,appdataCog.CognitionPipe);
    CFE_SB_Subscribe(ICAROUS_BANDS_ALT_MID,appdataCog.CognitionPipe);
    CFE_SB_Subscribe(ICAROUS_BANDS_VS_MID,appdataCog.CognitionPipe);

	// Initialize all messages that this App generates.
	// To perfrom sense and avoid, as a minimum, the following messages must be generated

	// Send event indicating app initialization
	CFE_EVS_SendEvent (COGNITION_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                       "Cognition Interface initialized. Version %d.%d",
					   COGNITION_MAJOR_VERSION,
					   COGNITION_MINOR_VERSION);

	// Register table with table services
	status = CFE_TBL_Register(&appdataCog.CogtblHandle,
				  "CognitionTable",
				  sizeof(cognitionTable_t),
				  CFE_TBL_OPT_DEFAULT,
				  &cognitionTableValidationFunc);

	// Load app table data for ports
	status = CFE_TBL_Load(appdataCog.CogtblHandle,CFE_TBL_SRC_ADDRESS,&cognition_TblStruct);

	cognitionTable_t *TblPtr;
	status = CFE_TBL_GetAddress((void**)&TblPtr,appdataCog.CogtblHandle);

    // Get talble data data

	// Free table pointer
	status = CFE_TBL_ReleaseAddress(appdataCog.CogtblHandle);

}


void COGNITION_ProcessSBData() {
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(appdataCog.CogMsgPtr);
    switch (MsgId)
    {
        case ICAROUS_FLIGHTPLAN_MID:{
            flightplan_t* fplan = (flightplan_t*)appdataCog.CogMsgPtr;
            memcpy(&appdataCog.flightplan,fplan,sizeof(flightplan_t));

            //TODO: The flight plan icarous uses is stored in appdataCog.flightplan
            break;
        }

        case ICAROUS_BANDS_TRACK_MID:{
            bands_t* bands = (bands_t*) appdataCog.CogMsgPtr;

            //TODO: Track bands can be displayed to the piot if there is an interface
            break;
        }

        case ICAROUS_BANDS_SPEED_MID:{
            bands_t* bands = (bands_t*) appdataCog.CogMsgPtr;

            //TODO: speed bands can be displayed to the piot if there is an interface
            break;
        }

        case ICAROUS_BANDS_ALT_MID:{
            bands_t* bands = (bands_t*) appdataCog.CogMsgPtr;

            //TODO: alt bands
            break;
        }

        case ICAROUS_BANDS_VS_MID:{
            bands_t* bands = (bands_t*) appdataCog.CogMsgPtr;

            //TODO: vertical speed bands
            break;
        }

    }
    return;
}

void COGNITION_AppCleanUp(){
    //TODO: clean up memory allocation here if necessary
}

int32_t cognitionTableValidationFunc(void *TblPtr){
  int32_t status = 0;
  return status;
}
