// MESSAGE HEARTBEAT_ICAROUS PACKING

#define MAVLINK_MSG_ID_HEARTBEAT_ICAROUS 227

typedef struct MAVLINK_PACKED __mavlink_heartbeat_icarous_t
{
 int8_t status; /*< FMS state (0-idle, 1-Takeoff, 2-Climb, 3-Cruise, 4-Land)*/
} mavlink_heartbeat_icarous_t;

#define MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN 1
#define MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_MIN_LEN 1
#define MAVLINK_MSG_ID_227_LEN 1
#define MAVLINK_MSG_ID_227_MIN_LEN 1

#define MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_CRC 10
#define MAVLINK_MSG_ID_227_CRC 10



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_HEARTBEAT_ICAROUS { \
	227, \
	"HEARTBEAT_ICAROUS", \
	1, \
	{  { "status", NULL, MAVLINK_TYPE_INT8_T, 0, 0, offsetof(mavlink_heartbeat_icarous_t, status) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_HEARTBEAT_ICAROUS { \
	"HEARTBEAT_ICAROUS", \
	1, \
	{  { "status", NULL, MAVLINK_TYPE_INT8_T, 0, 0, offsetof(mavlink_heartbeat_icarous_t, status) }, \
         } \
}
#endif

/**
 * @brief Pack a heartbeat_icarous message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param status FMS state (0-idle, 1-Takeoff, 2-Climb, 3-Cruise, 4-Land)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_heartbeat_icarous_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       int8_t status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN];
	_mav_put_int8_t(buf, 0, status);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN);
#else
	mavlink_heartbeat_icarous_t packet;
	packet.status = status;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_HEARTBEAT_ICAROUS;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_CRC);
}

/**
 * @brief Pack a heartbeat_icarous message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param status FMS state (0-idle, 1-Takeoff, 2-Climb, 3-Cruise, 4-Land)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_heartbeat_icarous_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           int8_t status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN];
	_mav_put_int8_t(buf, 0, status);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN);
#else
	mavlink_heartbeat_icarous_t packet;
	packet.status = status;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_HEARTBEAT_ICAROUS;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_CRC);
}

/**
 * @brief Encode a heartbeat_icarous struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param heartbeat_icarous C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_heartbeat_icarous_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_heartbeat_icarous_t* heartbeat_icarous)
{
	return mavlink_msg_heartbeat_icarous_pack(system_id, component_id, msg, heartbeat_icarous->status);
}

/**
 * @brief Encode a heartbeat_icarous struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param heartbeat_icarous C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_heartbeat_icarous_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_heartbeat_icarous_t* heartbeat_icarous)
{
	return mavlink_msg_heartbeat_icarous_pack_chan(system_id, component_id, chan, msg, heartbeat_icarous->status);
}

/**
 * @brief Send a heartbeat_icarous message
 * @param chan MAVLink channel to send the message
 *
 * @param status FMS state (0-idle, 1-Takeoff, 2-Climb, 3-Cruise, 4-Land)
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_heartbeat_icarous_send(mavlink_channel_t chan, int8_t status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN];
	_mav_put_int8_t(buf, 0, status);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS, buf, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_CRC);
#else
	mavlink_heartbeat_icarous_t packet;
	packet.status = status;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS, (const char *)&packet, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_CRC);
#endif
}

/**
 * @brief Send a heartbeat_icarous message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_heartbeat_icarous_send_struct(mavlink_channel_t chan, const mavlink_heartbeat_icarous_t* heartbeat_icarous)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_heartbeat_icarous_send(chan, heartbeat_icarous->status);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS, (const char *)heartbeat_icarous, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_CRC);
#endif
}

#if MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_heartbeat_icarous_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  int8_t status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_int8_t(buf, 0, status);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS, buf, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_CRC);
#else
	mavlink_heartbeat_icarous_t *packet = (mavlink_heartbeat_icarous_t *)msgbuf;
	packet->status = status;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS, (const char *)packet, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_MIN_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_CRC);
#endif
}
#endif

#endif

// MESSAGE HEARTBEAT_ICAROUS UNPACKING


/**
 * @brief Get field status from heartbeat_icarous message
 *
 * @return FMS state (0-idle, 1-Takeoff, 2-Climb, 3-Cruise, 4-Land)
 */
static inline int8_t mavlink_msg_heartbeat_icarous_get_status(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int8_t(msg,  0);
}

/**
 * @brief Decode a heartbeat_icarous message into a struct
 *
 * @param msg The message to decode
 * @param heartbeat_icarous C-struct to decode the message contents into
 */
static inline void mavlink_msg_heartbeat_icarous_decode(const mavlink_message_t* msg, mavlink_heartbeat_icarous_t* heartbeat_icarous)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	heartbeat_icarous->status = mavlink_msg_heartbeat_icarous_get_status(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN? msg->len : MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN;
        memset(heartbeat_icarous, 0, MAVLINK_MSG_ID_HEARTBEAT_ICAROUS_LEN);
	memcpy(heartbeat_icarous, _MAV_PAYLOAD(msg), len);
#endif
}
