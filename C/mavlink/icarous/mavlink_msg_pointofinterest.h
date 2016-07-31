// MESSAGE POINTOFINTEREST PACKING

#define MAVLINK_MSG_ID_POINTOFINTEREST 220

typedef struct MAVLINK_PACKED __mavlink_pointofinterest_t
{
 float latx; /*< Latitude (deg)/Relative North (m)*/
 float lony; /*< Longitude (deg)/Relative East (m)*/
 float altz; /*< Altitude (deg)/Relative UP (m)*/
 float heading; /*< Heading*/
 float vx; /*< Velocity (north m/s)*/
 float vy; /*< Velocity (east m/s)*/
 float vz; /*< Velocity (down m/s)*/
 int8_t id; /*< Identifier (0-Waypoint,1-geofence,2-Obstacle,3-Traffic,4-Others)*/
 int8_t subtype; /*< Sub type*/
 int8_t index; /*< Index (sequence number)*/
 int8_t geodesic; /*< 1 for lat/lon/alt,0 for NED*/
} mavlink_pointofinterest_t;

#define MAVLINK_MSG_ID_POINTOFINTEREST_LEN 32
#define MAVLINK_MSG_ID_POINTOFINTEREST_MIN_LEN 32
#define MAVLINK_MSG_ID_220_LEN 32
#define MAVLINK_MSG_ID_220_MIN_LEN 32

#define MAVLINK_MSG_ID_POINTOFINTEREST_CRC 31
#define MAVLINK_MSG_ID_220_CRC 31



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_POINTOFINTEREST { \
	220, \
	"POINTOFINTEREST", \
	11, \
	{  { "latx", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_pointofinterest_t, latx) }, \
         { "lony", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_pointofinterest_t, lony) }, \
         { "altz", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_pointofinterest_t, altz) }, \
         { "heading", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_pointofinterest_t, heading) }, \
         { "vx", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_pointofinterest_t, vx) }, \
         { "vy", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_pointofinterest_t, vy) }, \
         { "vz", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_pointofinterest_t, vz) }, \
         { "id", NULL, MAVLINK_TYPE_INT8_T, 0, 28, offsetof(mavlink_pointofinterest_t, id) }, \
         { "subtype", NULL, MAVLINK_TYPE_INT8_T, 0, 29, offsetof(mavlink_pointofinterest_t, subtype) }, \
         { "index", NULL, MAVLINK_TYPE_INT8_T, 0, 30, offsetof(mavlink_pointofinterest_t, index) }, \
         { "geodesic", NULL, MAVLINK_TYPE_INT8_T, 0, 31, offsetof(mavlink_pointofinterest_t, geodesic) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_POINTOFINTEREST { \
	"POINTOFINTEREST", \
	11, \
	{  { "latx", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_pointofinterest_t, latx) }, \
         { "lony", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_pointofinterest_t, lony) }, \
         { "altz", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_pointofinterest_t, altz) }, \
         { "heading", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_pointofinterest_t, heading) }, \
         { "vx", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_pointofinterest_t, vx) }, \
         { "vy", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_pointofinterest_t, vy) }, \
         { "vz", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_pointofinterest_t, vz) }, \
         { "id", NULL, MAVLINK_TYPE_INT8_T, 0, 28, offsetof(mavlink_pointofinterest_t, id) }, \
         { "subtype", NULL, MAVLINK_TYPE_INT8_T, 0, 29, offsetof(mavlink_pointofinterest_t, subtype) }, \
         { "index", NULL, MAVLINK_TYPE_INT8_T, 0, 30, offsetof(mavlink_pointofinterest_t, index) }, \
         { "geodesic", NULL, MAVLINK_TYPE_INT8_T, 0, 31, offsetof(mavlink_pointofinterest_t, geodesic) }, \
         } \
}
#endif

