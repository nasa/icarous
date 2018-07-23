/**
 * @file ardupilot_msgids.h
 * @brief Message ID definition of ardupilot app messages
 */

#ifndef CFETOP_MSGIDS_H
#define CFETOP_MSGIDS_H

/**
 * @defgroup ARDUPILOT_MESSAGE_TOPICS
 * @brief Message topics definitions for ardupilot messages
 * @ingroup ARDUPILOT
 * @ingroup TOPICS
 * @{
 */

#define ICAROUS_FLIGHTPLAN_MID    0x0811  ///< Flight plan message. message type: flightplan_t
#define ICAROUS_WPREACHED_MID 0x0812      ///< Waypoint reached. message type: missionItemReached_t
#define ICAROUS_GEOFENCE_MID 0x0813       ///< Geofence information. message type: geofence_t
#define ICAROUS_STARTMISSION_MID 0x0814   ///< Command to start a mission. message type: argsCmd_t
#define ICAROUS_RESET_MID 0x0815          ///< Reset the mission. message type: noArgsCmd_t
#define ICAROUS_TRAFFIC_MID 0x0816        ///< Traffic information. message type: object_t
#define ICAROUS_POSITION_MID 0x0817       ///< Position information. message type: position_t
#define ICAROUS_ATTITUDE_MID 0x0818       ///< Attitude information. message type: see attitude_t
#define ICAROUS_VELOCITY_MID 0x0819       ///< Velocity information.
#define ICAROUS_COMMANDS_MID 0x081A       ///< Icarous commands. message type: argsCmd_t
#define ICAROUS_COMACK_MID 0x081B         ///< Command acknowledgement. message type: cmdAck_t
#define ICAROUS_RESETFP_MID 0x081D        ///< Clear flight plan
#define ICAROUS_STATUS_MID 0x081E         ///< Icarous status message. message type:status_t
#define ICAROUS_DITCH_MID 0x081F          ///< Ditching signal: noArgsCmd_t
#define ICAROUS_TRACK_STATUS_MID 0x0820   ///< Command to start/stop tracking. message type: argsCmd_t
#define UPLINK_FLIGHTPLAN_MID 0x0821      ///< Uplink flight plan
#define DOWNLINK_FLIGHTPLAN_MID 0x0822    ///< Downlink flight plan

/**@}*/
#define SendSBMsg(msg)\
CFE_SB_TimeStampMsg((CFE_SB_Msg_t * ) &msg); \
CFE_SB_SendMsg((CFE_SB_Msg_t * ) &msg);


#endif //CFETOP_MSGIDS_H
