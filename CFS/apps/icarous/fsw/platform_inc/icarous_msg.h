/*
 * icarous_msg.h
 *
 */

#ifndef ICAROUS_MSG_H_
#define ICAROUS_MSG_H_

#include "cfe.h"
#include <stdint.h>

// Auto Pilot Modes enumeration
typedef enum { _PASSIVE_,_ACTIVE_} icarous_control_mode_t;

typedef enum { _ARM_, _TAKEOFF_, _SETMODE_, _LAND_,_GOTOWP_, _SETPOS_, _SETVEL_, _SETYAW_, _SETSPEED_ } command_name_t;

typedef struct{
	uint8_t  TlmHeader[CFE_SB_TLM_HDR_SIZE];
	uint16_t totalWayPoints;
	uint16_t wayPointIndex;
	float latitude;
	float longitude;
	float altitude;
	float speed;
}waypoint_t;

typedef struct{
	uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];
	uint8_t reachedwaypoint;
}missionItemReached_t;

typedef struct{
	uint8_t  TlmHeader[CFE_SB_TLM_HDR_SIZE];
	uint16_t index;
	uint8_t type;
	uint16_t totalvertices;
	uint16_t vertexIndex;
	float latitude;
	float longitude;
	float floor;
	float ceiling;
}geofence_t;

typedef struct{
	uint8_t  TlmHeader[CFE_SB_TLM_HDR_SIZE];
	uint8_t type;
	uint16_t index;
	float latitude;
	float longitude;
	float altiude;
	float vx;
	float vy;
	float vz;
}object_t;

typedef struct{
	uint8_t  TlmHeader[CFE_SB_TLM_HDR_SIZE];
	double time_gps;
	double latitude;
	double longitude;
	double altitude_abs;
	double altitude_rel;
	double vx;
	double vy;
	double vz;
	double hdop;
	double vdop;
	int numSats;
}position_t;

typedef struct{
	uint8_t  TlmHeader[CFE_SB_TLM_HDR_SIZE];
	float roll;
	float pitch;
	float yaw;
}attitude_t;

typedef struct{
	uint8_t  CmdHeader[CFE_SB_CMD_HDR_SIZE];
}NoArgsCmd_t;

typedef struct{
	uint8_t  CmdHeader[CFE_SB_CMD_HDR_SIZE];
	command_name_t name;
	float param1,param2;
	float param3,param4;
	float param5,param6;
	float param7,param8;
}ArgsCmd_t;

typedef struct{
	uint8_t  CmdHeader[CFE_SB_CMD_HDR_SIZE];
	command_name_t name;
	int result;
}CmdAck_t;

typedef struct{
	uint8_t  TlmHeader[CFE_SB_TLM_HDR_SIZE];
	uint8_t numBands;
	uint8_t type1,type2,type3,type4,type5;
	float min1,min2,min3,min4,min5;
	float max1,max2,max3,max4,max5;
}visbands_t;



#endif /* ICAROUS_MSG_H_ */
