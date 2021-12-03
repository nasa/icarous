/**
 * @file traffic_msgids.h
 * @brief Traffic message id definitions
 */
#ifndef ICAROUS_CFS_TRAFFIC_MSGIDS_H
#define ICAROUS_CFS_TRAFFIC_MSGIDS_H

#define ICAROUS_BANDS_TRACK_MID          0x0350 ///< Kinematic track traffic bands. message type: cfsBands_t
#define ICAROUS_BANDS_SPEED_MID          0x0351 ///< Kinematic speed traffic bands. message type: cfsBands_t
#define ICAROUS_BANDS_VS_MID             0x0352 ///< Kinematic vertical speed traffic bands. message type: cfsBands_t
#define ICAROUS_BANDS_ALT_MID            0x0353 ///< Kinematic altitude speed bands. message type: cfsBands_t
#define ICAROUS_BAND_REPORT_MID          0x0354 ///< Kinematic bands report. message type: band_report_t
#define TRAFFIC_TURN_CHECK_MID           0x0336 ///< Request to check if a turn is safe. message type: checkTurnSafety_t
#define TRAFFIC_TURN_CHKECK_RESPONSE_MID 0x0337 ///< Response to turn safety query. message type: checkTurnSafety_t
#define TRAFFIC_ALERTS_MID               0x0338 ///< Traffic Alerts information
#define TRAFFIC_PARAMETERS_MID           0x0903 ///< Parameters for the traffic application

#endif //ICAROUS_CFS_TRAFFIC_MSGIDS_H
