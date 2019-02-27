//
// Created by Swee Balachandran on 12/22/17.
//
#include <msgdef/ardupilot_msg.h>
#include <CWrapper/TrafficMonitor_proxy.h>
#include <msgdef/traffic_msg.h>
#include "traffic.h"
#include <math.h>
#include "UtilFunctions.h"
#include "traffic_tbl.c"

CFE_EVS_BinFilter_t  TRAFFIC_EventFilters[] =
{  /* Event ID    mask */
        {TRAFFIC_STARTUP_INF_EID,       0x0000},
        {TRAFFIC_COMMAND_ERR_EID,       0x0000},
        {TRAFFIC_RECEIVED_INTRUDER_EID, 0x0000}
}; /// Event ID definitions

/* Application entry points */
void TRAFFIC_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    TRAFFIC_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        status = CFE_SB_RcvMsg(&trafficAppData.Traffic_MsgPtr, trafficAppData.Traffic_Pipe, 10);

        if (status == CFE_SUCCESS)
        {
            TRAFFIC_ProcessPacket();
        }
    }

    TRAFFIC_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void TRAFFIC_AppInit(void) {

    memset(&trafficAppData, 0, sizeof(TrafficAppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(TRAFFIC_EventFilters,
                     sizeof(TRAFFIC_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe(&trafficAppData.Traffic_Pipe, /* Variable to hold Pipe ID */
                               TRAFFIC_PIPE_DEPTH,       /* Depth of Pipe */
                               TRAFFIC_PIPE_NAME);       /* Name of pipe */

    //Subscribe to plexil output messages from the SB
    CFE_SB_Subscribe(ICAROUS_TRAFFIC_MID,trafficAppData.Traffic_Pipe);
    CFE_SB_Subscribe(ICAROUS_POSITION_MID,trafficAppData.Traffic_Pipe);
    CFE_SB_Subscribe(FREQ_10_WAKEUP_MID,trafficAppData.Traffic_Pipe);
    CFE_SB_Subscribe(ICAROUS_FLIGHTPLAN_MID,trafficAppData.Traffic_Pipe);
    CFE_SB_Subscribe(TRAFFIC_PARAMETERS_MID,trafficAppData.Traffic_Pipe);

    // Initialize all messages that this App generates
    CFE_SB_InitMsg(&trafficAppData.trackBands, ICAROUS_BANDS_TRACK_MID, sizeof(bands_t), TRUE);
    CFE_SB_InitMsg(&trafficAppData.speedBands, ICAROUS_BANDS_SPEED_MID, sizeof(bands_t), TRUE);
    CFE_SB_InitMsg(&trafficAppData.vsBands, ICAROUS_BANDS_VS_MID, sizeof(bands_t), TRUE);
    CFE_SB_InitMsg(&trafficAppData.altBands,ICAROUS_BANDS_ALT_MID,sizeof(bands_t),TRUE);

    // Register table with table services
    status = CFE_TBL_Register(&trafficAppData.Traffic_tblHandle,
                              "TrafficTable",
                              sizeof(TrafficTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &TrafficTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(trafficAppData.Traffic_tblHandle,CFE_TBL_SRC_ADDRESS,&Traffic_TblStruct);

    TrafficTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr, trafficAppData.Traffic_tblHandle);

    // Send event indicating app initialization
    CFE_EVS_SendEvent(TRAFFIC_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "TRAFFIC App Initialized. Version %d.%d",
                      TRAFFIC_MAJOR_VERSION,
                      TRAFFIC_MINOR_VERSION);

    trafficAppData.log = TblPtr->log;
    trafficAppData.tfMonitor = new_TrafficMonitor(trafficAppData.log,TblPtr->configFile);
    trafficAppData.numTraffic = 0;
}

void TRAFFIC_AppCleanUp(){
    // Do clean up here
    delete_TrafficMonitor(trafficAppData.tfMonitor);
}

void TRAFFIC_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(trafficAppData.Traffic_MsgPtr);

    switch(MsgId){

        case ICAROUS_FLIGHTPLAN_MID:{
            flightplan_t* msg = (flightplan_t*) trafficAppData.Traffic_MsgPtr;
            memcpy(&trafficAppData.flightplan,msg, sizeof(flightplan_t));
            break;
        }

        case ICAROUS_TRAFFIC_MID:{
            object_t* msg;
            msg = (object_t*) trafficAppData.Traffic_MsgPtr;
            double pos[3] = {msg->latitude,msg->longitude,msg->altitude};
            double vel[3] = {msg->ve,msg->vn,msg->vd};
            int val = TrafficMonitor_InputTraffic(trafficAppData.tfMonitor,msg->index,pos,vel,trafficAppData.time);
            if(val) {
                trafficAppData.numTraffic++;
                CFE_EVS_SendEvent(TRAFFIC_RECEIVED_INTRUDER_EID, CFE_EVS_INFORMATION, "Received intruder:%d",
                                  msg->index);
            }
            break;
        }

        case ICAROUS_POSITION_MID:{
            position_t* msg;
            msg = (position_t*) trafficAppData.Traffic_MsgPtr;

            if (msg->aircraft_id != CFE_PSP_GetSpacecraftId()) {

                double pos[3] = {msg->latitude,msg->longitude,msg->altitude_rel};
                double vel[3] = {msg->ve,msg->vn,msg->vd};
                int val = TrafficMonitor_InputTraffic(trafficAppData.tfMonitor,msg->aircraft_id,pos,vel,trafficAppData.time);
                if(val)
                    CFE_EVS_SendEvent(TRAFFIC_RECEIVED_INTRUDER_EID, CFE_EVS_INFORMATION,"Received intruder:%d",msg->aircraft_id);
            }else{

                trafficAppData.position[0] = msg->latitude;
                trafficAppData.position[1] = msg->longitude;
                trafficAppData.position[2] = msg->altitude_rel;

                double track,groundSpeed,verticalSpeed;
                ConvertVnedToTrkGsVs(msg->vn,msg->ve,msg->vd,&track,&groundSpeed,&verticalSpeed);

                trafficAppData.velocity[0] = track;
                trafficAppData.velocity[1] = groundSpeed;
                trafficAppData.velocity[2] = verticalSpeed;
                trafficAppData.time = msg->time_gps;

            }

            break;
        }

        case FREQ_30_WAKEUP_MID:{

            if(trafficAppData.numTraffic == 0)
                break;

            TrafficMonitor_MonitorTraffic(trafficAppData.tfMonitor,trafficAppData.position,trafficAppData.velocity,trafficAppData.time);

            TrafficMonitor_GetTrackBands(trafficAppData.tfMonitor,
                                         &trafficAppData.trackBands.numBands,
                                         trafficAppData.trackBands.type,
                                         (double*)trafficAppData.trackBands.min,
                                         (double*)trafficAppData.trackBands.max,
                                          &trafficAppData.trackBands.recovery,
                                          &trafficAppData.trackBands.currentConflictBand,
                                          &trafficAppData.trackBands.timeToViolation,
                                          &trafficAppData.trackBands.timeToRecovery,
                                          &trafficAppData.trackBands.minHDist,
                                          &trafficAppData.trackBands.minVDist,
                                          &trafficAppData.trackBands.resUp,
                                          &trafficAppData.trackBands.resDown,
                                          &trafficAppData.trackBands.resPreferred);


            TrafficMonitor_GetGSBands(trafficAppData.tfMonitor,
                                      &trafficAppData.speedBands.numBands,
                                      trafficAppData.speedBands.type,
                                      (double*)trafficAppData.speedBands.min,
                                      (double*)trafficAppData.speedBands.max,
                                      &trafficAppData.speedBands.recovery,
                                      &trafficAppData.speedBands.currentConflictBand,
                                        &trafficAppData.speedBands.timeToViolation,
                                          &trafficAppData.speedBands.timeToRecovery,
                                          &trafficAppData.speedBands.minHDist,
                                          &trafficAppData.speedBands.minVDist,
                                          &trafficAppData.speedBands.resUp,
                                          &trafficAppData.speedBands.resDown,
                                          &trafficAppData.speedBands.resPreferred);

            TrafficMonitor_GetVSBands(trafficAppData.tfMonitor,
                                      &trafficAppData.vsBands.numBands,
                                      trafficAppData.vsBands.type,
                                      (double*)trafficAppData.vsBands.min,
                                      (double*)trafficAppData.vsBands.max,
                                      &trafficAppData.vsBands.recovery,
                                      &trafficAppData.vsBands.currentConflictBand,
                                      &trafficAppData.vsBands.timeToViolation,
                                          &trafficAppData.vsBands.timeToRecovery,
                                          &trafficAppData.vsBands.minHDist,
                                          &trafficAppData.vsBands.minVDist,
                                          &trafficAppData.vsBands.resUp,
                                          &trafficAppData.vsBands.resDown,
                                          &trafficAppData.vsBands.resPreferred);


            TrafficMonitor_GetAltBands(trafficAppData.tfMonitor,
                                      &trafficAppData.altBands.numBands,
                                      trafficAppData.altBands.type,
                                      (double*)trafficAppData.altBands.min,
                                      (double*)trafficAppData.altBands.max,
                                      &trafficAppData.altBands.recovery,
                                      &trafficAppData.altBands.currentConflictBand,
                                      &trafficAppData.altBands.timeToViolation,
                                      &trafficAppData.altBands.timeToRecovery,
                                      &trafficAppData.altBands.minHDist,
                                      &trafficAppData.altBands.minVDist,
                                      &trafficAppData.altBands.resUp,
                                      &trafficAppData.altBands.resDown,
                                      &trafficAppData.altBands.resPreferred);



            for(int i=0;i<trafficAppData.flightplan.num_waypoints;++i){
                double wp[3] = {trafficAppData.flightplan.waypoints[i].latitude,
                                trafficAppData.flightplan.waypoints[i].longitude,
                                trafficAppData.flightplan.waypoints[i].altitude};

                double originalVelocity[3] = {trafficAppData.velocity[0],
                                              trafficAppData.flightplan.waypoints[i].value_to_next_wp,
                                              trafficAppData.velocity[2]};

                bool feasibility = TrafficMonitor_MonitorWPFeasibility(trafficAppData.tfMonitor,
                                              trafficAppData.position,originalVelocity,wp);
                trafficAppData.trackBands.wpFeasibility[i] = feasibility;
                trafficAppData.speedBands.wpFeasibility[i] = feasibility;
                trafficAppData.vsBands.wpFeasibility[i] = feasibility;
                trafficAppData.altBands.wpFeasibility[i] = feasibility;
            }

            SendSBMsg(trafficAppData.trackBands);

            SendSBMsg(trafficAppData.speedBands);

            SendSBMsg(trafficAppData.vsBands);

            SendSBMsg(trafficAppData.altBands);

            break;
        }

        case TRAFFIC_PARAMETERS_MID:{
            traffic_parameters_t* msg = (traffic_parameters_t*) trafficAppData.Traffic_MsgPtr;
            char params[2000];
            int n = 0;
            n += sprintf(params,"lookahead_time=%f [s];",msg->lookahead_time);
            n += sprintf(params + n,"left_trk=%f [deg];",msg->left_trk);
            n += sprintf(params + n,"right_trk=%f [deg];",msg->right_trk);
            n += sprintf(params + n,"min_gs=%f [m/s];",msg->min_gs);
            n += sprintf(params + n,"max_gs=%f [m/s];",msg->max_gs);
            n += sprintf(params + n,"min_vs=%f [fpm];",msg->min_vs);
            n += sprintf(params + n,"max_vs=%f [fpm];",msg->max_vs);
            n += sprintf(params + n,"min_alt=%f [ft];",msg->min_alt);
            n += sprintf(params + n,"max_alt=%f [ft];",msg->max_alt);
            n += sprintf(params + n,"trk_step=%f [deg];",msg->trk_step);
            n += sprintf(params + n,"gs_step=%f [m/s];",msg->gs_step);
            n += sprintf(params + n,"vs_step=%f [fpm];",msg->vs_step);
            n += sprintf(params + n,"alt_step=%f [ft];",msg->alt_step);
            n += sprintf(params + n,"horizontal_accel=%f [m/s^2];",msg->horizontal_accel);
            n += sprintf(params + n,"vertical_accel=%f [m/s^2];",msg->vertical_accel);
            n += sprintf(params + n,"turn_rate=%f [deg/s];",msg->turn_rate);
            n += sprintf(params + n,"bank_angle=%f [deg];",msg->bank_angle);
            n += sprintf(params + n,"vertical_rate=%f [m/s];",msg->vertical_rate);
            n += sprintf(params + n,"recover_stability_time=%f [s];",msg->recovery_stability_time);
            n += sprintf(params + n,"min_horizontal_recovery=%f [ft];",msg->min_horizontal_recovery);
            n += sprintf(params + n,"min_vertical_recovery=%f [ft];",msg->min_vertical_recovery);
            n += msg->recovery_trk? sprintf(params + n,"recovery_trk=true;"):sprintf(params + n,"recovery_trk=false;");
            n += msg->recovery_gs? sprintf(params + n,"recovery_gs=true;"):sprintf(params + n,"recovery_gs=false;");
            n += msg->recovery_vs? sprintf(params + n,"recovery_vs=true;"):sprintf(params + n,"recovery_vs=false;");
            n += msg->recovery_alt? sprintf(params + n,"recovery_alt=true;"):sprintf(params + n,"recovery_alt=false;");
            n += msg->conflict_crit? sprintf(params + n,"conflict_crit=true;"):sprintf(params + n,"conflict_crit=false;");
            n += msg->recovery_crit? sprintf(params + n,"recovery_crit=true;"):sprintf(params + n,"recovery_crit=false;");
            n += msg->ca_bands? sprintf(params + n,"ca_bands=true;"):sprintf(params + n,"ca_bands=false;");
            n += sprintf(params + n,"ca_factor=%f;",msg->ca_factor);
            n += sprintf(params + n,"horizontal_nmac=%f [ft];",msg->horizontal_nmac);
            n += sprintf(params + n,"vertical_nmac=%f [ft];",msg->vertical_nmac);
            n += sprintf(params + n,"contour_thr=%f [deg];",msg->contour_thr);
            n += sprintf(params + n,"alert_1_alerting_time=%f [s];",msg->alert_1_alerting_time);
            n += sprintf(params + n,"alert_1_detector=%s;",msg->alert_1_detector);
            n += sprintf(params + n,"alert_1_early_alerting_time=%f [s];",msg->alert_1_alerting_time);
            n += sprintf(params + n,"alert_1_region=%s ;",msg->alert_1_region);
            n += sprintf(params + n,"alert_1_spread_alt=%f [m];",msg->alert_1_spread_alt);
            n += sprintf(params + n,"alert_1_spread_gs=%f [m/s];",msg->alert_1_spread_gs);
            n += sprintf(params + n,"alert_1_spread_trk=%f [deg];",msg->alert_1_spread_trk);
            n += sprintf(params + n,"alert_1_spread_vs=%f [fpm];",msg->alert_1_spread_vs);
            n += sprintf(params + n,"conflict_level = %d;",msg->conflict_level);
            n += sprintf(params + n,"det_1_WCV_DTHR = %f [ft];",msg->det_1_WCV_DTHR);
            n += sprintf(params + n,"det_1_WCV_TCOA = %f [s];",msg->det_1_WCV_TCOA);
            n += sprintf(params + n,"det_1_WCV_TTHR = %f [s];",msg->det_1_WCV_TTHR);
            n += sprintf(params + n,"det_1_WCV_ZTHR = %f [ft];",msg->det_1_WCV_ZTHR);
            n += sprintf(params + n,"load_core_detection_det_1 = gov.nasa.larcfm.ACCoRD.%s;",msg->load_core_detection_det_1);

            TrafficMonitor_UpdateDAAParameters(trafficAppData.tfMonitor,params);
            break;
        }
    }
}

int32_t TrafficTableValidationFunc(void *TblPtr){
    return 0;
}
