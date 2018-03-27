//
// Created by Swee Balachandran on 11/21/17.
//
#define EXTERN extern

#include <Icarous_msg.h>
#include "plexil.h"
#include "msgids/msgids.h"
#include "stdbool.h"

void PLEXIL_DistributeMessage(PlexilMsg msg){

    CFE_SB_MsgId_t  MsgId;

    if(CHECKNAME(msg,"MonitorTraffic") ||
       CHECKNAME(msg,"CheckSafeToTurn")){
        MsgId = SERVICE_TRAFFIC_MID;
    }

    else if (CHECKNAME(msg,"GetWaypoint") ||
             CHECKNAME(msg,"GetTotalWaypoints") ||
             CHECKNAME(msg,"FindNewPath") ||
             CHECKNAME(msg,"ComputeCrossTrackDeviation") ||
             CHECKNAME(msg,"ComputeDistance") ||
             CHECKNAME(msg,"GetExitPoint") ||
             CHECKNAME(msg,"GetInterceptHeadingToPlan") ||
             CHECKNAME(msg,"ManeuverToIntercept") ||
             CHECKNAME(msg,"allowedXtrackDev") ||
             CHECKNAME(msg,"resolutionSpeed") ||
             CHECKNAME(msg,"totalFences") ||
             CHECKNAME(msg,"totalTraffic") ||
             CHECKNAME(msg,"trafficResType")){
        MsgId = SERVICE_TRAJECTORY_MID;
    }

    else if( CHECKNAME(msg ,"CheckFenceViolation") ||
             CHECKNAME(msg ,"CheckDirectPathFeasibility") ||
             CHECKNAME(msg ,"GetRecoveryPosition")){
        MsgId = SERVICE_GEOFENCE_MID;
    }

    else if(CHECKNAME(msg ,"missionStart") ||
            CHECKNAME(msg ,"armStatus") ||
            CHECKNAME(msg ,"takeoffStatus") ||
            CHECKNAME(msg ,"position") ||
            CHECKNAME(msg ,"velocity") ||
            CHECKNAME(msg ,"numMissionWP") ||
            CHECKNAME(msg ,"nextMissionWPIndex") ||
            CHECKNAME(msg ,"ArmMotors") ||
            CHECKNAME(msg ,"Takeoff") ||
            CHECKNAME(msg ,"SetMode") ||
            CHECKNAME(msg ,"Land") ||
            CHECKNAME(msg ,"SetNextMissionWP")  ||
            CHECKNAME(msg ,"SetPos") ||
            CHECKNAME(msg ,"SetVel") ||
            CHECKNAME(msg ,"SetYaw")  ||
            CHECKNAME(msg ,"SetSpeed")){
        MsgId = SERVICE_INTERFACE_MID;
        //OS_printf("interface service request\n");
    } else if(CHECKNAME(msg,"requireDitchGuidance") ||
              CHECKNAME(msg,"ditchSite") ||
              CHECKNAME(msg,"ditchingStatus") ||
              CHECKNAME(msg,"ditchingComplete") ||
              CHECKNAME(msg,"resetDitching")) {
        MsgId = SERVICE_DITCH_MID;
    }else{
        OS_printf(" '%s' message not distributed:",msg.name);
    }

    service_t plexilRequestMsg;
    CFE_SB_InitMsg(&plexilRequestMsg, MsgId, sizeof(service_t), TRUE);
    plexilRequestMsg.sType = (servictType_t)msg.mType;
    plexilRequestMsg.id = msg.id;
    memcpy(plexilRequestMsg.name,msg.name,50);
    memcpy(plexilRequestMsg.buffer,msg.buffer,250);
    SendSBMsg(plexilRequestMsg);
}
