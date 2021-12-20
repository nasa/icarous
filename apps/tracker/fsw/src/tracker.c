//
// Created by Swee Balachandran on 4/30/2021.
//
#include "tracker.h"
#include "tracker_tbl.c"
#include "TargetTracker.h"
#include "UtilFunctions.h"
#include "Icarous.h"

CFE_EVS_BinFilter_t  tracker_EventFilters[] =
{  /* Event ID    mask */
        { TRACKER_STARTUP_INF_EID,       0x0000},
        { TRACKER_COMMAND_ERR_EID,       0x0000},
}; /// Event ID definitions

/* Application entry points */
void TRACKER_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    TRACKER_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        status = CFE_SB_RcvMsg(&trackerAppData.tracker_MsgPtr, trackerAppData.tracker_Pipe, 10);

        if (status == CFE_SUCCESS)
        {
		    TRACKER_ProcessPacket();
        }
    }

    TRACKER_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void TRACKER_AppInit(void) {

    memset(&trackerAppData, 0, sizeof(trackerAppData_t));

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(tracker_EventFilters,
                     sizeof(tracker_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    CFE_SB_CreatePipe(&trackerAppData.tracker_Pipe, /* Variable to hold Pipe ID */
		    TRACKER_PIPE_DEPTH,       /* Depth of Pipe */
		    TRACKER_PIPE_NAME);       /* Name of pipe */

    //Subscribe to output messages from the SB
    CFE_SB_Subscribe(ICAROUS_RAWTRAFFIC_MID,trackerAppData.tracker_Pipe);
    CFE_SB_Subscribe(ICAROUS_POSITION_MID,trackerAppData.tracker_Pipe);
    CFE_SB_Subscribe(FREQ_10_WAKEUP_MID,trackerAppData.tracker_Pipe);
    CFE_SB_Subscribe(FREQ_01_WAKEUP_MID,trackerAppData.tracker_Pipe);


    // Register table with table services
    CFE_TBL_Register(&trackerAppData.tracker_tblHandle,
                              "trackerTable",
                              sizeof(trackerTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &trackerTableValidationFunc);

    // Load app table data
    CFE_TBL_Load(trackerAppData.tracker_tblHandle,CFE_TBL_SRC_ADDRESS,&tracker_TblStruct);

    trackerTable_t *TblPtr;
    CFE_TBL_GetAddress((void**)&TblPtr, trackerAppData.tracker_tblHandle);

    trackerAppData.TargetTracker = (void*) new_TargetTracker("Ownship","../ram/IcarousConfig.txt");

    double sigmaP[6], sigmaV[6];
    memcpy(sigmaP,TblPtr->modelUncertaintyP,sizeof(double)*6);
    memcpy(sigmaV,TblPtr->modelUncertaintyV,sizeof(double)*6);
    // TODO: Read in values for sigmaP,sigmaV
    TargetTracker_SetModelUncertainty(trackerAppData.TargetTracker,sigmaP,sigmaV);
    TargetTracker_SetGateThresholds(trackerAppData.TargetTracker,TblPtr->pThreshold,TblPtr->vThreshold);
    trackerAppData.initialized = false;
    // Send event indicating app initialization
    CFE_EVS_SendEvent(TRACKER_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "tracker App Initialized. Version %d.%d",
                      TRACKER_MAJOR_VERSION,
                      TRACKER_MINOR_VERSION);
}

void TRACKER_AppCleanUp(void){
    // Do clean up here
}

void TRACKER_ProcessPacket(void){
    struct timespec ts;
    double timeNow;
    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(trackerAppData.tracker_MsgPtr);
    timespec_get(&ts,TIME_UTC);
    timeNow = ts.tv_sec + (double)(ts.tv_nsec)/1E9;

    switch(MsgId){

        case FREQ_01_WAKEUP_MID:{
            int n = TargetTracker_GetTotalIntruders(trackerAppData.TargetTracker);
            for(int i=0;i<n;++i){
                char callsign[MAX_CALLSIGN_LEN];
                double time;
                double pos[3];
                double vel[3];
                double sigmaP[6];
                double sigmaV[6];
                TargetTracker_GetIntruderData(trackerAppData.TargetTracker,i,callsign,&time,pos,vel,sigmaP,sigmaV);
                object_t intruder;
                CFE_SB_InitMsg(&intruder,ICAROUS_TRAFFIC_MID,sizeof(object_t),TRUE);
                strcpy(intruder.callsign.value,callsign);
                memcpy(intruder.sigmaP,sigmaP,sizeof(double)*6);
                memcpy(intruder.sigmaV,sigmaV,sizeof(double)*6);
                intruder.latitude  = pos[0];
                intruder.longitude = pos[1];
                intruder.altitude  = pos[2];
                intruder.vn = vel[1];
                intruder.ve = vel[0];
                intruder.vd = -vel[2];
                SendSBMsg(intruder);
            }
            break;
        }

        case FREQ_10_WAKEUP_MID:{
            TargetTracker_UpdatePredictions(trackerAppData.TargetTracker,timeNow);
            break;
        }

        case ICAROUS_POSITION_MID:{
            position_t* msg = (position_t*)trackerAppData.tracker_MsgPtr;
            double pos[3] = {msg->latitude,msg->longitude,msg->altitude_abs};
            double trkGsVs[3] = {0.0,0.0,0.0};
            ConvertVnedToTrkGsVs(msg->vn,msg->ve,msg->vd,trkGsVs,trkGsVs+1,trkGsVs+2);
            double sigmaP[6] = {25.0,25.0,25.0,0.0,0.0,0.0};
            double sigmaV[6] = {10.0,10.0,10.0,0.0,0.0,0.0};
            if(trackerAppData.initialized){
                TargetTracker_InputCurrentState(trackerAppData.TargetTracker,msg->time_gps,pos,trkGsVs,sigmaP,sigmaV);
            }else{
                TargetTracker_SetHomePosition(trackerAppData.TargetTracker,pos);
                trackerAppData.initialized = true;
            }
            break;
        }

        case ICAROUS_RAWTRAFFIC_MID:{
            object_t* msg = (object_t*)trackerAppData.tracker_MsgPtr;
            double pos[3] = {msg->latitude,msg->longitude,msg->altitude};
            double vel[3] = {msg->ve,msg->vn,-msg->vd};
            double sigmaP[6];
            double sigmaV[6];
            memcpy(sigmaP,msg->sigmaP,sizeof(double)*6);
            memcpy(sigmaV,msg->sigmaV,sizeof(double)*6);
            if(trackerAppData.initialized){
                TargetTracker_InputMeasurement(trackerAppData.TargetTracker,msg->callsign.value,timeNow,pos,vel,sigmaP,sigmaV);
            }
            break;
        }
    }
}

int32_t trackerTableValidationFunc(void *TblPtr){
    return 0;
}
