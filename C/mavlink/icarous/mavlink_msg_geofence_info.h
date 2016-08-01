// MESSAGE GEOFENCE_INFO PACKING

#define MAVLINK_MSG_ID_GEOFENCE_INFO 222

typedef struct MAVLINK_PACKED __mavlink_geofence_info_t
{
 float fenceFloor; /*<  Fence floor height */
 float fenceCeiling; /*<  Fence ceiling height */
 int32_t numVertices; /*<  Number of polygon verticies */
 uint8_t msgType; /*< Flag, 0-new fence, 1-remove existing fence*/
 uint8_t fenceType; /*< Flag, 0-keep in, 1-keep out*/
 uint8_t fenceID; /*<  Geo-fence ID*/
} mavlink_geofence_info_t;

#define MAVLINK_MSG_ID_GEOFENCE_INFO_LEN 15
#define MAVLINK_MSG_ID_GEOFENCE_INFO_MIN_LEN 15
#define MAVLINK_MSG_ID_222_LEN 15
#define MAVLINK_MSG_ID_222_MIN_LEN 15

#define MAVLINK_MSG_ID_GEOFENCE_INFO_CRC 218
#define MAVLINK_MSG_ID_222_CRC 218



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_GEOFENCE_INFO { \
	222, \
	"GEOFENCE_INFO", \
	6, \
	{  { "fenceFloor", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_geofence_info_t, fenceFloor) }, \
         { "fenceCeiling", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_geofence_info_t, fenceCeiling) }, \
         { "numVertices", NULL, MAVLINK_TYPE_INT32_T, 0, 8, offsetof(mavlink_geofence_info_t, numVertices) }, \
         { "msgType", NULL, MAVLINK_TYPE_UINT8_T, 0, 12, offsetof(mavlink_geofence_info_t, msgType) }, \
         { "fenceType", NULL, MAVLINK_TYPE_UINT8_T, 0, 13, offsetof(mavlink_geofence_info_t, fenceType) }, \
         { "fenceID", NULL, MAVLINK_TYPE_UINT8_T, 0, 14, offsetof(mavlink_geofence_info_t, fenceID) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_GEOFENCE_INFO { \
	"GEOFENCE_INFO", \
	6, \
	{  { "fenceFloor", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_geofence_info_t, fenceFloor) }, \
         { "fenceCeiling", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_geofence_info_t, fenceCeiling) }, \
         { "numVertices", NULL, MAVLINK_TYPE_INT32_T, 0, 8, offsetof(mavlink_geofence_info_t, numVertices) }, \
         { "msgType", NULL, MAVLINK_TYPE_UINT8_T, 0, 12, offsetof(mavlink_geofence_info_t, msgType) }, \
         { "fenceType", NULL, MAVLINK_TYPE_UINT8_T, 0, 13, offsetof(mavlink_geofence_info_t, fenceType) }, \
         { "fenceID", NULL, MAVLINK_TYPE_UINT8_T, 0, 14, offsetof(mavlink_geofence_info_t, fenceID) }, \
         } \
}
#endif

