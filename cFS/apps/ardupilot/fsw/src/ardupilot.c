/**
 * @file ardupilot.c
 * @brief function definitions for ardupilot app
 */

#define EXTERN

#include <msgdef/ardupilot_msg.h>
#include "ardupilot.h"
#include "ardupilot_table.h"
#include "ardupilot_version.h"
#include <fcntl.h>   // File control definitions
#include <termios.h> // POSIX terminal control definitions

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


	status = CFE_ES_CreateChildTask( &task_1_id, "Task 1", Task1, task_1_stack, TASK_1_STACK_SIZE, TASK_1_PRIORITY, 0);
	if ( status != OS_SUCCESS ){
		OS_printf("Error creating Task 1\n");
	}

	status = CFE_ES_CreateChildTask( &task_2_id, "Task 2", Task2, task_2_stack, TASK_2_STACK_SIZE, TASK_2_PRIORITY, 0);
	if ( status != OS_SUCCESS ){
		OS_printf("Error creating Task 2\n");
	}

	while(CFE_ES_RunLoop(&RunStatus) == TRUE){
		status = CFE_SB_RcvMsg(&appdataInt.INTERFACEMsgPtr, appdataInt.INTERFACE_Pipe, CFE_SB_PEND_FOREVER);

		if (status == CFE_SUCCESS)
		{
			ARDUPILOT_ProcessPacket();
		}
	}

	appdataInt.runThreads = CFE_ES_RunLoop(&RunStatus);
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

	status = CFE_SB_CreatePipe( &appdataInt.SchInterface_Pipe1, /* Variable to hold Pipe ID */
								ARDUPILOT_PIPE_DEPTH,    /* Depth of Pipe */
								SCH_ARDUPILOT_PIPE1_NAME);    /* Name of pipe */

    status = CFE_SB_CreatePipe( &appdataInt.SchInterface_Pipe2, /* Variable to hold Pipe ID */
								ARDUPILOT_PIPE_DEPTH,    /* Depth of Pipe */
                                SCH_ARDUPILOT_PIPE2_NAME);    /* Name of pipe */

	// Subscribe to wakeup messages from scheduler
	CFE_SB_Subscribe(INTERFACE_AP_WAKEUP_MID,appdataInt.SchInterface_Pipe1);
	CFE_SB_Subscribe(INTERFACE_GS_WAKEUP_MID,appdataInt.SchInterface_Pipe2);


	//Subscribe to command messages and kinematic band messages from the SB	 
	CFE_SB_Subscribe(ICAROUS_COMMANDS_MID, appdataInt.INTERFACE_Pipe);
	CFE_SB_Subscribe(ICAROUS_BANDS_TRACK_MID, appdataInt.INTERFACE_Pipe);
    CFE_SB_Subscribe(ICAROUS_STATUS_MID, appdataInt.INTERFACE_Pipe);
    CFE_SB_Subscribe(UPLINK_FLIGHTPLAN_MID,appdataInt.INTERFACE_Pipe);


	// Initialize all messages that this App generates
	CFE_SB_InitMsg(&fpdata,ICAROUS_FLIGHTPLAN_MID,sizeof(flightplan_t),TRUE);
	CFE_SB_InitMsg(&wpreached,ICAROUS_WPREACHED_MID,sizeof(missionItemReached_t),TRUE);
	CFE_SB_InitMsg(&gfdata,ICAROUS_GEOFENCE_MID,sizeof(geofence_t),TRUE);
	CFE_SB_InitMsg(&startMission,ICAROUS_STARTMISSION_MID,sizeof(argsCmd_t),TRUE);
	CFE_SB_InitMsg(&resetIcarous,ICAROUS_RESET_MID,sizeof(noArgsCmd_t),TRUE);
	CFE_SB_InitMsg(&traffic,ICAROUS_TRAFFIC_MID,sizeof(object_t),TRUE);	
	CFE_SB_InitMsg(&position,ICAROUS_POSITION_MID,sizeof(position_t),TRUE);	
	CFE_SB_InitMsg(&ack,ICAROUS_COMACK_MID,sizeof(cmdAck_t),TRUE);

	// Send event indicating app initialization
	CFE_EVS_SendEvent (ARDUPILOT_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                       "Ardupilot Interface initialized. Version %d.%d",
					   ARDUPILOT_INTERFACE_MAJOR_VERSION,
					   ARDUPILOT_INTERFACE_MINOR_VERSION);


	// Register table with table services
	status = CFE_TBL_Register(&appdataInt.INTERFACE_tblHandle,
				  "InterfaceTable",
				  sizeof(ArdupilotTable_t),
				  CFE_TBL_OPT_DEFAULT,
				  &ArdupilotTableValidationFunc);

	// Load app table data
	status = CFE_TBL_Load(appdataInt.INTERFACE_tblHandle,CFE_TBL_SRC_FILE,"/cf/intf_tbl.tbl");

	// Check which port to open from user defined parameters
	ArdupilotTable_t *TblPtr;
	status = CFE_TBL_GetAddress((void**)&TblPtr,appdataInt.INTERFACE_tblHandle);

	char apName[50],gsName[50];

	appdataInt.ap.id = 0;
	appdataInt.waypointSeq = 0;
	appdataInt.nextWaypointIndex = 0;
	appdataInt.ap.portType = TblPtr->apPortType;
    appdataInt.ap.baudrate = TblPtr->apBaudRate;
	appdataInt.ap.portin   = TblPtr->apPortin;
	appdataInt.ap.portout  = TblPtr->apPortout;
	memcpy(appdataInt.ap.target,TblPtr->apAddress,50);

	appdataInt.gs.id = 1;
	appdataInt.gs.portType = TblPtr->gsPortType;
    appdataInt.gs.baudrate = TblPtr->gsBaudRate;
	appdataInt.gs.portin   = TblPtr->gsPortin;
	appdataInt.gs.portout  = TblPtr->gsPortout;
	memcpy(appdataInt.gs.target,TblPtr->gsAddress,50);

    //Set mission start flag to -1
    startMission.param1 = -1;

	// Free table pointer
	status = CFE_TBL_ReleaseAddress(appdataInt.INTERFACE_tblHandle);

    //OS_printf("Port types: %d, %d\n",appdataInt.ap.portType,appdataInt.gs.portType);

	if (appdataInt.ap.portType == SOCKET){
		InitializeSocketPort(&appdataInt.ap);
	}else if(appdataInt.ap.portType == SERIAL){
		InitializeSerialPort(&appdataInt.ap,false);
	}

	if (appdataInt.gs.portType == SOCKET){
		InitializeSocketPort(&appdataInt.gs);
	}else if(appdataInt.gs.portType == SERIAL){
		InitializeSerialPort(&appdataInt.gs,false);
	}



	status = OS_MutSemCreate( &appdataInt.mutex_read, "InterfaceMRead", 0);
	if ( status != OS_SUCCESS )
		OS_printf("Error creating mutex1\n");

	status = OS_MutSemCreate( &appdataInt.mutex_read, "InterfaceMWrite", 0);
	if ( status != OS_SUCCESS )
		OS_printf("Error creating mutex2\n");

	appdataInt.waypoint_type = (int*)malloc(sizeof(int)*2);
	appdataInt.startWPUplink = false;
	appdataInt.startWPDownlink = false;
	appdataInt.downlinkRequestIndex = 0;
}

