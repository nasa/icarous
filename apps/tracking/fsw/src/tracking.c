/**
 * @file tracking.c
 * @brief application to track an object.
 */

#include "tracking.h"
#include "UtilFunctions.h"
#include "tracking_tbl.c"
CFE_EVS_BinFilter_t  TRACKING_EventFilters[] =
{  /* Event ID    mask */
        {TRACKING_STARTUP_INF_EID,       0x0000},
        {TRACKING_COMMAND_ERR_EID,       0x0000},
        {TRACKING_RECEIVED_OBJECT_EID,   0x0000}
}; /// Event ID definitions

/* Application entry points */
void TRACKING_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    TRACKING_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        status = CFE_SB_RcvMsg(&trackingAppData.Tracking_MsgPtr, trackingAppData.Tracking_Pipe, 10);

        if (status == CFE_SUCCESS)
        {
            TRACKING_ProcessPacket();
        }
    }

    TRACKING_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void TRACKING_AppInit(void) {

    memset(&trackingAppData, 0, sizeof(TrackingAppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(TRACKING_EventFilters,
                     sizeof(TRACKING_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe(&trackingAppData.Tracking_Pipe, /* Variable to hold Pipe ID */
                               TRACKING_PIPE_DEPTH,       /* Depth of Pipe */
                               TRACKING_PIPE_NAME);       /* Name of pipe */

    //Subscribe to plexil output messages from the SB
    CFE_SB_Subscribe(ICAROUS_TRAFFIC_MID,trackingAppData.Tracking_Pipe);
    CFE_SB_Subscribe(ICAROUS_POSITION_MID,trackingAppData.Tracking_Pipe);
    CFE_SB_Subscribe(ICAROUS_TRACK_STATUS_MID,trackingAppData.Tracking_Pipe);
    CFE_SB_Subscribe(FREQ_10_WAKEUP_MID,trackingAppData.Tracking_Pipe);

    CFE_SB_Subscribe(TRACKING_PARAMETERS_MID,trackingAppData.Tracking_Pipe);

    // Initialize messages
    CFE_SB_InitMsg(&trackingAppData.resolution,TRACKING_RESPONSE_MID,sizeof(trackingResolution_t),TRUE);

    // Register table with table services
    status = CFE_TBL_Register(&trackingAppData.Tracking_tblHandle,
                              "TrackingTable",
                              sizeof(TrackingTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &TrackingTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(trackingAppData.Tracking_tblHandle, CFE_TBL_SRC_ADDRESS, &Tracking_TblStruct);

    TrackingTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr, trackingAppData.Tracking_tblHandle);

    trackingAppData.trackingID = TblPtr->trackingObjId;
    trackingAppData.propGain[0] = TblPtr->pGainX;
    trackingAppData.propGain[1] = TblPtr->pGainY;
    trackingAppData.propGain[2] = TblPtr->pGainZ;
    trackingAppData.heading = TblPtr->heading;
    trackingAppData.distH = TblPtr->distH;
    trackingAppData.distV = TblPtr->distV;
    trackingAppData.command = TblPtr->command;

    // Send event indicating app initialization
    CFE_EVS_SendEvent(TRACKING_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "Tracking App Initialized. Version %d.%d",
                      TRACKING_MAJOR_VERSION,
                      TRACKING_MINOR_VERSION);

    trackingAppData.numObjects = 0;
    trackingAppData.track = false;

}

void TRACKING_AppCleanUp(){
    // Do clean up here
}

void TRACKING_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(trackingAppData.Tracking_MsgPtr);

    switch(MsgId){

        case TRACKING_PARAMETERS_MID:{
           tracking_parameters_t *trkParams = (tracking_parameters_t*) trackingAppData.Tracking_MsgPtr;
           trackingAppData.trackingID = trkParams->trackingObjId;
           trackingAppData.propGain[0] = trkParams->pGainX;
           trackingAppData.propGain[1] = trkParams->pGainY;
           trackingAppData.propGain[2] = trkParams->pGainZ;
           trackingAppData.heading     = trkParams->heading;
           trackingAppData.distH       = trkParams->distH;
           trackingAppData.distV       = trkParams->distV;
           trackingAppData.command     = trkParams->command;
           break;
        }

        case ICAROUS_TRACK_STATUS_MID:{
            argsCmd_t *trkCmd = (argsCmd_t*) trackingAppData.Tracking_MsgPtr;
            int val = (int) trkCmd->param1;
            trackingAppData.track = val==1?true:false;
            break;
        }

        case ICAROUS_TRAFFIC_MID:{
            object_t* msg;
            msg = (object_t*) trackingAppData.Tracking_MsgPtr;
            double pos[3] = {msg->latitude,msg->longitude,msg->altitude};
            double vel[3] = {msg->vn,msg->ve,msg->vd};

            int val = -1;
            for(int i=0;i<trackingAppData.numObjects;++i){
               if(msg->index == trackingAppData.objectID[i]) {
                   val = i;
                   trackingAppData.objectsPos[val][0] = pos[0];
                   trackingAppData.objectsPos[val][1] = pos[1];
                   trackingAppData.objectsPos[val][2] = pos[2];

                   trackingAppData.objectsVel[val][0] = vel[0];
                   trackingAppData.objectsVel[val][1] = vel[1];
                   trackingAppData.objectsVel[val][2] = vel[2];
                   break;
               }

            }

            if(val < 0) {
                CFE_EVS_SendEvent(TRACKING_RECEIVED_OBJECT_EID, CFE_EVS_INFORMATION, "Received object to track:%d",
                                  msg->index);
                trackingAppData.objectID[trackingAppData.numObjects] = msg->index;
                trackingAppData.objectsPos[trackingAppData.numObjects][0] = pos[0];
                trackingAppData.objectsPos[trackingAppData.numObjects][1] = pos[1];
                trackingAppData.objectsPos[trackingAppData.numObjects][2] = pos[2];

                trackingAppData.objectsVel[trackingAppData.numObjects][0] = vel[0];
                trackingAppData.objectsVel[trackingAppData.numObjects][1] = vel[1];
                trackingAppData.objectsVel[trackingAppData.numObjects][2] = vel[2];

                trackingAppData.numObjects++;
            }
            break;
        }

        case ICAROUS_POSITION_MID:{
            position_t* msg;
            msg = (position_t*) trackingAppData.Tracking_MsgPtr;

            if (msg->aircraft_id != CFE_PSP_GetSpacecraftId()) {

                double pos[3] = {msg->latitude,msg->longitude,msg->altitude_abs};
                double vel[3] = {msg->vn,msg->ve,msg->vd};

                int val = -1;
                for(int i=0;i<trackingAppData.numObjects;++i){
                    if(msg->aircraft_id == trackingAppData.objectID[i]) {
                        val = i;
                        trackingAppData.objectsPos[val][0] = pos[0];
                        trackingAppData.objectsPos[val][1] = pos[1];
                        trackingAppData.objectsPos[val][2] = pos[2];

                        trackingAppData.objectsVel[val][0] = vel[0];
                        trackingAppData.objectsVel[val][1] = vel[1];
                        trackingAppData.objectsVel[val][2] = vel[2];
                        break;
                    }

                }

                if(val < 0) {
                    CFE_EVS_SendEvent(TRACKING_RECEIVED_OBJECT_EID, CFE_EVS_INFORMATION, "Received object to track:%d",
                                      msg->aircraft_id);
                    trackingAppData.objectID[trackingAppData.numObjects] = msg->aircraft_id;
                    trackingAppData.objectsPos[trackingAppData.numObjects][0] = pos[0];
                    trackingAppData.objectsPos[trackingAppData.numObjects][1] = pos[1];
                    trackingAppData.objectsPos[trackingAppData.numObjects][2] = pos[2];

                    trackingAppData.objectsVel[trackingAppData.numObjects][0] = vel[0];
                    trackingAppData.objectsVel[trackingAppData.numObjects][1] = vel[1];
                    trackingAppData.objectsVel[trackingAppData.numObjects][2] = vel[2];

                    trackingAppData.numObjects++;
                }

            }else{

                trackingAppData.position[0] = msg->latitude;
                trackingAppData.position[1] = msg->longitude;
                trackingAppData.position[2] = msg->altitude_abs;

                double track,groundSpeed,verticalSpeed;
                ConvertVnedToTrkGsVs(msg->vn,msg->ve,msg->vd,&track,&groundSpeed,&verticalSpeed);

                trackingAppData.velocity[0] = track;
                trackingAppData.velocity[1] = groundSpeed;
                trackingAppData.velocity[2] = verticalSpeed;


            }

            break;
        }

        case FREQ_10_WAKEUP_MID:{

            double vn,ve,vd;
            for(int i=0;i<trackingAppData.numObjects;++i){
                double pos[3] = {trackingAppData.objectsPos[i][0],
                                 trackingAppData.objectsPos[i][1],
                                 trackingAppData.objectsPos[i][2]};
                double vel[3] = {trackingAppData.objectsVel[i][0],
                                 trackingAppData.objectsVel[i][1],
                                 trackingAppData.objectsVel[i][2]};
                double output[3] = {0,0,0};
                double trkHeading;

                ComputeTrackingResolution(pos,trackingAppData.position,trackingAppData.velocity,trackingAppData.heading,
                                          trackingAppData.distH,trackingAppData.distV,trackingAppData.propGain,output,&trkHeading);

                double trk,gs,vs;
                ConvertVnedToTrkGsVs(output[0],output[1],output[2],&trk,&gs,&vs);
                trackingAppData.resolution.trackingObjectID[i] = trackingAppData.objectID[i];
                trackingAppData.resolution.trackingVelocity[i][0] = trk;
                trackingAppData.resolution.trackingVelocity[i][1] = gs;
                trackingAppData.resolution.trackingVelocity[i][2] = vs;
                trackingAppData.resolution.trackingHeading[i] = trkHeading;


                if(trackingAppData.trackingID == i){
                    vn = output[0];
                    ve = output[1];
                    vd = output[2];

                }
            }

            if(trackingAppData.track)
                SendSBMsg(trackingAppData.resolution);

            if(trackingAppData.command && trackingAppData.track){
                 argsCmd_t cmd;
                 CFE_SB_InitMsg(&cmd,ICAROUS_COMMANDS_MID, sizeof(argsCmd_t),TRUE);
                 cmd.name = _SETVEL_;
                 cmd.param1 = vn;
                 cmd.param2 = ve;
                 cmd.param3 = vd;
                 SendSBMsg(cmd);

                 //OS_printf("Sending commands: %f,%f,%f\n",vn,ve,vd);
            }
            break;
        }
    }
}

int32_t TrackingTableValidationFunc(void *TblPtr){
    return 0;
}
