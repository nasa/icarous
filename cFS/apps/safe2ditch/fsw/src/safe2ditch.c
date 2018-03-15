/*******************************************************************************
 ** File: safe2ditch.c
 **
 ** Purpose:
 **   App to interface safe2ditch and Icarous
 **
 *******************************************************************************/
#define EXTERN

#include <Icarous_msg.h>
#include "safe2ditch.h"
#include "safe2ditch_version.h"

CFE_EVS_BinFilter_t  SAFE2DITCH_EventFilters[] =
{  /* Event ID    mask */
		{SAFE2DITCH_STARTUP_INF_EID,       0x0000},
		{SAFE2DITCH_COMMAND_ERR_EID,       0x0000},
}; /// Event ID definitions

appdataS2D_t appdataS2D;

/* SAFE2DITCH_AppMain() -- Application entry points */
void SAFE2DITCH_AppMain(void){

	int32 status;
	uint32 RunStatus = CFE_ES_APP_RUN;

    SAFE2DITCH_AppInit();

	while(CFE_ES_RunLoop(&RunStatus) == TRUE){
		status = CFE_SB_RcvMsg(&appdataS2D.SAFE2DITCHMsgPtr, appdataS2D.SAFE2DITCH_Pipe, 100);

		if (status == CFE_SUCCESS)
		{
			ProcessSBMessage();
		}

        GetMAVLinkMsg();
	}

    SAFE2DITCH_AppCleanUp();

	CFE_ES_ExitApp(RunStatus);
}