void ARDUPILOT_AppCleanUp(){
	free((void*)appdataInt.waypoint_type);
}

void Task1(void){
	//OS_printf("Starting read task\n");
	OS_TaskRegister();
	while(appdataInt.runThreads){
        int32 status = CFE_SB_RcvMsg(&appdataInt.Sch_MsgPtr1, appdataInt.SchInterface_Pipe1, CFE_SB_PEND_FOREVER);
        if (status == CFE_SUCCESS)
        {
            CFE_SB_MsgId_t  MsgId;
            MsgId = CFE_SB_GetMsgId(appdataInt.Sch_MsgPtr1);
            switch (MsgId){
                case INTERFACE_AP_WAKEUP_MID:
                    //OS_printf("received ap wakeup msg\n");
                    for(int i=0;i<10;i++)
                        GetMAVLinkMsgFromAP();
                    break;

            }
        }
	}
}

void Task2(void){
	//OS_printf("Starting write task\n");
	OS_TaskRegister();
	while(appdataInt.runThreads){
        int32 status = CFE_SB_RcvMsg(&appdataInt.Sch_MsgPtr2, appdataInt.SchInterface_Pipe2, CFE_SB_PEND_FOREVER);
        if (status == CFE_SUCCESS)
        {
            CFE_SB_MsgId_t  MsgId;
            MsgId = CFE_SB_GetMsgId(appdataInt.Sch_MsgPtr2);
            switch (MsgId){
                case INTERFACE_GS_WAKEUP_MID:
                    GetMAVLinkMsgFromGS();
                    break;
            }
        }
    }
}


