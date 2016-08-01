// MESSAGE FLIGHTPLAN_INFO PACKING

#define MAVLINK_MSG_ID_FLIGHTPLAN_INFO 221

typedef struct MAVLINK_PACKED __mavlink_flightplan_info_t
{
 float standoffDist; /*< Standoff distance*/
 float maxHorDev; /*< Maximum horizontal deviation from flight plan*/
 float maxVerDev; /*< Maximum vertical deviation from flight plan*/
 int32_t numWaypoints; /*< Number of waypoints*/
 uint8_t msgType; /*< Message type, 0-new flight plan, 1-update existing flight plan*/
} mavlink_flightplan_info_t;

#define MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN 17
#define MAVLINK_MSG_ID_FLIGHTPLAN_INFO_MIN_LEN 17
#define MAVLINK_MSG_ID_221_LEN 17
#define MAVLINK_MSG_ID_221_MIN_LEN 17

#define MAVLINK_MSG_ID_FLIGHTPLAN_INFO_CRC 134
#define MAVLINK_MSG_ID_221_CRC 134



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_FLIGHTPLAN_INFO { \
	221, \
	"FLIGHTPLAN_INFO", \
	5, \
	{  { "standoffDist", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_flightplan_info_t, standoffDist) }, \
         { "maxHorDev", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_flightplan_info_t, maxHorDev) }, \
         { "maxVerDev", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_flightplan_info_t, maxVerDev) }, \
         { "numWaypoints", NULL, MAVLINK_TYPE_INT32_T, 0, 12, offsetof(mavlink_flightplan_info_t, numWaypoints) }, \
         { "msgType", NULL, MAVLINK_TYPE_UINT8_T, 0, 16, offsetof(mavlink_flightplan_info_t, msgType) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_FLIGHTPLAN_INFO { \
	"FLIGHTPLAN_INFO", \
	5, \
	{  { "standoffDist", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_flightplan_info_t, standoffDist) }, \
         { "maxHorDev", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_flightplan_info_t, maxHorDev) }, \
         { "maxVerDev", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_flightplan_info_t, maxVerDev) }, \
         { "numWaypoints", NULL, MAVLINK_TYPE_INT32_T, 0, 12, offsetof(mavlink_flightplan_info_t, numWaypoints) }, \
         { "msgType", NULL, MAVLINK_TYPE_UINT8_T, 0, 16, offsetof(mavlink_flightplan_info_t, msgType) }, \
         } \
}
#endif

