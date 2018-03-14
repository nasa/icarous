/*
 * icarous_msg.h
 *
 */

#ifndef ICAROUS_MSG_H_
#define ICAROUS_MSG_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "cfe.h"

#define CHECKNAME(MSG,STRING) !strcmp(MSG.name,STRING)

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

typedef enum{
	_lookup_,
	_command_,
	_lookup_return_,
	_command_return_
}servictType_t;


// Icarous value types
typedef enum{
    _unknown_type_ = 0,
    // User scalar types
    _boolean_type_,
    _integer_type_,
    _real_type_,
    _string_type_,

    // User array types
    _array_type_ = 16, // Not a valid type, but an offset from scalar types
    _boolean_array_type_,
    _integer_array_type_,
    _real_array_type_,
    _string_array_type_,
    // more to come?
}value_types_t;

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

/**
 * service_t: Message to perform service requests/response among apps
 */
typedef struct{
	uint8_t TlmHeader[CFE_SB_CMD_HDR_SIZE];
	servictType_t sType;                     /**< service type */
	int id;                                  /**< request id */
	char name[50];                           /**< name of service (lookup/command name) */
	char buffer[250];                        /**< arguments */
}service_t;

static char* serializeBool(bool arrayelement,const bool o,char* b){
    arrayelement?0:(*b++ = _boolean_type_);
    *b++ = (char) o;
    return b;
}

static char* serializeInt(bool arrayelement,const int32_t val,char* b){

	int32_t o = val;
	arrayelement?0:(*b++ = _integer_type_);
	// Store in big-endian format
	*b++ = (char) (0xFF & (o >> 24));
	*b++ = (char) (0xFF & (o >> 16));
	*b++ = (char) (0xFF & (o >> 8));
	*b++ = (char) (0xFF & o);
	return b;
}

static char* serializeReal(bool arrayelement,const double val,char* b){
	union realInt{
		double r;
		uint64_t l;
	};
	union realInt data;
	data.r = val;
	data.l = data.l;
	arrayelement?0:(*b++ = _real_type_);
	// Store in big-endian format
	*b++ = (char) (0xFF & (data.l >> 56));
	*b++ = (char) (0xFF & (data.l >> 48));
	*b++ = (char) (0xFF & (data.l >> 40));
	*b++ = (char) (0xFF & (data.l >> 32));
	*b++ = (char) (0xFF & (data.l >> 24));
	*b++ = (char) (0xFF & (data.l >> 16));
	*b++ = (char) (0xFF & (data.l >> 8));
	*b++ = (char) (0xFF & data.l);
	return b;
}

static char* serializeString(int size,const char val[],char* b){

	unsigned long s = size;
	if (s > 0xFFFFFF)
		return NULL; // too big

	*b++ = _string_type_;
	// Put 3 bytes of size first - std::string may contain embedded NUL
	*b++ = (char) (0xFF & (s >> 16));
	*b++ = (char) (0xFF & (s >> 8));
	*b++ = (char) (0xFF & s);
	memcpy(b, val, s);
	return b + s;
}

static char *serializeBoolVector(int size,const bool o[], char *b)
{
	int s = size;
	int i = 0;
	while (s > 0) {
		uint8_t tmp = 0;
		uint8_t mask = 0x80;
		switch (s) {
			default: // s >= 8
				if (o[i++])
					tmp |= mask;
				mask = mask >> 1;

			case 7:
				if (o[i++])
					tmp |= mask;
				mask = mask >> 1;

			case 6:
				if (o[i++])
					tmp |= mask;
				mask = mask >> 1;

			case 5:
				if (o[i++])
					tmp |= mask;
				mask = mask >> 1;

			case 4:
				if (o[i++])
					tmp |= mask;
				mask = mask >> 1;

			case 3:
				if (o[i++])
					tmp |= mask;
				mask = mask >> 1;

			case 2:
				if (o[i++])
					tmp |= mask;
				mask = mask >> 1;

			case 1:
				if (o[i++])
					tmp |= mask;
				break;
		}

		*b++ = tmp;
		s -= 8;
	}

	return b;
}

// Internal function
// Read from buffer in big-endian form
// Presumes vector size has already been set.
static char const *deserializeBoolVector(int size,bool o[], const char *b)
{
	int s = size;
	int i = 0;
	while (s > 0) {
		uint8_t tmp = *b++;
		uint8_t mask = 0x80;
		switch (s) {
			default: // s >= 8
				o[i++] = (tmp & mask) ? true : false;
				mask = mask >> 1;

			case 7:
				o[i++] = (tmp & mask) ? true : false;
				mask = mask >> 1;

			case 6:
				o[i++] = (tmp & mask) ? true : false;
				mask = mask >> 1;

			case 5:
				o[i++] = (tmp & mask) ? true : false;
				mask = mask >> 1;

			case 4:
				o[i++] = (tmp & mask) ? true : false;
				mask = mask >> 1;

			case 3:
				o[i++] = (tmp & mask) ? true : false;
				mask = mask >> 1;

			case 2:
				o[i++] = (tmp & mask) ? true : false;
				mask = mask >> 1;

			case 1:
				o[i++] = (tmp & mask) ? true : false;
				break;
		}
		s -= 8;
	}
	return b;
}

static char* serializeBoolArray(int size,const bool val[],char* b){
	unsigned long s = size;
	if (s > 0xFFFFFF)
		return NULL; // too big to serialize

	// Write type code
	*b++ = (char) _boolean_array_type_;

	// Write 3 bytes of size
	*b++ = (char) (0xFF & (s >> 16));
	*b++ = (char) (0xFF & (s >> 8));
	*b++ = (char) (0xFF & s);

	//TODO: This should be a malloc;
	bool known[20];
	memset(known,1,20);

	// Write known vector
	b = serializeBoolVector(s,known, b);

	b = serializeBoolVector(s,val,b);

	return b;
}

