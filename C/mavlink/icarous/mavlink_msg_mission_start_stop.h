// MESSAGE MISSION_START_STOP PACKING

#define MAVLINK_MSG_ID_MISSION_START_STOP 223

typedef struct MAVLINK_PACKED __mavlink_mission_start_stop_t
{
 uint8_t missionStart; /*<  mission start (true)/stop (false)*/
} mavlink_mission_start_stop_t;

#define MAVLINK_MSG_ID_MISSION_START_STOP_LEN 1
#define MAVLINK_MSG_ID_MISSION_START_STOP_MIN_LEN 1
#define MAVLINK_MSG_ID_223_LEN 1
#define MAVLINK_MSG_ID_223_MIN_LEN 1

#define MAVLINK_MSG_ID_MISSION_START_STOP_CRC 205
#define MAVLINK_MSG_ID_223_CRC 205



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_MISSION_START_STOP { \
	223, \
	"MISSION_START_STOP", \
	1, \
	{  { "missionStart", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_mission_start_stop_t, missionStart) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_MISSION_START_STOP { \
	"MISSION_START_STOP", \
	1, \
	{  { "missionStart", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_mission_start_stop_t, missionStart) }, \
         } \
}
#endif

/**
 * @brief Pack a mission_start_stop message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param missionStart  mission start (true)/stop (false)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_mission_start_stop_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint8_t missionStart)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_MISSION_START_STOP_LEN];
	_mav_put_uint8_t(buf, 0, missionStart);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_MISSION_START_STOP_LEN);
#else
	mavlink_mission_start_stop_t packet;
	packet.missionStart = missionStart;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_MISSION_START_STOP_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_MISSION_START_STOP;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_MISSION_START_STOP_MIN_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_CRC);
}

/**
 * @brief Pack a mission_start_stop message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param missionStart  mission start (true)/stop (false)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_mission_start_stop_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint8_t missionStart)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_MISSION_START_STOP_LEN];
	_mav_put_uint8_t(buf, 0, missionStart);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_MISSION_START_STOP_LEN);
#else
	mavlink_mission_start_stop_t packet;
	packet.missionStart = missionStart;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_MISSION_START_STOP_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_MISSION_START_STOP;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_MISSION_START_STOP_MIN_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_CRC);
}

/**
 * @brief Encode a mission_start_stop struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param mission_start_stop C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_mission_start_stop_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_mission_start_stop_t* mission_start_stop)
{
	return mavlink_msg_mission_start_stop_pack(system_id, component_id, msg, mission_start_stop->missionStart);
}

/**
 * @brief Encode a mission_start_stop struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param mission_start_stop C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_mission_start_stop_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_mission_start_stop_t* mission_start_stop)
{
	return mavlink_msg_mission_start_stop_pack_chan(system_id, component_id, chan, msg, mission_start_stop->missionStart);
}

/**
 * @brief Send a mission_start_stop message
 * @param chan MAVLink channel to send the message
 *
 * @param missionStart  mission start (true)/stop (false)
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_mission_start_stop_send(mavlink_channel_t chan, uint8_t missionStart)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_MISSION_START_STOP_LEN];
	_mav_put_uint8_t(buf, 0, missionStart);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MISSION_START_STOP, buf, MAVLINK_MSG_ID_MISSION_START_STOP_MIN_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_CRC);
#else
	mavlink_mission_start_stop_t packet;
	packet.missionStart = missionStart;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MISSION_START_STOP, (const char *)&packet, MAVLINK_MSG_ID_MISSION_START_STOP_MIN_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_CRC);
#endif
}

/**
 * @brief Send a mission_start_stop message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_mission_start_stop_send_struct(mavlink_channel_t chan, const mavlink_mission_start_stop_t* mission_start_stop)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_mission_start_stop_send(chan, mission_start_stop->missionStart);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MISSION_START_STOP, (const char *)mission_start_stop, MAVLINK_MSG_ID_MISSION_START_STOP_MIN_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_CRC);
#endif
}

#if MAVLINK_MSG_ID_MISSION_START_STOP_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_mission_start_stop_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t missionStart)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint8_t(buf, 0, missionStart);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MISSION_START_STOP, buf, MAVLINK_MSG_ID_MISSION_START_STOP_MIN_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_CRC);
#else
	mavlink_mission_start_stop_t *packet = (mavlink_mission_start_stop_t *)msgbuf;
	packet->missionStart = missionStart;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_MISSION_START_STOP, (const char *)packet, MAVLINK_MSG_ID_MISSION_START_STOP_MIN_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_LEN, MAVLINK_MSG_ID_MISSION_START_STOP_CRC);
#endif
}
#endif

#endif

// MESSAGE MISSION_START_STOP UNPACKING


/**
 * @brief Get field missionStart from mission_start_stop message
 *
 * @return  mission start (true)/stop (false)
 */
static inline uint8_t mavlink_msg_mission_start_stop_get_missionStart(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  0);
}

/**
 * @brief Decode a mission_start_stop message into a struct
 *
 * @param msg The message to decode
 * @param mission_start_stop C-struct to decode the message contents into
 */
static inline void mavlink_msg_mission_start_stop_decode(const mavlink_message_t* msg, mavlink_mission_start_stop_t* mission_start_stop)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	mission_start_stop->missionStart = mavlink_msg_mission_start_stop_get_missionStart(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_MISSION_START_STOP_LEN? msg->len : MAVLINK_MSG_ID_MISSION_START_STOP_LEN;
        memset(mission_start_stop, 0, MAVLINK_MSG_ID_MISSION_START_STOP_LEN);
	memcpy(mission_start_stop, _MAV_PAYLOAD(msg), len);
#endif
}
