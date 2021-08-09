/**
 * @file flarm.c
 * @brief function definitions for FLARM app
 */

#include "Icarous_msg.h"
#include "Interfaces.h"
#include <stdbool.h>
#include "flarm.h"
#include "flarm_version.h"
#include "flarm_tbl.c"
#include "pflaa.h"

static FILE* FLARM_SENTENCE;
bool firstpass_FLAG = true;


FILE* flarminput;

/// Event filter definition for ardupilot
CFE_EVS_BinFilter_t  FLARM_EventFilters[] =
{  /* Event ID    mask */
	{FLARM_STARTUP_INF_EID,       0x0000},
	{FLARM_COMMAND_ERR_EID,       0x0000},
};

FlarmAppData_t flarmAppData;

/* FLARM_AppMain() -- Application entry points */
void FLARM_AppMain(void){

	int32 status;
	uint32 RunStatus = CFE_ES_APP_RUN;

	// All functions to initialize app and open  connection to autopilot goes in here
	FLARM_AppInit();

	while(CFE_ES_RunLoop(&RunStatus) == TRUE) {
		status = CFE_SB_RcvMsg(&flarmAppData.Sch_MsgPtr, flarmAppData.SchInterface_Pipe, CFE_SB_POLL);
		if (status == CFE_SUCCESS) {
			CFE_SB_MsgId_t MsgId;
			MsgId = CFE_SB_GetMsgId(flarmAppData.Sch_MsgPtr);
			switch (MsgId) {
				case FREQ_10_WAKEUP_MID:
					FLARM_ProcessData();
					break;
			}
		}

		// Get data from Software bus and send necessary commands to autopilot
		status = CFE_SB_RcvMsg(&flarmAppData.INTERFACEMsgPtr, flarmAppData.INTERFACE_Pipe, 10);

		if (status == CFE_SUCCESS)
		{
			FLARM_ProcessSBData();
		}
	}

	FLARM_AppCleanUp();

	CFE_ES_ExitApp(RunStatus);
}

