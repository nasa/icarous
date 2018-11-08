//
// Created by Swee Balachandran on 12/22/17.
//
#include <msgdef/ardupilot_msg.h>
#include <msgdef/merger_msg.h>
#include <msgids/merger_msgids.h>
#include "merger.h"
#include "merger_table.h"
#include <math.h>
#include <msgdef/geofence_msg.h>
#include <time.h>
#include <float.h>
#include "IntersectionScheduler.h"
#include "merger_tbl.c"

mergerAppData_t mergerAppData;

/// Event ID filter definition
CFE_EVS_BinFilter_t  MERGER_EventFilters[] =
        {  /* Event ID    mask */
                {MERGER_STARTUP_INF_EID,       0x0000},
                {MERGER_COMMAND_ERR_EID,       0x0000},
        };

/* Application entry points */
void MERGER_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    MERGER_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){

        status = CFE_SB_RcvMsg(&mergerAppData.Merger_MsgPtr, mergerAppData.Merger_Pipe, CFE_SB_PEND_FOREVER);

        if (status == CFE_SUCCESS)
        {
            MERGER_ProcessPacket();
        }
    }

    MERGER_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void MERGER_AppInit(void) {

    memset(&mergerAppData, 0, sizeof(mergerAppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(MERGER_EventFilters,
                     sizeof(MERGER_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe(&mergerAppData.Merger_Pipe, /* Variable to hold Pipe ID */
                               MERGER_PIPE_DEPTH,       /* Depth of Pipe */
                               MERGER_PIPE_NAME);       /* Name of pipe */

    CFE_SB_Subscribe(RAFT_LOGACC_MID,mergerAppData.Merger_Pipe);
    CFE_SB_Subscribe(ICAROUS_POSITION_MID,mergerAppData.Merger_Pipe);

    // Register table with table services
    status = CFE_TBL_Register(&mergerAppData.Merger_tblHandle,
                              "MergerTable",
                              sizeof(MergerTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &MergerTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(mergerAppData.Merger_tblHandle, CFE_TBL_SRC_ADDRESS, &MergerTblStruct);


    MergerTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr, mergerAppData.Merger_tblHandle);

    memcpy(&mergerAppData.mergerTable,TblPtr, sizeof(MergerTable_t));
    // Send event indicating app initialization
    CFE_EVS_SendEvent(MERGER_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "Merger App Initialized. Version %d.%d",
                      MERGER_MAJOR_VERSION,
                      MERGER_MINOR_VERSION);


    mergerAppData.nextIntersectionId = 0;
    mergerAppData.arrivalDataUpdated = false;
    mergerAppData.time2ZoneEntry = 0;
    mergerAppData.mergingSpeed = -1;
    mergerAppData.navState = _MERGER_NOOP_;
    memset(&mergerAppData.arrivalData,0,sizeof(arrivalData_t));
    memset(&mergerAppData.aircraftIdInt,-1,sizeof(int)*10);
    mergerAppData.defaultEntryPlan = true;
    mergerAppData.entryPointComputed = false;
    mergerAppData.inComputeZone = false;
    OS_printf("entry radius: %f\n",mergerAppData.mergerTable.entryRadius);

    double d = mergerAppData.mergerTable.entryRadius;//mergerAppData.mergerTable.corridorWidth;
    double  xc1 = mergerAppData.mergerTable.maxVehicleTurnRadius;
    double xc3 = 0;

    OS_printf("d, xc1:%f, %f\n",d,xc1);

    double nfac1 = sqrt( pow(xc1,2) + pow(xc3,2));
    double nfac2 = sqrt( pow(d - xc1,2) + pow(xc3,2) );
    double vres = (nfac1 + nfac2)/7;
    OS_printf("%d, %f, %f\n",nfac1,nfac2);
}


void MERGER_AppCleanUp(){
}


void MERGER_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(mergerAppData.Merger_MsgPtr);
    switch(MsgId) {
        case ICAROUS_POSITION_MID: {
            position_t *pos;
            pos = (position_t *) mergerAppData.Merger_MsgPtr;

            if (pos->aircraft_id == CFE_PSP_GetSpacecraftId()) {
                mergerAppData.position[0] = pos->latitude;
                mergerAppData.position[1] = pos->longitude;
                mergerAppData.position[2] = pos->altitude_rel;

                mergerAppData.velocity[0] = pos->vn;
                mergerAppData.velocity[1] = pos->ve;
                mergerAppData.velocity[2] = pos->vd;

                mergerAppData.currentSpeed = sqrt(pow(pos->vn,2) + pow(pos->ve,2) + pow(pos->vd,2));
                if(mergerAppData.mergingSpeed < 0){
                    mergerAppData.mergingSpeed = mergerAppData.currentSpeed;
                }

                ComputeArrivalData();

                CheckIntersectionConflict();

                ExecuteNewPath();
            }

            break;
        }


        case RAFT_LOGACC_MID:{
            log_ud_acc_t* logEntry;
            logEntry = (log_ud_acc_t*) mergerAppData.Merger_MsgPtr;
            mergerAppData.numNodesInt = logEntry->numNodes ;
            for(int i=0;i<5;++i){
                int aircraftID =logEntry->log_ud_acc[i].aircraftID;

                if(mergerAppData.inComputeZone && aircraftID == CFE_PSP_GetSpacecraftId()){
                    continue;
                }
                if(aircraftID != -1){
                    mergerAppData.aircraftIdInt[aircraftID] = aircraftID;
                    mergerAppData.intersectionEntryData[aircraftID].aircraftID = aircraftID;
                    mergerAppData.intersectionEntryData[aircraftID].currentArrivalTime = logEntry->log_ud_acc[i].currentArrivalTime;
                    mergerAppData.intersectionEntryData[aircraftID].intersectionID = logEntry->log_ud_acc[i].intersectionID;
                    mergerAppData.intersectionEntryData[aircraftID].earlyArrivalTime = logEntry->log_ud_acc[i].earlyArrivalTime;
                    mergerAppData.intersectionEntryData[aircraftID].lateArrivalTime = logEntry->log_ud_acc[i].lateArrivalTime;
                }
            }

            OS_printf("***********LOG ENTRIES*******************\n");
            for(int i=0;i<5;i++){
                OS_printf("Aircraft id: %d, log id: %d\n",mergerAppData.aircraftIdInt[i],logEntry->index);
                OS_printf("R, T, D: %lu, %lu, %lu\n",mergerAppData.intersectionEntryData[i].earlyArrivalTime,
                          mergerAppData.intersectionEntryData[i].currentArrivalTime,
                          mergerAppData.intersectionEntryData[i].lateArrivalTime);
            }
            break;
        }

    }
}

int32_t MergerTableValidationFunc(void *TblPtr){
    return 0;
}


