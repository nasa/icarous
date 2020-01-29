//
// Created by Swee Balachandran on 12/22/17.
//
#include "geofence.h"
#include <math.h>

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
    CFE_SB_SubscribeLocal(ICAROUS_RESET_MID,geofenceAppData.Geofence_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_POSITION_MID,geofenceAppData.Geofence_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(FREQ_30_WAKEUP_MID,geofenceAppData.Geofence_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_FLIGHTPLAN_MID,geofenceAppData.Geofence_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(GEOFENCE_PATH_CHECK_MID,geofenceAppData.Geofence_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(GEOFENCE_PARAMETERS_MID,geofenceAppData.Geofence_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);

    // Register table with table services
    status = CFE_TBL_Register(&geofenceAppData.Geofence_tblHandle,
                              "GeofenceTable",
                              sizeof(GeofenceTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &GeofenceTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(geofenceAppData.Geofence_tblHandle, CFE_TBL_SRC_ADDRESS, &Geofence_TblStruct);


    GeofenceTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr, geofenceAppData.Geofence_tblHandle);



   
    double inputParam[5] = {TblPtr->lookahead,
                            TblPtr->hbuffer,TblPtr->vbuffer,
                            TblPtr->hstepback,TblPtr->vstepback};

    geofenceAppData.gfMonitor = new_GeofenceMonitor(inputParam);
    geofenceAppData.receivedFence = false;
    geofenceAppData.receivedFP1 = false;
    geofenceAppData.receivedFP2 = false;
    geofenceAppData.position[0] = 0;
    geofenceAppData.position[1] = 0;
    geofenceAppData.position[2] = 0;

    memset(geofenceAppData.waypointConflict1,false,50);
    memset(geofenceAppData.directPathToWP1,true,50);
    memset(geofenceAppData.waypointConflict2,false,50);
    memset(geofenceAppData.directPathToWP2,true,50);
    geofenceAppData.flightplan1.num_waypoints = 0;
    geofenceAppData.numFences = 0;


    if(status == CFE_SUCCESS){
        // Send event indicating app initialization
        CFE_EVS_SendEvent(GEOFENCE_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                        "Geofence App Initialized. Version %d.%d",
                        GEOFENCE_MAJOR_VERSION,
                        GEOFENCE_MINOR_VERSION);
    }

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

            double vertices[MAX_VERTICES][2];

            for(int i=0;i<gf->totalvertices;++i) {
                vertices[i][0] = gf->vertices[i][0];
                vertices[i][1] = gf->vertices[i][1];
            }
            geofenceAppData.numFences++;
            GeofenceMonitor_InputGeofenceData(geofenceAppData.gfMonitor,gf->type,gf->index,gf->totalvertices,gf->floor,gf->ceiling,vertices);
            geofenceAppData.receivedFence = true;

            
            CFE_ES_WriteToSysLog("Geofence:Received geofence:%d of type: %d\n",gf->index,gf->type);

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
            memcpy(&geofenceAppData.flightplan1,fp,sizeof(flightplan_t));
            geofenceAppData.receivedFP1 = true;
            break;
        }

        case ICAROUS_TRAJECTORY_MID:
        {
            flightplan_t *fp;
            fp = (flightplan_t *)geofenceAppData.Geofence_MsgPtr;
            memcpy(&geofenceAppData.flightplan2, fp, sizeof(flightplan_t));
            geofenceAppData.receivedFP2 = true;
            break;
        }

        case ICAROUS_RESET_MID:{
            argsCmd_t *reset = (argsCmd_t*) geofenceAppData.Geofence_MsgPtr;
            if(reset->param1 == 2){
                 GeofenceMonitor_ClearFences(geofenceAppData.gfMonitor);
            }
            break;
        }

        case GEOFENCE_PATH_CHECK_MID:{
            pathFeasibilityCheck_t *pfcheck = (pathFeasibilityCheck_t*) geofenceAppData.Geofence_MsgPtr;
            pathFeasibilityCheck_t pfresult;
            CFE_SB_InitMsg(&pfresult,GEOFENCE_PATH_CHECK_RESULT_MID,sizeof(pathFeasibilityCheck_t),TRUE);
            memcpy(pfresult.fromPosition,pfcheck->fromPosition,sizeof(double)*3);
            memcpy(pfresult.toPosition,pfcheck->toPosition,sizeof(double)*3);
            pfresult.feasibleA = !GeofenceMonitor_CheckViolation(geofenceAppData.gfMonitor,pfresult.fromPosition,0,0,0);
            pfresult.feasibleB = !GeofenceMonitor_CheckViolation(geofenceAppData.gfMonitor,pfresult.toPosition,0,0,0);
            pfresult.feasibleAB = GeofenceMonitor_CheckWPFeasibility(geofenceAppData.gfMonitor,pfresult.fromPosition,pfresult.toPosition);
            SendSBMsg(pfresult);
            break;
        }

        case FREQ_30_WAKEUP_MID:{
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

            if(geofenceAppData.receivedFP1){
                for(int i=0;i<geofenceAppData.flightplan1.num_waypoints; ++i) {
                    double wpPos[3] = {geofenceAppData.flightplan1.waypoints[i].latitude,
                                       geofenceAppData.flightplan1.waypoints[i].longitude,
                                       geofenceAppData.flightplan1.waypoints[i].altitude};
                    geofenceAppData.waypointConflict1[i] = GeofenceMonitor_CheckViolation(geofenceAppData.gfMonitor, wpPos , 0, 0, 0);

                    double position[3] = {geofenceAppData.position[0],geofenceAppData.position[1],geofenceAppData.position[2]};
                    geofenceAppData.directPathToWP1[i] = GeofenceMonitor_CheckWPFeasibility(geofenceAppData.gfMonitor,position,wpPos);
                }
            }

            if(geofenceAppData.receivedFP2){
                for(int i=0;i<geofenceAppData.flightplan2.num_waypoints; ++i) {
                    double wpPos[3] = {geofenceAppData.flightplan2.waypoints[i].latitude,
                                       geofenceAppData.flightplan2.waypoints[i].longitude,
                                       geofenceAppData.flightplan2.waypoints[i].altitude};
                    geofenceAppData.waypointConflict2[i] = GeofenceMonitor_CheckViolation(geofenceAppData.gfMonitor, wpPos , 0, 0, 0);

                    double position[3] = {geofenceAppData.position[0],geofenceAppData.position[1],geofenceAppData.position[2]};
                    geofenceAppData.directPathToWP2[i] = GeofenceMonitor_CheckWPFeasibility(geofenceAppData.gfMonitor,position,wpPos);
                }
            }

            memcpy(geofenceAppData.gfConflictData.waypointConflict1,geofenceAppData.waypointConflict1,sizeof(bool)*50);
            memcpy(geofenceAppData.gfConflictData.directPathToWaypoint1,geofenceAppData.directPathToWP1,sizeof(bool)*50);
            memcpy(geofenceAppData.gfConflictData.waypointConflict2,geofenceAppData.waypointConflict2,sizeof(bool)*50);
            memcpy(geofenceAppData.gfConflictData.directPathToWaypoint2,geofenceAppData.directPathToWP2,sizeof(bool)*50);
            SendSBMsg(geofenceAppData.gfConflictData);

            break;
        }

        case GEOFENCE_PARAMETERS_MID:{
            geofence_parameters_t* params = (geofence_parameters_t*)geofenceAppData.Geofence_MsgPtr;
            double geoParams[5] = {params->lookahead,
                                   params->hthreshold,
                                   params->vthreshold,
                                   params->hstepback,
                                   params->vstepback};
            GeofenceMonitor_SetGeofenceParameters(geofenceAppData.gfMonitor,geoParams);
            break;
        }
    }
}

int32_t GeofenceTableValidationFunc(void *TblPtr){
    return 0;
}
