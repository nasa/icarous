//
// Created by Swee Balachandran on 12/22/17.
//
#include <merger_msg.h>
#include <merger_msgids.h>
#include "merg.h"
#include "merger_table.h"
#include <math.h>
#include <time.h>
#include <float.h>
#include <raft_msgids.h>
#include <raft_msg.h>
#include "merger_tbl.c"
#include "UtilFunctions.h"

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

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(MERGER_EventFilters,
                     sizeof(MERGER_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    CFE_SB_CreatePipe(&mergerAppData.Merger_Pipe, /* Variable to hold Pipe ID */
                               MERGER_PIPE_DEPTH,       /* Depth of Pipe */
                               MERGER_PIPE_NAME);       /* Name of pipe */

    CFE_SB_SubscribeLocal(ICAROUS_RAFT_DATALOG,mergerAppData.Merger_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_POSITION_MID,mergerAppData.Merger_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(MERGER_PARAMETERS_MID,mergerAppData.Merger_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(FREQ_10_WAKEUP_MID,mergerAppData.Merger_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);

    // Register table with table services
    CFE_TBL_Register(&mergerAppData.Merger_tblHandle,
                              "MergerTable",
                              sizeof(MergerTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &MergerTableValidationFunc);

    // Load app table data
    CFE_TBL_Load(mergerAppData.Merger_tblHandle, CFE_TBL_SRC_ADDRESS, &MergerTblStruct);


    MergerTable_t *TblPtr;
    CFE_TBL_GetAddress((void**)&TblPtr, mergerAppData.Merger_tblHandle);

    memcpy(&mergerAppData.mergerTable,TblPtr, sizeof(MergerTable_t));
    // Send event indicating app initialization
    CFE_EVS_SendEvent(MERGER_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "Merger App Initialized. Version %d.%d",
                      MERGER_MAJOR_VERSION,
                      MERGER_MINOR_VERSION);

    MERGER_AppInitializeData();
    
}

void MERGER_AppInitializeData(){

    char callsign[30];
    memset(callsign,0,30);
    sprintf(callsign,"aircraft%d",CFE_PSP_GetSpacecraftId());
    mergerAppData.merger = MergerInit(callsign,"../ram/IcarousConfig.txt",CFE_PSP_GetSpacecraftId());

    char filename[]= "../ram/merge_fixes.txt";
    int val = ReadFlightplanFromFile(filename,&mergerAppData.mgData);
    if(val < 0){
        OS_printf("Merge fixes not available\n");
    }else{
        OS_printf("Merge fixes read successfully\n");
    }

    memset(mergerAppData.mergerTable.IntersectionID,0,sizeof(uint32_t)*20);
    for(int i=0;i < mergerAppData.mgData.num_waypoints; ++i){
        double locInt[3] = {mergerAppData.mgData.waypoints[i].latitude,
                            mergerAppData.mgData.waypoints[i].longitude,
                            mergerAppData.mgData.waypoints[i].altitude};
        memcpy(mergerAppData.mergerTable.IntersectionLocation[i],locInt,sizeof(double)*3);
        mergerAppData.mergerTable.IntersectionID[i] = atoi(mergerAppData.mgData.waypoints[i].name);
    }
    mergerAppData.mgStatus = -1;
}


void MERGER_AppCleanUp(){
}


void MERGER_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(mergerAppData.Merger_MsgPtr);
    switch(MsgId) {

        case FREQ_10_WAKEUP_MID:{

            struct timespec ts;
            clock_gettime(CLOCK_REALTIME,&ts);
            double time = ts.tv_sec + (double)(ts.tv_nsec)/1E9;
            mergeStatus_e mergeStatus = MergerRun(mergerAppData.merger,time);

            if(mergerAppData.mgStatus != mergeStatus){
                argsCmd_t command;
                CFE_SB_InitMsg(&command,MERGER_STATUS_MID,sizeof(argsCmd_t),TRUE);
                command.param1 = mergeStatus;
                SendSBMsg(command);
                mergerAppData.mgStatus = mergeStatus;
            }

            if(mergeStatus == MERGING_GUIDANCE_ACTIVE){
                argsCmd_t command;
                CFE_SB_InitMsg(&command,GUIDANCE_COMMAND_MID,sizeof(argsCmd_t),TRUE);
                double trk,gs,vs;
                MergerOutputVelocity(mergerAppData.merger,&trk,&gs,&vs);
                command.name = SPEED_CHANGE;
                memset(command.buffer,0,sizeof(char)*50);
                strcpy(command.buffer,"Plan0");
                command.param1 = gs;
                command.param2 = 0;     // Only hold this speed to the next waypoint
                SendSBMsg(command);
            }
            
            mergingData_t arrivalData;
            bool status = MergerGetArrivalTimes(mergerAppData.merger,&arrivalData);
            if(status){
                cMergingData_t cArrivalData;
                CFE_SB_InitMsg(&cArrivalData,ICAROUS_ARRIVALTIMES_MID,sizeof(cMergingData_t),TRUE);
                memcpy(&cArrivalData.aircraftID,&arrivalData,sizeof(mergingData_t));
                SendSBMsg(cArrivalData);
            }
            break;
        }

        case ICAROUS_POSITION_MID: {
            position_t *pos;
            pos = (position_t *) mergerAppData.Merger_MsgPtr;

            if (pos->aircraft_id == CFE_PSP_GetSpacecraftId()) {
                double position[3],velocity[3];
                position[0] = pos->latitude;
                position[1] = pos->longitude;
                position[2] = pos->altitude_abs;

                
                double trk,gs,vs; 
                ConvertVnedToTrkGsVs(pos->vn,pos->ve,pos->vd,&trk,&gs,&vs);
                velocity[0] = trk;
                velocity[1] = gs;
                velocity[2] = vs;
                MergerSetAircraftState(mergerAppData.merger,position,velocity);
            }

            break;
        }

        case ICAROUS_RAFT_DATALOG:{
            dataLog_t* log;
            log = CFE_SB_GetUserData(mergerAppData.Merger_MsgPtr);
            MergerSetNodeLog(mergerAppData.merger,log);
            break;
        }

        case MERGER_PARAMETERS_MID:{
            merger_parameters_t* params = (merger_parameters_t*) mergerAppData.Merger_MsgPtr;
            mergerAppData.mergerTable.maxVehicleSpeed = params->maxVehicleSpeed;
            mergerAppData.mergerTable.minVehicleSpeed = params->minVehicleSpeed;
            mergerAppData.mergerTable.corridorWidth = params->corridorWidth;
            mergerAppData.mergerTable.entryRadius = params->entryRadius;
            mergerAppData.mergerTable.coordZone = params->coordZone;
            mergerAppData.mergerTable.scheduleZone = params->scheduleZone;
            mergerAppData.mergerTable.minSeparationDistance = params->minSeparationDistance;
            mergerAppData.mergerTable.minSeparationTime = params->minSeparationTime;
            mergerAppData.mergerTable.maxVehicleTurnRadius = params->maxVehicleTurnRadius;
            mergerAppData.mergerTable.startIntersection = params->startIntersection;
            mergerAppData.mergerTable.missionSpeed = params->missionSpeed;

            // If corridor with is 0, we are not allowed to make turns
            if(mergerAppData.mergerTable.corridorWidth < 1e-3){
              mergerAppData.mergerTable.maxVehicleTurnRadius = 0; 
            }

            /*
            MergerSetVehicleConstraints(mergerAppData.merger,mergerAppData.mergerTable.minVehicleSpeed,
                                       mergerAppData.mergerTable.maxVehicleSpeed,mergerAppData.mergerTable.maxVehicleTurnRadius);

            MergerSetFixParams(mergerAppData.merger,params->minSeparationTime,params->coordZone,params->scheduleZone,params->entryRadius,params->corridorWidth);
            */

            for(int i=0;i<mergerAppData.mgData.num_waypoints;++i){
                MergerSetIntersectionData(mergerAppData.merger,i,mergerAppData.mergerTable.IntersectionID[i],mergerAppData.mergerTable.IntersectionLocation[i]);
            }
        }
    }
}

int32_t MergerTableValidationFunc(void *TblPtr){
    return 0;
}
