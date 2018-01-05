//
// Created by Swee Balachandran on 11/21/17.
//
#define EXTERN extern

#include <Plexil_msg.h>
#include "plexil.h"
#include "msgids/msgids.h"
#include "stdbool.h"

void PLEXIL_DistributeMessage(PlexilMsg msg){

    CFE_SB_MsgId_t  MsgId;

    if(CHECK_NAME(msg,"MonitorTraffic") ||
       CHECK_NAME(msg ,"CheckSafeToTurn")){
        MsgId = PLEXIL_OUTPUT_TRAFFIC_MID;
    }

    else if (CHECK_NAME(msg , "GetWaypoint") ||
             CHECK_NAME(msg , "GetTotalWaypoints") ||
             CHECK_NAME(msg , "FindNewPath") ||
             CHECK_NAME(msg , "ComputeCrossTrackDeviation") ||
             CHECK_NAME(msg , "ComputeDistance") ||
             CHECK_NAME(msg , "GetExitPoint") ||
             CHECK_NAME(msg ,"GetInterceptHeadingToPlan") ||
             CHECK_NAME(msg ,"ManeuverToIntercept") ||
             CHECK_NAME(msg ,"allowedXtrackDev")){
        MsgId = PLEXIL_OUTPUT_TRAJECTORY_MID;
    }

    else if( CHECK_NAME(msg ,"CheckFenceViolation") ||
             CHECK_NAME(msg ,"CheckDirectPathFeasibility") ||
             CHECK_NAME(msg ,"GetRecoveryPosition")){
        MsgId = PLEXIL_OUTPUT_GEOFENCE_MID;
    }

    else if(CHECK_NAME(msg , "missionStart") ||
            CHECK_NAME(msg , "armStatus") ||
            CHECK_NAME(msg , "takeoffStatus") ||
            CHECK_NAME(msg , "position") ||
            CHECK_NAME(msg , "velocity") ||
            CHECK_NAME(msg , "numMissionWP") ||
            CHECK_NAME(msg , "nextMissionWPIndex") ||
            CHECK_NAME(msg , "ArmMotors") ||
            CHECK_NAME(msg , "Takeoff") ||
            CHECK_NAME(msg , "SetMode") ||
            CHECK_NAME(msg , "Land") ||
            CHECK_NAME(msg , "SetNextMissionWP")  ||
            CHECK_NAME(msg , "SetPos") ||
            CHECK_NAME(msg , "SetVel") ||
            CHECK_NAME(msg ,"SetYaw")  ||
            CHECK_NAME(msg , "SetSpeed")){
        MsgId = PLEXIL_OUTPUT_INTERFACE_MID;
    } else{
        OS_printf(" '%s' message not distributed:",msg.name);
    }

    plexil_interface_t plexilDistributionMsg;
    CFE_SB_InitMsg(&plexilDistributionMsg, MsgId, sizeof(plexil_interface_t), TRUE);
    memcpy(&plexilDistributionMsg.plxData, &msg, sizeof(PlexilMsg));
    SendSBMsg(plexilDistributionMsg);
}
