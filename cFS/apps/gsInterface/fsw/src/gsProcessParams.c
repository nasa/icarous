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

void gsInterface_InitializeParamIds(){
    int i = 0;
    //Traffic Parameters
    AddParamId("LOGDAADATA");
    AddParamId("LOOKAHEAD_TIME");
    AddParamId("LEFT_TRK");
    AddParamId("RIGHT_TRK");
    AddParamId("MIN_GS");
    AddParamId("MAX_GS");
    AddParamId("MIN_VS");
    AddParamId("MAX_VS");
    AddParamId("MIN_ALT");
    AddParamId("MAX_ALT");
    AddParamId("TRK_STEP");
    AddParamId("GS_STEP");
    AddParamId("VS_STEP");
    AddParamId("ALT_STEP");
    AddParamId("HORIZONTAL_ACCL");
    AddParamId("VERTICAL_ACCL");
    AddParamId("TURN_RATE");
    AddParamId("BANK_ANGLE");
    AddParamId("VERTICAL_RATE");
    AddParamId("RECOV_STAB_TIME");
    AddParamId("MIN_HORIZ_RECOV");
    AddParamId("MIN_VERT_RECOV");
    AddParamId("RECOVERY_TRK");
    AddParamId("RECOVERY_GS");
    AddParamId("RECOVERY_VS");
    AddParamId("RECOVERY_ALT");
    AddParamId("CA_BANDS");
    AddParamId("CA_FACTOR");
    AddParamId("HORIZONTAL_NMAC");
    AddParamId("VERTICAL_NMAC");
    AddParamId("CONFLICT_CRIT");
    AddParamId("RECOVERY_CRIT");
    AddParamId("CONTOUR_THR");
    AddParamId("AL_1_ALERT_T");
    AddParamId("AL_1_DETECTOR");  //Hard coded for now, not a parameter
    AddParamId("AL_1_E_ALERT_T");
    AddParamId("AL_1_REGION");    //Hard coded for now, not a parameter
    AddParamId("AL_1_SPREAD_ALT");
    AddParamId("AL_1_SPREAD_GS");
    AddParamId("AL_1_SPREAD_TRK");
    AddParamId("AL_1_SPREAD_VS");
    AddParamId("CONFLICT_LEVEL");
    AddParamId("LD_CR_DTCN_DET_1"); //Hard coded for now, not a parameter
    AddParamId("DET_1_WCV_DTHR");
    AddParamId("DET_1_WCV_TCOA");
    AddParamId("DET_1_WCV_TTHR");
    AddParamId("DET_1_WCV_ZTHR");
    // Tracking parameters
    AddParamId("COMMAND");
    AddParamId("TRACKINGOBJID");
    AddParamId("PGAINX");
    AddParamId("PGAINY");
    AddParamId("PGAINZ");
    AddParamId("HEADING");
    AddParamId("DISTH");
    AddParamId("DISTV");
    // Trajectory parameters
    AddParamId("OBSBUFFER");
    AddParamId("MAXCEILING");
    AddParamId("ASTAR_ENABLE3D");
    AddParamId("ASTAR_GRIDSIZE");
    AddParamId("ASTAR_RESSPEED");
    AddParamId("ASTAR_LOOKAHEAD");
    AddParamId("ASTAR_DAACONFILE");   //Hard coded for now, not a parameter
    AddParamId("RRT_RESSPEED");
    AddParamId("RRT_NITERATIONS");
    AddParamId("RRT_DT");
    AddParamId("RRT_MACROSTEPS");
    AddParamId("RRT_CAPR");
    AddParamId("RRT_DAACONFILE");     //Hard coded for now, not a parameter
    AddParamId("XTRKDEV");
    AddParamId("XTRKGAIN");
    AddParamId("RESSPEED");
    AddParamId("SEARCHALGORITHM");
    // Geofence parameters
    AddParamId("LOOKAHEAD");
    AddParamId("HTHRESHOLD");
    AddParamId("VTHRESHOLD");
    AddParamId("HSTEPBACK");
    AddParamId("VSTEPBACK");
}

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
    localTrafficParams.logDAAdata = (bool) NextParam;
    localTrafficParams.lookahead_time = NextParam;
    localTrafficParams.left_trk = NextParam;
    localTrafficParams.right_trk = NextParam;
    localTrafficParams.min_gs = NextParam;
    localTrafficParams.max_gs = NextParam;
    localTrafficParams.min_vs = NextParam;
    localTrafficParams.max_vs = NextParam;
    localTrafficParams.min_alt = NextParam;
    localTrafficParams.max_alt = NextParam;
    localTrafficParams.trk_step = NextParam;
    localTrafficParams.gs_step = NextParam;
    localTrafficParams.vs_step = NextParam;
    localTrafficParams.alt_step = NextParam;
    localTrafficParams.horizontal_accel = NextParam;
    localTrafficParams.vertical_accel = NextParam;
    localTrafficParams.turn_rate = NextParam;
    localTrafficParams.bank_angle = NextParam;
    localTrafficParams.vertical_rate = NextParam;
    localTrafficParams.recovery_stability_time = NextParam;
    localTrafficParams.min_horizontal_recovery = NextParam;
    localTrafficParams.min_vertical_recovery = NextParam;
    localTrafficParams.recovery_trk = (bool) NextParam;
    localTrafficParams.recovery_gs = (bool) NextParam;
    localTrafficParams.recovery_vs = (bool) NextParam;
    localTrafficParams.recovery_alt = (bool) NextParam;
    localTrafficParams.ca_bands = (bool) NextParam;
    localTrafficParams.ca_factor = NextParam;
    localTrafficParams.horizontal_nmac = NextParam;
    localTrafficParams.vertical_nmac = NextParam;
    localTrafficParams.conflict_crit = (bool) NextParam;
    localTrafficParams.recovery_crit = (bool) NextParam;
    localTrafficParams.contour_thr = NextParam;
    localTrafficParams.alert_1_alerting_time = NextParam;
    strcpy(localTrafficParams.alert_1_detector, "det 1");   //Hard coded, not parameter
    NextParam;
    localTrafficParams.alert_1_early_alerting_time = NextParam;
    strcpy(localTrafficParams.alert_1_region, "NEAR");      //Hard coded, not parameter
    NextParam;
    localTrafficParams.alert_1_spread_alt = NextParam;
    localTrafficParams.alert_1_spread_gs = NextParam;
    localTrafficParams.alert_1_spread_trk = NextParam;
    localTrafficParams.alert_1_spread_vs = NextParam;
    localTrafficParams.conflict_level = (uint8_t) NextParam;
    strcpy(localTrafficParams.load_core_detection_det_1, "WCV_TAUMOD"); //Hard coded, not parameter
    NextParam;
    localTrafficParams.det_1_WCV_DTHR = NextParam;
    localTrafficParams.det_1_WCV_TCOA = NextParam;
    localTrafficParams.det_1_WCV_TTHR = NextParam;
    localTrafficParams.det_1_WCV_ZTHR = NextParam;
    // Tracking parameters
    localTrackingParams.command = (bool) NextParam;
    localTrackingParams.trackingObjId = (int) NextParam;
    localTrackingParams.pGainX = (double) NextParam;
    localTrackingParams.pGainY = (double) NextParam;
    localTrackingParams.pGainZ = (double) NextParam;
    localTrackingParams.heading = (double) NextParam;
    localTrackingParams.distH = (double) NextParam;
    localTrackingParams.distV = (double) NextParam;
    // Trajectory parameters
    localTrajectoryParams.obsbuffer = (double) NextParam;
    localTrajectoryParams.maxCeiling = (double) NextParam;
    localTrajectoryParams.astar_enable3D = (bool) NextParam;
    localTrajectoryParams.astar_gridSize = (double) NextParam;
    localTrajectoryParams.astar_resSpeed = (double) NextParam;
    localTrajectoryParams.astar_lookahead = (double) NextParam;
    strcpy(localTrajectoryParams.astar_daaConfigFile, "../ram/DaidalusQuadConfig.txt");   //Hard coded, not parameter
    NextParam;
    localTrajectoryParams.rrt_resSpeed = (double) NextParam;
    localTrajectoryParams.rrt_numIterations = (int) NextParam;
    localTrajectoryParams.rrt_dt = (double) NextParam;
    localTrajectoryParams.rrt_macroSteps = (int) NextParam;
    localTrajectoryParams.rrt_capR = (double) NextParam;
    strcpy(localTrajectoryParams.rrt_daaConfigFile, "../ram/DaidalusQuadConfig.txt");     //Hard coded, not parameter
    NextParam;
    localTrajectoryParams.xtrkDev = (double) NextParam;
    localTrajectoryParams.xtrkGain = (double) NextParam;
    localTrajectoryParams.resSpeed = (double) NextParam;
    localTrajectoryParams.searchAlgorithm = (uint8_t) NextParam;
    // Geofence parameters
    localGeofenceParams.lookahead = (double) NextParam;
    localGeofenceParams.hthreshold = (double) NextParam;
    localGeofenceParams.vthreshold = (double) NextParam;
    localGeofenceParams.hstepback = (double) NextParam;
    localGeofenceParams.vstepback = (double) NextParam;

    //Send Traffic Params SB message

    SendSBMsg(localTrafficParams);
    SendSBMsg(localTrackingParams);
    SendSBMsg(localTrajectoryParams);
    SendSBMsg(localGeofenceParams);

    //printf("gsInterface Sent SB Param Messages\n");
}