void InitializeSocketPort(port_t* prt){
	int32_t                     CFE_SB_status;
	uint16_t                   size;

	memset(&prt->self_addr, 0, sizeof(prt->self_addr));
	prt->self_addr.sin_family      = AF_INET;
	prt->self_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	prt->self_addr.sin_port        = htons(prt->portin);

	// Open a UDP socket
	if ( (prt->sockId = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		OS_printf("couldn't open socket\n");
	}

	// Bind the socket to a specific port
	if (bind(prt->sockId, (struct sockaddr *)&prt->self_addr, sizeof(prt->self_addr)) < 0) {
		// Handle case where binding failed.
		OS_printf("couldn't bind socket\n");
	}

	// Setup output port address
	memset(&prt->target_addr, 0, sizeof(prt->target_addr));
	prt->target_addr.sin_family      = AF_INET;
	prt->target_addr.sin_addr.s_addr = inet_addr(prt->target);
	prt->target_addr.sin_port        = htons(prt->portout);

	fcntl(prt->sockId, F_SETFL, O_NONBLOCK);

	//OS_printf("Sock id: %d,Address: %s,Port in:%d,out: %d\n",prt->sockId,prt->target,prt->portin,prt->portout);
}

int InitializeSerialPort(port_t* prt,bool should_block){

	prt->id = open (prt->target, O_RDWR | O_NOCTTY | O_SYNC);
	if (prt->id < 0)
	{
		OS_printf("Error operning port\n");
		return -1;
	}

	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (prt->id, &tty) != 0)
	{
		OS_printf("error in tcgetattr 1\n");
		return -1;
	}

	cfsetospeed (&tty, prt->baudrate);
	cfsetispeed (&tty, prt->baudrate);

	tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;         /* 8-bit characters */
	tty.c_cflag &= ~PARENB;     /* no parity bit */
	tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */


	/* setup for non-canonical mode */
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;

	/* fetch bytes as they become available */
	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 1;                      // 0.5 seconds read timeout


	if (tcsetattr (prt->id, TCSANOW, &tty) != 0)
	{
		OS_printf("error from tcsetattr 2\n");
		return -1;
	}

    OS_printf("Opened serial port %s\n",prt->target);

}

int readPort(port_t* prt){

	OS_MutSemTake(appdataInt.mutex_read);
	int n = 0;
	if (prt->portType == SOCKET){
		memset(prt->recvbuffer, 0, BUFFER_LENGTH);
		n = recvfrom(prt->sockId, (void *)prt->recvbuffer, BUFFER_LENGTH, 0, (struct sockaddr *)&prt->target_addr, &prt->recvlen);
	}else if(prt->portType == SERIAL){
		n = read (prt->id, prt->recvbuffer, BUFFER_LENGTH);
	}else{

	}
	OS_MutSemGive(appdataInt.mutex_read);

	return n;
}


void writePort(port_t* prt,mavlink_message_t* message){

	char sendbuffer[300];
	uint16_t len = mavlink_msg_to_send_buffer((uint8_t*)sendbuffer, message);
	OS_MutSemTake(appdataInt.mutex_write);
	if(prt->portType == SOCKET){
		int n = sendto(prt->sockId, sendbuffer, len, 0, (struct sockaddr*)&prt->target_addr, sizeof (struct sockaddr_in));
	}else if(prt->portType == SERIAL){
		for(int i=0;i<len;i++){
			char c = sendbuffer[i];
			write(prt->id,&c,1);
		}
	}else{
		// unimplemented port type
	}
	OS_MutSemGive(appdataInt.mutex_write);

}

int GetMAVLinkMsgFromAP(){
	int n = readPort(&appdataInt.ap);
	mavlink_message_t message;
	mavlink_status_t status;
	uint8_t msgReceived = 0;
	for(int i=0;i<n;i++){
		uint8_t cp = appdataInt.ap.recvbuffer[i];
		msgReceived = mavlink_parse_char(MAVLINK_COMM_0, cp, &message, &status);
		if(msgReceived){
			// Send message to ground station
			writePort(&appdataInt.gs,&message);
			// Send SB message if necessary
			ProcessAPMessage(message);
		}
	}

	return n;
}

int GetMAVLinkMsgFromGS(){
	int n = readPort(&appdataInt.gs);
	mavlink_message_t message;
	mavlink_status_t status;
	uint8_t msgReceived = 0;
	for(int i=0;i<n;i++){
		uint8_t cp = appdataInt.gs.recvbuffer[i];
		msgReceived = mavlink_parse_char(MAVLINK_COMM_1, cp, &message, &status);
		if(msgReceived){
			bool val = ProcessGSMessage(message);

            // Send message to autopilot
            if(val) {
                writePort(&appdataInt.ap, &message);
            }
		}
	}
	return n;
}

int32_t ArdupilotTableValidationFunc(void *TblPtr){

  int32_t status = 0;

  return status;
}


