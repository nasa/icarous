/*
 * process changes to parameters
 *
 *
 */

#include <msgdef/ardupilot_msg.h>
#include "gsInterface.h"
#include "UtilFunctions.h"
#include <stdlib.h>
#include <string.h>

void gsInterface_PublishParams() {
    //Initialize SB messages
    traffic_parameters_t localTrafficParams;
    tracking_parameters_t localTrackingParams;
    trajectory_parameters_t localTrajectoryParams;
    geofence_parameters_t localGeofenceParams;
    CFE_SB_InitMsg(&localTrafficParams,TRAFFIC_PARAMETERS_MID,sizeof(traffic_parameters_t),TRUE);
    CFE_SB_InitMsg(&localTrackingParams,TRACKING_PARAMETERS_MID,sizeof(tracking_parameters_t),TRUE);
    CFE_SB_InitMsg(&localTrajectoryParams,TRAJECTORY_PARAMETERS_MID,sizeof(trajectory_parameters_t),TRUE);
    CFE_SB_InitMsg(&localGeofenceParams,GEOFENCE_PARAMETERS_MID,sizeof(geofence_parameters_t),TRUE);

    //Store the locally saved parameters to the messages
    int i = 0;
    //Traffic Parameters
    localTrafficParams.trafficSource = (uint32_t) gsNextParam;
    localTrafficParams.logDAAdata = (bool) gsNextParam;
    localTrafficParams.lookahead_time = gsNextParam;
    localTrafficParams.left_trk = gsNextParam;
    localTrafficParams.right_trk = gsNextParam;
    localTrafficParams.min_gs = gsNextParam;
    localTrafficParams.max_gs = gsNextParam;
    localTrafficParams.min_vs = gsNextParam;
    localTrafficParams.max_vs = gsNextParam;
    localTrafficParams.min_alt = gsNextParam;
    localTrafficParams.max_alt = gsNextParam;
    localTrafficParams.trk_step = gsNextParam;
    localTrafficParams.gs_step = gsNextParam;
    localTrafficParams.vs_step = gsNextParam;
    localTrafficParams.alt_step = gsNextParam;
    localTrafficParams.horizontal_accel = gsNextParam;
    localTrafficParams.vertical_accel = gsNextParam;
    localTrafficParams.turn_rate = gsNextParam;
    localTrafficParams.bank_angle = gsNextParam;
    localTrafficParams.vertical_rate = gsNextParam;
    localTrafficParams.recovery_stability_time = gsNextParam;
    localTrafficParams.min_horizontal_recovery = gsNextParam;
    localTrafficParams.min_vertical_recovery = gsNextParam;
    localTrafficParams.recovery_trk = (bool) gsNextParam;
    localTrafficParams.recovery_gs = (bool) gsNextParam;
    localTrafficParams.recovery_vs = (bool) gsNextParam;
    localTrafficParams.recovery_alt = (bool) gsNextParam;
    localTrafficParams.ca_bands = (bool) gsNextParam;
    localTrafficParams.ca_factor = gsNextParam;
    localTrafficParams.horizontal_nmac = gsNextParam;
    localTrafficParams.vertical_nmac = gsNextParam;
    localTrafficParams.conflict_crit = (bool) gsNextParam;
    localTrafficParams.recovery_crit = (bool) gsNextParam;
    localTrafficParams.contour_thr = gsNextParam;
    localTrafficParams.alert_1_alerting_time = gsNextParam;
    strcpy(localTrafficParams.alert_1_detector, "det_1");   //Hard coded, not parameter
    //gsNextParam;
    localTrafficParams.alert_1_early_alerting_time = gsNextParam;
    strcpy(localTrafficParams.alert_1_region, "NEAR");      //Hard coded, not parameter
    //gsNextParam;
    localTrafficParams.alert_1_spread_alt = gsNextParam;
    localTrafficParams.alert_1_spread_gs = gsNextParam;
    localTrafficParams.alert_1_spread_trk = gsNextParam;
    localTrafficParams.alert_1_spread_vs = gsNextParam;
    localTrafficParams.conflict_level = (uint8_t) gsNextParam;
    strcpy(localTrafficParams.load_core_detection_det_1, "WCV_TAUMOD"); //Hard coded, not parameter
    //gsNextParam;
    localTrafficParams.det_1_WCV_DTHR = gsNextParam;
    localTrafficParams.det_1_WCV_TCOA = gsNextParam;
    localTrafficParams.det_1_WCV_TTHR = gsNextParam;
    localTrafficParams.det_1_WCV_ZTHR = gsNextParam;
    // Tracking parameters
    localTrackingParams.command = (bool) gsNextParam;
    localTrackingParams.trackingObjId = (int) gsNextParam;
    localTrackingParams.pGainX = (double) gsNextParam;
    localTrackingParams.pGainY = (double) gsNextParam;
    localTrackingParams.pGainZ = (double) gsNextParam;
    localTrackingParams.heading = (double) gsNextParam;
    localTrackingParams.distH = (double) gsNextParam;
    localTrackingParams.distV = (double) gsNextParam;
    // Trajectory parameters
    localTrajectoryParams.obsbuffer = (double) gsNextParam;
    localTrajectoryParams.maxCeiling = (double) gsNextParam;
    localTrajectoryParams.astar_enable3D = (bool) gsNextParam;
    localTrajectoryParams.astar_gridSize = (double) gsNextParam;
    localTrajectoryParams.astar_resSpeed = (double) gsNextParam;
    localTrajectoryParams.astar_lookahead = (double) gsNextParam;
    strcpy(localTrajectoryParams.astar_daaConfigFile, "../ram/DaidalusQuadConfig.txt");   //Hard coded, not parameter
    //gsNextParam;
    localTrajectoryParams.rrt_resSpeed = (double) gsNextParam;
    localTrajectoryParams.rrt_numIterations = (int) gsNextParam;
    localTrajectoryParams.rrt_dt = (double) gsNextParam;
    localTrajectoryParams.rrt_macroSteps = (int) gsNextParam;
    localTrajectoryParams.rrt_capR = (double) gsNextParam;
    strcpy(localTrajectoryParams.rrt_daaConfigFile, "../ram/DaidalusQuadConfig.txt");     //Hard coded, not parameter
    //gsNextParam;
    localTrajectoryParams.xtrkDev = (double) gsNextParam;
    localTrajectoryParams.xtrkGain = (double) gsNextParam;
    localTrajectoryParams.resSpeed = (double) gsNextParam;
    localTrajectoryParams.searchAlgorithm = (uint8_t) gsNextParam;
    // Geofence parameters
    localGeofenceParams.lookahead = (double) gsNextParam;
    localGeofenceParams.hthreshold = (double) gsNextParam;
    localGeofenceParams.vthreshold = (double) gsNextParam;
    localGeofenceParams.hstepback = (double) gsNextParam;
    localGeofenceParams.vstepback = (double) gsNextParam;

    //Send Traffic Params SB message

    SendSBMsg(localTrafficParams);
    SendSBMsg(localTrackingParams);
    SendSBMsg(localTrajectoryParams);
    SendSBMsg(localGeofenceParams);

    //printf("gsInterface Sent SB Param Messages\n");
}
