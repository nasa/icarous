// Define your custom mission specific lookup/command subscriptions in a separate file as shown here


#define EXTERN extern

#include <math.h>
#include <cfs-data-format.hh>
#include "Serialize.h"
#include "custom_datastructure.h"
#include "plexil.h"

#include "Icarous_msgids.h"
#include "geofence_msgids.h"
#include "traffic_msgids.h"
#include "trajectory_msgids.h"
#include "safe2ditch_msgids.h"
#include "guidance_msgids.h"
#include "../../../../../Modules/Utils/fence.h"


#include <Icarous_msg.h>
#include <geofence_msg.h>
#include <traffic_msg.h>
#include <trajectory_msg.h>
#include <safe2ditch_msg.h>
#include <guidance_msg.h>
#include <float.h>
#include <UtilFunctions.h>

void PLEXIL_InitializeCustomData(void){

    plexilCustomData.missionStart = -1;
    plexilCustomData.armStatus = -1;
    plexilCustomData.takeoffStatus = -1;

    plexilCustomData.keepInConflict = false;
    plexilCustomData.keepOutConflict = false;
    plexilCustomData.trafficConflict = false;
    plexilCustomData.flightPlanConflict = false;
    plexilCustomData.ditch = false;
    plexilCustomData.allowedXtrackError = DBL_MAX;
    plexilCustomData.resolutionSpeed = 1;
    plexilCustomData.totalFences = 0;
    plexilCustomData.totalTraffic = 0;
    plexilCustomData.searchType = _ASTAR;
    plexilCustomData.resolutionTraj.num_waypoints = 0;
    plexilCustomData.nextMissionWP = 1;
    plexilCustomData.trafficResType = 1;
    plexilCustomData.preferredTrack = -1e5;
    plexilCustomData.preferredSpeed = -1e5;
    plexilCustomData.preferredAlt = -1e5;
    plexilCustomData.interceptHeadingToPlan = 0;
    plexilCustomData.restartMission = false; 
    memset(&plexilCustomData.pendingRequest,0,sizeof(PlexilMsg));
}