static const char* deSerializeBool(bool arrayelement,bool* o,const char* b){
	if (!arrayelement && _boolean_type_ != (value_types_t) *b++)
		return NULL;
	*o = (bool) *b++;
	return b;
}

static const char* deSerializeInt(bool arrayelement,int32_t* val,const char* b){
	if (!arrayelement && _integer_type_ != (value_types_t) *b++)
		return NULL;
	uint32_t n = ((uint32_t) (unsigned char) *b++) << 8;
	n = (n + (uint32_t) (unsigned char) *b++) << 8;
	n = (n + (uint32_t) (unsigned char) *b++) << 8;
	n = (n + (uint32_t) (unsigned char) *b++);

	*val = (int32_t) n;
	return b;
}

static const char* deSerializeReal(bool arrayelement,double* val,const char* b){
	if (!arrayelement && _real_type_ != (value_types_t) *b++)
		return NULL;
	union realInt{
		double r;
		uint64_t l;
	};
	union realInt data;
	data.l = (uint64_t) (unsigned char) *b++;  data.l = data.l << 8;
	data.l += (uint64_t) (unsigned char) *b++; data.l = data.l << 8;
	data.l += (uint64_t) (unsigned char) *b++; data.l = data.l << 8;
	data.l += (uint64_t) (unsigned char) *b++; data.l = data.l << 8;
	data.l += (uint64_t) (unsigned char) *b++; data.l = data.l << 8;
	data.l += (uint64_t) (unsigned char) *b++; data.l = data.l << 8;
	data.l += (uint64_t) (unsigned char) *b++; data.l = data.l << 8;
	data.l += (uint64_t) (unsigned char) *b++;
	*val = data.r;
	return b;

}

static const char* deSerializeString(char val[],const char* b){
	if (_string_type_ != (value_types_t) *b++)
		return NULL;

	// Get 3 bytes of size
	size_t s = ((size_t) (unsigned char) *b++) << 8;
	s = (s + (size_t) (unsigned char) *b++) << 8;
	s = s + (size_t) (unsigned char) *b++;

	memcpy(val,b,s);
	return b + s;
}

static char* serializeIntArray(int size,const int32_t val[],char* b){
	unsigned long s = size;
	if (s > 0xFFFFFF)
		return NULL; // too big to serialize

	// Write type code
	*b++ = (char) _integer_array_type_;

	// Write 3 bytes of size
	*b++ = (char) (0xFF & (s >> 16));
	*b++ = (char) (0xFF & (s >> 8));
	*b++ = (char) (0xFF & s);

	//TODO: This should be a malloc;
	bool known[20];
	memset(known,1,20);

	// Write known vector
	b = serializeBoolVector(s,known, b);

	// Write array contents
	for (size_t i = 0; i < s; ++i) {
		b = serializeInt(true,val[i], b);
		if (!b)
			return NULL; // serializeElement failed
	}
	return b;
}

static char* serializeRealArray(int size,const double val[],char* b){
	unsigned long s = size;
	if (s > 0xFFFFFF)
		return NULL; // too big to serialize

	// Write type code
	*b++ = (char) _real_array_type_;

	// Write 3 bytes of size
	*b++ = (char) (0xFF & (s >> 16));
	*b++ = (char) (0xFF & (s >> 8));
	*b++ = (char) (0xFF & s);

	//TODO: This should be a malloc;
	bool known[20];
	memset(known,1,20);

	// Write known vector
	b = serializeBoolVector(s,known, b);

	// Write array contents
	for (size_t i = 0; i < s; ++i) {
		b = serializeReal(true,val[i], b);
		if (!b)
			return NULL; // serializeElement failed
	}
	return b;
}

static char const *deSerializeBoolArray(bool val[],const char* b)
{
	// Check type code
	if (_boolean_array_type_ !=  *b++)
		return NULL; // not an appropriate array

	// Get 3 bytes of size
	size_t s = (size_t) *b++; s = s << 8;
	s += (size_t) *b++; s = s << 8;
	s += (size_t) *b++;

	//TODO: This should be a malloc;
	bool known[20];
	memset(known,1,20);

	b = deserializeBoolVector(s,known, b);
	b = deserializeBoolVector(s,val,b);

	return b;
}

static char const *deSerializeIntArray(int32_t val[],const char* b)
{
	// Check type code
	if (_integer_array_type_ !=  *b++)
		return NULL; // not an appropriate array

	// Get 3 bytes of size
	size_t s = (size_t) *b++; s = s << 8;
	s += (size_t) *b++; s = s << 8;
	s += (size_t) *b++;

	//TODO: This should be a malloc;
	bool known[20];
	memset(known,1,20);

	b = deserializeBoolVector(s,known, b);
	for (size_t i = 0; i < s; ++i)
		b = deSerializeInt(true,val+i, b);

	return b;
}

static char const *deSerializeRealArray(double val[],const char* b)
{
	// Check type code
	if (_real_array_type_ !=  *b++)
		return NULL; // not an appropriate array

	// Get 3 bytes of size
	size_t s = (size_t) *b++; s = s << 8;
	s += (size_t) *b++; s = s << 8;
	s += (size_t) *b++;

	//TODO: This should be a malloc;
	bool known[20];
	memset(known,1,20);

	b = deserializeBoolVector(s,known, b);
	for (size_t i = 0; i < s; ++i)
		b = deSerializeReal(true,val+i, b);

	return b;
}




#endif /* ICAROUS_MSG_H_ */