/**
 * @brief Pack a geofence_info message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param msgType Flag, 0-new fence, 1-remove existing fence
 * @param fenceType Flag, 0-keep in, 1-keep out
 * @param fenceID  Geo-fence ID
 * @param fenceFloor  Fence floor height 
 * @param fenceCeiling  Fence ceiling height 
 * @param numVertices  Number of polygon verticies 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_geofence_info_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint8_t msgType, uint8_t fenceType, uint8_t fenceID, float fenceFloor, float fenceCeiling, int32_t numVertices)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GEOFENCE_INFO_LEN];
	_mav_put_float(buf, 0, fenceFloor);
	_mav_put_float(buf, 4, fenceCeiling);
	_mav_put_int32_t(buf, 8, numVertices);
	_mav_put_uint8_t(buf, 12, msgType);
	_mav_put_uint8_t(buf, 13, fenceType);
	_mav_put_uint8_t(buf, 14, fenceID);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GEOFENCE_INFO_LEN);
#else
	mavlink_geofence_info_t packet;
	packet.fenceFloor = fenceFloor;
	packet.fenceCeiling = fenceCeiling;
	packet.numVertices = numVertices;
	packet.msgType = msgType;
	packet.fenceType = fenceType;
	packet.fenceID = fenceID;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GEOFENCE_INFO_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_GEOFENCE_INFO;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_GEOFENCE_INFO_MIN_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_CRC);
}

/**
 * @brief Pack a geofence_info message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param msgType Flag, 0-new fence, 1-remove existing fence
 * @param fenceType Flag, 0-keep in, 1-keep out
 * @param fenceID  Geo-fence ID
 * @param fenceFloor  Fence floor height 
 * @param fenceCeiling  Fence ceiling height 
 * @param numVertices  Number of polygon verticies 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_geofence_info_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint8_t msgType,uint8_t fenceType,uint8_t fenceID,float fenceFloor,float fenceCeiling,int32_t numVertices)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GEOFENCE_INFO_LEN];
	_mav_put_float(buf, 0, fenceFloor);
	_mav_put_float(buf, 4, fenceCeiling);
	_mav_put_int32_t(buf, 8, numVertices);
	_mav_put_uint8_t(buf, 12, msgType);
	_mav_put_uint8_t(buf, 13, fenceType);
	_mav_put_uint8_t(buf, 14, fenceID);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GEOFENCE_INFO_LEN);
#else
	mavlink_geofence_info_t packet;
	packet.fenceFloor = fenceFloor;
	packet.fenceCeiling = fenceCeiling;
	packet.numVertices = numVertices;
	packet.msgType = msgType;
	packet.fenceType = fenceType;
	packet.fenceID = fenceID;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GEOFENCE_INFO_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_GEOFENCE_INFO;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_GEOFENCE_INFO_MIN_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_CRC);
}

/**
 * @brief Encode a geofence_info struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param geofence_info C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_geofence_info_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_geofence_info_t* geofence_info)
{
	return mavlink_msg_geofence_info_pack(system_id, component_id, msg, geofence_info->msgType, geofence_info->fenceType, geofence_info->fenceID, geofence_info->fenceFloor, geofence_info->fenceCeiling, geofence_info->numVertices);
}

/**
 * @brief Encode a geofence_info struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param geofence_info C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_geofence_info_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_geofence_info_t* geofence_info)
{
	return mavlink_msg_geofence_info_pack_chan(system_id, component_id, chan, msg, geofence_info->msgType, geofence_info->fenceType, geofence_info->fenceID, geofence_info->fenceFloor, geofence_info->fenceCeiling, geofence_info->numVertices);
}

/**
 * @brief Send a geofence_info message
 * @param chan MAVLink channel to send the message
 *
 * @param msgType Flag, 0-new fence, 1-remove existing fence
 * @param fenceType Flag, 0-keep in, 1-keep out
 * @param fenceID  Geo-fence ID
 * @param fenceFloor  Fence floor height 
 * @param fenceCeiling  Fence ceiling height 
 * @param numVertices  Number of polygon verticies 
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_geofence_info_send(mavlink_channel_t chan, uint8_t msgType, uint8_t fenceType, uint8_t fenceID, float fenceFloor, float fenceCeiling, int32_t numVertices)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GEOFENCE_INFO_LEN];
	_mav_put_float(buf, 0, fenceFloor);
	_mav_put_float(buf, 4, fenceCeiling);
	_mav_put_int32_t(buf, 8, numVertices);
	_mav_put_uint8_t(buf, 12, msgType);
	_mav_put_uint8_t(buf, 13, fenceType);
	_mav_put_uint8_t(buf, 14, fenceID);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GEOFENCE_INFO, buf, MAVLINK_MSG_ID_GEOFENCE_INFO_MIN_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_CRC);
#else
	mavlink_geofence_info_t packet;
	packet.fenceFloor = fenceFloor;
	packet.fenceCeiling = fenceCeiling;
	packet.numVertices = numVertices;
	packet.msgType = msgType;
	packet.fenceType = fenceType;
	packet.fenceID = fenceID;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GEOFENCE_INFO, (const char *)&packet, MAVLINK_MSG_ID_GEOFENCE_INFO_MIN_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_CRC);
#endif
}

/**
 * @brief Send a geofence_info message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_geofence_info_send_struct(mavlink_channel_t chan, const mavlink_geofence_info_t* geofence_info)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_geofence_info_send(chan, geofence_info->msgType, geofence_info->fenceType, geofence_info->fenceID, geofence_info->fenceFloor, geofence_info->fenceCeiling, geofence_info->numVertices);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GEOFENCE_INFO, (const char *)geofence_info, MAVLINK_MSG_ID_GEOFENCE_INFO_MIN_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_CRC);
#endif
}

#if MAVLINK_MSG_ID_GEOFENCE_INFO_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_geofence_info_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t msgType, uint8_t fenceType, uint8_t fenceID, float fenceFloor, float fenceCeiling, int32_t numVertices)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_float(buf, 0, fenceFloor);
	_mav_put_float(buf, 4, fenceCeiling);
	_mav_put_int32_t(buf, 8, numVertices);
	_mav_put_uint8_t(buf, 12, msgType);
	_mav_put_uint8_t(buf, 13, fenceType);
	_mav_put_uint8_t(buf, 14, fenceID);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GEOFENCE_INFO, buf, MAVLINK_MSG_ID_GEOFENCE_INFO_MIN_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_CRC);
#else
	mavlink_geofence_info_t *packet = (mavlink_geofence_info_t *)msgbuf;
	packet->fenceFloor = fenceFloor;
	packet->fenceCeiling = fenceCeiling;
	packet->numVertices = numVertices;
	packet->msgType = msgType;
	packet->fenceType = fenceType;
	packet->fenceID = fenceID;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GEOFENCE_INFO, (const char *)packet, MAVLINK_MSG_ID_GEOFENCE_INFO_MIN_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_LEN, MAVLINK_MSG_ID_GEOFENCE_INFO_CRC);
#endif
}
#endif

#endif

// MESSAGE GEOFENCE_INFO UNPACKING


/**
 * @brief Get field msgType from geofence_info message
 *
 * @return Flag, 0-new fence, 1-remove existing fence
 */
