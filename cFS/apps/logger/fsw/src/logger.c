/**
 * @file logger.c
 * @brief function definitions for logger app
 */

#include <msgdef/ardupilot_msg.h>
#include <cfe_time.h>
#include "logger.h"

/// Event filter definition for logger
CFE_EVS_BinFilter_t  LOGGER_EventFilters[] =
{  /* Event ID    mask */
		{LOGGER_STARTUP_INF_EID,       0x0000},
		{LOGGER_COMMAND_ERR_EID,       0x0000},
};

/* LOGGER_AppMain() -- Application entry points */
void LOGGER_AppMain(void){

	int32 status;
	uint32 RunStatus = CFE_ES_APP_RUN;

    LOGGER_AppInit();

	while(CFE_ES_RunLoop(&RunStatus) == TRUE){
		if(appdataLog.record)
            RecordPackets();
        else
			PlaybackPackets();
	}

    LOGGER_AppCleanUp();

	CFE_ES_ExitApp(RunStatus);
}

void LOGGER_AppInit(void){

	memset(&appdataLog,0,sizeof(appdataLog_t));

    appdataLog.appStartTime = CFE_TIME_GetUTC();

	int32 status;

	// Register the app with executive services
	CFE_ES_RegisterApp();

	// Register the events
	CFE_EVS_Register(LOGGER_EventFilters,
			sizeof(LOGGER_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
			CFE_EVS_BINARY_FILTER);

	// Create pipe to receive SB messages
	status = CFE_SB_CreatePipe( &appdataLog.LOG_AutopilotData_Pipe,     /* Variable to hold Pipe ID */
                                LOG_PIPE_DEPTH,                         /* Depth of Pipe */
								LOG_PIPE_APDATA_NAME);                  /* Name of pipe */

	status = CFE_SB_CreatePipe( &appdataLog.LOG_Commands_Pipe,          /* Variable to hold Pipe ID */
                                LOG_PIPE_DEPTH,                         /* Depth of Pipe */
								LOG_PIPE_COMMANDS_NAME);                /* Name of pipe */

	// Send event indicating app initialization
	CFE_EVS_SendEvent (LOGGER_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                       "Logging application initialized. Version %d.%d",
					   1,
					   0);

	// Register table with table services
	status = CFE_TBL_Register(&appdataLog.LOGGER_tblHandle,
				  "LoggerTable",
				  sizeof(LoggerTable_t),
				  CFE_TBL_OPT_DEFAULT,
				  &LoggerTableValidationFunc);

	// Load app table data
	status = CFE_TBL_Load(appdataLog.LOGGER_tblHandle,CFE_TBL_SRC_FILE,"/cf/logger_tbl.tbl");

	// Check which port to open from user defined parameters
	LoggerTable_t *TblPtr;
	status = CFE_TBL_GetAddress((void**)&TblPtr,appdataLog.LOGGER_tblHandle);

	appdataLog.record = TblPtr->logRecord;

	// Free table pointer
	status = CFE_TBL_ReleaseAddress(appdataLog.LOGGER_tblHandle);

	sentCmdAck = true;
	sentCommand = true;
	sentFlightPlan = true;
	sentGeofence = true;
	sentPos = true;
	sentTraffic = true;
	sentStartMission = true;
    sentWPReached = true;


	if(appdataLog.record) {
		// Subscribe to relevant inputs
		SubscribeInputs();

		// Prepare log files
		PrepareLogFiles();
	}else{

		// Open log files
		OpenLogFiles();
	}
}

void PrepareLogFiles(){

	appdataLog.fpPos = fopen("../ram/IClog/Position.log","w");
	appdataLog.fpCommands = fopen("../ram/IClog/Commands.log","w");
	appdataLog.fpFlightPlan = fopen("../ram/IClog/MissionPlan.log","w");
	appdataLog.fpGeofence =  fopen("../ram/IClog/Geofence.log","w");
	appdataLog.fpCmdAck = fopen("../ram/IClog/CommandAck.log","w");
	appdataLog.fpTraffic = fopen("../ram/IClog/Traffic.log","w");
	appdataLog.fpStartMission = fopen("../ram/IClog/StartMission.log","w");
	appdataLog.fpWpReached = fopen("../ram/IClog/WPReached.log","w");

    CFE_TIME_SysTime_t currentTime = CFE_TIME_GetUTC();
	fwrite(&currentTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpPos);
	fwrite(&currentTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpCommands);
    fwrite(&currentTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpFlightPlan);
    fwrite(&currentTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpGeofence);
    fwrite(&currentTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpCmdAck);
    fwrite(&currentTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpTraffic);
    fwrite(&currentTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpStartMission);
    fwrite(&currentTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpWpReached);
}


void OpenLogFiles(){
    appdataLog.fpPos = fopen("../ram/IClog/Position.log","r");
	appdataLog.fpCommands = fopen("../ram/IClog/Commands.log","r");
	appdataLog.fpFlightPlan = fopen("../ram/IClog/MissionPlan.log","r");
	appdataLog.fpGeofence =  fopen("../ram/IClog/Geofence.log","r");
	appdataLog.fpCmdAck = fopen("../ram/IClog/CommandAck.log","r");
	appdataLog.fpTraffic = fopen("../ram/IClog/Traffic.log","r");
	appdataLog.fpStartMission = fopen("../ram/IClog/StartMission.log","r");
    appdataLog.fpWpReached = fopen("../ram/IClog/WPReached.log","r");

	fread(&appdataLog.logRecStartTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpPos);
	fread(&appdataLog.logRecStartTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpCommands);
    fread(&appdataLog.logRecStartTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpFlightPlan);
    fread(&appdataLog.logRecStartTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpGeofence);
    fread(&appdataLog.logRecStartTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpCmdAck);
    fread(&appdataLog.logRecStartTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpTraffic);
    fread(&appdataLog.logRecStartTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpStartMission);
    fread(&appdataLog.logRecStartTime, sizeof(CFE_TIME_SysTime_t),1,appdataLog.fpWpReached);
}

void SubscribeInputs(){
	CFE_SB_Subscribe(ICAROUS_POSITION_MID,appdataLog.LOG_AutopilotData_Pipe);
    CFE_SB_Subscribe(ICAROUS_COMMANDS_MID,appdataLog.LOG_Commands_Pipe);
	CFE_SB_Subscribe(ICAROUS_FLIGHTPLAN_MID,appdataLog.LOG_Commands_Pipe);
	CFE_SB_Subscribe(ICAROUS_GEOFENCE_MID,appdataLog.LOG_Commands_Pipe);
	CFE_SB_Subscribe(ICAROUS_COMACK_MID,appdataLog.LOG_AutopilotData_Pipe);
	CFE_SB_Subscribe(ICAROUS_STARTMISSION_MID,appdataLog.LOG_Commands_Pipe);
    CFE_SB_Subscribe(ICAROUS_TRAFFIC_MID,appdataLog.LOG_AutopilotData_Pipe);
	CFE_SB_Subscribe(ICAROUS_WPREACHED_MID,appdataLog.LOG_AutopilotData_Pipe);
}


void LOGGER_AppCleanUp(){

}

void RecordPackets(){
    int32_t status = CFE_SB_RcvMsg(&appdataLog.LOG_APdata_MsgPtr, appdataLog.LOG_AutopilotData_Pipe, 1);

    if (status == CFE_SUCCESS)
    {
        LOGGER_ProcessPacket(appdataLog.LOG_APdata_MsgPtr);
    }

    status = CFE_SB_RcvMsg(&appdataLog.LOG_Command_MsgPtr, appdataLog.LOG_Commands_Pipe, 1);

    if (status == CFE_SUCCESS)
    {
        LOGGER_ProcessPacket(appdataLog.LOG_Command_MsgPtr);
    }
}

void LOGGER_ProcessPacket(CFE_SB_Msg_t* sbMsg){

	CFE_SB_MsgId_t  MsgId;
	MsgId = CFE_SB_GetMsgId(sbMsg);

	switch(MsgId){

		case ICAROUS_POSITION_MID:{
			position_t* msg = (position_t*) sbMsg;
			fwrite(msg, sizeof(position_t), 1, appdataLog.fpPos);
            fflush(appdataLog.fpPos);
			break;
		}

		case ICAROUS_TRAFFIC_MID:{
            object_t* msg = (object_t*) sbMsg;
			fwrite(msg,sizeof(object_t),1,appdataLog.fpTraffic);
            fflush(appdataLog.fpTraffic);
			break;
		}

		case ICAROUS_FLIGHTPLAN_MID:{
			flightplan_t* msg = (flightplan_t*) sbMsg;
			fwrite(msg, sizeof(flightplan_t),1,appdataLog.fpFlightPlan);
			fflush(appdataLog.fpFlightPlan);
			break;
		}

		case ICAROUS_GEOFENCE_MID:{
			geofence_t* msg = (geofence_t*) sbMsg;
			fwrite(msg, sizeof(geofence_t),1,appdataLog.fpGeofence);
			fflush(appdataLog.fpGeofence);
			break;
		}

		case ICAROUS_COMMANDS_MID:{
			argsCmd_t* msg = (argsCmd_t*) sbMsg;
			fwrite(msg, sizeof(argsCmd_t),1,appdataLog.fpCommands);
			fflush(appdataLog.fpCommands);
			break;
		}

		case ICAROUS_COMACK_MID:{
			cmdAck_t* msg = (cmdAck_t*) sbMsg;
			fwrite(msg, sizeof(cmdAck_t),1,appdataLog.fpCmdAck);
			fflush(appdataLog.fpCmdAck);
			break;
		}

		case ICAROUS_STARTMISSION_MID:{
			argsCmd_t* msg = (argsCmd_t*) sbMsg;
			fwrite(msg, sizeof(argsCmd_t),1,appdataLog.fpStartMission);
			fflush(appdataLog.fpStartMission);
			break;
		}

		case ICAROUS_WPREACHED_MID:{
			missionItemReached_t* msg = (missionItemReached_t*) sbMsg;
			fwrite(msg, sizeof(missionItemReached_t),1,appdataLog.fpWpReached);
			fflush(appdataLog.fpWpReached);
			break;
		}
	}
}


void PlaybackPackets(){

	CFE_TIME_SysTime_t timeLocalB = CFE_TIME_GetUTC();
	CFE_TIME_SysTime_t delLocal = CFE_TIME_Subtract(timeLocalB, appdataLog.appStartTime);
	int n;

	if(sentPos) {
		n = fread(&logPos, sizeof(position_t), 1, appdataLog.fpPos);
		havePos = (n>0)?true:false;
        sentPos = false;
	}

	if(havePos)
        sentPos = PublishSBMessage(delLocal,(CFE_SB_Msg_t*) &logPos);


	if(sentGeofence) {
		n = fread(&logGf, sizeof(geofence_t), 1, appdataLog.fpGeofence);
		haveGeofence = (n>0)?true:false;
		sentGeofence = false;
	}

	if(haveGeofence)
        sentGeofence = PublishSBMessage(delLocal,(CFE_SB_MsgPtr_t) &logGf);


	if(sentFlightPlan) {
		n = fread(&logFp, sizeof(flightplan_t), 1, appdataLog.fpFlightPlan);
		haveFlightPlan = (n>0)?true:false;
		sentFlightPlan = false;
	}

	if(haveFlightPlan)
        sentFlightPlan = PublishSBMessage(delLocal,(CFE_SB_MsgPtr_t) &logFp);

	if(sentCommand) {
		n = fread(&logCmd, sizeof(argsCmd_t), 1, appdataLog.fpCommands);
		haveCommand = (n>0)?true:false;
		sentCommand = false;
	}

	if(haveCommand)
        sentCommand = PublishSBMessage(delLocal,(CFE_SB_MsgPtr_t) &logCmd);

	if(sentTraffic) {
		n = fread(&logTraffic, sizeof(object_t), 1, appdataLog.fpTraffic);
		haveTraffic = (n>0)?true:false;
		sentTraffic = false;
	}

	if(haveTraffic)
        sentTraffic = PublishSBMessage(delLocal,(CFE_SB_MsgPtr_t) &logTraffic);

	if(sentCmdAck) {
		n = fread(&logCmdAck, sizeof(cmdAck_t), 1, appdataLog.fpCmdAck);
		haveCmdAck = (n>0)?true:false;
		sentCmdAck = false;
	}

	if(haveCmdAck)
        sentCmdAck = PublishSBMessage(delLocal,(CFE_SB_MsgPtr_t) &logCmdAck);

	if(sentStartMission) {
		n = fread(&logStartMission, sizeof(argsCmd_t), 1, appdataLog.fpStartMission);
		haveStartMission = (n>0)?true:false;
		sentStartMission = false;
	}

	if(haveStartMission)
        sentStartMission = PublishSBMessage(delLocal,(CFE_SB_MsgPtr_t) &logStartMission);

	if(sentWPReached){
		n = fread(&logWPReached, sizeof(missionItemReached_t), 1, appdataLog.fpWpReached);
		haveWPReached = (n>0)?true:false;
		sentWPReached = false;
	}

	if(haveWPReached)
		sentWPReached =  PublishSBMessage(delLocal,(CFE_SB_MsgPtr_t) &logWPReached);
}

bool PublishSBMessage(CFE_TIME_SysTime_t delLocal,CFE_SB_Msg_t* msg){

	CFE_TIME_SysTime_t timeLogMsg = CFE_SB_GetMsgTime(msg);
	CFE_TIME_SysTime_t delMsg = CFE_TIME_Subtract(timeLogMsg, appdataLog.logRecStartTime);

	if(CFE_TIME_Compare(delLocal,delMsg) >= 0){
		CFE_SB_SendMsg(msg);
		return true;
	}else{
		return false;
	}
}

int32_t LoggerTableValidationFunc(void *TblPtr){

	int32_t status = 0;

	return status;
}
