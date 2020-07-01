//
// Created by Swee Balachandran on 12/22/17.
//

#include <math.h>
#include <CWrapper/TrafficMonitor_proxy.h>
#include "traffic.h"
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
        status = CFE_SB_RcvMsg(&trafficAppData.Traffic_MsgPtr, trafficAppData.Traffic_Pipe, CFE_SB_PEND_FOREVER);

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
    CFE_SB_Subscribe(ICAROUS_POSITION_MID,trafficAppData.Traffic_Pipe);
    CFE_SB_SubscribeLocal(ICAROUS_TRAFFIC_MID,trafficAppData.Traffic_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(FREQ_10_WAKEUP_MID,trafficAppData.Traffic_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_FLIGHTPLAN_MID,trafficAppData.Traffic_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(TRAFFIC_PARAMETERS_MID,trafficAppData.Traffic_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_TRAJECTORY_MID,trafficAppData.Traffic_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(FLIGHTPLAN_MONITOR_MID,trafficAppData.Traffic_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);

    // Initialize all messages that this App generates
    CFE_SB_InitMsg(&trafficAppData.tfAlerts,TRAFFIC_ALERTS_MID,sizeof(traffic_alerts_t),TRUE);

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

    
    trafficAppData.log = TblPtr->log;
    trafficAppData.tfMonitor = new_TrafficMonitor(trafficAppData.log,TblPtr->configFile);
    trafficAppData.numTraffic = 0;
    trafficAppData.updateDaaParams = TblPtr->updateParams;

    // Send event indicating app initialization
    if(status == CFE_SUCCESS){
        CFE_EVS_SendEvent(TRAFFIC_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                        "TRAFFIC App Initialized. Version %d.%d",
                        TRAFFIC_MAJOR_VERSION,
                        TRAFFIC_MINOR_VERSION);
    }

}

void TRAFFIC_AppCleanUp(void){
    // Do clean up here
    delete_TrafficMonitor(trafficAppData.tfMonitor);
}

void TRAFFIC_ProcessPacket(void){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(trafficAppData.Traffic_MsgPtr);

    switch(MsgId){

        case ICAROUS_FLIGHTPLAN_MID:{
            flightplan_t* msg = (flightplan_t*) trafficAppData.Traffic_MsgPtr;
            memcpy(&trafficAppData.flightplan1,msg, sizeof(flightplan_t));
            break;
        }

        case ICAROUS_TRAJECTORY_MID:{
            flightplan_t* msg = (flightplan_t*) trafficAppData.Traffic_MsgPtr;
            memcpy(&trafficAppData.flightplan2,msg, sizeof(flightplan_t));
            break;
        }

        case ICAROUS_TRAFFIC_MID:{
            object_t* msg;
            msg = (object_t*) trafficAppData.Traffic_MsgPtr;

            // If traffic source is set to 0 (ALL), this app can ingest the traffic data
            if (trafficAppData.trafficSrc != 0)
            {
                // If traffic source is non zero, only let the selected source
                if (msg->type != trafficAppData.trafficSrc)
                {
                    break;
                }
            }

            double pos[3] = {msg->latitude,msg->longitude,msg->altitude};
            double vel[3] = {msg->ve,msg->vn,msg->vd};
            int val = TrafficMonitor_InputTraffic(trafficAppData.tfMonitor,msg->index,(char*)msg->callsign.value,pos,vel,trafficAppData.time);
            if(val) {
                trafficAppData.numTraffic++;
                CFE_ES_WriteToSysLog("Traffic:Received intruder:%d\n",msg->index);
            }
            break;
        }

        case FLIGHTPLAN_MONITOR_MID:{
            flightplan_monitor_t* msg = (flightplan_monitor_t*)trafficAppData.Traffic_MsgPtr;

            int nextWP = msg->nextWP;  
            double xtrackDev = msg->crossTrackDeviation;
            double dist2nextWP = msg->dist2NextWP; 

            // This is the distance of the current flight plan leg completed
            double offset = sqrt(dist2nextWP*dist2nextWP - xtrackDev*xtrackDev); 

            flightplan_t *fp;
            bool first = false;
            if(strcmp(msg->planID,"Plan0") == 0){
                fp = &trafficAppData.flightplan1;
                first = true;
            }else if(strcmp(msg->planID,"Plan1") == 0){
                fp = &trafficAppData.flightplan2;
            }

            double A[3] = {fp->waypoints[nextWP-1].latitude,
                                   fp->waypoints[nextWP-1].longitude,
                                   fp->waypoints[nextWP-1].altitude};
            double B[3] = {fp->waypoints[nextWP].latitude,
                                   fp->waypoints[nextWP].longitude,
                                   fp->waypoints[nextWP].altitude};

            double distAB = ComputeDistance(A,B); 
            double n = (distAB - offset)/distAB;

            double _c[3] = {B[0] - A[0], B[1] - A[1],B[2] - A[2]};
            double C[3] = {A[0] + n*_c[0],A[1] + n*_c[1],A[1] + n*_c[1] };

            bool feasibility = TrafficMonitor_MonitorWPFeasibility(trafficAppData.tfMonitor,
                                              trafficAppData.position,trafficAppData.velocity,C);

            if(first){
                trafficAppData.return2fp1leg = feasibility;
            }else{
                trafficAppData.return2fp2leg = feasibility;
            }
            break;
        }

        case ICAROUS_POSITION_MID:{
            position_t* msg;
            msg = (position_t*) trafficAppData.Traffic_MsgPtr;

            if (msg->aircraft_id != CFE_PSP_GetSpacecraftId()) {

                double pos[3] = {msg->latitude,msg->longitude,msg->altitude_rel};
                double vel[3] = {msg->ve,msg->vn,msg->vd};
                int val = TrafficMonitor_InputTraffic(trafficAppData.tfMonitor,msg->aircraft_id,(char*)msg->callsign.value,pos,vel,trafficAppData.time);
                if(val){
                    trafficAppData.numTraffic++;
                    CFE_EVS_SendEvent(TRAFFIC_RECEIVED_INTRUDER_EID, CFE_EVS_INFORMATION,"Received intruder:%d",msg->aircraft_id);
                }
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

        case FREQ_10_WAKEUP_MID:{

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

            // Set band times
            trafficAppData.altBands.time = trafficAppData.time;
            trafficAppData.vsBands.time = trafficAppData.time;
            trafficAppData.speedBands.time = trafficAppData.time;
            trafficAppData.trackBands.time = trafficAppData.time;


            // Get feasibility data for primary flight plan
            double speed = 0.0;
            double time = 0.0;
            for(int i=0;i<trafficAppData.flightplan1.num_waypoints;++i){
                double wp[3] = {trafficAppData.flightplan1.waypoints[i].latitude,
                                trafficAppData.flightplan1.waypoints[i].longitude,
                                trafficAppData.flightplan1.waypoints[i].altitude};

                double currentVelocity[3] =  {trafficAppData.velocity[0],
                                              trafficAppData.velocity[1],	
                                              trafficAppData.velocity[2]};

                bool feasibility = TrafficMonitor_MonitorWPFeasibility(trafficAppData.tfMonitor,
                                              trafficAppData.position,currentVelocity,wp);
                trafficAppData.trackBands.wpFeasibility1[i] = feasibility;
                trafficAppData.speedBands.wpFeasibility1[i] = feasibility;
                trafficAppData.altBands.wpFeasibility1[i] = feasibility;
                trafficAppData.vsBands.wpFeasibility1[i] = feasibility;
                trafficAppData.trackBands.fp1ClosestPointFeasible = trafficAppData.return2fp1leg;

                if (trafficAppData.flightplan1.waypoints[i].wp_metric == WP_METRIC_SPEED){
                    speed = trafficAppData.flightplan1.waypoints[i].value;
                }else if(trafficAppData.flightplan1.waypoints[i].wp_metric == WP_METRIC_ETA){
                    time = trafficAppData.flightplan1.waypoints[i].value;
                }

                if(i > 0){
                    if(speed < 1e-6){
                        double wp2[3] = {trafficAppData.flightplan1.waypoints[i-1].latitude,
                                        trafficAppData.flightplan1.waypoints[i-1].longitude,
                                        trafficAppData.flightplan1.waypoints[i-1].altitude};
                        double dist = ComputeDistance(wp,wp2);

                        if(time > 0){
                            speed = dist/time;
                        }else{
                            speed = 1;
                        }

                    }

                    double originalVelocity[3] = {trafficAppData.velocity[0],
                                                  speed,
                                                  trafficAppData.velocity[2]};
                    feasibility = TrafficMonitor_MonitorWPFeasibility(trafficAppData.tfMonitor,
                                                trafficAppData.position,originalVelocity,wp);
                    trafficAppData.speedBands.wpFeasibility1[i] &= feasibility;
                }

            }

            speed = 0.0;
            time  = 0.0;
            // Get feasibility data for secondary flight plan
            for(int i=0;i<trafficAppData.flightplan2.num_waypoints;++i){
                double wp[3] = {trafficAppData.flightplan2.waypoints[i].latitude,
                                trafficAppData.flightplan2.waypoints[i].longitude,
                                trafficAppData.flightplan2.waypoints[i].altitude};

                double currentVelocity[3] = {trafficAppData.velocity[0],
                                              trafficAppData.velocity[1],	
                                              trafficAppData.velocity[2]};

                bool feasibility = TrafficMonitor_MonitorWPFeasibility(trafficAppData.tfMonitor,
                                              trafficAppData.position,currentVelocity,wp);
                trafficAppData.trackBands.wpFeasibility2[i] = feasibility;
                trafficAppData.speedBands.wpFeasibility2[i] = feasibility;
                trafficAppData.altBands.wpFeasibility2[i] = feasibility;
                trafficAppData.vsBands.wpFeasibility2[i] = feasibility;
                trafficAppData.trackBands.fp2ClosestPointFeasible = trafficAppData.return2fp2leg;


                if (trafficAppData.flightplan1.waypoints[i].wp_metric == WP_METRIC_SPEED){
                    speed = trafficAppData.flightplan1.waypoints[i].value;
                }else if(trafficAppData.flightplan1.waypoints[i].wp_metric == WP_METRIC_ETA){
                    time = trafficAppData.flightplan1.waypoints[i].value;
                }

                if(i > 0){
                    if(speed < 1e-6){
                        double wp2[3] = {trafficAppData.flightplan2.waypoints[i-1].latitude,
                                        trafficAppData.flightplan2.waypoints[i-1].longitude,
                                        trafficAppData.flightplan2.waypoints[i-1].altitude};
                        double dist = ComputeDistance(wp,wp2);

                        if(time > 0){
                            speed = dist/time;
                        }else{
                            speed = 1;
                        }

                    }
                    double originalVelocity[3] = {trafficAppData.velocity[0],
                                                  speed,
                                                  trafficAppData.velocity[2]};
                    feasibility = TrafficMonitor_MonitorWPFeasibility(trafficAppData.tfMonitor,
                                                trafficAppData.position,originalVelocity,wp);
                    trafficAppData.speedBands.wpFeasibility2[i] &= feasibility;
                }
            }

            cfsBands_t track_bands_msg;
            CFE_SB_InitMsg(&track_bands_msg, ICAROUS_BANDS_TRACK_MID, sizeof(cfsBands_t), TRUE);
            memcpy(track_bands_msg.databuffer,(char*) &trafficAppData.trackBands,sizeof(bands_t));
            SendSBMsg(track_bands_msg);

            cfsBands_t speed_bands_msg;
            CFE_SB_InitMsg(&speed_bands_msg, ICAROUS_BANDS_SPEED_MID, sizeof(cfsBands_t), TRUE);
            memcpy(speed_bands_msg.databuffer,(char*) &trafficAppData.speedBands,sizeof(bands_t));
            SendSBMsg(speed_bands_msg);

            cfsBands_t vs_bands_msg;
            CFE_SB_InitMsg(&vs_bands_msg, ICAROUS_BANDS_VS_MID, sizeof(cfsBands_t), TRUE);
            memcpy(vs_bands_msg.databuffer,(char*) &trafficAppData.vsBands,sizeof(bands_t));
            SendSBMsg(vs_bands_msg);

            cfsBands_t alt_bands_msg;
            CFE_SB_InitMsg(&alt_bands_msg,ICAROUS_BANDS_ALT_MID,sizeof(cfsBands_t),TRUE);
            memcpy(alt_bands_msg.databuffer,(char*) &trafficAppData.altBands,sizeof(bands_t));
            SendSBMsg(alt_bands_msg);

            int count = 1;
            for(int i=0;i<count;++i){
                int alert;
                char callsign[MAX_CALLSIGN_LEN];
                count = TrafficMonitor_GetTrafficAlerts(trafficAppData.tfMonitor,i,callsign,&alert);
                count = fmin(count,MAX_TRAFFIC_ALERTS);
                if(count > 0){
                    memcpy(trafficAppData.tfAlerts.callsign[i].value,callsign,MAX_CALLSIGN_LEN);
                    trafficAppData.tfAlerts.trafficAlerts[i] = alert;
                    //OS_printf("Alert level for traffic: %ld is %d\n",id,alert);
                }
            }

            if (count > 0) {
                trafficAppData.tfAlerts.numAlerts = count;
                SendSBMsg(trafficAppData.tfAlerts);
            }

            break;
        }

        case TRAFFIC_PARAMETERS_MID:{
            traffic_parameters_t* msg = (traffic_parameters_t*) trafficAppData.Traffic_MsgPtr;
            memcpy(&trafficAppData.params,msg,sizeof(traffic_parameters_t));
            char params[2000];
            ConstructDAAParamString(msg, params);
            if(trafficAppData.updateDaaParams){
                TrafficMonitor_UpdateDAAParameters(trafficAppData.tfMonitor,params,(bool)msg->logDAAdata);
            }
            trafficAppData.trafficSrc = msg->trafficSource;
            break;
        }
    }
}

int32_t TrafficTableValidationFunc(void *TblPtr){
    return 0;
}
