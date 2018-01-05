//
// Created by swee on 1/1/18.
//
#include <Plexil_msg.h>
#include <cfs-data-format.hh>
#include "trajectory.h"

void TrajPlxMsgHandler(plexil_interface_t* msg){
    char* b = msg->plxData.buffer;

    if(msg->plxData.mType == _COMMAND_) {
        trajPlexilMsg.plxData.id = msg->plxData.id;
        trajPlexilMsg.plxData.mType = _COMMAND_RETURN_;
        if (CHECK_NAME(msg->plxData, "GetWaypoint")) {
            char planID[10]={0};
            int index;
            double waypoint[3];
            b = deSerializeString(planID, b);
            b = deSerializeInt(false, &index, b);
            PathPlanner_GetWaypoint(TrajectoryAppData.pplanner, planID, index, waypoint);
            //OS_printf("wp %d:%f,%f\n",index,waypoint[0],waypoint[1]);
            serializeRealArray(3, waypoint, trajPlexilMsg.plxData.buffer);
            SendSBMsg(trajPlexilMsg);
        } else if (CHECK_NAME(msg->plxData, "GetTotalWaypoints")) {
            char planID[10]={0};
            int nWP;
            b = deSerializeString(planID,b);
            nWP = PathPlanner_GetTotalWaypoints(TrajectoryAppData.pplanner,planID);
            OS_printf("Total waypoints %d\n",nWP);
            serializeInt(false,nWP,trajPlexilMsg.plxData.buffer);
            SendSBMsg(trajPlexilMsg);
        } else if (CHECK_NAME(msg->plxData, "FindNewPath")) {
            char planID[10]={0};
            char algorithmID[10] = {0};
            double fromPosition[3];
            double fromVelocity[3];
            double toPosition[3];
            b = deSerializeString(planID, b);
            b = deSerializeString(algorithmID, b);
            b = deSerializeRealArray(fromPosition, b);
            b = deSerializeRealArray(fromVelocity, b);
            b = deSerializeRealArray(toPosition, b);

            int algType;
            if (!strcmp(algorithmID, "ASTAR")) {
                algType = 0;
            } else if (!strcmp(algorithmID, "RRT")) {
                algType = 1;
            }
            OS_printf("computing path %s using alg: %s\n",planID,algorithmID);

            int32_t n;
            n = PathPlanner_FindPath(TrajectoryAppData.pplanner, algType, planID, fromPosition, toPosition, fromVelocity);

            //OS_printf("solution status %d\n",n);
            serializeInt(false,n,trajPlexilMsg.plxData.buffer);
            SendSBMsg(trajPlexilMsg);

        } else if (CHECK_NAME(msg->plxData, "ComputeCrossTrackDeviation")) {
            char planID[10]={0};
            int leg;
            double position[3];
            double offsets[2];
            double xtrackdist;

            b = deSerializeString(planID, b);
            b = deSerializeInt(false, &leg, b);
            b = deSerializeRealArray(position, b);

            xtrackdist = PathPlanner_ComputeXtrackDistance_c(TrajectoryAppData.pplanner, planID, leg, position,
                                                             offsets);
            serializeReal(false,xtrackdist,trajPlexilMsg.plxData.buffer);
            SendSBMsg(trajPlexilMsg);
        } else if(CHECK_NAME(msg->plxData, "ComputeDistance")){
            double positionA[3];
            double positionB[3];
            b = deSerializeRealArray(positionA,b);
            b = deSerializeRealArray(positionB,b);
            double distance = PathPlanner_Dist2Waypoint(TrajectoryAppData.pplanner,positionA,positionB);

            serializeReal(false,distance,trajPlexilMsg.plxData.buffer);
            SendSBMsg(trajPlexilMsg);
        } else if(CHECK_NAME(msg->plxData, "GetExitPoint")){

            double exitPosition[3];
            double currPosition[3];
            char planID[10] = {0};
            int32_t nextWP;

            b = deSerializeString(planID,b);
            b = deSerializeRealArray(currPosition,b);
            b = deSerializeInt(false,&nextWP,b);
            PathPlanner_GetExitPoint(TrajectoryAppData.pplanner,planID,currPosition,nextWP,exitPosition);

            serializeRealArray(3,exitPosition,trajPlexilMsg.plxData.buffer);
            SendSBMsg(trajPlexilMsg);
        } else if(CHECK_NAME(msg->plxData,"GetInterceptHeadingToPlan")){

            char planID[10] = {0};
            int nextWP;
            double position[3];

            b = deSerializeString(planID,b);
            b = deSerializeInt(false,&nextWP,b);
            b = deSerializeRealArray(position,b);

            //OS_printf("Get intercept to plan %s,at %f,%f,%f, to %d\n",planID,position[0],position[1],position[2],nextWP);

            double track = PathPlanner_GetInterceptHeadingToPlan_c(TrajectoryAppData.pplanner,planID,nextWP,position);
            serializeReal(false,track,trajPlexilMsg.plxData.buffer);
            SendSBMsg(trajPlexilMsg);
        } else if(CHECK_NAME(msg->plxData,"ManeuverToIntercept")){

            char planID[10] = {0};
            int nextWP;
            double position[3];
            double cmdVelocity[3];

            b = deSerializeString(planID,b);
            b = deSerializeInt(false,&nextWP,b);
            b = deSerializeRealArray(position,b);

            PathPlanner_ManueverToIntercept_c(TrajectoryAppData.pplanner,planID,nextWP,position,cmdVelocity);
            serializeRealArray(3,cmdVelocity,trajPlexilMsg.plxData.buffer);
            SendSBMsg(trajPlexilMsg);
        }
    }else{
        trajPlexilMsg.plxData.id = msg->plxData.id;
        trajPlexilMsg.plxData.mType = _LOOKUP_RETURN_;
        if(CHECK_NAME(msg->plxData,"allowedXtrackDev")){
            double val;
            val = FlightData_GetAllowedXtracDeviation(TrajectoryAppData.fdata);
            serializeReal(false,val,trajPlexilMsg.plxData.buffer);
            SendSBMsg(trajPlexilMsg);
        }

    }

}