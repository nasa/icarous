// MESSAGE POWERLINE_DATA PACKING

#define MAVLINK_MSG_ID_POWERLINE_DATA 226

typedef struct MAVLINK_PACKED __mavlink_powerline_data_t
{
 float distance; /*< Perpendicular distance to power line [m]*/
 float heading; /*< Direction of the perpendicular distance [deg]*/
} mavlink_powerline_data_t;

#define MAVLINK_MSG_ID_POWERLINE_DATA_LEN 8
#define MAVLINK_MSG_ID_POWERLINE_DATA_MIN_LEN 8
#define MAVLINK_MSG_ID_226_LEN 8
#define MAVLINK_MSG_ID_226_MIN_LEN 8

#define MAVLINK_MSG_ID_POWERLINE_DATA_CRC 189
#define MAVLINK_MSG_ID_226_CRC 189



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_POWERLINE_DATA { \
	226, \
	"POWERLINE_DATA", \
	2, \
	{  { "distance", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_powerline_data_t, distance) }, \
         { "heading", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_powerline_data_t, heading) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_POWERLINE_DATA { \
	"POWERLINE_DATA", \
	2, \
	{  { "distance", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_powerline_data_t, distance) }, \
         { "heading", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_powerline_data_t, heading) }, \
         } \
}
#endif

/**
 * @brief Pack a powerline_data message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param distance Perpendicular distance to power line [m]
 * @param heading Direction of the perpendicular distance [deg]
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_powerline_data_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       float distance, float heading)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_POWERLINE_DATA_LEN];
	_mav_put_float(buf, 0, distance);
	_mav_put_float(buf, 4, heading);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_POWERLINE_DATA_LEN);
#else
	mavlink_powerline_data_t packet;
	packet.distance = distance;
	packet.heading = heading;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_POWERLINE_DATA_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_POWERLINE_DATA;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_POWERLINE_DATA_MIN_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_CRC);
}

/**
 * @brief Pack a powerline_data message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param distance Perpendicular distance to power line [m]
 * @param heading Direction of the perpendicular distance [deg]
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_powerline_data_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           float distance,float heading)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_POWERLINE_DATA_LEN];
	_mav_put_float(buf, 0, distance);
	_mav_put_float(buf, 4, heading);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_POWERLINE_DATA_LEN);
#else
	mavlink_powerline_data_t packet;
	packet.distance = distance;
	packet.heading = heading;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_POWERLINE_DATA_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_POWERLINE_DATA;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_POWERLINE_DATA_MIN_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_CRC);
}

/**
 * @brief Encode a powerline_data struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param powerline_data C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_powerline_data_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_powerline_data_t* powerline_data)
{
	return mavlink_msg_powerline_data_pack(system_id, component_id, msg, powerline_data->distance, powerline_data->heading);
}

/**
 * @brief Encode a powerline_data struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param powerline_data C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_powerline_data_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_powerline_data_t* powerline_data)
{
	return mavlink_msg_powerline_data_pack_chan(system_id, component_id, chan, msg, powerline_data->distance, powerline_data->heading);
}

/**
 * @brief Send a powerline_data message
 * @param chan MAVLink channel to send the message
 *
 * @param distance Perpendicular distance to power line [m]
 * @param heading Direction of the perpendicular distance [deg]
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_powerline_data_send(mavlink_channel_t chan, float distance, float heading)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_POWERLINE_DATA_LEN];
	_mav_put_float(buf, 0, distance);
	_mav_put_float(buf, 4, heading);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POWERLINE_DATA, buf, MAVLINK_MSG_ID_POWERLINE_DATA_MIN_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_CRC);
#else
	mavlink_powerline_data_t packet;
	packet.distance = distance;
	packet.heading = heading;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POWERLINE_DATA, (const char *)&packet, MAVLINK_MSG_ID_POWERLINE_DATA_MIN_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_CRC);
#endif
}

/**
 * @brief Send a powerline_data message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_powerline_data_send_struct(mavlink_channel_t chan, const mavlink_powerline_data_t* powerline_data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_powerline_data_send(chan, powerline_data->distance, powerline_data->heading);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POWERLINE_DATA, (const char *)powerline_data, MAVLINK_MSG_ID_POWERLINE_DATA_MIN_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_CRC);
#endif
}

#if MAVLINK_MSG_ID_POWERLINE_DATA_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_powerline_data_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  float distance, float heading)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_float(buf, 0, distance);
	_mav_put_float(buf, 4, heading);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POWERLINE_DATA, buf, MAVLINK_MSG_ID_POWERLINE_DATA_MIN_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_CRC);
#else
	mavlink_powerline_data_t *packet = (mavlink_powerline_data_t *)msgbuf;
	packet->distance = distance;
	packet->heading = heading;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POWERLINE_DATA, (const char *)packet, MAVLINK_MSG_ID_POWERLINE_DATA_MIN_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_LEN, MAVLINK_MSG_ID_POWERLINE_DATA_CRC);
#endif
}
#endif

#endif

// MESSAGE POWERLINE_DATA UNPACKING


/**
 * @brief Get field distance from powerline_data message
 *
 * @return Perpendicular distance to power line [m]
 */
static inline float mavlink_msg_powerline_data_get_distance(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  0);
}

/**
 * @brief Get field heading from powerline_data message
 *
 * @return Direction of the perpendicular distance [deg]
 */
static inline float mavlink_msg_powerline_data_get_heading(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  4);
}

/**
 * @brief Decode a powerline_data message into a struct
 *
 * @param msg The message to decode
 * @param powerline_data C-struct to decode the message contents into
 */
static inline void mavlink_msg_powerline_data_decode(const mavlink_message_t* msg, mavlink_powerline_data_t* powerline_data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	powerline_data->distance = mavlink_msg_powerline_data_get_distance(msg);
	powerline_data->heading = mavlink_msg_powerline_data_get_heading(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_POWERLINE_DATA_LEN? msg->len : MAVLINK_MSG_ID_POWERLINE_DATA_LEN;
        memset(powerline_data, 0, MAVLINK_MSG_ID_POWERLINE_DATA_LEN);
	memcpy(powerline_data, _MAV_PAYLOAD(msg), len);
#endif
}
