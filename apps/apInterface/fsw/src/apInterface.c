/**
 * @file apInterface.c
 * @brief function definitions for apInterface app
 */

#include <Icarous_msg.h>
#include "apInterface.h"
#include "apInterface_version.h"
#include "apIntf_tbl.c"

/// Event filter definition for ardupilot
CFE_EVS_BinFilter_t  APInterface_EventFilters[] =
{  /* Event ID    mask */
		{APINTERFACE_STARTUP_INF_EID,       0x0000},
		{APINTERFACE_COMMAND_ERR_EID,       0x0000},
};

/* APINTERFACE_AppMain() -- Application entry points */
void APINTERFACE_AppMain(void){

	int32 status;
	uint32 RunStatus = CFE_ES_APP_RUN;

    // All functions to initialize app and open  connection to autopilot goes in here
    APINTERFACE_AppInit();

	while(CFE_ES_RunLoop(&RunStatus) == TRUE) {
		status = CFE_SB_RcvMsg(&appdataApIntf.Sch_MsgPtr, appdataApIntf.SchInterface_Pipe, CFE_SB_PEND_FOREVER);
		if (status == CFE_SUCCESS) {
			CFE_SB_MsgId_t MsgId;
			MsgId = CFE_SB_GetMsgId(appdataApIntf.Sch_MsgPtr);
			switch (MsgId) {

				case FREQ_50_WAKEUP_MID:
					break;


				case FREQ_30_WAKEUP_MID:
					break;

				case FREQ_10_WAKEUP_MID:
                    APINTERFACE_ProcessAPData();
					break;

				case FREQ_01_WAKEUP_MID:
				    //TODO: If the autopilot doesn't provide ADSB data and you
                    // have to rely on other sources, you may have to read
                    // that data separately. Those functions can go in here.
				    //APINTERFACE_GetADSBData();
					break;
			}
		}

        // Get data from Software bus and send necessary commands to autopilot
		status = CFE_SB_RcvMsg(&appdataApIntf.INTERFACEMsgPtr, appdataApIntf.INTERFACE_Pipe, 10);

		if (status == CFE_SUCCESS)
		{
			APINTERFACE_ProcessSBData();
		}
	}

    APINTERFACE_AppCleanUp();

	CFE_ES_ExitApp(RunStatus);
}