static inline uint8_t mavlink_msg_geofence_info_get_msgType(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  12);
}

/**
 * @brief Get field fenceType from geofence_info message
 *
 * @return Flag, 0-keep in, 1-keep out
 */
static inline uint8_t mavlink_msg_geofence_info_get_fenceType(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  13);
}

/**
 * @brief Get field fenceID from geofence_info message
 *
 * @return  Geo-fence ID
 */
static inline uint8_t mavlink_msg_geofence_info_get_fenceID(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  14);
}

/**
 * @brief Get field fenceFloor from geofence_info message
 *
 * @return  Fence floor height 
 */
static inline float mavlink_msg_geofence_info_get_fenceFloor(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  0);
}

/**
 * @brief Get field fenceCeiling from geofence_info message
 *
 * @return  Fence ceiling height 
 */
static inline float mavlink_msg_geofence_info_get_fenceCeiling(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  4);
}

/**
 * @brief Get field numVertices from geofence_info message
 *
 * @return  Number of polygon verticies 
 */
static inline int32_t mavlink_msg_geofence_info_get_numVertices(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int32_t(msg,  8);
}

/**
 * @brief Decode a geofence_info message into a struct
 *
 * @param msg The message to decode
 * @param geofence_info C-struct to decode the message contents into
 */
static inline void mavlink_msg_geofence_info_decode(const mavlink_message_t* msg, mavlink_geofence_info_t* geofence_info)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	geofence_info->fenceFloor = mavlink_msg_geofence_info_get_fenceFloor(msg);
	geofence_info->fenceCeiling = mavlink_msg_geofence_info_get_fenceCeiling(msg);
	geofence_info->numVertices = mavlink_msg_geofence_info_get_numVertices(msg);
	geofence_info->msgType = mavlink_msg_geofence_info_get_msgType(msg);
	geofence_info->fenceType = mavlink_msg_geofence_info_get_fenceType(msg);
	geofence_info->fenceID = mavlink_msg_geofence_info_get_fenceID(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_GEOFENCE_INFO_LEN? msg->len : MAVLINK_MSG_ID_GEOFENCE_INFO_LEN;
        memset(geofence_info, 0, MAVLINK_MSG_ID_GEOFENCE_INFO_LEN);
	memcpy(geofence_info, _MAV_PAYLOAD(msg), len);
#endif
}
