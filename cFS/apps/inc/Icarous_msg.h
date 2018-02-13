/*
 * icarous_msg.h
 *
 */

#ifndef ICAROUS_MSG_H_
#define ICAROUS_MSG_H_

#include <stdint.h>

#include "cfe.h"

// Icarous Modes enumeration
typedef enum {
	_PASSIVE_,
	_ACTIVE_,
	_INACTIVE_
} icarous_control_mode_t;

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
	_STATUS_
} command_name_t;

typedef enum{
    _KEEPIN_,
    _KEEPOUT_
}geofence_type_t;

typedef enum{
    _TRAFFIC_,
    _OBSTACLE_
}object_type_t;

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

/**
 * waypoint_t: message encoding waypoint information.
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
	uint16_t totalWayPoints;                /**< total number of waypoints in flight plan*/
	uint16_t wayPointIndex;                 /**< current waypoint index being uploaded */
	float latitude;                         /**< latitude (degrees) */
	float longitude;                        /**< longitude (degrees) */
	float altitude;                         /**< altitude (degrees) */
	float speed;                            /**< speed between previous waypoint and this waypoint */
}waypoint_t;

/**
 * missionItemReached_t: message indicating a specific waypoint has been reached.
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
	uint8_t reachedwaypoint;                /**< waypoint index that was reached */
}missionItemReached_t;

/**
 * geofence_t: message encoding geofence vertex information.
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
	uint16_t index;                         /**< geofence index */
	uint8_t type;                           /**< geofence type: see geofence_type_t */
	uint16_t totalvertices;                 /**< total vertices in this geofence */
	uint16_t vertexIndex;                   /**< index of vertex being uploaded */
	float latitude;                         /**< latitude (degrees) */
	float longitude;                        /**< longitude (degrees) */
	float floor;                            /**< floor of geofence (m) */
	float ceiling;                          /**< roof of geofence (m) */
}geofence_t;

/**
 * object_t: message to represent information about an static/dynamic object
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
	uint8_t type;                            /**< object type: see object_type_t */
	uint16_t index;                          /**< id of object */
	float latitude;                          /**< latitude (degrees) */
	float longitude;                         /**< longitude (degrees) */
	float altiude;                           /**< altitude (degrees) */
	float vx;                                /**< velocity East component */
	float vy;                                /**< velocity North component */
	float vz;                                /**< velocity Up component */
}object_t;

/**
 * position_t: position information of aircraft.
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];   /**< cFS header information */
	double time_gps;                          /**< gps time */
	double latitude;                          /**< latitude (degrees) */
	double longitude;                         /**< longitude (degrees) */
	double altitude_abs;                      /**< absolution altitude, ASL (m)  */
	double altitude_rel;                      /**< relative altitude, AGL (m) */
	double vx;                                /**< velocity North component (m/s)*/
	double vy;                                /**< velocity East component (m/s)*/
	double vz;                                /**< velocity Down component (m/s)*/
	double hdop;                              /**< GPS Horizontal Dilution of Precision */
	double vdop;                              /**< GPS Vertical Dilution of Precision */
	int numSats;                              /**< Total number of satellites being used for localization */
}position_t;


/**
 * attitude_t: aircraft attitude information.
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];   /**< cFS header information */
	float roll;                               /**< roll angle (degree) */
	float pitch;                              /**< pitch angle (degree) */
	float yaw;                                /**< yaw angle (degree) */
}attitude_t;

/**
 * NoArgsCmd_t: Command without arguments
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_CMD_HDR_SIZE];  /**< cFS header information */
	command_name_t name;                     /**< command name */
}NoArgsCmd_t;

/**
 * ArgsCmd_t: Command with arguments
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_CMD_HDR_SIZE];  /**< cFS header information */
	command_name_t name;                     /**< command name: see command_name_t */
	float param1,param2;                     /**< command arguments */
	float param3,param4;                     /**< command arguments */
	float param5,param6;                     /**< command arguments */
	float param7,param8;                     /**< command arguments */
	char buffer[50];                         /**< command arguments */
}ArgsCmd_t;

/**
 * CmdAck_t: Command acknowledgement
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_CMD_HDR_SIZE];  /**< cFS header information */
	command_name_t name;                     /**< command name: see command_name_t */
	int result;                              /**< result */
}CmdAck_t;

/**
 * visbands_t: Message encoding kinematic track band information
 * from DAIDALUS
 */
typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
	uint8_t numBands;                        /**< total number of track bands (max:5)*/
	uint8_t type1,type2,type3,type4,type5;   /**< type of each band */
	float min1,min2,min3,min4,min5;          /**< min values of each band (degree)*/
	float max1,max2,max3,max4,max5;          /**< max values of each band (degree) */
}visbands_t;



#endif /* ICAROUS_MSG_H_ */
