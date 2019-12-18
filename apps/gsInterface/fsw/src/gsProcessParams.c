/*
 * process changes to parameters
 *
 *
 */

#include "gsInterface.h"
#include "UtilFunctions.h"
#include <stdlib.h>
#include <string.h>

void gsInterface_PublishParams() {
    //Initialize SB messages

    //Store the locally saved parameters to the messages
    int i = 0;
    //Traffic Parameters

    #ifdef APPDEF_TRAFFIC
    traffic_parameters_t localTrafficParams;
    CFE_SB_InitMsg(&localTrafficParams,TRAFFIC_PARAMETERS_MID,sizeof(traffic_parameters_t),TRUE);
    localTrafficParams.trafficSource = (uint32_t) gsNextParam;
    localTrafficParams.resType = (uint32_t) gsNextParam;
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

    SendSBMsg(localTrafficParams);
    #else
       for(int k=0;k<46;++k){
          gsNextParam;
       }
    #endif

    // Tracking parameters
    #ifdef APPDEF_TRACKING
    tracking_parameters_t localTrackingParams;
    CFE_SB_InitMsg(&localTrackingParams,TRACKING_PARAMETERS_MID,sizeof(tracking_parameters_t),TRUE);
    localTrackingParams.command = (bool) gsNextParam;
    localTrackingParams.trackingObjId = (int) gsNextParam;
    localTrackingParams.pGainX = (double) gsNextParam;
    localTrackingParams.pGainY = (double) gsNextParam;
    localTrackingParams.pGainZ = (double) gsNextParam;
    localTrackingParams.heading = (double) gsNextParam;
    localTrackingParams.distH = (double) gsNextParam;
    localTrackingParams.distV = (double) gsNextParam;
    SendSBMsg(localTrackingParams);
    #else
       for(int k=0;k<8;++k){
          gsNextParam;
       }
    #endif

    // Trajectory parameters
    #ifdef APPDEF_TRAJECTORY
    trajectory_parameters_t localTrajectoryParams;
    CFE_SB_InitMsg(&localTrajectoryParams,TRAJECTORY_PARAMETERS_MID,sizeof(trajectory_parameters_t),TRUE);
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

    SendSBMsg(localTrajectoryParams);
    #else
       for(int k=0;k<15;++k){
          gsNextParam;
       }
    #endif

    // Geofence parameters
    #ifdef APPDEF_GEOFENCE
    geofence_parameters_t localGeofenceParams;
    CFE_SB_InitMsg(&localGeofenceParams,GEOFENCE_PARAMETERS_MID,sizeof(geofence_parameters_t),TRUE);
    localGeofenceParams.lookahead = (double) gsNextParam;
    localGeofenceParams.hthreshold = (double) gsNextParam;
    localGeofenceParams.vthreshold = (double) gsNextParam;
    localGeofenceParams.hstepback = (double) gsNextParam;
    localGeofenceParams.vstepback = (double) gsNextParam;
    SendSBMsg(localGeofenceParams);
    #else
       for(int k=0;k<5;++k){
          gsNextParam;
       }
    #endif

    #ifdef APPDEF_ROTORSIM
    // Rotorsim parameters
    rotorsim_parameters_t localRotorsimParams;
    localRotorsimParams.speed = (double) gsNextParam;
    SendSBMsg(localRotorsimParams);
    #else
       for(int k=0;k<1;++k){
          gsNextParam;
       }
    #endif

    #ifdef APPDEF_MERGER
    merger_parameters_t localMergerParams;
    CFE_SB_InitMsg(&localMergerParams,MERGER_PARAMETERS_MID,sizeof(merger_parameters_t),TRUE);
    localMergerParams.missionSpeed = appdataIntGS.storedparams[i].value;
    localMergerParams.maxVehicleSpeed = (double) gsNextParam;
    localMergerParams.minVehicleSpeed = (double) gsNextParam;
    localMergerParams.corridorWidth = (double) gsNextParam;
    localMergerParams.entryRadius = (double) gsNextParam;
    localMergerParams.coordZone = (double) gsNextParam;
    localMergerParams.scheduleZone = (double) gsNextParam;
    localMergerParams.minSeparationDistance = (double) gsNextParam;
    localMergerParams.minSeparationTime = (double) gsNextParam;
    localMergerParams.maxVehicleTurnRadius = (double) gsNextParam;
    localMergerParams.startIntersection = (int) gsNextParam;

    memset(localMergerParams.IntersectionID,0,INTERSECTION_MAX*sizeof(uint32_t));
    for(int i=0;i<appdataIntGS.mgData.num_waypoints;++i){
        localMergerParams.IntersectionLocation[i][0] = appdataIntGS.mgData.waypoints[i].latitude;
        localMergerParams.IntersectionLocation[i][1] = appdataIntGS.mgData.waypoints[i].longitude;
        localMergerParams.IntersectionLocation[i][2] = appdataIntGS.mgData.waypoints[i].altitude;
        localMergerParams.IntersectionID[i] = atoi(appdataIntGS.mgData.waypoints[i].name);
    }
    SendSBMsg(localMergerParams);
    OS_printf("publishing merger params\n");
    #endif


}