/**
 * @brief Pack a flightplan_info message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param msgType Message type, 0-new flight plan, 1-update existing flight plan
 * @param standoffDist Standoff distance
 * @param maxHorDev Maximum horizontal deviation from flight plan
 * @param maxVerDev Maximum vertical deviation from flight plan
 * @param numWaypoints Number of waypoints
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_flightplan_info_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint8_t msgType, float standoffDist, float maxHorDev, float maxVerDev, int32_t numWaypoints)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN];
	_mav_put_float(buf, 0, standoffDist);
	_mav_put_float(buf, 4, maxHorDev);
	_mav_put_float(buf, 8, maxVerDev);
	_mav_put_int32_t(buf, 12, numWaypoints);
	_mav_put_uint8_t(buf, 16, msgType);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN);
#else
	mavlink_flightplan_info_t packet;
	packet.standoffDist = standoffDist;
	packet.maxHorDev = maxHorDev;
	packet.maxVerDev = maxVerDev;
	packet.numWaypoints = numWaypoints;
	packet.msgType = msgType;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_FLIGHTPLAN_INFO;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_MIN_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_CRC);
}

/**
 * @brief Pack a flightplan_info message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param msgType Message type, 0-new flight plan, 1-update existing flight plan
 * @param standoffDist Standoff distance
 * @param maxHorDev Maximum horizontal deviation from flight plan
 * @param maxVerDev Maximum vertical deviation from flight plan
 * @param numWaypoints Number of waypoints
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_flightplan_info_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint8_t msgType,float standoffDist,float maxHorDev,float maxVerDev,int32_t numWaypoints)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN];
	_mav_put_float(buf, 0, standoffDist);
	_mav_put_float(buf, 4, maxHorDev);
	_mav_put_float(buf, 8, maxVerDev);
	_mav_put_int32_t(buf, 12, numWaypoints);
	_mav_put_uint8_t(buf, 16, msgType);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN);
#else
	mavlink_flightplan_info_t packet;
	packet.standoffDist = standoffDist;
	packet.maxHorDev = maxHorDev;
	packet.maxVerDev = maxVerDev;
	packet.numWaypoints = numWaypoints;
	packet.msgType = msgType;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_FLIGHTPLAN_INFO;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_MIN_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_CRC);
}

/**
 * @brief Encode a flightplan_info struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param flightplan_info C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_flightplan_info_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_flightplan_info_t* flightplan_info)
{
	return mavlink_msg_flightplan_info_pack(system_id, component_id, msg, flightplan_info->msgType, flightplan_info->standoffDist, flightplan_info->maxHorDev, flightplan_info->maxVerDev, flightplan_info->numWaypoints);
}

/**
 * @brief Encode a flightplan_info struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param flightplan_info C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_flightplan_info_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_flightplan_info_t* flightplan_info)
{
	return mavlink_msg_flightplan_info_pack_chan(system_id, component_id, chan, msg, flightplan_info->msgType, flightplan_info->standoffDist, flightplan_info->maxHorDev, flightplan_info->maxVerDev, flightplan_info->numWaypoints);
}

/**
 * @brief Send a flightplan_info message
 * @param chan MAVLink channel to send the message
 *
 * @param msgType Message type, 0-new flight plan, 1-update existing flight plan
 * @param standoffDist Standoff distance
 * @param maxHorDev Maximum horizontal deviation from flight plan
 * @param maxVerDev Maximum vertical deviation from flight plan
 * @param numWaypoints Number of waypoints
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_flightplan_info_send(mavlink_channel_t chan, uint8_t msgType, float standoffDist, float maxHorDev, float maxVerDev, int32_t numWaypoints)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN];
	_mav_put_float(buf, 0, standoffDist);
	_mav_put_float(buf, 4, maxHorDev);
	_mav_put_float(buf, 8, maxVerDev);
	_mav_put_int32_t(buf, 12, numWaypoints);
	_mav_put_uint8_t(buf, 16, msgType);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FLIGHTPLAN_INFO, buf, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_MIN_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_CRC);
#else
	mavlink_flightplan_info_t packet;
	packet.standoffDist = standoffDist;
	packet.maxHorDev = maxHorDev;
	packet.maxVerDev = maxVerDev;
	packet.numWaypoints = numWaypoints;
	packet.msgType = msgType;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FLIGHTPLAN_INFO, (const char *)&packet, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_MIN_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_CRC);
#endif
}

/**
 * @brief Send a flightplan_info message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_flightplan_info_send_struct(mavlink_channel_t chan, const mavlink_flightplan_info_t* flightplan_info)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_flightplan_info_send(chan, flightplan_info->msgType, flightplan_info->standoffDist, flightplan_info->maxHorDev, flightplan_info->maxVerDev, flightplan_info->numWaypoints);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FLIGHTPLAN_INFO, (const char *)flightplan_info, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_MIN_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_CRC);
#endif
}

#if MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_flightplan_info_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t msgType, float standoffDist, float maxHorDev, float maxVerDev, int32_t numWaypoints)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_float(buf, 0, standoffDist);
	_mav_put_float(buf, 4, maxHorDev);
	_mav_put_float(buf, 8, maxVerDev);
	_mav_put_int32_t(buf, 12, numWaypoints);
	_mav_put_uint8_t(buf, 16, msgType);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FLIGHTPLAN_INFO, buf, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_MIN_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_CRC);
#else
	mavlink_flightplan_info_t *packet = (mavlink_flightplan_info_t *)msgbuf;
	packet->standoffDist = standoffDist;
	packet->maxHorDev = maxHorDev;
	packet->maxVerDev = maxVerDev;
	packet->numWaypoints = numWaypoints;
	packet->msgType = msgType;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FLIGHTPLAN_INFO, (const char *)packet, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_MIN_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_CRC);
#endif
}
#endif

#endif

// MESSAGE FLIGHTPLAN_INFO UNPACKING


/**
 * @brief Get field msgType from flightplan_info message
 *
 * @return Message type, 0-new flight plan, 1-update existing flight plan
 */
static inline uint8_t mavlink_msg_flightplan_info_get_msgType(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  16);
}

/**
 * @brief Get field standoffDist from flightplan_info message
 *
 * @return Standoff distance
 */
static inline float mavlink_msg_flightplan_info_get_standoffDist(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  0);
}

/**
 * @brief Get field maxHorDev from flightplan_info message
 *
 * @return Maximum horizontal deviation from flight plan
 */
static inline float mavlink_msg_flightplan_info_get_maxHorDev(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  4);
}

/**
 * @brief Get field maxVerDev from flightplan_info message
 *
 * @return Maximum vertical deviation from flight plan
 */
static inline float mavlink_msg_flightplan_info_get_maxVerDev(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field numWaypoints from flightplan_info message
 *
 * @return Number of waypoints
 */
static inline int32_t mavlink_msg_flightplan_info_get_numWaypoints(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int32_t(msg,  12);
}

/**
 * @brief Decode a flightplan_info message into a struct
 *
 * @param msg The message to decode
 * @param flightplan_info C-struct to decode the message contents into
 */
static inline void mavlink_msg_flightplan_info_decode(const mavlink_message_t* msg, mavlink_flightplan_info_t* flightplan_info)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	flightplan_info->standoffDist = mavlink_msg_flightplan_info_get_standoffDist(msg);
	flightplan_info->maxHorDev = mavlink_msg_flightplan_info_get_maxHorDev(msg);
	flightplan_info->maxVerDev = mavlink_msg_flightplan_info_get_maxVerDev(msg);
	flightplan_info->numWaypoints = mavlink_msg_flightplan_info_get_numWaypoints(msg);
	flightplan_info->msgType = mavlink_msg_flightplan_info_get_msgType(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN? msg->len : MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN;
        memset(flightplan_info, 0, MAVLINK_MSG_ID_FLIGHTPLAN_INFO_LEN);
	memcpy(flightplan_info, _MAV_PAYLOAD(msg), len);
#endif
}
