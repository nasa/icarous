//
// Created by Swee Balachandran on 12/22/17.
//
#include <msgdef/ardupilot_msg.h>
#include "geofence.h"
#include <math.h>
#include <msgdef/geofence_msg.h>

/// Event ID filter definition
CFE_EVS_BinFilter_t  GEOFENCE_EventFilters[] =
        {  /* Event ID    mask */
                {GEOFENCE_STARTUP_INF_EID,       0x0000},
                {GEOFENCE_COMMAND_ERR_EID,       0x0000},
        };

/* Application entry points */
void GEOFENCE_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    GEOFENCE_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        status = CFE_SB_RcvMsg(&geofenceAppData.Geofence_MsgPtr, geofenceAppData.Geofence_Pipe, CFE_SB_PEND_FOREVER);

        if (status == CFE_SUCCESS)
        {
            GEOFENCE_ProcessPacket();
        }
    }

    GEOFENCE_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void GEOFENCE_AppInit(void) {

    memset(&geofenceAppData, 0, sizeof(geofenceAppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(GEOFENCE_EventFilters,
                     sizeof(GEOFENCE_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe(&geofenceAppData.Geofence_Pipe, /* Variable to hold Pipe ID */
                               GEOFENCE_PIPE_DEPTH,       /* Depth of Pipe */
                               GEOFENCE_PIPE_NAME);       /* Name of pipe */

    //Subscribe to plexil output messages from the SB
    CFE_SB_Subscribe(ICAROUS_GEOFENCE_MID,geofenceAppData.Geofence_Pipe);
    CFE_SB_Subscribe(ICAROUS_RESET_MID,geofenceAppData.Geofence_Pipe);
    CFE_SB_Subscribe(ICAROUS_POSITION_MID,geofenceAppData.Geofence_Pipe);
    CFE_SB_Subscribe(GEOFENCE_WAKEUP_MID,geofenceAppData.Geofence_Pipe);
    CFE_SB_Subscribe(ICAROUS_FLIGHTPLAN_MID,geofenceAppData.Geofence_Pipe);
    CFE_SB_Subscribe(GEOFENCE_PATH_CHECK_MID,geofenceAppData.Geofence_Pipe);

    // Register table with table services
    status = CFE_TBL_Register(&geofenceAppData.Geofence_tblHandle,
                              "GeofenceTable",
                              sizeof(GeofenceTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &GeofenceTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(geofenceAppData.Geofence_tblHandle, CFE_TBL_SRC_FILE, "/cf/geofence_tbl.tbl");


    GeofenceTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr, geofenceAppData.Geofence_tblHandle);



    // Send event indicating app initialization
    CFE_EVS_SendEvent(GEOFENCE_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "Geofence App Initialized. Version %d.%d",
                      GEOFENCE_MAJOR_VERSION,
                      GEOFENCE_MINOR_VERSION);

    double inputParam[5] = {TblPtr->lookahead,
                            TblPtr->hbuffer,TblPtr->vbuffer,
                            TblPtr->hstepback,TblPtr->vstepback};

    geofenceAppData.gfMonitor = new_GeofenceMonitor(inputParam);
    geofenceAppData.receivedFence = false;
    geofenceAppData.receivedFP = false;
    geofenceAppData.position[0] = 0;
    geofenceAppData.position[1] = 0;
    geofenceAppData.position[2] = 0;

    memset(geofenceAppData.waypointConflict,false,50);
    memset(geofenceAppData.directPathToWP,true,50);
    geofenceAppData.flightplan.totalWayPoints = 0;
    geofenceAppData.numFences = 0;
}

void GEOFENCE_AppCleanUp(){
    // Do clean up here
    delete_GeofenceMonitor(geofenceAppData.gfMonitor);
}

void GEOFENCE_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(geofenceAppData.Geofence_MsgPtr);

    switch(MsgId){
        case ICAROUS_GEOFENCE_MID: {
            geofence_t *gf;
            gf = (geofence_t *) geofenceAppData.Geofence_MsgPtr;

            double vertices[50][2];

            for(int i=0;i<gf->totalvertices;++i) {
                vertices[i][0] = gf->vertices[i][0];
                vertices[i][1] = gf->vertices[i][1];
            }
            geofenceAppData.numFences++;
            GeofenceMonitor_InputGeofenceData(geofenceAppData.gfMonitor,gf->type,gf->index,gf->totalvertices,gf->floor,gf->ceiling,vertices);
            geofenceAppData.receivedFence = true;

            if(geofenceAppData.receivedFP){
                for(int i=0;i<geofenceAppData.flightplan.totalWayPoints; ++i) {
                    double wpPos[3] = {geofenceAppData.flightplan.position[i][0],
                                       geofenceAppData.flightplan.position[i][1],
                                       geofenceAppData.flightplan.position[i][2]};
                    geofenceAppData.waypointConflict[i] = GeofenceMonitor_CheckViolation(geofenceAppData.gfMonitor, wpPos , 0, 0, 0);

                    double position[3] = {geofenceAppData.position[0],geofenceAppData.position[1],geofenceAppData.position[2]};
                    geofenceAppData.directPathToWP[i] = GeofenceMonitor_CheckWPFeasibility(geofenceAppData.gfMonitor,position,wpPos);
                }
            }
            CFE_EVS_SendEvent(GEOFENCE_RECEIVED_FENCE_EID,CFE_EVS_INFORMATION,"Received geofence:%d of type: %d",gf->index,gf->type);

            break;
        }

        case ICAROUS_POSITION_MID:{
            position_t* pos;
            pos = (position_t*) geofenceAppData.Geofence_MsgPtr;

            if(pos->aircraft_id == CFE_PSP_GetSpacecraftId()) {
                geofenceAppData.position[0] = pos->latitude;
                geofenceAppData.position[1] = pos->longitude;
                geofenceAppData.position[2] = pos->altitude_rel;

                geofenceAppData.velocity[0] = pos->vn;
                geofenceAppData.velocity[1] = pos->ve;
                geofenceAppData.velocity[2] = pos->vd;
            }

            break;
        }

        case ICAROUS_FLIGHTPLAN_MID:{
            flightplan_t* fp;
            fp = (flightplan_t*) geofenceAppData.Geofence_MsgPtr;
            memcpy(&geofenceAppData.flightplan,fp,sizeof(flightplan_t));
            geofenceAppData.receivedFP = true;

           for(int i=0;i<geofenceAppData.flightplan.totalWayPoints; ++i) {
                    double wpPos[3] = {geofenceAppData.flightplan.position[i][0],
                                       geofenceAppData.flightplan.position[i][1],
                                       geofenceAppData.flightplan.position[i][2]};
                    geofenceAppData.waypointConflict[i] = GeofenceMonitor_CheckViolation(geofenceAppData.gfMonitor, wpPos , 0, 0, 0);

                    double position[3] = {geofenceAppData.position[0],geofenceAppData.position[1],geofenceAppData.position[2]};
                    geofenceAppData.directPathToWP[i] = GeofenceMonitor_CheckWPFeasibility(geofenceAppData.gfMonitor,position,wpPos);
                }

            break;
        }

        case ICAROUS_RESET_MID:{
            GeofenceMonitor_ClearFences(geofenceAppData.gfMonitor);
            break;
        }

        case GEOFENCE_PATH_CHECK_MID:{
            pathFeasibilityCheck_t *pfcheck = (pathFeasibilityCheck_t*) geofenceAppData.Geofence_MsgPtr;
            pathFeasibilityCheck_t pfresult;
            CFE_SB_InitMsg(&pfresult,GEOFENCE_PATH_CHECK_RESULT_MID,sizeof(pathFeasibilityCheck_t),TRUE);
            memcpy(pfresult.fromPosition,pfcheck->fromPosition,sizeof(double)*3);
            memcpy(pfresult.toPosition,pfcheck->toPosition,sizeof(double)*3);
            pfresult.feasibleA = GeofenceMonitor_CheckViolation(geofenceAppData.gfMonitor,pfresult.fromPosition,0,0,0);
            pfresult.feasibleB = GeofenceMonitor_CheckViolation(geofenceAppData.gfMonitor,pfresult.toPosition,0,0,0);
            pfresult.feasibleAB = GeofenceMonitor_CheckWPFeasibility(geofenceAppData.gfMonitor,pfresult.fromPosition,pfresult.toPosition);
            SendSBMsg(pfresult);
            break;
        }

        case GEOFENCE_WAKEUP_MID:{
            CFE_SB_InitMsg(&geofenceAppData.gfConflictData,ICAROUS_GEOFENCE_MONITOR_MID,sizeof(geofenceConflict_t),TRUE);

            double angle = 360 + atan2(geofenceAppData.velocity[1], geofenceAppData.velocity[0]) * 180 / M_PI;
            double track = fmod(angle, 360);
            double groundSpeed = sqrt(pow(geofenceAppData.velocity[0], 2) + pow(geofenceAppData.velocity[1], 2));
            double verticalSpeed = geofenceAppData.velocity[2];

            GeofenceMonitor_CheckViolation(geofenceAppData.gfMonitor,
                                           geofenceAppData.position,
                                           track,
                                           groundSpeed,
                                           verticalSpeed);

            int numConflicts = GeofenceMonitor_GetNumConflicts(geofenceAppData.gfMonitor);
            geofenceAppData.gfConflictData.numConflicts = (uint8_t)numConflicts;
            geofenceAppData.gfConflictData.numFences = geofenceAppData.numFences;
            for(int i=0;i<numConflicts;++i){
               if(numConflicts > 4){
                   break;
               }
               int id,type;
               bool conflict,violation;
               double recoveryPosition[3];
               GeofenceMonitor_GetConflict(geofenceAppData.gfMonitor,i,&id,&conflict,&violation,recoveryPosition,&type);
               geofenceAppData.gfConflictData.conflictFenceIDs[i] = (uint8_t)id;
               geofenceAppData.gfConflictData.conflictTypes[i] = (uint8_t)type;
               memcpy(geofenceAppData.gfConflictData.recoveryPosition,recoveryPosition, sizeof(double)*3);
               //TODO: add time to violation data;
               //OS_printf("Geofence conflict type:%d at %f,%f\n",type,geofenceAppData.position[0],geofenceAppData.position[1]);
            }

            memcpy(geofenceAppData.gfConflictData.waypointConflict,geofenceAppData.waypointConflict,sizeof(bool)*50);
            memcpy(geofenceAppData.gfConflictData.directPathToWaypoint,geofenceAppData.directPathToWP,sizeof(bool)*50);
            SendSBMsg(geofenceAppData.gfConflictData);

            break;
        }
    }
}

int32_t GeofenceTableValidationFunc(void *TblPtr){
    return 0;
}