void FLARM_AppInit(void){

	memset(&flarmAppData,0,sizeof(FlarmAppData_t));

	int32 status;

	// Register the app with executive services
	CFE_ES_RegisterApp();

	// Register the events
	CFE_EVS_Register(FLARM_EventFilters,
			sizeof(FLARM_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
			CFE_EVS_BINARY_FILTER);

	// Create pipe to receive SB messages
	status = CFE_SB_CreatePipe( &flarmAppData.INTERFACE_Pipe, /* Variable to hold Pipe ID */
			FLARM_PIPE_DEPTH,        /* Depth of Pipe */
			FLARM_PIPE_NAME);        /* Name of pipe */

	// Create pipe to receive scheduler messages
	status = CFE_SB_CreatePipe( &flarmAppData.SchInterface_Pipe, /* Variable to hold Pipe ID */
			FLARM_PIPE_DEPTH,           /* Depth of Pipe */
			SCH_FLARM_PIPE1_NAME);      /* Name of pipe */

	// Subscribe to wakeup messages from scheduler
	CFE_SB_Subscribe(FREQ_10_WAKEUP_MID,flarmAppData.SchInterface_Pipe);

	//Subscribe to command messages and kinematic band messages from the SB
	CFE_SB_Subscribe(ICAROUS_POSITION_MID,flarmAppData.INTERFACE_Pipe);

	// Initialize all messages that this App generates.
	// To perfrom sense and avoid, as a minimum, the following messages must be generated
	CFE_SB_InitMsg(&flarmAppData.traffic,ICAROUS_TRAFFIC_MID,sizeof(object_t),TRUE);

	flarmAppData.gpsOrigin[0] = 0;
	flarmAppData.gpsOrigin[1] = 0;
	flarmAppData.gpsOrigin[2] = 0;

	flarmAppData.gpsEND[0] = 0;
	flarmAppData.gpsEND[1] = 0;
	flarmAppData.gpsEND[2] = 0;

	// Send event indicating app initialization
	CFE_EVS_SendEvent (FLARM_STARTUP_INF_EID, CFE_EVS_INFORMATION,
			"Flarm Interface initialized. Version %d.%d",
			FLARM_MAJOR_VERSION,
			FLARM_MINOR_VERSION);

	// Register table with table services
	status = CFE_TBL_Register(&flarmAppData.INTERFACE_tblHandle,
			"FlarmTable",
			sizeof(FlarmTable_t),
			CFE_TBL_OPT_DEFAULT,
			&FLARMTableValidationFunc);

	// Load app table data for ports
	status = CFE_TBL_Load(flarmAppData.INTERFACE_tblHandle,CFE_TBL_SRC_ADDRESS,&Flarm_TblStruct);

	FlarmTable_t *TblPtr;
	status = CFE_TBL_GetAddress((void**)&TblPtr,flarmAppData.INTERFACE_tblHandle);

	// Get data from tables
	flarmAppData.fp.id = 1;
	flarmAppData.fp.portType = TblPtr->PortType;
	flarmAppData.fp.baudrate = TblPtr->BaudRate;
	flarmAppData.fp.portin   = TblPtr->Portin;
	flarmAppData.fp.portout  = TblPtr->Portout;
	memcpy(flarmAppData.fp.target,TblPtr->Address,50);

	// Free table pointer
	status = CFE_TBL_ReleaseAddress(flarmAppData.INTERFACE_tblHandle);

	// Use parameters obtained from the tables to open ports here
	FLARM_InitializePorts();

	// Initialize app data here
	flarmAppData.currentWP = 0;
    flarminput = fopen("flarminput.txt","w");
}

void FLARM_InitializePorts(){

	// Check which port to open from user defined parameters
	if (flarmAppData.fp.portType == SOCKET){
		InitializeSocketPort(&flarmAppData.fp);
	}else if(flarmAppData.fp.portType == SERIAL){
		InitializeSerialPort(&flarmAppData.fp,false);
	}
}

void FLARM_ProcessData() {
	int n = 0;
	bool sentenced_logged = false;
	double pos_gps[3];
	double vn = 0;
	double ve = 0;
	double vd = 0;
	const char *SentenceStart = "$PFLAA";
	const char *SentenceEnd = "\r\n";
	char *start;
	char *end;
	char fullSentence[100] = {0};

	//char rawdata[] = "$PFLAA,1,19163,26802,10296,1,A418B2,212,,222,-0.3,8*7E \r\n";

	// Pointer to struct containing the parsed data. Should be freed manually.
	nmea_pflaa_s pflaa;

	n = readPort(&flarmAppData.fp);
    //printf("bytes read: %d\n",n);

	//Grab $PFLAA sentence from buffer
	char* recvb = flarmAppData.fp.recvbuffer;

	fprintf(flarminput,"%s",recvb);
	fflush(flarminput);

    //char rawdata[500];
    //fread(rawdata,1,200,flarminput);
    //char* recvb = rawdata;
    //OS_printf("%s\n",rawdata);
  
	while(recvb != NULL){

		start = strstr( recvb, SentenceStart );
		if (start != NULL)
		{
			
			start += strlen( SentenceStart );
			end = strstr( start, SentenceEnd );
			if (end != NULL)
			{
				strcpy(fullSentence, "$PFLAA");
				strncat( fullSentence, start, end - start );
				//strncat(fullSentence, "\r\n", strlen("\r\n"));
			}else{
				recvb = end;
                                //printf("breaking in end\n");
				break;
			}
		}else{
                     //printf("breaking at start\n");
                     break;
                }
                
		recvb = end;
	
		sentenced_logged = logSentence(fullSentence, &pflaa);

		if(sentenced_logged){

			//For TESTING only
			   //printf("PFLAA Sentence\n");
			   //printf("\tAlarm Level: %d\n", pflaa.alarmLevel);
			   //printf("\tRelative North: %d\n", pflaa.relNorth);
			   //printf("\tRelative East: %d\n", pflaa.relEast);
			   //printf("\tRelative Vertical: %d\n", pflaa.relVertical);
			   //printf("\tID Type: %d\n", pflaa.idType);
			   //printf("\tID: %s\n", pflaa.ID);
			   //printf("\tTrack: %d\n", pflaa.track);
			   //printf("\tGround Speed: %d\n", pflaa.groundSpeed);
			   //printf("\tClimb Rate: %f\n", pflaa.climbRate);
			   //printf("\tAircraft Type: %s\n", pflaa.acftType);
			   //printf("\tNo Track: %d\n\n", pflaa.noTrack);
			//END TESTING

			flarmAppData.gpsOrigin[0] = flarmAppData.position.latitude;
			flarmAppData.gpsOrigin[1] = flarmAppData.position.longitude;
			flarmAppData.gpsOrigin[2] = flarmAppData.position.altitude_abs;

			flarmAppData.gpsEND[0] = pflaa.relEast;
			flarmAppData.gpsEND[1] = pflaa.relNorth;
			flarmAppData.gpsEND[2] = -pflaa.relVertical;

			ConvertEND2LLA(flarmAppData.gpsOrigin,flarmAppData.gpsEND,pos_gps);

			ConvertTrkGsVsToVned(pflaa.track, pflaa.groundSpeed, pflaa.climbRate, &vn, &ve, &vd);
			flarmAppData.traffic.type = _TRAFFIC_FLARM_;
			//flarmAppData.traffic.index = (int32_t) pflaa.ID;
                        //printf("%s\n",pflaa.ID);
			flarmAppData.traffic.index = (uint32_t) strtol(pflaa.ID, NULL, 16);
			flarmAppData.traffic.latitude = pos_gps[0];
			flarmAppData.traffic.longitude = pos_gps[1];
			flarmAppData.traffic.altitude = pos_gps[2];
			flarmAppData.traffic.ve = ve;
			flarmAppData.traffic.vn = vn;
			flarmAppData.traffic.vd = vd;

			//Publish Flarm data
			SendSBMsg(flarmAppData.traffic);

			//clear sentence buffer
			memset(fullSentence, 0, sizeof(fullSentence));

		}else{
			//printf("\n%s is not a full sentence.\n", fullSentence);

		}

	}
}

bool logSentence(char *recvbuffer, nmea_pflaa_s *pflaa){
	bool ret;
	int numComma = 0;
	int i;
	int val_index = -1;
	bool logged = false;
	char *logtime;
	const char comma = ',';
	char FLARM_LOG[50] = "FLARM_LOG_";
	char* value;
	char* acftTypeVal;
	char fullSentence[100] = {0};

	for(i = 0; i < strlen(recvbuffer); i++){
		if(recvbuffer[i] == comma){
			numComma++;
		}
	}

	if(numComma != 11){
		return false;
	}

	//Copy buffer into another buffer for modification
	strcpy(fullSentence, recvbuffer);

	//look for token in string
	value = strtok(fullSentence, ",");

	while(value != NULL){
		//OS_printf("%s\n", value);
		//OS_printf("val_index = %d\n", val_index);
		// Parse...
		switch (val_index) {
			case NMEA_PFLAA_ALARMLEVEL:
				/* Parse alarm level */
				//OS_printf("NMEA_PFLAA_ALARMLEVEL\n");
				pflaa->alarmLevel = atoi(value);
				break;

			case NMEA_PFLAA_RELATIVE_NORTH:
				/* Parse relative North */
				//OS_printf("NMEA_PFLAA_RELATIVE_NORTH\n");
				pflaa->relNorth = atoi(value);
				break;

			case NMEA_PFLAA_RELATIVE_EAST:
				//OS_printf("NMEA_PFLAA_RELATIVE_EAST\n");
				/* Parse relative East */
				pflaa->relEast = atoi(value);
				break;

			case NMEA_PFLAA_RELATIVE_VERTICAL:
				//OS_printf("NMEA_PFLAA_RELATIVE_VERTICAL\n");
				/* Parse relative Vertical */
				pflaa->relVertical = atoi(value);
				break;

			case NMEA_PFLAA_IDTYPE:
				//OS_printf("NMEA_PFLAA_IDTYPE\n");
				/* Parse ID type */
				pflaa->idType = atoi(value);
				break;

			case NMEA_PFLAA_ID:
				//OS_printf("NMEA_PFLAA_ID\n");
				/* Parse ID */
				strcpy(pflaa->ID, value);
				break;

			case NMEA_PFLAA_TRACK:
				//OS_printf("NMEA_PFLAA_TRACK\n");
				/* Parse track */
				pflaa->track = atoi(value);
				break;

			//case NMEA_PFLAA_TURN_RATE:
			//	OS_printf("NMEA_PFLAA_TURN_RATE\n");
				/* Currently this field is empty */
			//	break;

			case NMEA_PFLAA_GROUND_SPEED:
				//OS_printf("NMEA_PFLAA_GROUND_SPEED\n");
				/* Parse ground speed */
				pflaa->groundSpeed = atoi(value);
				break;

			case NMEA_PFLAA_CLIMB_RATE:
				//OS_printf("NMEA_PFLAA_CLIMB_RATE\n");
				/* Parse climb rate */
				pflaa->climbRate = atof(value);
				break;

			case NMEA_PFLAA_ACFTTYPE:
				//OS_printf("NMEA_PFLAA_ACFTTYPE\n");
				/* Parse aircraft type */
				acftTypeVal = strtok(value, "*");
				strcpy(pflaa->acftType, acftTypeVal);
				strtok(value,",");
				break;

			case NMEA_PFLAA_NO_TRACK:
				//OS_printf("NMEA_PFLAA_NO_TRACK\n");
				/* Parse no track flag */
				//pflaa->noTrack = atoi(value);
				break;

			default:
				break;
			}
		value = strtok(NULL, ",");
		val_index++;

	}
	
     	//Log valid sentence
	if(numComma == 11){
		//For TESTING only
		//printf("Logging NMEA sentence: %s\n", recvbuffer);
		//END TESTING
		
		//Only open log file once
		if (firstpass_FLAG == true){
			//obtain current time and date
			getFileTimeandDate(&logtime);

			//append time and date
			strncat(logtime, ".txt", strlen(".txt"));
			strncat(FLARM_LOG, logtime, strlen(logtime));

			//Open log file
			FLARM_SENTENCE = fopen(FLARM_LOG, "w");
			if (FLARM_SENTENCE == NULL)
			{
				printf("Error opening %s!\n", FLARM_LOG);
			}

			firstpass_FLAG = false;
		}

		//Log sentence
		fprintf(FLARM_SENTENCE, "%f:", flarmAppData.position.time_boot);
		fprintf(FLARM_SENTENCE, " %s", recvbuffer);

		logged = true;
	}
	else{
		logged = false;
	}

	return logged;
}

void getFileTimeandDate(char** Filename) 
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char filename[100];

	strftime(filename,80,"%Y_%m_%d__%I_%M_%S%p", &tm);

	// Copy filemname to output pointer Filename
	*Filename = (char *)malloc(strlen(filename)+1);
	strcpy(*Filename,filename);
}

void FLARM_ProcessSBData() {
	CFE_SB_MsgId_t  MsgId;

	MsgId = CFE_SB_GetMsgId(flarmAppData.INTERFACEMsgPtr);
	switch (MsgId)
	{
		case ICAROUS_POSITION_MID:{
						  position_t* pos = (position_t*) flarmAppData.INTERFACEMsgPtr;

						  if(pos->aircraft_id == CFE_PSP_GetSpacecraftId()) {
							  memcpy(&flarmAppData.position, pos, sizeof(position_t));

						  }
						  break;
					  }
		case ICAROUS_FLIGHTPLAN_MID:{
						    flightplan_t* fplan = (flightplan_t*)flarmAppData.INTERFACEMsgPtr;
						    memcpy(&flarmAppData.flightplan,fplan,sizeof(flightplan_t));

						    //TODO: The flight plan icarous uses is stored in flarmAppData.flightplan
						    break;
					    }        
	}
	return;
}

void FLARM_AppCleanUp(){
	//TODO: clean up memory allocation here if necessary
	fclose(FLARM_SENTENCE);
}

int32_t FLARMTableValidationFunc(void *TblPtr){
	int32_t status = 0;
	return status;
}
