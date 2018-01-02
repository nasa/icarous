//
// Created by swee on 1/1/18.
//
#include <Plexil_msg.h>
#include "trajectory.h"

void TrajPlxMsgHandler(plexil_interface_t* msg){
    char* b = msg->plxData.buffer;

    if(msg->plxData.mType == _COMMAND_) {
        trajPlexilMsg.plxData.id = msg->plxData.id;
        trajPlexilMsg.plxData.mType = _COMMAND_RETURN_;
        if (CHECK_NAME(msg->plxData, "GetWaypoint")) {
            char planID[10];
            int index;
            double waypoint[3];
            b = deSerializeString(planID, b);
            b = deSerializeInt(false, &index, b);
            PathPlanner_GetWaypoint(TrajectoryAppData.pplanner, planID, index, waypoint);
            serializeRealArray(3, waypoint, trajPlexilMsg.plxData.buffer);
            SendSBMsg(trajPlexilMsg);
        } else if (CHECK_NAME(msg->plxData, "totalWP")) {


        } else if (CHECK_NAME(msg->plxData, "FindNewPath")) {
            char planID[10];
            char algorithmID[10];
            memset(planID, 0, 10);
            memset(algorithmID, 0, 10);
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

            int n;
            n = PathPlanner_FindPath(TrajectoryAppData.pplanner, algType, planID, fromPosition, toPosition, fromVelocity);
            serializeInt(false,n,trajPlexilMsg.plxData.buffer);
            SendSBMsg(trajPlexilMsg);

        } else if (CHECK_NAME(msg->plxData, "ComputeCrossTrackDeviation")) {

            char planID[10];
            int leg;
            double position[3];
            double offsets[2];
            double xtrackdist;

            memset(planID, 0, 10);
            b = deSerializeString(planID, b);
            b = deSerializeInt(false, &leg, b);
            b = deSerializeRealArray(position, b);
            b = deSerializeRealArray(offsets, b);

            xtrackdist = PathPlanner_ComputeXtrackDistance_c(TrajectoryAppData.pplanner, planID, leg, position,
                                                             offsets);
            serializeReal(false,xtrackdist,trajPlexilMsg.plxData.buffer);
            SendSBMsg(trajPlexilMsg);
        }
    }else{




    }

}