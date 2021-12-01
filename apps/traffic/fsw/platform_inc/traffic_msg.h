/**
 * @file traffic_msg.h
 * @brief Traffic message definition
 */
#ifndef ICAROUS_CFS_TRAFFIC_MSG_H
#define ICAROUS_CFS_TRAFFIC_MSG_H

#include <stdint.h>
#include "cfe.h"
#include "stdbool.h"
#include "Icarous_msg.h"

#define MAX_TRAFFIC_ALERTS 10

/**
 * @struct band_report_t
 * @brief kinematics bands report
 *
 * Published under the following topics: ICAROUS_BAND_REPORT_MID
 */
typedef struct {
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];  /**< cFS header information */
    bands_t altitudeBands;
    bands_t groundSpeedBands;
    bands_t trackBands;
    bands_t verticalSpeedBands;
} band_report_t;

typedef struct{
    uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];
    callsign_t callsign[MAX_TRAFFIC_ALERTS];
    double time;
    uint32_t numAlerts;
    int32_t trafficAlerts[MAX_TRAFFIC_ALERTS];
}traffic_alerts_t;

/**
 * @struct checkTurnSafety_t
 * @brief Message used to request/respond to check if a turn from heading A to heading B is safe.
 *
 * A request is published under TRAFFIC_TURN_CHECK_MID.
 * A response to the above query is also sent in this message published in the TRAFFIC_TURN_CHECK_RESPONSE_MID.
 */
typedef struct{
   uint8_t	TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< cFS header information */
   double fromHeading;                      /**< from heading (degree) */
   double toHeading;	                    /**< to heading (degree)*/
   bool safe;                               /**< true if safe to turn (only used in the response) */
}checkTurnSafety_t;

/**
 * @struct traffic_parameters_t
 * @brief parameters used by the traffic application
 */

typedef struct{
  uint8_t TlmHeader[CFE_SB_TLM_HDR_SIZE];
  bool logDAAdata;
  float lookahead_time;
  float left_trk;
  float right_trk;
  float min_gs;
  float max_gs;
  float min_vs;
  float max_vs;
  float min_alt;
  float max_alt;
  float trk_step;
  float	gs_step;
  float	vs_step;
  float	alt_step;
  float	horizontal_accel;
  float	vertical_accel;
  float	turn_rate;
  float	bank_angle;
  float	vertical_rate;
  float	recovery_stability_time;
  float persistence_time;
  float	min_horizontal_recovery;
  float	min_vertical_recovery;
  bool recovery_trk;
  bool recovery_gs;
  bool recovery_vs;
  bool recovery_alt;
  bool ca_bands;
  float	ca_factor;
  float	horizontal_nmac;
  float	vertical_nmac;
  bool conflict_crit;
  bool recovery_crit;
  float	contour_thr;
  float	alert_1_alerting_time;
  char	alert_1_detector[10];
  float	alert_1_early_alerting_time;
  char	alert_1_region[10];
  float	alert_1_spread_alt;
  float	alert_1_spread_gs;
  float	alert_1_spread_trk;
  float	alert_1_spread_vs;
  uint8_t conflict_level;
  char load_core_detection_det_1[50];
  float det_1_WCV_DTHR;
  float det_1_WCV_TCOA;
  float det_1_WCV_TTHR;
  float det_1_WCV_ZTHR;
  uint32_t trafficSource;
  uint8_t resType;
}traffic_parameters_t;

/**
 * @enum Region
 * Defines the band types
 */
enum Region {UNKNOWN,NONE,FAR,MID,NEAR,RECOVERY,END_OF_REGION};

/**@}*/
#endif //ICAROUS_CFS_TRAFFIC_MSG_H
