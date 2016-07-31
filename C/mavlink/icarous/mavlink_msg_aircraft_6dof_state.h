// MESSAGE AIRCRAFT_6DOF_STATE PACKING

#define MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE 225

typedef struct MAVLINK_PACKED __mavlink_aircraft_6dof_state_t
{
 float lat; /*< latitude*/
 float lon; /*< longitude*/
 float alt; /*< altitude*/
 float roll; /*< roll*/
 float pitch; /*< pitch*/
 float yaw; /*< yaw*/
} mavlink_aircraft_6dof_state_t;

#define MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN 24
#define MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_MIN_LEN 24
#define MAVLINK_MSG_ID_225_LEN 24
#define MAVLINK_MSG_ID_225_MIN_LEN 24

#define MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_CRC 239
#define MAVLINK_MSG_ID_225_CRC 239



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_AIRCRAFT_6DOF_STATE { \
	225, \
	"AIRCRAFT_6DOF_STATE", \
	6, \
	{  { "lat", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_aircraft_6dof_state_t, lat) }, \
         { "lon", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_aircraft_6dof_state_t, lon) }, \
         { "alt", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_aircraft_6dof_state_t, alt) }, \
         { "roll", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_aircraft_6dof_state_t, roll) }, \
         { "pitch", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_aircraft_6dof_state_t, pitch) }, \
         { "yaw", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_aircraft_6dof_state_t, yaw) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_AIRCRAFT_6DOF_STATE { \
	"AIRCRAFT_6DOF_STATE", \
	6, \
	{  { "lat", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_aircraft_6dof_state_t, lat) }, \
         { "lon", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_aircraft_6dof_state_t, lon) }, \
         { "alt", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_aircraft_6dof_state_t, alt) }, \
         { "roll", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_aircraft_6dof_state_t, roll) }, \
         { "pitch", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_aircraft_6dof_state_t, pitch) }, \
         { "yaw", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_aircraft_6dof_state_t, yaw) }, \
         } \
}
#endif

/**
 * @brief Pack a aircraft_6dof_state message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param lat latitude
 * @param lon longitude
 * @param alt altitude
 * @param roll roll
 * @param pitch pitch
 * @param yaw yaw
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_aircraft_6dof_state_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       float lat, float lon, float alt, float roll, float pitch, float yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN];
	_mav_put_float(buf, 0, lat);
	_mav_put_float(buf, 4, lon);
	_mav_put_float(buf, 8, alt);
	_mav_put_float(buf, 12, roll);
	_mav_put_float(buf, 16, pitch);
	_mav_put_float(buf, 20, yaw);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN);
#else
	mavlink_aircraft_6dof_state_t packet;
	packet.lat = lat;
	packet.lon = lon;
	packet.alt = alt;
	packet.roll = roll;
	packet.pitch = pitch;
	packet.yaw = yaw;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_MIN_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_CRC);
}

/**
 * @brief Pack a aircraft_6dof_state message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param lat latitude
 * @param lon longitude
 * @param alt altitude
 * @param roll roll
 * @param pitch pitch
 * @param yaw yaw
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_aircraft_6dof_state_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           float lat,float lon,float alt,float roll,float pitch,float yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN];
	_mav_put_float(buf, 0, lat);
	_mav_put_float(buf, 4, lon);
	_mav_put_float(buf, 8, alt);
	_mav_put_float(buf, 12, roll);
	_mav_put_float(buf, 16, pitch);
	_mav_put_float(buf, 20, yaw);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN);
#else
	mavlink_aircraft_6dof_state_t packet;
	packet.lat = lat;
	packet.lon = lon;
	packet.alt = alt;
	packet.roll = roll;
	packet.pitch = pitch;
	packet.yaw = yaw;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_MIN_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_CRC);
}

/**
 * @brief Encode a aircraft_6dof_state struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param aircraft_6dof_state C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_aircraft_6dof_state_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_aircraft_6dof_state_t* aircraft_6dof_state)
{
	return mavlink_msg_aircraft_6dof_state_pack(system_id, component_id, msg, aircraft_6dof_state->lat, aircraft_6dof_state->lon, aircraft_6dof_state->alt, aircraft_6dof_state->roll, aircraft_6dof_state->pitch, aircraft_6dof_state->yaw);
}

/**
 * @brief Encode a aircraft_6dof_state struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param aircraft_6dof_state C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_aircraft_6dof_state_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_aircraft_6dof_state_t* aircraft_6dof_state)
{
	return mavlink_msg_aircraft_6dof_state_pack_chan(system_id, component_id, chan, msg, aircraft_6dof_state->lat, aircraft_6dof_state->lon, aircraft_6dof_state->alt, aircraft_6dof_state->roll, aircraft_6dof_state->pitch, aircraft_6dof_state->yaw);
}

/**
 * @brief Send a aircraft_6dof_state message
 * @param chan MAVLink channel to send the message
 *
 * @param lat latitude
 * @param lon longitude
 * @param alt altitude
 * @param roll roll
 * @param pitch pitch
 * @param yaw yaw
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_aircraft_6dof_state_send(mavlink_channel_t chan, float lat, float lon, float alt, float roll, float pitch, float yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN];
	_mav_put_float(buf, 0, lat);
	_mav_put_float(buf, 4, lon);
	_mav_put_float(buf, 8, alt);
	_mav_put_float(buf, 12, roll);
	_mav_put_float(buf, 16, pitch);
	_mav_put_float(buf, 20, yaw);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE, buf, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_MIN_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_CRC);
#else
	mavlink_aircraft_6dof_state_t packet;
	packet.lat = lat;
	packet.lon = lon;
	packet.alt = alt;
	packet.roll = roll;
	packet.pitch = pitch;
	packet.yaw = yaw;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE, (const char *)&packet, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_MIN_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_CRC);
#endif
}

/**
 * @brief Send a aircraft_6dof_state message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_aircraft_6dof_state_send_struct(mavlink_channel_t chan, const mavlink_aircraft_6dof_state_t* aircraft_6dof_state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_aircraft_6dof_state_send(chan, aircraft_6dof_state->lat, aircraft_6dof_state->lon, aircraft_6dof_state->alt, aircraft_6dof_state->roll, aircraft_6dof_state->pitch, aircraft_6dof_state->yaw);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE, (const char *)aircraft_6dof_state, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_MIN_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_CRC);
#endif
}

#if MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_aircraft_6dof_state_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  float lat, float lon, float alt, float roll, float pitch, float yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_float(buf, 0, lat);
	_mav_put_float(buf, 4, lon);
	_mav_put_float(buf, 8, alt);
	_mav_put_float(buf, 12, roll);
	_mav_put_float(buf, 16, pitch);
	_mav_put_float(buf, 20, yaw);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE, buf, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_MIN_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_CRC);
#else
	mavlink_aircraft_6dof_state_t *packet = (mavlink_aircraft_6dof_state_t *)msgbuf;
	packet->lat = lat;
	packet->lon = lon;
	packet->alt = alt;
	packet->roll = roll;
	packet->pitch = pitch;
	packet->yaw = yaw;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE, (const char *)packet, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_MIN_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_CRC);
#endif
}
#endif

#endif

// MESSAGE AIRCRAFT_6DOF_STATE UNPACKING


/**
 * @brief Get field lat from aircraft_6dof_state message
 *
 * @return latitude
 */