/**
 * @brief Pack a pointofinterest message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param id Identifier (0-Waypoint,1-geofence,2-Obstacle,3-Traffic,4-Others)
 * @param subtype Sub type
 * @param index Index (sequence number)
 * @param geodesic 1 for lat/lon/alt,0 for NED
 * @param latx Latitude (deg)/Relative North (m)
 * @param lony Longitude (deg)/Relative East (m)
 * @param altz Altitude (deg)/Relative UP (m)
 * @param heading Heading
 * @param vx Velocity (north m/s)
 * @param vy Velocity (east m/s)
 * @param vz Velocity (down m/s)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_pointofinterest_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       int8_t id, int8_t subtype, int8_t index, int8_t geodesic, float latx, float lony, float altz, float heading, float vx, float vy, float vz)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_POINTOFINTEREST_LEN];
	_mav_put_float(buf, 0, latx);
	_mav_put_float(buf, 4, lony);
	_mav_put_float(buf, 8, altz);
	_mav_put_float(buf, 12, heading);
	_mav_put_float(buf, 16, vx);
	_mav_put_float(buf, 20, vy);
	_mav_put_float(buf, 24, vz);
	_mav_put_int8_t(buf, 28, id);
	_mav_put_int8_t(buf, 29, subtype);
	_mav_put_int8_t(buf, 30, index);
	_mav_put_int8_t(buf, 31, geodesic);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_POINTOFINTEREST_LEN);
#else
	mavlink_pointofinterest_t packet;
	packet.latx = latx;
	packet.lony = lony;
	packet.altz = altz;
	packet.heading = heading;
	packet.vx = vx;
	packet.vy = vy;
	packet.vz = vz;
	packet.id = id;
	packet.subtype = subtype;
	packet.index = index;
	packet.geodesic = geodesic;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_POINTOFINTEREST_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_POINTOFINTEREST;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_POINTOFINTEREST_MIN_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_CRC);
}

/**
 * @brief Pack a pointofinterest message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param id Identifier (0-Waypoint,1-geofence,2-Obstacle,3-Traffic,4-Others)
 * @param subtype Sub type
 * @param index Index (sequence number)
 * @param geodesic 1 for lat/lon/alt,0 for NED
 * @param latx Latitude (deg)/Relative North (m)
 * @param lony Longitude (deg)/Relative East (m)
 * @param altz Altitude (deg)/Relative UP (m)
 * @param heading Heading
 * @param vx Velocity (north m/s)
 * @param vy Velocity (east m/s)
 * @param vz Velocity (down m/s)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_pointofinterest_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           int8_t id,int8_t subtype,int8_t index,int8_t geodesic,float latx,float lony,float altz,float heading,float vx,float vy,float vz)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_POINTOFINTEREST_LEN];
	_mav_put_float(buf, 0, latx);
	_mav_put_float(buf, 4, lony);
	_mav_put_float(buf, 8, altz);
	_mav_put_float(buf, 12, heading);
	_mav_put_float(buf, 16, vx);
	_mav_put_float(buf, 20, vy);
	_mav_put_float(buf, 24, vz);
	_mav_put_int8_t(buf, 28, id);
	_mav_put_int8_t(buf, 29, subtype);
	_mav_put_int8_t(buf, 30, index);
	_mav_put_int8_t(buf, 31, geodesic);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_POINTOFINTEREST_LEN);
#else
	mavlink_pointofinterest_t packet;
	packet.latx = latx;
	packet.lony = lony;
	packet.altz = altz;
	packet.heading = heading;
	packet.vx = vx;
	packet.vy = vy;
	packet.vz = vz;
	packet.id = id;
	packet.subtype = subtype;
	packet.index = index;
	packet.geodesic = geodesic;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_POINTOFINTEREST_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_POINTOFINTEREST;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_POINTOFINTEREST_MIN_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_CRC);
}

/**
 * @brief Encode a pointofinterest struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param pointofinterest C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_pointofinterest_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_pointofinterest_t* pointofinterest)
{
	return mavlink_msg_pointofinterest_pack(system_id, component_id, msg, pointofinterest->id, pointofinterest->subtype, pointofinterest->index, pointofinterest->geodesic, pointofinterest->latx, pointofinterest->lony, pointofinterest->altz, pointofinterest->heading, pointofinterest->vx, pointofinterest->vy, pointofinterest->vz);
}

/**
 * @brief Encode a pointofinterest struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param pointofinterest C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_pointofinterest_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_pointofinterest_t* pointofinterest)
{
	return mavlink_msg_pointofinterest_pack_chan(system_id, component_id, chan, msg, pointofinterest->id, pointofinterest->subtype, pointofinterest->index, pointofinterest->geodesic, pointofinterest->latx, pointofinterest->lony, pointofinterest->altz, pointofinterest->heading, pointofinterest->vx, pointofinterest->vy, pointofinterest->vz);
}

/**
 * @brief Send a pointofinterest message
 * @param chan MAVLink channel to send the message
 *
 * @param id Identifier (0-Waypoint,1-geofence,2-Obstacle,3-Traffic,4-Others)
 * @param subtype Sub type
 * @param index Index (sequence number)
 * @param geodesic 1 for lat/lon/alt,0 for NED
 * @param latx Latitude (deg)/Relative North (m)
 * @param lony Longitude (deg)/Relative East (m)
 * @param altz Altitude (deg)/Relative UP (m)
 * @param heading Heading
 * @param vx Velocity (north m/s)
 * @param vy Velocity (east m/s)
 * @param vz Velocity (down m/s)
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_pointofinterest_send(mavlink_channel_t chan, int8_t id, int8_t subtype, int8_t index, int8_t geodesic, float latx, float lony, float altz, float heading, float vx, float vy, float vz)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_POINTOFINTEREST_LEN];
	_mav_put_float(buf, 0, latx);
	_mav_put_float(buf, 4, lony);
	_mav_put_float(buf, 8, altz);
	_mav_put_float(buf, 12, heading);
	_mav_put_float(buf, 16, vx);
	_mav_put_float(buf, 20, vy);
	_mav_put_float(buf, 24, vz);
	_mav_put_int8_t(buf, 28, id);
	_mav_put_int8_t(buf, 29, subtype);
	_mav_put_int8_t(buf, 30, index);
	_mav_put_int8_t(buf, 31, geodesic);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POINTOFINTEREST, buf, MAVLINK_MSG_ID_POINTOFINTEREST_MIN_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_CRC);
#else
	mavlink_pointofinterest_t packet;
	packet.latx = latx;
	packet.lony = lony;
	packet.altz = altz;
	packet.heading = heading;
	packet.vx = vx;
	packet.vy = vy;
	packet.vz = vz;
	packet.id = id;
	packet.subtype = subtype;
	packet.index = index;
	packet.geodesic = geodesic;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POINTOFINTEREST, (const char *)&packet, MAVLINK_MSG_ID_POINTOFINTEREST_MIN_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_CRC);
#endif
}

/**
 * @brief Send a pointofinterest message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_pointofinterest_send_struct(mavlink_channel_t chan, const mavlink_pointofinterest_t* pointofinterest)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_pointofinterest_send(chan, pointofinterest->id, pointofinterest->subtype, pointofinterest->index, pointofinterest->geodesic, pointofinterest->latx, pointofinterest->lony, pointofinterest->altz, pointofinterest->heading, pointofinterest->vx, pointofinterest->vy, pointofinterest->vz);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POINTOFINTEREST, (const char *)pointofinterest, MAVLINK_MSG_ID_POINTOFINTEREST_MIN_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_CRC);
#endif
}

#if MAVLINK_MSG_ID_POINTOFINTEREST_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_pointofinterest_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  int8_t id, int8_t subtype, int8_t index, int8_t geodesic, float latx, float lony, float altz, float heading, float vx, float vy, float vz)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_float(buf, 0, latx);
	_mav_put_float(buf, 4, lony);
	_mav_put_float(buf, 8, altz);
	_mav_put_float(buf, 12, heading);
	_mav_put_float(buf, 16, vx);
	_mav_put_float(buf, 20, vy);
	_mav_put_float(buf, 24, vz);
	_mav_put_int8_t(buf, 28, id);
	_mav_put_int8_t(buf, 29, subtype);
	_mav_put_int8_t(buf, 30, index);
	_mav_put_int8_t(buf, 31, geodesic);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POINTOFINTEREST, buf, MAVLINK_MSG_ID_POINTOFINTEREST_MIN_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_CRC);
#else
	mavlink_pointofinterest_t *packet = (mavlink_pointofinterest_t *)msgbuf;
	packet->latx = latx;
	packet->lony = lony;
	packet->altz = altz;
	packet->heading = heading;
	packet->vx = vx;
	packet->vy = vy;
	packet->vz = vz;
	packet->id = id;
	packet->subtype = subtype;
	packet->index = index;
	packet->geodesic = geodesic;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POINTOFINTEREST, (const char *)packet, MAVLINK_MSG_ID_POINTOFINTEREST_MIN_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_LEN, MAVLINK_MSG_ID_POINTOFINTEREST_CRC);
#endif
}
#endif

#endif

// MESSAGE POINTOFINTEREST UNPACKING


/**
 * @brief Get field id from pointofinterest message
 *
 * @return Identifier (0-Waypoint,1-geofence,2-Obstacle,3-Traffic,4-Others)
 */
