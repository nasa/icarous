// MESSAGE HEARTBEAT_COMBOX PACKING

#define MAVLINK_MSG_ID_HEARTBEAT_COMBOX 219

typedef struct MAVLINK_PACKED __mavlink_heartbeat_combox_t
{
 uint64_t count; /*< count*/
} mavlink_heartbeat_combox_t;

#define MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN 8
#define MAVLINK_MSG_ID_HEARTBEAT_COMBOX_MIN_LEN 8
#define MAVLINK_MSG_ID_219_LEN 8
#define MAVLINK_MSG_ID_219_MIN_LEN 8

#define MAVLINK_MSG_ID_HEARTBEAT_COMBOX_CRC 104
#define MAVLINK_MSG_ID_219_CRC 104



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_HEARTBEAT_COMBOX { \
	219, \
	"HEARTBEAT_COMBOX", \
	1, \
	{  { "count", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_heartbeat_combox_t, count) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_HEARTBEAT_COMBOX { \
	"HEARTBEAT_COMBOX", \
	1, \
	{  { "count", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_heartbeat_combox_t, count) }, \
         } \
}
#endif

/**
 * @brief Pack a heartbeat_combox message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param count count
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_heartbeat_combox_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint64_t count)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN];
	_mav_put_uint64_t(buf, 0, count);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN);
#else
	mavlink_heartbeat_combox_t packet;
	packet.count = count;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_HEARTBEAT_COMBOX;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_CRC);
}

/**
 * @brief Pack a heartbeat_combox message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param count count
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_heartbeat_combox_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint64_t count)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN];
	_mav_put_uint64_t(buf, 0, count);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN);
#else
	mavlink_heartbeat_combox_t packet;
	packet.count = count;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_HEARTBEAT_COMBOX;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_CRC);
}

/**
 * @brief Encode a heartbeat_combox struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param heartbeat_combox C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_heartbeat_combox_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_heartbeat_combox_t* heartbeat_combox)
{
	return mavlink_msg_heartbeat_combox_pack(system_id, component_id, msg, heartbeat_combox->count);
}

/**
 * @brief Encode a heartbeat_combox struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param heartbeat_combox C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_heartbeat_combox_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_heartbeat_combox_t* heartbeat_combox)
{
	return mavlink_msg_heartbeat_combox_pack_chan(system_id, component_id, chan, msg, heartbeat_combox->count);
}

/**
 * @brief Send a heartbeat_combox message
 * @param chan MAVLink channel to send the message
 *
 * @param count count
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_heartbeat_combox_send(mavlink_channel_t chan, uint64_t count)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN];
	_mav_put_uint64_t(buf, 0, count);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_HEARTBEAT_COMBOX, buf, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_CRC);
#else
	mavlink_heartbeat_combox_t packet;
	packet.count = count;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_HEARTBEAT_COMBOX, (const char *)&packet, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_CRC);
#endif
}

/**
 * @brief Send a heartbeat_combox message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_heartbeat_combox_send_struct(mavlink_channel_t chan, const mavlink_heartbeat_combox_t* heartbeat_combox)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_heartbeat_combox_send(chan, heartbeat_combox->count);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_HEARTBEAT_COMBOX, (const char *)heartbeat_combox, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_CRC);
#endif
}

#if MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_heartbeat_combox_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t count)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint64_t(buf, 0, count);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_HEARTBEAT_COMBOX, buf, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_CRC);
#else
	mavlink_heartbeat_combox_t *packet = (mavlink_heartbeat_combox_t *)msgbuf;
	packet->count = count;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_HEARTBEAT_COMBOX, (const char *)packet, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_CRC);
#endif
}
#endif

#endif

// MESSAGE HEARTBEAT_COMBOX UNPACKING


/**
 * @brief Get field count from heartbeat_combox message
 *
 * @return count
 */
static inline uint64_t mavlink_msg_heartbeat_combox_get_count(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Decode a heartbeat_combox message into a struct
 *
 * @param msg The message to decode
 * @param heartbeat_combox C-struct to decode the message contents into
 */
static inline void mavlink_msg_heartbeat_combox_decode(const mavlink_message_t* msg, mavlink_heartbeat_combox_t* heartbeat_combox)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	heartbeat_combox->count = mavlink_msg_heartbeat_combox_get_count(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN? msg->len : MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN;
        memset(heartbeat_combox, 0, MAVLINK_MSG_ID_HEARTBEAT_COMBOX_LEN);
	memcpy(heartbeat_combox, _MAV_PAYLOAD(msg), len);
#endif
}
