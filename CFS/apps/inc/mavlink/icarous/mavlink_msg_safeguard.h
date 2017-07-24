#pragma once
// MESSAGE SAFEGUARD PACKING

#define MAVLINK_MSG_ID_SAFEGUARD 224

MAVPACKED(
typedef struct __mavlink_safeguard_t {
 int8_t value; /*< 1-success,0-failure*/
}) mavlink_safeguard_t;

#define MAVLINK_MSG_ID_SAFEGUARD_LEN 1
#define MAVLINK_MSG_ID_SAFEGUARD_MIN_LEN 1
#define MAVLINK_MSG_ID_224_LEN 1
#define MAVLINK_MSG_ID_224_MIN_LEN 1

#define MAVLINK_MSG_ID_SAFEGUARD_CRC 59
#define MAVLINK_MSG_ID_224_CRC 59



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_SAFEGUARD { \
    224, \
    "SAFEGUARD", \
    1, \
    {  { "value", NULL, MAVLINK_TYPE_INT8_T, 0, 0, offsetof(mavlink_safeguard_t, value) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_SAFEGUARD { \
    "SAFEGUARD", \
    1, \
    {  { "value", NULL, MAVLINK_TYPE_INT8_T, 0, 0, offsetof(mavlink_safeguard_t, value) }, \
         } \
}
#endif

/**
 * @brief Pack a safeguard message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param value 1-success,0-failure
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_safeguard_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               int8_t value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_SAFEGUARD_LEN];
    _mav_put_int8_t(buf, 0, value);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SAFEGUARD_LEN);
#else
    mavlink_safeguard_t packet;
    packet.value = value;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SAFEGUARD_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_SAFEGUARD;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_SAFEGUARD_MIN_LEN, MAVLINK_MSG_ID_SAFEGUARD_LEN, MAVLINK_MSG_ID_SAFEGUARD_CRC);
}

/**
 * @brief Pack a safeguard message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param value 1-success,0-failure
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_safeguard_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   int8_t value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_SAFEGUARD_LEN];
    _mav_put_int8_t(buf, 0, value);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SAFEGUARD_LEN);
#else
    mavlink_safeguard_t packet;
    packet.value = value;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SAFEGUARD_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_SAFEGUARD;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_SAFEGUARD_MIN_LEN, MAVLINK_MSG_ID_SAFEGUARD_LEN, MAVLINK_MSG_ID_SAFEGUARD_CRC);
}

/**
 * @brief Encode a safeguard struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param safeguard C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_safeguard_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_safeguard_t* safeguard)
{
    return mavlink_msg_safeguard_pack(system_id, component_id, msg, safeguard->value);
}

/**
 * @brief Encode a safeguard struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param safeguard C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_safeguard_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_safeguard_t* safeguard)
{
    return mavlink_msg_safeguard_pack_chan(system_id, component_id, chan, msg, safeguard->value);
}

/**
 * @brief Send a safeguard message
 * @param chan MAVLink channel to send the message
 *
 * @param value 1-success,0-failure
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_safeguard_send(mavlink_channel_t chan, int8_t value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_SAFEGUARD_LEN];
    _mav_put_int8_t(buf, 0, value);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SAFEGUARD, buf, MAVLINK_MSG_ID_SAFEGUARD_MIN_LEN, MAVLINK_MSG_ID_SAFEGUARD_LEN, MAVLINK_MSG_ID_SAFEGUARD_CRC);
#else
    mavlink_safeguard_t packet;
    packet.value = value;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SAFEGUARD, (const char *)&packet, MAVLINK_MSG_ID_SAFEGUARD_MIN_LEN, MAVLINK_MSG_ID_SAFEGUARD_LEN, MAVLINK_MSG_ID_SAFEGUARD_CRC);
#endif
}

/**
 * @brief Send a safeguard message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_safeguard_send_struct(mavlink_channel_t chan, const mavlink_safeguard_t* safeguard)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_safeguard_send(chan, safeguard->value);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SAFEGUARD, (const char *)safeguard, MAVLINK_MSG_ID_SAFEGUARD_MIN_LEN, MAVLINK_MSG_ID_SAFEGUARD_LEN, MAVLINK_MSG_ID_SAFEGUARD_CRC);
#endif
}

#if MAVLINK_MSG_ID_SAFEGUARD_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_safeguard_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  int8_t value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_int8_t(buf, 0, value);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SAFEGUARD, buf, MAVLINK_MSG_ID_SAFEGUARD_MIN_LEN, MAVLINK_MSG_ID_SAFEGUARD_LEN, MAVLINK_MSG_ID_SAFEGUARD_CRC);
#else
    mavlink_safeguard_t *packet = (mavlink_safeguard_t *)msgbuf;
    packet->value = value;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SAFEGUARD, (const char *)packet, MAVLINK_MSG_ID_SAFEGUARD_MIN_LEN, MAVLINK_MSG_ID_SAFEGUARD_LEN, MAVLINK_MSG_ID_SAFEGUARD_CRC);
#endif
}
#endif

#endif

// MESSAGE SAFEGUARD UNPACKING


/**
 * @brief Get field value from safeguard message
 *
 * @return 1-success,0-failure
 */
static inline int8_t mavlink_msg_safeguard_get_value(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int8_t(msg,  0);
}

/**
 * @brief Decode a safeguard message into a struct
 *
 * @param msg The message to decode
 * @param safeguard C-struct to decode the message contents into
 */
static inline void mavlink_msg_safeguard_decode(const mavlink_message_t* msg, mavlink_safeguard_t* safeguard)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    safeguard->value = mavlink_msg_safeguard_get_value(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_SAFEGUARD_LEN? msg->len : MAVLINK_MSG_ID_SAFEGUARD_LEN;
        memset(safeguard, 0, MAVLINK_MSG_ID_SAFEGUARD_LEN);
    memcpy(safeguard, _MAV_PAYLOAD(msg), len);
#endif
}
