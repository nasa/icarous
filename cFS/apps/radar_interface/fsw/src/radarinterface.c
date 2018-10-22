/*******************************************************************************
 ** File: safe2ditch.c
 **
 ** Purpose:
 **   App to interface safe2ditch and Icarous
 **
 *******************************************************************************/
#define EXTERN

#include <ardupilot_msg.h>
#include "radarinterface.h"
#include "radarinterface_version.h"
#include "UtilFunctions.h"

CFE_EVS_BinFilter_t  RADARINTERFACE_EventFilters[] =
{  /* Event ID    mask */
		{RADARINTERFACE_STARTUP_INF_EID,       0x0000},
		{RADARINTERFACE_COMMAND_ERR_EID,       0x0000},
}; /// Event ID definitions

appdataRadar_t appdataRadar;

/* RADARINTERFACE_AppMain() -- Application entry points */
void RADARINTF_AppMain(void){

	int32 status;
	uint32 RunStatus = CFE_ES_APP_RUN;

    RADARINTERFACE_AppInit();

	while(CFE_ES_RunLoop(&RunStatus) == TRUE){
		status = CFE_SB_RcvMsg(&appdataRadar.RADARINTERFACEMsgPtr, appdataRadar.RADARINTERFACE_Pipe, 100);

		if (status == CFE_SUCCESS)
		{
			RADARINTERFACE_ProcessSBMessage();
		}

        RADARINTERFACE_GetMAVLinkMsg();
	}

    RADARINTERFACE_AppCleanUp();

	CFE_ES_ExitApp(RunStatus);
}

