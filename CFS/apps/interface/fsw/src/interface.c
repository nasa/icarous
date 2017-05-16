/*******************************************************************************
** File: Interface.c
**
** Purpose:
**   App to interface mavlink compatible autpilot (ardupilot), ground station 
**   and Icarous.
**
*******************************************************************************/
#include "interface.h"

#include "interface_events.h"
#include "interface_table.h"
#include "interface_version.h"
#include "interface_perfids.h"
#include "network_includes.h"
#include "mavlink/icarous/mavlink.h"

CFE_SB_PipeId_t    INTERFACE_CommandPipe;
CFE_SB_MsgPtr_t    INTERFACEsgPtr;
CFE_TBL_Handle_t   INTERFACE_tblHandle;

struct sockaddr_in target_addr;
int apSockId;
int apPort;
socklen_t recvlen;
uint8_t runThreads = 1;

static CFE_EVS_BinFilter_t  SAMPLE_EventFilters[] =
       {  /* Event ID    mask */
          {INTERFACE_STARTUP_INF_EID,       0x0000},
          {INTERFACE_COMMAND_ERR_EID,       0x0000},
       };


void INTERFACE_AppInit(void){

	// Register the app with executive services
	CFE_ES_RegisterApp();

	// Register the events
	CFE_EVS_Register(SAMPLE_EventFilters,
			sizeof(SAMPLE_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
			CFE_EVS_BINARY_FILTER);

	CFE_SB_CreatePipe(&INTERFACE_CommandPipe, INTERFACE_PIPE_DEPTH,"INTERFACE_CMD_PIPE");

	// Initialize all messages that this App generates
	//  ....
	//  ....

	CFE_EVS_SendEvent (INTERFACE_STARTUP_INF_EID, CFE_EVS_INFORMATION,
	               "Interface Initialized. Version %d.%d",
	                INTERFACE_MAJOR_VERSION,
	                INTERFACE_MINOR_VERSION);

	// Register table with table services
	int32 status = CFE_TBL_Register(&INTERFACE_tblHandle,
									"InterfaceTable",
									sizeof(InterfaceTable_t),
									CFE_TBL_OPT_DEFAULT,
									&IcarousTableValidationFunc);

	status = CFE_TBL_Load(INTERFACE_tblHandle,CFE_TBL_SRC_FILE,"/cf/apps/intf_tbl.tbl");

	// Check which port to open from user defined parameters
	InterfaceTable_t *TblPtr;
	status = CFE_TBL_GetAddress(&TblPtr,INTERFACE_tblHandle);

	char apName[50],gsName[50];

	PortType_t apPortType = TblPtr->apPortType;
	uint16_t apPort        = TblPtr->apPort;
	memcpy(apName,TblPtr->apAddress,50);

	PortType_t gsPortType = TblPtr->gsPortType;
	uint16_t gsPort        = TblPtr->gsPort;
	memcpy(gsName,TblPtr->gsAddress,50);



	// Free table pointer
	status = CFE_TBL_ReleaseAddress(INTERFACE_tblHandle);

	if (apPortType == SOCKET){
		InitializeSocketPort(apName,apPort,0);
	}else if(apPortType == SERIAL){
		//InitializeSerialPort();
	}

	if (gsPortType == SOCKET){
		//InitializeSocketPort();
	}else if(gsPortType == SERIAL){
		//InitializeSocketPort();
	}
}

/* Interface_AppMain() -- Application entry points */
void INTERFACE_AppMain(void){

  int32 status;
  uint32 RunStatus = CFE_ES_APP_RUN;

  CFE_ES_PerfLogEntry(INTERFACE_PERF_ID);

  INTERFACE_AppInit();


  status = OS_TaskCreate( &task_1_id, "Task 1", readTask, task_1_stack, TASK_1_STACK_SIZE, TASK_1_PRIORITY, 0);
  if ( status != OS_SUCCESS ){
	  OS_printf("Error creating Task 1\n");
  }

  status = OS_TaskCreate( &task_2_id, "Task 2", writeTask, task_2_stack, TASK_2_STACK_SIZE, TASK_2_PRIORITY, 0);
  if ( status != OS_SUCCESS ){
  	 OS_printf("Error creating Task 2\n");
  }

  while(CFE_ES_RunLoop(&RunStatus) == TRUE){

  }

  runThreads = CFE_ES_RunLoop(&RunStatus);

  CFE_ES_ExitApp(RunStatus);
}

void InitializeSocketPort(char targetip[], int inportno, int outportno){

	static struct sockaddr_in s_addr;
	int32                     CFE_SB_status;
	uint16                    size;

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_family      = AF_INET;
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_port        = htons(inportno);

	// Open a UDP socket
	if ( (apSockId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		OS_printf("couldn't open socket\n");
	}

	// Bind the socket to a specific port
	if (bind(apSockId, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0) {
		// Handle case where binding failed.
		OS_printf("couldn't bind socket\n");
	}

	// Setup output port address
	memset(&target_addr, 0, sizeof(target_addr));
	target_addr.sin_family      = AF_INET;
	target_addr.sin_addr.s_addr = inet_addr(targetip);
	target_addr.sin_port        = htons(outportno);
}

int readSocket(){

    int n = 0;
    memset(recvbuffer, 0, BUFFER_LENGTH);
    n = recvfrom(apSockId, (void *)recvbuffer, BUFFER_LENGTH, 0, (struct sockaddr *)&target_addr, &recvlen);
    return n;
}

int GetMAVLinkMsg(){

    int n = readSocket();
    mavlink_message_t message;
    mavlink_status_t status;

    uint8_t msgReceived = 0;

    for(int i=0;i<n;i++){
        uint8_t cp = recvbuffer[i];
        msgReceived = mavlink_parse_char(MAVLINK_COMM_0, cp, &message, &status);
        if(msgReceived){

        }
    }

    return n;
}

void readTask(void){

	OS_printf("Starting read task\n");

	OS_TaskRegister();

	uint32 RunStatus = CFE_ES_APP_RUN;

	while(runThreads){
		GetMAVLinkMsg();
	}
}

void writeTask(void){
	OS_printf("Starting write task\n");

	OS_TaskRegister();

	uint32 RunStatus = CFE_ES_APP_RUN;

	while(runThreads){

	}
}