void APINTERFACE_AppInit(void){

	memset(&appdataApIntf,0,sizeof(appdataInt_t));

	int32 status;

	// Register the app with executive services
	CFE_ES_RegisterApp();

	// Register the events
	CFE_EVS_Register(APInterface_EventFilters,
			sizeof(APInterface_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
			CFE_EVS_BINARY_FILTER);

	// Create pipe to receive SB messages
	status = CFE_SB_CreatePipe( &appdataApIntf.INTERFACE_Pipe, /* Variable to hold Pipe ID */
								APINTERFACE_PIPE_DEPTH,        /* Depth of Pipe */
								APINTERFACE_PIPE_NAME);        /* Name of pipe */

	// Create pipe to receive scheduler messages
	status = CFE_SB_CreatePipe( &appdataApIntf.SchInterface_Pipe, /* Variable to hold Pipe ID */
								APINTERFACE_PIPE_DEPTH,           /* Depth of Pipe */
								SCH_APINTERFACE_PIPE1_NAME);      /* Name of pipe */

	// Subscribe to wakeup messages from scheduler
	CFE_SB_Subscribe(FREQ_50_WAKEUP_MID,appdataApIntf.SchInterface_Pipe);
	CFE_SB_Subscribe(FREQ_30_WAKEUP_MID,appdataApIntf.SchInterface_Pipe);
	CFE_SB_Subscribe(FREQ_10_WAKEUP_MID,appdataApIntf.SchInterface_Pipe);
	CFE_SB_Subscribe(FREQ_01_WAKEUP_MID,appdataApIntf.SchInterface_Pipe);

	// Subscribe to messages from the software bus
	//Subscribe to command messages from the SB to command the autopilot
	CFE_SB_Subscribe(ICAROUS_COMMANDS_MID, appdataApIntf.INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_FLIGHTPLAN_MID,appdataApIntf.INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_BANDS_TRACK_MID,appdataApIntf.INTERFACE_Pipe);
    //CFE_SB_Subscribe(ICAROUS_BANDS_SPEED_MID,appdataApIntf.INTERFACE_Pipe);
    //CFE_SB_Subscribe(ICAROUS_BANDS_ALT_MID,appdataApIntf.INTERFACE_Pipe);
    //CFE_SB_Subscribe(ICAROUS_BANDS_VS_MID,appdataApIntf.INTERFACE_Pipe);

	// Initialize all messages that this App generates.
	// To perfrom sense and avoid, as a minimum, the following messages must be generated
	CFE_SB_InitMsg(&appdataApIntf.wpReached,ICAROUS_WPREACHED_MID,sizeof(missionItemReached_t),TRUE);
	CFE_SB_InitMsg(&appdataApIntf.position,ICAROUS_POSITION_MID,sizeof(position_t),TRUE);
	CFE_SB_InitMsg(&appdataApIntf.attitude,ICAROUS_ATTITUDE_MID,sizeof(attitude_t),TRUE);
	CFE_SB_InitMsg(&appdataApIntf.traffic,ICAROUS_TRAFFIC_MID,sizeof(object_t),TRUE);

	// Send event indicating app initialization
	CFE_EVS_SendEvent (APINTERFACE_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                       "Autopilot Interface initialized. Version %d.%d",
					   APINTERFACE_MAJOR_VERSION,
					   APINTERFACE_MINOR_VERSION);

	// Register table with table services
	status = CFE_TBL_Register(&appdataApIntf.INTERFACE_tblHandle,
				  "APInterfaceTable",
				  sizeof(ApInterfaceTable_t),
				  CFE_TBL_OPT_DEFAULT,
				  &ApInterfaceTableValidationFunc);

	// Load app table data for ports
	status = CFE_TBL_Load(appdataApIntf.INTERFACE_tblHandle,CFE_TBL_SRC_ADDRESS,&ApInterface_TblStruct);

	ApInterfaceTable_t *TblPtr;
	status = CFE_TBL_GetAddress((void**)&TblPtr,appdataApIntf.INTERFACE_tblHandle);

	// Get data from tables
	appdataApIntf.ap.id = 0;
	appdataApIntf.ap.portType = TblPtr->PortType;
    appdataApIntf.ap.baudrate = TblPtr->BaudRate;
	appdataApIntf.ap.portin   = TblPtr->Portin;
	appdataApIntf.ap.portout  = TblPtr->Portout;
	memcpy(appdataApIntf.ap.target,TblPtr->Address,50);

	// Free table pointer
	status = CFE_TBL_ReleaseAddress(appdataApIntf.INTERFACE_tblHandle);

	// Use parameters obtained from the tables to open ports here
    APINTERFACE_InitializeAPPorts();


    // Initialize app data here
    appdataApIntf.currentWP = 0;
}

void APINTERFACE_InitializeAPPorts(){

   // Check which port to open from user defined parameters
   if (appdataApIntf.ap.portType == SOCKET){
		InitializeSocketPort(&appdataApIntf.ap);
   }else if(appdataApIntf.ap.portType == SERIAL){
		InitializeSerialPort(&appdataApIntf.ap,false);
   }
}

void APINTERFACE_ProcessAPData() {

   //TODO: Get position data from autopilot

   //TODO: Populate the position data in the appdataApIntf.position message

   // Publish the position data
   SendSBMsg(appdataApIntf.position);

   //TODO: Get attitude data from autopilot
   // Publish the attitude data
   SendSBMsg(appdataApIntf.attitude);


   bool waypointReached = FALSE;
   int currentWP = appdataApIntf.currentWP;
   double currentWPlat = appdataApIntf.flightplan.waypoints[currentWP].latitude;
   double currentWPlon = appdataApIntf.flightplan.waypoints[currentWP].longitude;
   double currentWPalt = appdataApIntf.flightplan.waypoints[currentWP].altitude;


   //TODO: Use the current position and check if the current waypoint
   //      has been reached and set waypointReached accordingly.
   //      You can avoid calculating manually the value of waypointReached if the
   //      autopilot gives you this data directly.

   if(waypointReached) {
       appdataApIntf.wpReached.feedback = TRUE;
       appdataApIntf.wpReached.reachedwaypoint = (uint8_t) appdataApIntf.currentWP;
       appdataApIntf.currentWP++;
       SendSBMsg(appdataApIntf.wpReached);
   }

   //TODO: ADSB data
   // If autopilot provides this data, you can construct and send the traffic message
   // right here. Traffic data should be populated in appdataApIntf.traffic
   // Publish ADSB data
   // SendSBMsg(appdataApIntf.traffic);
}

void APINTERFACE_GetADSBData(){


    //TODO: Get ADSB data and populate appdataApIntf.traffic

    // Publish ADSB data
    //SendSBMsg(appdataApIntf.traffic)
}

void APINTERFACE_ProcessSBData() {
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(appdataApIntf.INTERFACEMsgPtr);
    switch (MsgId)
    {
        case ICAROUS_FLIGHTPLAN_MID:{
            flightplan_t* fplan = (flightplan_t*)appdataApIntf.INTERFACEMsgPtr;
            memcpy(&appdataApIntf.flightplan,fplan,sizeof(flightplan_t));

            //TODO: The flight plan icarous uses is stored in appdataApIntf.flightplan
            break;
        }

        case ICAROUS_BANDS_TRACK_MID:{
            bands_t* bands = (bands_t*) appdataApIntf.INTERFACEMsgPtr;

            //TODO: Track bands can be displayed to the piot if there is an interface
            break;
        }

        case ICAROUS_BANDS_SPEED_MID:{
            bands_t* bands = (bands_t*) appdataApIntf.INTERFACEMsgPtr;

            //TODO: speed bands can be displayed to the piot if there is an interface
            break;
        }

        case ICAROUS_BANDS_ALT_MID:{
            bands_t* bands = (bands_t*) appdataApIntf.INTERFACEMsgPtr;

            //TODO: alt bands
            break;
        }

        case ICAROUS_BANDS_VS_MID:{
            bands_t* bands = (bands_t*) appdataApIntf.INTERFACEMsgPtr;

            //TODO: vertical speed bands
            break;
        }

        case ICAROUS_COMMANDS_MID:
        {
            argsCmd_t *cmd = (argsCmd_t*) appdataApIntf.INTERFACEMsgPtr;

            switch (cmd->name) {

                case _SETMODE_:
                {
                    if ((int)cmd->param1 == _PASSIVE_) {
                    //TODO: Handle mode change to passive command here

                    }else if ((int)cmd->param1 == _ACTIVE_) {
                    //TODO: Handle mode change to active command here
                    }
                    break;
                }

                case _GOTOWP_:
                {

                    int tempSeq = (int)cmd->param1; // Next active waypoint

                    if(tempSeq>0) {
                        appdataApIntf.wpReached.reachedwaypoint = (uint8_t) (tempSeq - 1);
                        appdataApIntf.wpReached.feedback = false;
                    }
                    SendSBMsg(appdataApIntf.wpReached);


                    //TODO: send change active waypoint command to autopilot

                    break;
                }

                case _SETPOS_:
                {
                    double commandlat = cmd->param1; // commanded latitude in degrees
                    double commandlon = cmd->param2; // commanded longitude in degrees
                    double commandalt = cmd->param3; // commanded altitude (AGL) in m

                    //TODO: send set position command to autopilot
                    break;
                }

                case _SETVEL_:
                {

                    double commandVn = cmd->param1; // commanded Velocity north component (m/s)
                    double commandVe = cmd->param2; // commanded Velocity east component (m/s)
                    double commandVd = cmd->param3; // commanded Velocity down component (m/s)
                    double heading = fmod(2*M_PI + atan2(commandVe,commandVn),2*M_PI)*180/M_PI;
                    double speed = sqrt( commandVn*commandVn + commandVe*commandVe + commandVd*commandVd );

                    //TODO: send set Vn,Ve,Vd velocity components command to autopilot or
                    // send heading and speed to autopilot
                    break;
                }

                case _SETSPEED_:
                {
                    double speed = cmd->param1; // commanded speed in m/s

                    //TODO: send speed change command to autopilot
                    break;
                }


                default:{

                }

            }
            break;
        }
    }
    return;
}

void APINTERFACE_AppCleanUp(){
    //TODO: clean up memory allocation here if necessary
}

int32_t ApInterfaceTableValidationFunc(void *TblPtr){
  int32_t status = 0;
  return status;
}