static inline int8_t mavlink_msg_pointofinterest_get_id(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int8_t(msg,  28);
}

/**
 * @brief Get field subtype from pointofinterest message
 *
 * @return Sub type
 */
static inline int8_t mavlink_msg_pointofinterest_get_subtype(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int8_t(msg,  29);
}

/**
 * @brief Get field index from pointofinterest message
 *
 * @return Index (sequence number)
 */
static inline int8_t mavlink_msg_pointofinterest_get_index(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int8_t(msg,  30);
}

/**
 * @brief Get field geodesic from pointofinterest message
 *
 * @return 1 for lat/lon/alt,0 for NED
 */
static inline int8_t mavlink_msg_pointofinterest_get_geodesic(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int8_t(msg,  31);
}

/**
 * @brief Get field latx from pointofinterest message
 *
 * @return Latitude (deg)/Relative North (m)
 */
static inline float mavlink_msg_pointofinterest_get_latx(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  0);
}

/**
 * @brief Get field lony from pointofinterest message
 *
 * @return Longitude (deg)/Relative East (m)
 */
static inline float mavlink_msg_pointofinterest_get_lony(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  4);
}

/**
 * @brief Get field altz from pointofinterest message
 *
 * @return Altitude (deg)/Relative UP (m)
 */
static inline float mavlink_msg_pointofinterest_get_altz(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field heading from pointofinterest message
 *
 * @return Heading
 */
static inline float mavlink_msg_pointofinterest_get_heading(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field vx from pointofinterest message
 *
 * @return Velocity (north m/s)
 */
static inline float mavlink_msg_pointofinterest_get_vx(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Get field vy from pointofinterest message
 *
 * @return Velocity (east m/s)
 */
static inline float mavlink_msg_pointofinterest_get_vy(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  20);
}

/**
 * @brief Get field vz from pointofinterest message
 *
 * @return Velocity (down m/s)
 */
static inline float mavlink_msg_pointofinterest_get_vz(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  24);
}

/**
 * @brief Decode a pointofinterest message into a struct
 *
 * @param msg The message to decode
 * @param pointofinterest C-struct to decode the message contents into
 */
static inline void mavlink_msg_pointofinterest_decode(const mavlink_message_t* msg, mavlink_pointofinterest_t* pointofinterest)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	pointofinterest->latx = mavlink_msg_pointofinterest_get_latx(msg);
	pointofinterest->lony = mavlink_msg_pointofinterest_get_lony(msg);
	pointofinterest->altz = mavlink_msg_pointofinterest_get_altz(msg);
	pointofinterest->heading = mavlink_msg_pointofinterest_get_heading(msg);
	pointofinterest->vx = mavlink_msg_pointofinterest_get_vx(msg);
	pointofinterest->vy = mavlink_msg_pointofinterest_get_vy(msg);
	pointofinterest->vz = mavlink_msg_pointofinterest_get_vz(msg);
	pointofinterest->id = mavlink_msg_pointofinterest_get_id(msg);
	pointofinterest->subtype = mavlink_msg_pointofinterest_get_subtype(msg);
	pointofinterest->index = mavlink_msg_pointofinterest_get_index(msg);
	pointofinterest->geodesic = mavlink_msg_pointofinterest_get_geodesic(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_POINTOFINTEREST_LEN? msg->len : MAVLINK_MSG_ID_POINTOFINTEREST_LEN;
        memset(pointofinterest, 0, MAVLINK_MSG_ID_POINTOFINTEREST_LEN);
	memcpy(pointofinterest, _MAV_PAYLOAD(msg), len);
#endif
}
