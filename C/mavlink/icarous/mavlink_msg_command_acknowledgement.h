// MESSAGE COMMAND_ACKNOWLEDGEMENT PACKING

#define MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT 224

typedef struct MAVLINK_PACKED __mavlink_command_acknowledgement_t
{
 uint8_t acktype; /*< Acknowledgement type (0-waypoints,1-geofence)*/
 uint8_t value; /*< 1-success,0-failure*/
} mavlink_command_acknowledgement_t;

#define MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN 2
#define MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_MIN_LEN 2
#define MAVLINK_MSG_ID_224_LEN 2
#define MAVLINK_MSG_ID_224_MIN_LEN 2

#define MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_CRC 184
#define MAVLINK_MSG_ID_224_CRC 184



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_COMMAND_ACKNOWLEDGEMENT { \
	224, \
	"COMMAND_ACKNOWLEDGEMENT", \
	2, \
	{  { "acktype", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_command_acknowledgement_t, acktype) }, \
         { "value", NULL, MAVLINK_TYPE_UINT8_T, 0, 1, offsetof(mavlink_command_acknowledgement_t, value) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_COMMAND_ACKNOWLEDGEMENT { \
	"COMMAND_ACKNOWLEDGEMENT", \
	2, \
	{  { "acktype", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_command_acknowledgement_t, acktype) }, \
         { "value", NULL, MAVLINK_TYPE_UINT8_T, 0, 1, offsetof(mavlink_command_acknowledgement_t, value) }, \
         } \
}
#endif

/**
 * @brief Pack a command_acknowledgement message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param acktype Acknowledgement type (0-waypoints,1-geofence)
 * @param value 1-success,0-failure
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_command_acknowledgement_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint8_t acktype, uint8_t value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN];
	_mav_put_uint8_t(buf, 0, acktype);
	_mav_put_uint8_t(buf, 1, value);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN);
#else
	mavlink_command_acknowledgement_t packet;
	packet.acktype = acktype;
	packet.value = value;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_MIN_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_CRC);
}

/**
 * @brief Pack a command_acknowledgement message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param acktype Acknowledgement type (0-waypoints,1-geofence)
 * @param value 1-success,0-failure
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_command_acknowledgement_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint8_t acktype,uint8_t value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN];
	_mav_put_uint8_t(buf, 0, acktype);
	_mav_put_uint8_t(buf, 1, value);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN);
#else
	mavlink_command_acknowledgement_t packet;
	packet.acktype = acktype;
	packet.value = value;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_MIN_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_CRC);
}

/**
 * @brief Encode a command_acknowledgement struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param command_acknowledgement C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_command_acknowledgement_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_command_acknowledgement_t* command_acknowledgement)
{
	return mavlink_msg_command_acknowledgement_pack(system_id, component_id, msg, command_acknowledgement->acktype, command_acknowledgement->value);
}

/**
 * @brief Encode a command_acknowledgement struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param command_acknowledgement C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_command_acknowledgement_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_command_acknowledgement_t* command_acknowledgement)
{
	return mavlink_msg_command_acknowledgement_pack_chan(system_id, component_id, chan, msg, command_acknowledgement->acktype, command_acknowledgement->value);
}

/**
 * @brief Send a command_acknowledgement message
 * @param chan MAVLink channel to send the message
 *
 * @param acktype Acknowledgement type (0-waypoints,1-geofence)
 * @param value 1-success,0-failure
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_command_acknowledgement_send(mavlink_channel_t chan, uint8_t acktype, uint8_t value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN];
	_mav_put_uint8_t(buf, 0, acktype);
	_mav_put_uint8_t(buf, 1, value);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT, buf, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_MIN_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_CRC);
#else
	mavlink_command_acknowledgement_t packet;
	packet.acktype = acktype;
	packet.value = value;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT, (const char *)&packet, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_MIN_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_CRC);
#endif
}

/**
 * @brief Send a command_acknowledgement message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_command_acknowledgement_send_struct(mavlink_channel_t chan, const mavlink_command_acknowledgement_t* command_acknowledgement)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_command_acknowledgement_send(chan, command_acknowledgement->acktype, command_acknowledgement->value);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT, (const char *)command_acknowledgement, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_MIN_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_CRC);
#endif
}

#if MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_command_acknowledgement_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t acktype, uint8_t value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint8_t(buf, 0, acktype);
	_mav_put_uint8_t(buf, 1, value);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT, buf, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_MIN_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_CRC);
#else
	mavlink_command_acknowledgement_t *packet = (mavlink_command_acknowledgement_t *)msgbuf;
	packet->acktype = acktype;
	packet->value = value;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT, (const char *)packet, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_MIN_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_CRC);
#endif
}
#endif

#endif

// MESSAGE COMMAND_ACKNOWLEDGEMENT UNPACKING


/**
 * @brief Get field acktype from command_acknowledgement message
 *
 * @return Acknowledgement type (0-waypoints,1-geofence)
 */
static inline uint8_t mavlink_msg_command_acknowledgement_get_acktype(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  0);
}

/**
 * @brief Get field value from command_acknowledgement message
 *
 * @return 1-success,0-failure
 */
static inline uint8_t mavlink_msg_command_acknowledgement_get_value(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  1);
}

/**
 * @brief Decode a command_acknowledgement message into a struct
 *
 * @param msg The message to decode
 * @param command_acknowledgement C-struct to decode the message contents into
 */
static inline void mavlink_msg_command_acknowledgement_decode(const mavlink_message_t* msg, mavlink_command_acknowledgement_t* command_acknowledgement)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	command_acknowledgement->acktype = mavlink_msg_command_acknowledgement_get_acktype(msg);
	command_acknowledgement->value = mavlink_msg_command_acknowledgement_get_value(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN? msg->len : MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN;
        memset(command_acknowledgement, 0, MAVLINK_MSG_ID_COMMAND_ACKNOWLEDGEMENT_LEN);
	memcpy(command_acknowledgement, _MAV_PAYLOAD(msg), len);
#endif
}
