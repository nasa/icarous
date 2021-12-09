/**
 * @file icarous_msgids.h
 * @brief Message ID definition of common icarous messages
 */

#ifndef CFETOP_MSGIDS_H
#define CFETOP_MSGIDS_H

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
#define ICAROUS_RTL_MID 0x0820            ///< Return to launch: noArgsCmd_t
#ifdef APPDEF_TRACKER
    #define ICAROUS_RAWTRAFFIC_MID 0x0820   ///< Raw traffic message message type: object_t
#else
    #define ICAROUS_RAWTRAFFIC_MID ICAROUS_TRAFFIC_MID ///< If tracker is not running, assume raw traffic is used as final traffic data
#endif
#define UPLINK_FLIGHTPLAN_MID 0x0821      ///< Uplink flight plan
#define DOWNLINK_FLIGHTPLAN_MID 0x0822    ///< Downlink flight plan
#define ICAROUS_VFRHUD_MID 0x0823         ///< VFR hud data
#define ICAROUS_BATTERY_STATUS_MID 0x0824 ///< Battery status
#define RADAR_TRIGGER_MID 0x0825          ///< Radar trigger
#define ICAROUS_RC_CHANNELS_MID 0x0826    ///< RC channels
#define ICAROUS_LOCAL_POSITION_MID 0x0827 ///< Local position information. message type: local_position_t
#define ICAROUS_HOME_POSITION_MID 0x0828  ///< Home position message id
#define ICAROUS_WPREACHED_EXTERNAL_MID 0x0829      ///< Waypoint reached. message type: missionItemReached_t
#define ICAROUS_PARAMUPDATE_MID 0x0830      ///< Waypoint reached. message type: missionItemReached_t
/**@}*/
#define SendSBMsg(msg)\
CFE_SB_TimeStampMsg((CFE_SB_Msg_t * ) &msg); \
CFE_SB_SendMsg((CFE_SB_Msg_t * ) &msg);

#define SetStatus(msg,txt,level)\
memset(msg.buffer,0,sizeof(char)*250); \
strcpy(msg.buffer,txt); \
msg.severity=level; \
SendSBMsg(msg);

#endif //CFETOP_MSGIDS_H
