//
// Created by Swee Balachandran on 12/22/17.
//

#include <math.h>
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
    CFE_SB_SubscribeLocal(TRAFFIC_PARAMETERS_MID,trafficAppData.Traffic_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);

    // Initialize all messages that this App generates
    CFE_SB_InitMsg(&trafficAppData.bandReport,ICAROUS_BAND_REPORT_MID,sizeof(band_report_t),TRUE);
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
    char callsign[30];
    sprintf(callsign,"aircraft%d",CFE_PSP_GetSpacecraftId());
    trafficAppData.tfMonitor = newDaidalusTrafficMonitor(callsign,TblPtr->configFile,trafficAppData.log);
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
    
    delDaidalusTrafficMonitor(trafficAppData.tfMonitor);
}

void TRAFFIC_ProcessPacket(void){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(trafficAppData.Traffic_MsgPtr);

    switch(MsgId){

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
            double trkGsVs[3] = {0,0,0};
            double sumPos[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
            double sumVel[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
            ConvertVnedToTrkGsVs(msg->vn,msg->ve,msg->vd,trkGsVs,trkGsVs+1,trkGsVs+2);
            int val = TrafficMonitor_InputIntruderData(trafficAppData.tfMonitor,msg->index,(char*)msg->callsign.value,pos,trkGsVs,trafficAppData.time,sumPos,sumVel);
            trafficAppData.numTraffic = val;
            break;
        }

        case ICAROUS_POSITION_MID:{
            position_t* msg;
            msg = (position_t*) trafficAppData.Traffic_MsgPtr;

            if (msg->aircraft_id != CFE_PSP_GetSpacecraftId()) {

                double pos[3] = {msg->latitude,msg->longitude,msg->altitude_rel};
                double trkGsVs[3] = {0,0,0};
                ConvertVnedToTrkGsVs(msg->vn,msg->ve,msg->vd,trkGsVs,trkGsVs+1,trkGsVs+2);
                double sumPos[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
                double sumVel[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
                int val = TrafficMonitor_InputIntruderData(trafficAppData.tfMonitor,msg->aircraft_id,(char*)msg->callsign.value,pos,trkGsVs,trafficAppData.time,sumPos,sumVel);
                trafficAppData.numTraffic = val;
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
            
            double winds[2] = {0.0,0.0}; // wind from, wind speed
            double sumPos[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
            double sumVel[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
            TrafficMonitor_InputOwnshipData(trafficAppData.tfMonitor,trafficAppData.position,trafficAppData.velocity,trafficAppData.time,sumPos,sumVel);
            TrafficMonitor_MonitorTraffic(trafficAppData.tfMonitor,winds);

            TrafficMonitor_GetTrackBands(trafficAppData.tfMonitor, &trafficAppData.trackBands);


            TrafficMonitor_GetSpeedBands(trafficAppData.tfMonitor,&trafficAppData.speedBands);

            TrafficMonitor_GetVerticalSpeedBands(trafficAppData.tfMonitor, &trafficAppData.vsBands);


            TrafficMonitor_GetAltBands(trafficAppData.tfMonitor, &trafficAppData.altBands);

            // Set band times
            trafficAppData.altBands.time = trafficAppData.time;
            trafficAppData.vsBands.time = trafficAppData.time;
            trafficAppData.speedBands.time = trafficAppData.time;
            trafficAppData.trackBands.time = trafficAppData.time;

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

            // Pack Band Report
            trafficAppData.bandReport.altitudeBands      = trafficAppData.altBands;
            trafficAppData.bandReport.groundSpeedBands   = trafficAppData.speedBands;
            trafficAppData.bandReport.trackBands         = trafficAppData.trackBands;
            trafficAppData.bandReport.verticalSpeedBands = trafficAppData.vsBands;

            // Publish Band Report
            CFE_SB_ZeroCopyHandle_t copyHandle;
            band_report_t * dataPtr = (band_report_t *) CFE_SB_ZeroCopyGetPtr(sizeof(band_report_t), &copyHandle);
            *dataPtr = trafficAppData.bandReport;
            CFE_SB_TimeStampMsg( (CFE_SB_Msg_t *) dataPtr);
            int32 status = CFE_SB_ZeroCopySend((CFE_SB_Msg_t *) dataPtr, copyHandle);
            if (status != CFE_SUCCESS) {
                OS_printf("[traffic] Error publishing ICAROUS_BAND_REPORT_MID with zero-copy: status %x\n", status);
            }

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
            //char params[2000];
            //ConstructDAAParamString(msg, params);
            if(trafficAppData.updateDaaParams){
                //TrafficMonitor_UpdateParameters(trafficAppData.tfMonitor,params,(bool)msg->logDAAdata);
            }
            trafficAppData.trafficSrc = msg->trafficSource;
            break;
        }
    }
}

int32_t TrafficTableValidationFunc(void *TblPtr){
    return 0;
}
