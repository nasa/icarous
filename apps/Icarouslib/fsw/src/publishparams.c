
#include <string.h>
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_platform_cfg.h"
#include "paramdef.h"
#include "Icarous_msgids.h"

#ifdef APPDEF_TRAFFIC
#include "traffic_msg.h"
#include "traffic_msgids.h"
#endif

#ifdef APPDEF_TRACKING
#include "tracking_msg.h"
#include "tracking_msgids.h"
#endif 

#ifdef APPDEF_TRAJECTORY
#include "trajectory_msg.h"
#include "trajectory_msgids.h"
#endif 

#ifdef APPDEF_GEOFENCE
#include "geofence_msg.h"
#include "geofence_msgids.h"
#endif

#ifdef APPDEF_ROTORSIM
#include "rotorsim_msg.h"
#include "rotorsim_msgids.h"
#endif

#ifdef APPDEF_MERGER
#include "merger_msg.h"
#include "merger_msgids.h"
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Library Initialization Routine                                  */
/* cFE requires that a library have an initialization routine      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 Icarous_LibInit(void)
{

    OS_printf ("ICAROUS_LIB Initialized.\n");

    return CFE_SUCCESS;

}

void PublishParams(param_t *params) {

    //Store the locally saved parameters to the messages
    int i = 0;
    //Traffic Parameters

    #ifdef APPDEF_TRAFFIC
    traffic_parameters_t localTrafficParams;
    CFE_SB_InitMsg(&localTrafficParams,TRAFFIC_PARAMETERS_MID,sizeof(traffic_parameters_t),TRUE);
    localTrafficParams.trafficSource = (uint32_t) params[i].value; i++;
    localTrafficParams.resType = (uint32_t) params[i].value;i++;;
    localTrafficParams.logDAAdata = (bool) params[i].value;i++;;
    localTrafficParams.lookahead_time = params[i].value;i++;;
    localTrafficParams.left_trk = params[i].value;i++;;
    localTrafficParams.right_trk = params[i].value;i++;;
    localTrafficParams.min_gs = params[i].value;i++;;
    localTrafficParams.max_gs = params[i].value;i++;;
    localTrafficParams.min_vs = params[i].value;i++;;
    localTrafficParams.max_vs = params[i].value;i++;;
    localTrafficParams.min_alt = params[i].value;i++;;
    localTrafficParams.max_alt = params[i].value;i++;;
    localTrafficParams.trk_step = params[i].value;i++;;
    localTrafficParams.gs_step = params[i].value;i++;;
    localTrafficParams.vs_step = params[i].value;i++;;
    localTrafficParams.alt_step = params[i].value;i++;;
    localTrafficParams.horizontal_accel = params[i].value;i++;;
    localTrafficParams.vertical_accel = params[i].value;i++;;
    localTrafficParams.turn_rate = params[i].value;i++;;
    localTrafficParams.bank_angle = params[i].value;i++;;
    localTrafficParams.vertical_rate = params[i].value;i++;;
    localTrafficParams.recovery_stability_time = params[i].value;i++;;
    localTrafficParams.min_horizontal_recovery = params[i].value;i++;;
    localTrafficParams.min_vertical_recovery = params[i].value;i++;;
    localTrafficParams.recovery_trk = (bool) params[i].value;i++;;
    localTrafficParams.recovery_gs = (bool) params[i].value;i++;;
    localTrafficParams.recovery_vs = (bool) params[i].value;i++;;
    localTrafficParams.recovery_alt = (bool) params[i].value;i++;;
    localTrafficParams.ca_bands = (bool) params[i].value;i++;;
    localTrafficParams.ca_factor = params[i].value;i++;;
    localTrafficParams.horizontal_nmac = params[i].value;i++;;
    localTrafficParams.vertical_nmac = params[i].value;i++;;
    localTrafficParams.conflict_crit = (bool) params[i].value;i++;;
    localTrafficParams.recovery_crit = (bool) params[i].value;i++;;
    localTrafficParams.contour_thr = params[i].value;i++;;
    localTrafficParams.alert_1_alerting_time = params[i].value;i++;;
    strcpy(localTrafficParams.alert_1_detector, "det_1");   //Hard coded, not parameter
    localTrafficParams.alert_1_early_alerting_time = params[i].value;i++;;
    strcpy(localTrafficParams.alert_1_region, "NEAR");      //Hard coded, not parameter
    localTrafficParams.alert_1_spread_alt = params[i].value;i++;;
    localTrafficParams.alert_1_spread_gs = params[i].value;i++;;
    localTrafficParams.alert_1_spread_trk = params[i].value;i++;;
    localTrafficParams.alert_1_spread_vs = params[i].value;i++;;
    localTrafficParams.conflict_level = (uint8_t) params[i].value;i++;;
    strcpy(localTrafficParams.load_core_detection_det_1, "WCV_TAUMOD"); //Hard coded, not parameter
    localTrafficParams.det_1_WCV_DTHR = params[i].value;i++;;
    localTrafficParams.det_1_WCV_TCOA = params[i].value;i++;;
    localTrafficParams.det_1_WCV_TTHR = params[i].value;i++;;
    localTrafficParams.det_1_WCV_ZTHR = params[i].value;i++;;

    SendSBMsg(localTrafficParams);
    #else
       for(int k=0;k<46;++k) i++;
    #endif

    // Tracking parameters
    #ifdef APPDEF_TRACKING
    tracking_parameters_t localTrackingParams;
    CFE_SB_InitMsg(&localTrackingParams,TRACKING_PARAMETERS_MID,sizeof(tracking_parameters_t),TRUE);
    localTrackingParams.command = (bool) params[i].value;i++;;
    localTrackingParams.trackingObjId = (int) params[i].value;i++;;
    localTrackingParams.pGainX = (double) params[i].value;i++;;
    localTrackingParams.pGainY = (double) params[i].value;i++;;
    localTrackingParams.pGainZ = (double) params[i].value;i++;;
    localTrackingParams.heading = (double) params[i].value;i++;;
    localTrackingParams.distH = (double) params[i].value;i++;;
    localTrackingParams.distV = (double) params[i].value;i++;;
    SendSBMsg(localTrackingParams);
    #else
       for(int k=0;k<8;++k) i++;
    #endif

    // Trajectory parameters
    #ifdef APPDEF_TRAJECTORY
    trajectory_parameters_t localTrajectoryParams;
    CFE_SB_InitMsg(&localTrajectoryParams,TRAJECTORY_PARAMETERS_MID,sizeof(trajectory_parameters_t),TRUE);
    localTrajectoryParams.obsbuffer = (double) params[i].value;i++;;
    localTrajectoryParams.maxCeiling = (double) params[i].value;i++;;
    localTrajectoryParams.astar_enable3D = (bool) params[i].value;i++;;
    localTrajectoryParams.astar_gridSize = (double) params[i].value;i++;;
    localTrajectoryParams.astar_resSpeed = (double) params[i].value;i++;;
    localTrajectoryParams.astar_lookahead = (double) params[i].value;i++;;
    strcpy(localTrajectoryParams.astar_daaConfigFile, "../ram/DaidalusQuadConfig.txt");   //Hard coded, not parameter
    //params[i].value;i++;;
    localTrajectoryParams.rrt_resSpeed = (double) params[i].value;i++;;
    localTrajectoryParams.rrt_numIterations = (int) params[i].value;i++;;
    localTrajectoryParams.rrt_dt = (double) params[i].value;i++;;
    localTrajectoryParams.rrt_macroSteps = (int) params[i].value;i++;;
    localTrajectoryParams.rrt_capR = (double) params[i].value;i++;;
    strcpy(localTrajectoryParams.rrt_daaConfigFile, "../ram/DaidalusQuadConfig.txt");     //Hard coded, not parameter
    //params[i].value;i++;;
    localTrajectoryParams.xtrkDev = (double) params[i].value;i++;;
    localTrajectoryParams.xtrkGain = (double) params[i].value;i++;;
    localTrajectoryParams.resSpeed = (double) params[i].value;i++;;
    localTrajectoryParams.searchAlgorithm = (uint8_t) params[i].value;i++;;

    SendSBMsg(localTrajectoryParams);
    #else
       for(int k=0;k<15;++k) i++;
    #endif

    // Geofence parameters
    #ifdef APPDEF_GEOFENCE
    geofence_parameters_t localGeofenceParams;
    CFE_SB_InitMsg(&localGeofenceParams,GEOFENCE_PARAMETERS_MID,sizeof(geofence_parameters_t),TRUE);
    localGeofenceParams.lookahead = (double) params[i].value;i++;;
    localGeofenceParams.hthreshold = (double) params[i].value;i++;;
    localGeofenceParams.vthreshold = (double) params[i].value;i++;;
    localGeofenceParams.hstepback = (double) params[i].value;i++;;
    localGeofenceParams.vstepback = (double) params[i].value;i++;;
    SendSBMsg(localGeofenceParams);
    #else
       for(int k=0;k<5;++k) i++;
    #endif

    #ifdef APPDEF_ROTORSIM
    // Rotorsim parameters
    rotorsim_parameters_t localRotorsimParams;
    localRotorsimParams.speed = (double) params[i].value;i++;;
    SendSBMsg(localRotorsimParams);
    #else
       for(int k=0;k<1;++k) i++;
    #endif

    #ifdef APPDEF_MERGER
    merger_parameters_t localMergerParams;
    CFE_SB_InitMsg(&localMergerParams,MERGER_PARAMETERS_MID,sizeof(merger_parameters_t),TRUE);
    localMergerParams.missionSpeed = appdataIntGS.storedparams[i].value;
    localMergerParams.maxVehicleSpeed = (double) params[i].value;i++;;
    localMergerParams.minVehicleSpeed = (double) params[i].value;i++;;
    localMergerParams.corridorWidth = (double) params[i].value;i++;;
    localMergerParams.entryRadius = (double) params[i].value;i++;;
    localMergerParams.coordZone = (double) params[i].value;i++;;
    localMergerParams.scheduleZone = (double) params[i].value;i++;;
    localMergerParams.minSeparationDistance = (double) params[i].value;i++;;
    localMergerParams.minSeparationTime = (double) params[i].value;i++;;
    localMergerParams.maxVehicleTurnRadius = (double) params[i].value;i++;;
    localMergerParams.startIntersection = (int) params[i].value;i++;;

    memset(localMergerParams.IntersectionID,0,INTERSECTION_MAX*sizeof(uint32_t));
    for(int i=0;i<appdataIntGS.mgData.num_waypoints;++i){
        localMergerParams.IntersectionLocation[i][0] = appdataIntGS.mgData.waypoints[i].latitude;
        localMergerParams.IntersectionLocation[i][1] = appdataIntGS.mgData.waypoints[i].longitude;
        localMergerParams.IntersectionLocation[i][2] = appdataIntGS.mgData.waypoints[i].altitude;
        localMergerParams.IntersectionID[i] = atoi(appdataIntGS.mgData.waypoints[i].name);
    }
    SendSBMsg(localMergerParams);
    #endif

    OS_printf("Published parameters\n");
}