void PLEXIL_CustomSubscription(void){

    // Plexil command/lookup return messages


    // Data messages
    CFE_SB_SubscribeLocal(ICAROUS_POSITION_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_STARTMISSION_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_RESET_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_COMACK_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_WPREACHED_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_FLIGHTPLAN_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_GEOFENCE_MONITOR_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_TRAJECTORY_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_BANDS_TRACK_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_BANDS_SPEED_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_BANDS_ALT_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(FLIGHTPLAN_MONITOR_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(GEOFENCE_PATH_CHECK_RESULT_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(SAFE2DITCH_STATUS_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(TRAFFIC_PARAMETERS_MID,plexilAppData.DATA_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
}


void PLEXIL_ProcessCustomPackets(bool data){

    CFE_SB_MsgId_t  MsgId;
    if(data) {
        MsgId = CFE_SB_GetMsgId(plexilAppData.DATA_MsgPtr);
    }else{
        MsgId = CFE_SB_GetMsgId(plexilAppData.PLEXIL_MsgPtr);
    }

    switch(MsgId){
        case ICAROUS_POSITION_MID: {
            position_t * msg = (position_t *) plexilAppData.DATA_MsgPtr;
            if(msg->aircraft_id == CFE_PSP_GetSpacecraftId()) {

                double trk,gs,vs;
                ConvertVnedToTrkGsVs(msg->vn,msg->ve,msg->vd,&trk,&gs,&vs);
                plexilCustomData.position[0] = msg->latitude;
                plexilCustomData.position[1] = msg->longitude;
                plexilCustomData.position[2] = msg->altitude_rel;
                plexilCustomData.velocity[0] = trk;
                plexilCustomData.velocity[1] = gs;
                plexilCustomData.velocity[2] = vs;
            }
            break;
        }

        case ICAROUS_FLIGHTPLAN_MID:{
            flightplan_t* msg = (flightplan_t*) plexilAppData.DATA_MsgPtr;
            plexilCustomData.totalMissionWP = msg->num_waypoints;
            memcpy(&plexilCustomData.missionFP,msg,sizeof(flightplan_t));
            break;
        }

        case ICAROUS_TRAJECTORY_MID:{
            flightplan_t* traj = (flightplan_t*) plexilAppData.DATA_MsgPtr;
            memcpy(&plexilCustomData.resolutionTraj,traj,sizeof(flightplan_t));

            PlexilMsg plxInput;
            PlexilMsg* msg = &plexilCustomData.pendingRequest;
            if(CHECKNAME(msg,"FindNewPath")){
                    memcpy(&plxInput,msg,sizeof(PlexilMsg));
            }else{
                OS_printf("plexil: No matching requests found: %s\n",msg->name);
            }

            serializeInt(false,traj->num_waypoints,plxInput.buffer);
            plexil_return(plexilAppData.adap,&plxInput);
            break;
        }

        case ICAROUS_STARTMISSION_MID:{
            plexilCustomData.restartMission = false;
            argsCmd_t* msg = (argsCmd_t*) plexilAppData.DATA_MsgPtr;
            plexilCustomData.missionStart = (int)msg->param1;
            plexilCustomData.nextMissionWP = (int)msg->param1;
            if(plexilCustomData.nextMissionWP == 0){
                plexilCustomData.nextMissionWP = 1;
            }

            if(plexilCustomData.nextMissionWP > 0){
                argsCmd_t cmd;
                CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(argsCmd_t),TRUE);
                cmd.name = PRIMARY_FLIGHTPLAN;
                cmd.param1 = 1;
                SendSBMsg(cmd); 
            }


            break;
        }

        case ICAROUS_COMACK_MID:{
            cmdAck_t* msg = (cmdAck_t*) plexilAppData.DATA_MsgPtr;

             switch(msg->name){
                 case _ARM_:
                     plexilCustomData.armStatus = (msg->result == 0) ? 1:0;
                     break;

                 case _TAKEOFF_:
                     plexilCustomData.takeoffStatus = (msg->result==0)? 1:0;
                     break;

             }
            break;
        }

        case ICAROUS_WPREACHED_MID:{
            missionItemReached_t* msg = (missionItemReached_t*) plexilAppData.DATA_MsgPtr;
            if(msg->feedback) {
                if(strcmp(msg->planID,"Takeoff") == 0){
                    plexilCustomData.takeoffStatus = msg->reachedwaypoint;

                    
                }else if(strcmp(msg->planID,"Plan0") == 0){
                    plexilCustomData.nextMissionWP = msg->reachedwaypoint + 1;
                }
            }
            break;
        }

        case ICAROUS_GEOFENCE_MONITOR_MID:{
            geofenceConflict_t* gfConflct = (geofenceConflict_t*) plexilAppData.DATA_MsgPtr;
            if(gfConflct->numConflicts > 0 ) {
                if (gfConflct->conflictTypes[0] == KEEP_IN)
                    plexilCustomData.keepInConflict = true;
                else if (gfConflct->conflictTypes[0] == KEEP_OUT)
                    plexilCustomData.keepOutConflict = true;
            }else{
                plexilCustomData.keepOutConflict = false;
                plexilCustomData.keepInConflict = false;
            }

            plexilCustomData.totalFences = gfConflct->numFences;
            memcpy(plexilCustomData.recoveryPosition,gfConflct->recoveryPosition, sizeof(double)*3);

            plexilCustomData.nextFeasibleWP = plexilCustomData.nextMissionWP;
            for(int i=plexilCustomData.nextMissionWP;i<plexilCustomData.totalMissionWP;++i) {
                if(!gfConflct->waypointConflict1[i]){
                    plexilCustomData.nextFeasibleWP = i;
                    break;
                }
            }

            bool *fp = gfConflct->waypointConflict1;
            plexilCustomData.directPathToFeasibleWP = gfConflct->directPathToWaypoint1[plexilCustomData.nextFeasibleWP];
            plexilCustomData.directPathToFeasibleWP &= plexilCustomData.trkBands.wpFeasibility1[plexilCustomData.nextMissionWP];
            plexilCustomData.directPathToFeasibleWP &= plexilCustomData.gsBands.wpFeasibility1[plexilCustomData.nextMissionWP];

            break;
        }

        case ICAROUS_BANDS_TRACK_MID:{
            bands_t* trk = (bands_t*) plexilAppData.DATA_MsgPtr;

            memcpy(&plexilCustomData.trkBands,trk,sizeof(bands_t));

            if(trk->currentConflictBand == 1){
               plexilCustomData.trafficTrackConflict = true;
               if(trk->resPreferred >= 0){
                   plexilCustomData.preferredTrack = trk->resPreferred;
               }
               else{
                   plexilCustomData.preferredTrack = -1000;
               }
            }else{
               plexilCustomData.trafficTrackConflict = false;
               plexilCustomData.preferredTrack = -10000;
            }

            break;
        }

        case ICAROUS_BANDS_SPEED_MID:{
            bands_t* gs = (bands_t*) plexilAppData.DATA_MsgPtr;

            memcpy(&plexilCustomData.gsBands,gs,sizeof(bands_t));

            if(gs->currentConflictBand == 1){
               plexilCustomData.trafficSpeedConflict = true;
               if(!isinf(gs->resPreferred))
                   plexilCustomData.preferredSpeed = gs->resPreferred;
               else
                   plexilCustomData.preferredSpeed = -10000;
            }else{
               int id = plexilCustomData.nextMissionWP;
               if (id >= plexilCustomData.totalMissionWP)
                   id = plexilCustomData.totalMissionWP-1;

               if(gs->wpFeasibility1[id]) {
                   plexilCustomData.trafficSpeedConflict = false;
               }
               else {
                   plexilCustomData.trafficSpeedConflict = true;
               }
               plexilCustomData.preferredSpeed = -1000;
            }

            break;
        }

        case ICAROUS_BANDS_ALT_MID:{
            bands_t* alt = (bands_t*) plexilAppData.DATA_MsgPtr;

            memcpy(&plexilCustomData.altBands,alt,sizeof(bands_t));
            if(alt->currentConflictBand == 1){
               plexilCustomData.trafficAltConflict = true;
               if(!isinf(alt->resPreferred))
                   plexilCustomData.preferredAlt = alt->resPreferred;
               else
                   plexilCustomData.preferredAlt = -10000;
            }else{
               plexilCustomData.trafficAltConflict = false;
               plexilCustomData.preferredAlt = -1000;
            }

            break;

        }

        case FLIGHTPLAN_MONITOR_MID:{
            flightplan_monitor_t *fpm = (flightplan_monitor_t *)plexilAppData.DATA_MsgPtr;
            if (strcmp(fpm->planID, "Plan0") == 0)
            {
                if (fabs(fpm->crossTrackDeviation) > fpm->allowedXtrackError)
                {
                    plexilCustomData.flightPlanConflict = true;
                }
                else
                {
                    plexilCustomData.flightPlanConflict = false;
                }
                plexilCustomData.resolutionSpeed = fpm->resolutionSpeed;
                plexilCustomData.allowedXtrackError = fpm->allowedXtrackError;
                memcpy(plexilCustomData.interceptManeuver, fpm->interceptManeuver, sizeof(double) * 3);
                plexilCustomData.interceptHeadingToPlan = fpm->interceptHeadingToPlan;
                plexilCustomData.xtrackError = fabs(fpm->crossTrackDeviation);
                plexilCustomData.searchType = fpm->searchType;
            }
            break;
        }

        case GEOFENCE_PATH_CHECK_RESULT_MID:{
            pathFeasibilityCheck_t *fpresult = (pathFeasibilityCheck_t*) plexilAppData.DATA_MsgPtr;

            bool result  = fpresult->feasibleAB;
            // If starting location is not feasible but ending location is feasible,
            // lets try to get out of the infeasible starting location
            if(!fpresult-> feasibleA && fpresult->feasibleB){
                result = true;
            }

            PlexilMsg plxInput;
            PlexilMsg* msg = &plexilCustomData.pendingRequest;
            if(CHECKNAME(msg,"CheckDirectPathFeasibility")){
                    memcpy(&plxInput,msg,sizeof(PlexilMsg));
            }else{
                OS_printf("plexil: No matching requests found:%s\n",msg->name);
            }

            serializeBool(false,result,plxInput.buffer);
            plexil_return(plexilAppData.adap,&plxInput);
            break;

        }

        case SAFE2DITCH_STATUS_MID:{
            safe2ditchStatus_t *s2dstatus = (safe2ditchStatus_t*) plexilAppData.DATA_MsgPtr;
            plexilCustomData.ditchsite[0] = s2dstatus->ditchsite[0];
            plexilCustomData.ditchsite[1] = s2dstatus->ditchsite[1];
            plexilCustomData.ditchsite[2] = s2dstatus->ditchsite[2];
            plexilCustomData.ditchGuidanceRequired = s2dstatus->ditchGuidanceRequired;
            plexilCustomData.ditch = s2dstatus->ditchRequested;
            plexilCustomData.resetDitch = s2dstatus->resetDitch;
            plexilCustomData.endDitch = s2dstatus->endDitch;
            break;
        }

        case TRAFFIC_PARAMETERS_MID:{
            traffic_parameters_t* msg = (traffic_parameters_t*) plexilAppData.DATA_MsgPtr;
            memcpy(&plexilCustomData.trafficparameters,msg,sizeof(traffic_parameters_t));
            break;
        }

        case ICAROUS_RESET_MID:{
            plexilCustomData.restartMission = true;
            break;
        }

        default:
            break;
    }

}

void PLEXIL_HandleCustomLookups(PlexilMsg *msg){

    PlexilMsg plxInput;
    plxInput.mType = _LOOKUP_RETURN_;
    plxInput.id = msg->id;
    memcpy(plxInput.name,msg->name,50);
    char* b = plxInput.buffer;

    if(CHECKNAME(msg ,"missionStart")){
        b = serializeInt(false, plexilCustomData.missionStart, b);
        plexilCustomData.missionStart = -1;
    }else if(CHECKNAME(msg ,"armStatus")){
        b = serializeInt(false, plexilCustomData.armStatus, b);
        plexilCustomData.armStatus = -1;
    }else if(CHECKNAME(msg ,"takeoffStatus")){
        b = serializeInt(false, plexilCustomData.takeoffStatus, b);
        plexilCustomData.takeoffStatus = -1;
    }else if(CHECKNAME(msg ,"position")){
        b = serializeRealArray(3, plexilCustomData.position, b);
    }else if(CHECKNAME(msg ,"velocity")){
        b = serializeRealArray(3, plexilCustomData.velocity, b);
    }else if(CHECKNAME(msg ,"numMissionWP")){
        b = serializeInt(false, plexilCustomData.totalMissionWP, b);
    }else if(CHECKNAME(msg ,"nextMissionWPIndex")){
        b = serializeInt(false, plexilCustomData.nextMissionWP, b);
    }else if(CHECKNAME(msg ,"fenceConflict")){
        bool conflict[2];
        conflict[0] = plexilCustomData.keepInConflict;
        conflict[1] = plexilCustomData.keepOutConflict;
        b = serializeBoolArray(2,conflict,b);
    }else if(CHECKNAME(msg ,"trafficConflict")){
        if((plexilCustomData.trafficTrackConflict || plexilCustomData.trafficSpeedConflict) || plexilCustomData.trafficAltConflict){
            plexilCustomData.trafficConflict = true;
        }else{
            plexilCustomData.trafficConflict = false;
        }
        b = serializeBool(false,plexilCustomData.trafficConflict,b);
    }else if(CHECKNAME(msg ,"trafficSpeedConflict")){
        b = serializeBool(false,plexilCustomData.trafficSpeedConflict,b);
    }else if(CHECKNAME(msg ,"trafficAltConflict")){
        b = serializeBool(false,plexilCustomData.trafficAltConflict,b);
    }else if(CHECKNAME(msg ,"trafficTrackConflict")){
        b = serializeBool(false,plexilCustomData.trafficTrackConflict,b);
    }else if(CHECKNAME(msg ,"flightPlanConflict")){
        b = serializeBool(false,plexilCustomData.flightPlanConflict,b);
    }else if(CHECKNAME(msg ,"totalFences")){
        b = serializeInt(false,plexilCustomData.totalFences,b);
    }else if(CHECKNAME(msg ,"totalTraffic")){
        b = serializeInt(false,plexilCustomData.totalTraffic,b);
    }else if(CHECKNAME(msg ,"allowedXtrackDev")){
        b = serializeReal(false,plexilCustomData.allowedXtrackError,b);
    }else if(CHECKNAME(msg ,"xtrackDev")){
        b = serializeReal(false,plexilCustomData.xtrackError,b);
    }else if(CHECKNAME(msg ,"resolutionSpeed")){
        b = serializeReal(false,plexilCustomData.resolutionSpeed,b);
    }else if(CHECKNAME(msg ,"ditchingStatus")){
        b = serializeBool(false,plexilCustomData.ditch,b);
    }else if(CHECKNAME(msg ,"GFRecoveryPosition")){
        b = serializeRealArray(3,plexilCustomData.recoveryPosition,b);
    }else if(CHECKNAME(msg ,"searchAlgorithm")){
        if(plexilCustomData.searchType == _GRID){
            char name[6]="GRID";
            b = serializeString(4,name,b);
        }else if(plexilCustomData.searchType == _ASTAR){
            char name[6]="ASTAR";
            b = serializeString(5,name,b);
        }else if(plexilCustomData.searchType == _RRT){
            char name[6]="RRT";
            b = serializeString(3,name,b);
        }else if(plexilCustomData.searchType == _SPLINES){
            char name[8]="SPLINES";
            b = serializeString(7,name,b);
        }
    }else if(CHECKNAME(msg ,"nextFeasibleWP")){
        b = serializeInt(false,plexilCustomData.nextFeasibleWP,b);
    }else if(CHECKNAME(msg ,"directPathToNextFeasibleWP")){
        b = serializeBool(false,plexilCustomData.directPathToFeasibleWP,b);
    }else if(CHECKNAME(msg ,"totalResolutionWP")){
        b = serializeInt(false,plexilCustomData.resolutionTraj.num_waypoints,b);
    }else if(CHECKNAME(msg ,"interceptManeuver")){
        b = serializeRealArray(3,plexilCustomData.interceptManeuver,b);
    }else if(CHECKNAME(msg ,"interceptHeading")){
        b = serializeReal(false,plexilCustomData.interceptHeadingToPlan,b);
    }else if(CHECKNAME(msg ,"trafficResType")){
        b = serializeInt(false,plexilCustomData.trafficResType,b);
    }else if(CHECKNAME(msg ,"preferredTrack")){
        b = serializeReal(false,plexilCustomData.preferredTrack,b);
    }else if(CHECKNAME(msg ,"preferredSpeed")){
        b = serializeReal(false,plexilCustomData.preferredSpeed,b);
    }else if(CHECKNAME(msg ,"preferredAltitude")){
        b = serializeReal(false,plexilCustomData.preferredAlt,b);
    }else if(CHECKNAME(msg ,"requireDitchGuidance")){
        b = serializeBool(false,plexilCustomData.ditchGuidanceRequired,b);
    }else if(CHECKNAME(msg ,"ditchingComplete")){
        b = serializeBool(false,plexilCustomData.endDitch,b);
    }else if(CHECKNAME(msg ,"resetDitching")){
        b = serializeBool(false,plexilCustomData.resetDitch,b);
    }else if(CHECKNAME(msg ,"ditchSite")){
        b = serializeRealArray(3,plexilCustomData.ditchsite,b);
    }else if(CHECKNAME(msg ,"restartMission")){
        b = serializeBool(false,plexilCustomData.restartMission,b);
    }else if(CHECKNAME(msg ,"resolutionType")){
        b = serializeInt(false,plexilCustomData.trafficparameters.resType,b);
    }else{
       //TODO: add event saying lookup not handled
        OS_printf("lookup not handled: %s\n",msg->name);
    }

    plexil_return(plexilAppData.adap, &plxInput);

    //plexil_BoolValueChange(plexilAppData.adap,"restartMission",&plexilCustomData.restartMission);
}


void PLEXIL_HandleCustomCommands(PlexilMsg* msg){
    PlexilMsg plxInput;
    plxInput.mType = _COMMAND_RETURN_;
    plxInput.id = msg->id;
    memcpy(plxInput.name,msg->name,50);
    const char* b = msg->buffer;


    argsCmd_t cmd;
    CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID, sizeof(argsCmd_t),TRUE);

    if(CHECKNAME(msg ,"ArmMotors")){
        cmd.name = _ARM_;
        cmd.param1 = 1;
        //SendSBMsg(cmd);
    }else if(CHECKNAME(msg ,"Status")){
        status_t plxstatus;
        CFE_SB_InitMsg(&plxstatus,ICAROUS_STATUS_MID,sizeof(status_t),TRUE);
        int severity;
        memset(plxstatus.buffer,0,50);
        b = deSerializeString(plxstatus.buffer,b);
        b = deSerializeInt(false,&plxstatus.severity,b);
        SendSBMsg(plxstatus);
    }else if(CHECKNAME(msg ,"Takeoff")){
        cmd.name = TAKEOFF;
        double takeoffAlt;
        b = deSerializeReal(false,&takeoffAlt,b);
        cmd.param1 = (float)takeoffAlt;
        SendSBMsg(cmd);
    }else if(CHECKNAME(msg ,"SetMode")){

        b = deSerializeString(cmd.buffer,b);
        bool passive = (strcmp(cmd.buffer, "PASSIVE") == 0)?true:false;

        if(passive){
            cmd.name = PRIMARY_FLIGHTPLAN;
            cmd.param1 = plexilCustomData.nextMissionWP;
            SendSBMsg(cmd);
        }

        const bool val = true;
        serializeBool(false,val,plxInput.buffer);
        plexil_return(plexilAppData.adap, &plxInput);

    }else if(CHECKNAME(msg ,"Land")){
        cmd.name = _LAND_;
        //SendSBMsg(cmd);
    }else if(CHECKNAME(msg ,"SetNextMissionWP")){
        int32_t tempSeq;
        b = deSerializeInt(false,&tempSeq,b);
        //SendSBMsg(cmd);
        plexilCustomData.nextMissionWP = tempSeq;

        missionItemReached_t itemReached;
        CFE_SB_InitMsg(&itemReached, ICAROUS_WPREACHED_MID, sizeof(itemReached), TRUE);
        itemReached.feedback = false;
        strcpy(itemReached.planID, "Plan0");
        itemReached.reachedwaypoint = plexilCustomData.nextMissionWP-1;
        SendSBMsg(itemReached);

        const bool val = true;
        serializeBool(false,val,plxInput.buffer);
        plexil_return(plexilAppData.adap, &plxInput);

    }else if(CHECKNAME(msg ,"SetPos")){
        double _position[3];
        b = deSerializeRealArray(_position,b);
        cmd.name = POINT2POINT;
        cmd.param1 = (float)_position[0];
        cmd.param2 = (float)_position[1];
        cmd.param3 = (float)_position[2];
        cmd.param4 = plexilCustomData.resolutionSpeed;
        SendSBMsg(cmd);
    }else if(CHECKNAME(msg ,"SetVel")){
        double _velocity[3];
        b = deSerializeRealArray(_velocity,b);

        double track = _velocity[0];
        double groundspeed =_velocity[1];
        double verticalspeed = _velocity[2];

        double vn = groundspeed*cos(track*M_PI/180);
        double ve = groundspeed*sin(track*M_PI/180);
        double vd = verticalspeed;

        bool valid = true;
        if(groundspeed > plexilCustomData.trafficparameters.max_gs){
            groundspeed = plexilCustomData.trafficparameters.max_gs * 0.514; // Convert from knots to m/s
        }

        if(groundspeed < plexilCustomData.trafficparameters.min_gs){
            groundspeed = plexilCustomData.trafficparameters.min_gs * 0.514; // Convert from knots to m/s
        }

        if(verticalspeed > plexilCustomData.trafficparameters.max_vs*0.3/60){ // Convert from fpm to m/s
            verticalspeed = plexilCustomData.trafficparameters.max_vs*0.3/60;
        } 

        if(verticalspeed < plexilCustomData.trafficparameters.min_vs*0.3/60){ // Convert from fpm to m/s
            verticalspeed = plexilCustomData.trafficparameters.min_vs*0.3/60;
        }

        //if(valid)
        {
            cmd.name = VECTOR;
            cmd.param1 = (float)vn;
            cmd.param2 = (float)ve;
            cmd.param3 = (float)vd;
            SendSBMsg(cmd);
        }
    }else if(CHECKNAME(msg ,"SetYaw")){
        double heading;
        int32_t relative;
        b = deSerializeReal(false,&heading,b);
        b = deSerializeInt(false,&relative,b);
        cmd.name = _SETYAW_;
        cmd.param1 = (float)heading;
        cmd.param2 = 0;
        cmd.param3 = 1;
        cmd.param4 = (float)relative;
        //SendSBMsg(cmd);
    }else if(CHECKNAME(msg ,"SetSpeed")){
        double speed;
        deSerializeReal(false,&speed,b);
        cmd.name = _SETSPEED_;
        cmd.param1 = (float)speed;
        //SendSBMsg(cmd);
    }else if(CHECKNAME(msg ,"ComputeDistance")){
        double positionA[3];
        double positionB[3];
        double distance;

        b = deSerializeRealArray(positionA,b);
        b = deSerializeRealArray(positionB,b);

        distance = ComputeDistance(positionA,positionB);

        serializeReal(false,distance,plxInput.buffer);
        plexil_return(plexilAppData.adap,&plxInput);

    }else if(CHECKNAME(msg ,"CheckDirectPathFeasibility")){
        double positionA[3];
        double positionB[3];
        double distance;

        b = deSerializeRealArray(positionA,b);
        b = deSerializeRealArray(positionB,b);

        pathFeasibilityCheck_t pfcheck;
        CFE_SB_InitMsg(&pfcheck,GEOFENCE_PATH_CHECK_MID,sizeof(pathFeasibilityCheck_t),TRUE);
        memcpy(pfcheck.fromPosition,positionA, sizeof(double)*3);
        memcpy(pfcheck.toPosition,positionB, sizeof(double)*3);
        memcpy(&plexilCustomData.pendingRequest,&plxInput,sizeof(PlexilMsg));
        SendSBMsg(pfcheck);

    }else if(CHECKNAME(msg ,"GetResolutionWP")){
        int nrWP;

        deSerializeInt(false,&nrWP,b);

        double waypoint[3] = {plexilCustomData.resolutionTraj.waypoints[nrWP].latitude,
                              plexilCustomData.resolutionTraj.waypoints[nrWP].longitude,
                              plexilCustomData.resolutionTraj.waypoints[nrWP].altitude};

        serializeRealArray(3,waypoint,plxInput.buffer);
        plexil_return(plexilAppData.adap,&plxInput);

    }else if(CHECKNAME(msg ,"GetMissionWP")){
        int nmWP;

        deSerializeInt(false,&nmWP,b);

        double waypoint[3] = {plexilCustomData.missionFP.waypoints[nmWP].latitude,
                              plexilCustomData.missionFP.waypoints[nmWP].longitude,
                              plexilCustomData.missionFP.waypoints[nmWP].altitude};

        serializeRealArray(3,waypoint,plxInput.buffer);
        plexil_return(plexilAppData.adap,&plxInput);

    }else if(CHECKNAME(msg ,"CheckSafeToTurn")){
        double fromHeading;
        double toHeading;
        b = deSerializeReal(false,&fromHeading,b);
        b = deSerializeReal(false,&toHeading,b);

        bool conflict = false;
        for(int i=0;i<plexilCustomData.trkBands.numBands;i++){
                bool val;
                val = CheckTurnConflict(plexilCustomData.trkBands.min[i],
                                            plexilCustomData.trkBands.max[i],
                                            fromHeading,toHeading);
                conflict |= val;
        }

        conflict |= !plexilCustomData.trkBands.wpFeasibility1[plexilCustomData.nextMissionWP];

        serializeBool(false,!conflict,plxInput.buffer);
        plexil_return(plexilAppData.adap,&plxInput);

    }else if(CHECKNAME(msg ,"FindNewPath")){

        char algName[10];
        double positionA[3];
        double velocityA[3];
        double positionB[3];
        memset(algName,0,10);
        b = deSerializeString(algName,b);
        b = deSerializeRealArray(positionA,b);
        b = deSerializeRealArray(velocityA,b);
        b = deSerializeRealArray(positionB,b);

        trajectory_request_t pathRequest;
        CFE_SB_InitMsg(&pathRequest,ICAROUS_TRAJECTORY_REQUEST_MID,sizeof(trajectory_request_t),TRUE);

        if(!strcmp(algName,"ASTAR"))
            pathRequest.algorithm = _ASTAR;
        else if(!strcmp(algName,"RRT"))
            pathRequest.algorithm = _RRT;
        else if(!strcmp(algName,"GRID"))
            pathRequest.algorithm = _GRID;
        else{
            OS_printf("Algorithm %s not available",algName);
            pathRequest.algorithm = _ASTAR;
        }

        memcpy(pathRequest.initialPosition,positionA,sizeof(double)*3);
        memcpy(pathRequest.initialVelocity,velocityA,sizeof(double)*3);
        memcpy(pathRequest.finalPosition,positionB,sizeof(double)*3);

        memcpy(&plexilCustomData.pendingRequest,&plxInput,sizeof(plxInput));
        SendSBMsg(pathRequest);
    }else{
       //TODO: add event saying command not handled
        OS_printf("command not handled %s\n",msg->name);
    }

}

