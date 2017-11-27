/*
 * icarous_msg.h
 *
 */

#ifndef ICAROUS_MSG_H_
#define ICAROUS_MSG_H_

#include <stdint.h>

#ifdef CFS 
#include "cfe.h"
#define TLM_HDR_SIZE CFE_SB_TLM_HDR_SIZE
#define CMD_HDR_SIZE CFE_SB_CMD_HDR_SIZE
#else
#define TLM_HDR_SIZE 1
#define CMD_HDR_SIZE 1
#endif

// Icarous Modes enumeration
typedef enum {
	_PASSIVE_,
	_ACTIVE_,
	_INACTIVE_} icarous_control_mode_t;

// Icarous Command types
typedef enum {
	_ARM_,
	_TAKEOFF_,
	_SETMODE_,
	_LAND_,
	_GOTOWP_,
	_SETPOS_,
	_SETVEL_,
	_SETYAW_,
	_SETSPEED_,
	_STATUS_} command_name_t;

// Icarous message id
typedef enum {
	MSG_ID_WAYPOINT,
	MSG_ID_WAYPOINT_REACHED,
	MSG_ID_GEOFENCE,
	MSG_ID_OBJECT,
	MSG_ID_POSITION,
	MSG_ID_ATTITUDE,
	MSG_ID_CMD,
	MSG_ID_CMD_NOARGS,
	MSG_ID_ACK,
	MSG_ID_BANDS,
} msgType;

// Icarous internal messages  
typedef struct{
        uint8_t TlmHeader[TLM_HDR_SIZE];
	uint16_t totalWayPoints;
	uint16_t wayPointIndex;
	float latitude;
	float longitude;
	float altitude;
	float speed;
}waypoint_t;

typedef struct{
        uint8_t TlmHeader[TLM_HDR_SIZE];
	uint8_t reachedwaypoint;
}missionItemReached_t;

typedef struct{
        uint8_t TlmHeader[TLM_HDR_SIZE];
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
        uint8_t TlmHeader[TLM_HDR_SIZE];
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
        uint8_t TlmHeader[TLM_HDR_SIZE];
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
        uint8_t TlmHeader[TLM_HDR_SIZE];
	float roll;
	float pitch;
	float yaw;
}attitude_t;

typedef struct{
        uint8_t TlmHeader[CMD_HDR_SIZE];
	command_name_t name;
}NoArgsCmd_t;

typedef struct{
        uint8_t TlmHeader[CMD_HDR_SIZE];
	command_name_t name;
	float param1,param2;
	float param3,param4;
	float param5,param6;
	float param7,param8;
	char buffer[50];
}ArgsCmd_t;

typedef struct{
        uint8_t TlmHeader[CMD_HDR_SIZE];
	command_name_t name;
	int result;
}CmdAck_t;

typedef struct{
        uint8_t TlmHeader[TLM_HDR_SIZE];
	uint8_t numBands;
	uint8_t type1,type2,type3,type4,type5;
	float min1,min2,min3,min4,min5;
	float max1,max2,max3,max4,max5;
}visbands_t;



#endif /* ICAROUS_MSG_H_ */