static inline float mavlink_msg_aircraft_6dof_state_get_lat(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  0);
}

/**
 * @brief Get field lon from aircraft_6dof_state message
 *
 * @return longitude
 */
static inline float mavlink_msg_aircraft_6dof_state_get_lon(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  4);
}

/**
 * @brief Get field alt from aircraft_6dof_state message
 *
 * @return altitude
 */
static inline float mavlink_msg_aircraft_6dof_state_get_alt(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field roll from aircraft_6dof_state message
 *
 * @return roll
 */
static inline float mavlink_msg_aircraft_6dof_state_get_roll(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field pitch from aircraft_6dof_state message
 *
 * @return pitch
 */
static inline float mavlink_msg_aircraft_6dof_state_get_pitch(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Get field yaw from aircraft_6dof_state message
 *
 * @return yaw
 */
static inline float mavlink_msg_aircraft_6dof_state_get_yaw(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  20);
}

/**
 * @brief Decode a aircraft_6dof_state message into a struct
 *
 * @param msg The message to decode
 * @param aircraft_6dof_state C-struct to decode the message contents into
 */
static inline void mavlink_msg_aircraft_6dof_state_decode(const mavlink_message_t* msg, mavlink_aircraft_6dof_state_t* aircraft_6dof_state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	aircraft_6dof_state->lat = mavlink_msg_aircraft_6dof_state_get_lat(msg);
	aircraft_6dof_state->lon = mavlink_msg_aircraft_6dof_state_get_lon(msg);
	aircraft_6dof_state->alt = mavlink_msg_aircraft_6dof_state_get_alt(msg);
	aircraft_6dof_state->roll = mavlink_msg_aircraft_6dof_state_get_roll(msg);
	aircraft_6dof_state->pitch = mavlink_msg_aircraft_6dof_state_get_pitch(msg);
	aircraft_6dof_state->yaw = mavlink_msg_aircraft_6dof_state_get_yaw(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN? msg->len : MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN;
        memset(aircraft_6dof_state, 0, MAVLINK_MSG_ID_AIRCRAFT_6DOF_STATE_LEN);
	memcpy(aircraft_6dof_state, _MAV_PAYLOAD(msg), len);
#endif
}