void SAFE2DITCH_AppInit(void){

	memset(&appdataS2D,0,sizeof(appdataS2D_t));

	int32 status;

	// Register the app with executive services
	CFE_ES_RegisterApp();

	// Register the events
	CFE_EVS_Register(SAFE2DITCH_EventFilters,
			sizeof(SAFE2DITCH_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
			CFE_EVS_BINARY_FILTER);

	// Create pipe to receive SB messages
	status = CFE_SB_CreatePipe( &appdataS2D.SAFE2DITCH_Pipe, /* Variable to hold Pipe ID */
								SAFE2DITCH_PIPE_DEPTH,       /* Depth of Pipe */
								SAFE2DITCH_PIPE_NAME);       /* Name of pipe */

	//Subscribe to command messages and kinematic band messages from the SB
	CFE_SB_Subscribe(ICAROUS_POSITION_MID, appdataS2D.SAFE2DITCH_Pipe);
	CFE_SB_Subscribe(ICAROUS_VELOCITY_MID, appdataS2D.SAFE2DITCH_Pipe);
	CFE_SB_Subscribe(ICAROUS_ATTITUDE_MID, appdataS2D.SAFE2DITCH_Pipe);
    CFE_SB_Subscribe(SERVICE_DITCH_MID, appdataS2D.SAFE2DITCH_Pipe);
	CFE_SB_Subscribe(ICAROUS_RESET_MID, appdataS2D.SAFE2DITCH_Pipe);
    CFE_SB_Subscribe(ICAROUS_COMMANDS_MID, appdataS2D.SAFE2DITCH_Pipe);

	// Send event indicating app initialization
	CFE_EVS_SendEvent (SAFE2DITCH_STARTUP_INF_EID, CFE_EVS_INFORMATION,
					   "Safe2Ditch bridge Initialized. Version %d.%d",
					   SAFE2DITCH_INTERFACE_MAJOR_VERSION,
					   SAFE2DITCH_INTERFACE_MINOR_VERSION);

	//TODO: read port parameters from table instead of hard coding it here!
	appdataS2D.s2dport.portin  = 14556;
	appdataS2D.s2dport.portout = 14557;
	memcpy(appdataS2D.s2dport.target,"127.0.0.1",9);

	s2d_InitializeSocketPort(&appdataS2D.s2dport);

    appdataS2D.resetDitch = false;
    appdataS2D.endDitch = false;
    appdataS2D.ditchGuidanceRequired = false;
    appdataS2D.ditchRequested = false;
    appdataS2D.ditchLocation[0] = 0.0;
    appdataS2D.ditchLocation[1] = 0.0;
    appdataS2D.ditchLocation[2] = -10000.0;
}

void SAFE2DITCH_AppCleanUp(){

}

void s2d_InitializeSocketPort(s2d_port_t* prt){
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

	OS_printf("Sock id: %d,Address: %s,Port in:%d,out: %d\n",prt->sockId,prt->target,prt->portin,prt->portout);
}

int s2d_readPort(s2d_port_t* prt){
	int n = 0;
	memset(prt->recvbuffer, 0, S2D_BUFFER_LENGTH);
	n = recvfrom(prt->sockId, (void *)prt->recvbuffer, S2D_BUFFER_LENGTH, 0, (struct sockaddr *)&prt->target_addr, &prt->recvlen);
	return n;
}

void s2d_writePort(s2d_port_t* prt,mavlink_message_t* message){
	char sendbuffer[300];
	uint16_t len = mavlink_msg_to_send_buffer((uint8_t*)sendbuffer, message);
	int n = sendto(prt->sockId, sendbuffer, len, 0, (struct sockaddr*)&prt->target_addr, sizeof (struct sockaddr_in));
}

int GetMAVLinkMsg(){
	int n = s2d_readPort(&appdataS2D.s2dport);
	mavlink_message_t message;
	mavlink_status_t status;
	uint8_t msgReceived = 0;
	for(int i=0;i<n;i++){
		uint8_t cp = appdataS2D.s2dport.recvbuffer[i];
		msgReceived = mavlink_parse_char(MAVLINK_COMM_0, cp, &message, &status);
		if(msgReceived){
			ProcessMavlinkMessage(message);
		}
	}
	return n;
}

void ProcessMavlinkMessage(mavlink_message_t message){
	switch(message.msgid) {

        case MAVLINK_MSG_ID_COMMAND_INT: {

			mavlink_command_int_t intcmd;
			mavlink_msg_command_int_decode(&message,&intcmd);
			if (intcmd.command == MAV_CMD_USER_1) {
                if(intcmd.param1 == _STARTDITCH_){
                    appdataS2D.ditchRequested   = (bool) intcmd.param2;
                    appdataS2D.ditchGuidanceRequired = (bool) intcmd.param3;
                    appdataS2D.ditchLocation[0] = intcmd.x/1E7;
                    appdataS2D.ditchLocation[1] = intcmd.y/1E7;
                    appdataS2D.ditchLocation[2] = intcmd.z/1E3;

				}else if(intcmd.param1 == _RESETDITCH_){
                    appdataS2D.resetDitch = true;
                }
                else if(intcmd.param1 == _ENDDITCH_){
                    appdataS2D.endDitch = true;
                }
				break;
		    }
        }
	}
}

void ProcessSBMessage(void){
    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(appdataS2D.SAFE2DITCHMsgPtr);
    mavlink_message_t msg;

    switch (MsgId){
        case ICAROUS_POSITION_MID: {
            position_t *msgPos = (position_t *) appdataS2D.SAFE2DITCHMsgPtr;

            mavlink_msg_global_position_int_pack(1, 0, &msg, (int32_t) msgPos->time_gps * 1E3,
                                                 (int32_t) msgPos->latitude * 1E7,
                                                 (int32_t) msgPos->longitude * 1E7,
                                                 (int32_t) msgPos->altitude_abs * 1E3,
                                                 (int32_t) msgPos->altitude_rel * 1E3,
                                                 (int32_t) msgPos->vx * 1E2,
                                                 (int32_t) msgPos->vy * 1E2,
                                                 (int32_t) msgPos->vz * 1E2,
                                                 (int32_t) msgPos->hdg * 1E3);

            s2d_writePort(&appdataS2D.s2dport,&msg);
            break;
        }

        case SERVICE_DITCH_MID: {
            service_t *msgSrv = (service_t *) appdataS2D.SAFE2DITCHMsgPtr;
            if(CHECKNAME((*msgSrv), "GetDitchSite")){
                service_t returnMsg;
                CFE_SB_InitMsg(&returnMsg,P)
                returnMsg.sType = _command_return_;
                returnMsg.id = msgSrv->id;
                serializeRealArray(3,appdataS2D.ditchLocation,returnMsg.buffer);
                SendSBMsg(returnMsg);
                appdataS2D.ditchLocation[2] = -100000;
            }else if(CHECKNAME((*msgSrv), "ditchingStatus")){
                service_t returnMsg;
                returnMsg.sType = _lookup_return_;
                returnMsg.id = msgSrv->id;
                serializeBool(false, appdataS2D.ditchRequested, returnMsg.buffer);
                SendSBMsg(returnMsg);
                OS_printf("Getting ditching status\n");
            }else if(CHECKNAME((*msgSrv), "requireDitchGuidance")){
                service_t returnMsg;
                returnMsg.sType = _lookup_return_;
                returnMsg.id = msgSrv->id;
                serializeBool(false, appdataS2D.ditchGuidanceRequired, returnMsg.buffer);
                SendSBMsg(returnMsg);
                OS_printf("require ditch guidance\n");
            }else if(CHECKNAME((*msgSrv), "resetDitching")){
                service_t returnMsg;
                returnMsg.sType = _lookup_return_;
                returnMsg.id = msgSrv->id;
                serializeBool(false, appdataS2D.resetDitch, returnMsg.buffer);
                SendSBMsg(returnMsg);
                OS_printf("reseting ditch");
            }else if(CHECKNAME((*msgSrv), "ditchingComplete")){
                service_t returnMsg;
                returnMsg.sType = _lookup_return_;
                returnMsg.id = msgSrv->id;
                serializeBool(false, appdataS2D.endDitch, returnMsg.buffer);
                SendSBMsg(returnMsg);
            }

            break;
        }

        case ICAROUS_RESET_MID:{
            appdataS2D.resetDitch = false;
            appdataS2D.endDitch = false;
            appdataS2D.ditchGuidanceRequired = false;
            appdataS2D.ditchRequested = false;
            break;
        }

        case ICAROUS_COMMANDS_MID:{
            NoArgsCmd_t* cmd = (NoArgsCmd_t*) appdataS2D.SAFE2DITCHMsgPtr;
            switch(cmd->name){
                case _DITCH_:
                    OS_printf("Received ditch command from ground station\n");
                    mavlink_msg_command_int_pack(255, 0, &msg, 1, 0, 0, MAV_CMD_USER_1,
                                                 0, 0, _INITIALIZE_, 0, 0, 0, 0, 0, 0);

                    s2d_writePort(&appdataS2D.s2dport, &msg);
                    break;
            }
        }



    }
}