void RADARINTERFACE_AppInit(void){

	memset(&appdataRadar,0,sizeof(appdataRadar_t));

	int32 status;

	// Register the app with executive services
	CFE_ES_RegisterApp();

	// Register the events
	CFE_EVS_Register(RADARINTERFACE_EventFilters,
			sizeof(RADARINTERFACE_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
			CFE_EVS_BINARY_FILTER);

	// Create pipe to receive SB messages
	status = CFE_SB_CreatePipe( &appdataRadar.RADARINTERFACE_Pipe, /* Variable to hold Pipe ID */
								RADARINTERFACE_PIPE_DEPTH,       /* Depth of Pipe */
								RADARINTERFACE_PIPE_NAME);       /* Name of pipe */

	//Subscribe to command messages and kinematic band messages from the SB
	CFE_SB_Subscribe(ICAROUS_POSITION_MID, appdataRadar.RADARINTERFACE_Pipe);

	// Send event indicating app initialization
	CFE_EVS_SendEvent (RADARINTERFACE_STARTUP_INF_EID, CFE_EVS_INFORMATION,
					   "Radar interfacea app Initialized. Version %d.%d",
					   RADAR_INTERFACE_MAJOR_VERSION,
					   RADAR_INTERFACE_MINOR_VERSION);

	//TODO: read port parameters from table instead of hard coding it here!
	appdataRadar.radarInterfacePort.portin  = 14558;
	appdataRadar.radarInterfacePort.portout = 14559;
	memcpy(appdataRadar.radarInterfacePort.target,"127.0.0.1",9);

	RADARINTERFACE_InitializeSocketPort(&appdataRadar.radarInterfacePort);

    CFE_SB_InitMsg(&appdataRadar.traffic,ICAROUS_TRAFFIC_MID,sizeof(object_t),TRUE);
    appdataRadar.acPosition[0] = 0;
    appdataRadar.acPosition[1] = 0;
    appdataRadar.acPosition[2] = 0;
}

void RADARINTERFACE_AppCleanUp(){

}

void RADARINTERFACE_InitializeSocketPort(radarInterface_port_t* prt){
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

int RADARINTERFACE_readPort(radarInterface_port_t* prt){
	int n = 0;
	memset(prt->recvbuffer, 0, RDINTF_BUFFER_LENGTH);
	n = recvfrom(prt->sockId, (void *)prt->recvbuffer, RDINTF_BUFFER_LENGTH, 0, (struct sockaddr *)&prt->target_addr, &prt->recvlen);
	return n;
}

void RADARINTERFACE_writePort(radarInterface_port_t* prt,mavlink_message_t* message){
	char sendbuffer[300];
	uint16_t len = mavlink_msg_to_send_buffer((uint8_t*)sendbuffer, message);
	int n = sendto(prt->sockId, sendbuffer, len, 0, (struct sockaddr*)&prt->target_addr, sizeof (struct sockaddr_in));
}

int RADARINTERFACE_GetMAVLinkMsg(){
	int n = RADARINTERFACE_readPort(&appdataRadar.radarInterfacePort);
	mavlink_message_t message;
	mavlink_status_t status;
	uint8_t msgReceived = 0;
	for(int i=0;i<n;i++){
		uint8_t cp = appdataRadar.radarInterfacePort.recvbuffer[i];
		msgReceived = mavlink_parse_char(MAVLINK_COMM_0, cp, &message, &status);
		if(msgReceived){
			RADARINTERFACE_ProcessMavlinkMessage(message);
		}
	}
	return n;
}

void RADARINTERFACE_ProcessMavlinkMessage(mavlink_message_t message){
	switch(message.msgid) {

        case MAVLINK_MSG_ID_ADSB_VEHICLE: {

            // radar coordinate frame (frame 1)
		    // x axis - side to side (positive left) from behind radar
		    // y axis - up and down (positive up)
		    // z axis - in and out of radar antenna plane (positive out)

	        // radar frame rotated to align with conventional euclidean frame (frame 2)
		    // x axis - side to side (positive right)
			// y axis - in and out of radar plane (positive out)
			// z axis - up and down (positive up)

			// NEU xyz frame (frame 3)
			// x axis - + east
			// y axis - + north
			// z axis - + up

			mavlink_adsb_vehicle_t target;
			mavlink_msg_follow_target_decode(&message,&target);

			// This assumes heading is computed as atan2(-Vx_est,Vz_est) in frame 1
			// This way, this heading is bearing relative to Vz (frame 1) or Vy (frame 2)
            // heading should be between (-180,180) or (0,360)
			double track = target.heading / 1.0E2;

			// If heading is negative because of (-180,180) range, convert to (0,360) range.
			if(track < 0)
				track = 360 + track;

		    // These are scalar values and don't change with frames
			double groundspeed = target.hor_velocity / 1.0E2;
			double verticalspeed = target.ver_velocity / 1.0E2;

			// Flip coordinates so that position is converted from frame1 to frame2
			double pos_xyz_radarframe[3];
		    pos_xyz_radarframe[0] = -target.lat/1E7;         // Vx_est
		    pos_xyz_radarframe[1] = target.altitude/1E7;     // Vz_est
		    pos_xyz_radarframe[2] = target.lon/1E7;          // Vy_est

		    // Convert position from frame 2 to frame 3
		    double pos_xyz_vehicle[3];
		    double heading_rad = appdataRadar.yaw * M_PI/180;
			pos_xyz_vehicle[0] = pos_xyz_radarframe[1]*sin(heading_rad) + pos_xyz_radarframe[0]*cos(heading_rad);
            pos_xyz_vehicle[1] = pos_xyz_radarframe[1]*cos(heading_rad) - pos_xyz_radarframe[0]*sin(heading_rad);
            pos_xyz_vehicle[2] = pos_xyz_radarframe[2];

		    // Convert velocity from radar xyz to vehicle xyz frame
			double vn = groundspeed * cos(track * M_PI / 180 + heading_rad);
			double ve = groundspeed * sin(track * M_PI / 180 + heading_rad);
			double vu = verticalspeed;

            double pos_gps[3];
            ComputeLatLngAlt(appdataRadar.acPosition,pos_xyz_vehicle,pos_gps);

            appdataRadar.traffic.index = target.ICAO_address;
            appdataRadar.traffic.latitude = pos_gps[0];
            appdataRadar.traffic.longitude = pos_gps[1];
            appdataRadar.traffic.altitude = pos_gps[2];
     	    appdataRadar.traffic.vn = vn;
			appdataRadar.traffic.ve = ve;
			appdataRadar.traffic.vd = vu;

            SendSBMsg(appdataRadar.traffic);
            break;
        }
	}
}

void RADARINTERFACE_ProcessSBMessage(void){
    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(appdataRadar.RADARINTERFACEMsgPtr);
    mavlink_message_t msg;

    switch (MsgId){
        case ICAROUS_POSITION_MID: {
            position_t *msgPos = (position_t *) appdataRadar.RADARINTERFACEMsgPtr;


            appdataRadar.acPosition[0] = msgPos->latitude;
            appdataRadar.acPosition[1] = msgPos->longitude;
            appdataRadar.acPosition[2] = msgPos->altitude_abs;
            appdataRadar.yaw = msgPos->hdg;
            break;
        }
    }
}