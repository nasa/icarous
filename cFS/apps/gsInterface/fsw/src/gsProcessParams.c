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
    AddParamId("LOGDAADATA",1);
    AddParamId("LOOKAHEAD_TIME",20.0);
    AddParamId("LEFT_TRK",180.0);
    AddParamId("RIGHT_TRK",-180.0);
    AddParamId("MIN_GS",0.2);
    AddParamId("MAX_GS",5.0);
    AddParamId("MIN_VS",-400);
    AddParamId("MAX_VS",400);
    AddParamId("MIN_ALT",0);
    AddParamId("MAX_ALT",500);
    AddParamId("TRK_STEP",1);
    AddParamId("GS_STEP",0.1);
    AddParamId("VS_STEP",5);
    AddParamId("ALT_STEP",1);
    AddParamId("HORIZONTAL_ACCL",1.0);
    AddParamId("VERTICAL_ACCL",1.0);
    AddParamId("TURN_RATE",0.0);
    AddParamId("BANK_ANGLE",0.0);
    AddParamId("VERTICAL_RATE",0.0);
    AddParamId("RECOV_STAB_TIME",0.0);
    AddParamId("MIN_HORIZ_RECOV",2.0);
    AddParamId("MIN_VERT_RECOV",1.0);
    AddParamId("RECOVERY_TRK",1);
    AddParamId("RECOVERY_GS",0);
    AddParamId("RECOVERY_VS",0);
    AddParamId("RECOVERY_ALT",0);
    AddParamId("CA_BANDS",1);
    AddParamId("CA_FACTOR",0.2);
    AddParamId("HORIZONTAL_NMAC",1.0);
    AddParamId("VERTICAL_NMAC",0.5);
    AddParamId("CONFLICT_CRIT",0);
    AddParamId("RECOVERY_CRIT",0);
    AddParamId("CONTOUR_THR",180.0);
    AddParamId("AL_1_ALERT_T",10.0);
    AddParamId("AL_1_DETECTOR",0);  //Hard coded for now, not a parameter
    AddParamId("AL_1_E_ALERT_T",15.0);
    AddParamId("AL_1_REGION",0);    //Hard coded for now, not a parameter
    AddParamId("AL_1_SPREAD_ALT",0);
    AddParamId("AL_1_SPREAD_GS",0);
    AddParamId("AL_1_SPREAD_TRK",0);
    AddParamId("AL_1_SPREAD_VS",0);
    AddParamId("CONFLICT_LEVEL",1);
    AddParamId("LD_CR_DTCN_DET_1",0); //Hard coded for now, not a parameter
    AddParamId("DET_1_WCV_DTHR",5.0);
    AddParamId("DET_1_WCV_TCOA",0.0);
    AddParamId("DET_1_WCV_TTHR",0.0);
    AddParamId("DET_1_WCV_ZTHR",200.0);
    // Tracking parameters
    AddParamId("COMMAND",1);
    AddParamId("TRACKINGOBJID",0);
    AddParamId("PGAINX",0.5);
    AddParamId("PGAINY",0.5);
    AddParamId("PGAINZ",0.5);
    AddParamId("HEADING",270);
    AddParamId("DISTH",2);
    AddParamId("DISTV",2);
    // Trajectory parameters
    AddParamId("OBSBUFFER",1);
    AddParamId("MAXCEILING",50);
    AddParamId("ASTAR_ENABLE3D",0);
    AddParamId("ASTAR_GRIDSIZE",1.0);
    AddParamId("ASTAR_RESSPEED",1.0);
    AddParamId("ASTAR_LOOKAHEAD",1.0);
    AddParamId("ASTAR_DAACONFILE",0);   //Hard coded for now, not a parameter
    AddParamId("RRT_RESSPEED",1.0);
    AddParamId("RRT_NITERATIONS",2000);
    AddParamId("RRT_DT",1.0);
    AddParamId("RRT_MACROSTEPS",5);
    AddParamId("RRT_CAPR",5.0);
    AddParamId("RRT_DAACONFILE",0);     //Hard coded for now, not a parameter
    AddParamId("XTRKDEV",1000.0);
    AddParamId("XTRKGAIN",0.6);
    AddParamId("RESSPEED",1.0);
    AddParamId("SEARCHALGORITHM",1);
    // Geofence parameters
    AddParamId("LOOKAHEAD",2.0);
    AddParamId("HTHRESHOLD",2.0);
    AddParamId("VTHRESHOLD",2.0);
    AddParamId("HSTEPBACK",2.0);
    AddParamId("VSTEPBACK",2.0);
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
