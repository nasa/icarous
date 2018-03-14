//
// Created by swee on 1/1/18.
//
#include "trajectory.h"

void TrajServiceHandler(service_t* msg){
    const char* b = msg->buffer;

    if(msg->sType == _command_) {
        trajServiceResponse.id = msg->id;
        trajServiceResponse.sType = _command_return_;
        if (CHECKNAME((*msg), "GetWaypoint")) {
            char planID[10]={0};
            int index;
            double waypoint[3];
            b = deSerializeString(planID, b);
            b = deSerializeInt(false, &index, b);
            PathPlanner_GetWaypoint(TrajectoryAppData.pplanner, planID, index, waypoint);
            //OS_printf("wp %d:%f,%f\n",index,waypoint[0],waypoint[1]);
            serializeRealArray(3, waypoint, trajServiceResponse.buffer);
            SendSBMsg(trajServiceResponse);
        } else if (CHECKNAME((*msg), "GetTotalWaypoints")) {
            char planID[10]={0};
            int nWP;
            b = deSerializeString(planID,b);
            nWP = PathPlanner_GetTotalWaypoints(TrajectoryAppData.pplanner,planID);
            //OS_printf("Total waypoints %d\n",nWP);
            serializeInt(false,nWP,trajServiceResponse.buffer);
            SendSBMsg(trajServiceResponse);
        } else if (CHECKNAME((*msg), "FindNewPath")) {
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
            OS_printf("Trajectory: Computing path %s using alg: %s\n",planID,algorithmID);

            int32_t n;
            n = PathPlanner_FindPath(TrajectoryAppData.pplanner, algType, planID, fromPosition, toPosition, fromVelocity);

            //OS_printf("solution status %d\n",n);
            serializeInt(false,n,trajServiceResponse.buffer);
            SendSBMsg(trajServiceResponse);

        } else if (CHECKNAME((*msg), "ComputeCrossTrackDeviation")) {
            char planID[10]={0};
            int leg;
            double position[3];
            double offsets[2];
            double xtrackdist;

            b = deSerializeString(planID, b);
            b = deSerializeInt(false, &leg, b);
            b = deSerializeRealArray(position, b);

            //OS_printf("checking xtrack deviation from %s to leg %d at %f,%f,%f\n",planID,leg,position[0],position[1],position[2]);

            xtrackdist = PathPlanner_ComputeXtrackDistance_c(TrajectoryAppData.pplanner, planID, leg, position,
                                                             offsets);
            serializeReal(false,xtrackdist,trajServiceResponse.buffer);
            SendSBMsg(trajServiceResponse);
            //OS_printf("xtrack %f\n",xtrackdist);

        } else if(CHECKNAME((*msg), "ComputeDistance")){
            double positionA[3];
            double positionB[3];
            b = deSerializeRealArray(positionA,b);
            b = deSerializeRealArray(positionB,b);
            double distance = PathPlanner_Dist2Waypoint(TrajectoryAppData.pplanner,positionA,positionB);

            serializeReal(false,distance,trajServiceResponse.buffer);
            SendSBMsg(trajServiceResponse);
        } else if(CHECKNAME((*msg), "GetExitPoint")){

            double exitPosition[3];
            double currPosition[3];
            char planID[10] = {0};
            int32_t nextWP;

            b = deSerializeString(planID,b);
            b = deSerializeRealArray(currPosition,b);
            b = deSerializeInt(false,&nextWP,b);
            PathPlanner_GetExitPoint(TrajectoryAppData.pplanner,planID,currPosition,nextWP,exitPosition);

            serializeRealArray(3,exitPosition,trajServiceResponse.buffer);
            SendSBMsg(trajServiceResponse);
        } else if(CHECKNAME((*msg),"GetInterceptHeadingToPlan")){

            char planID[10] = {0};
            int nextWP;
            double position[3];

            b = deSerializeString(planID,b);
            b = deSerializeInt(false,&nextWP,b);
            b = deSerializeRealArray(position,b);

            //OS_printf("Get intercept to plan %s,at %f,%f,%f, to %d\n",planID,position[0],position[1],position[2],nextWP);

            double track = PathPlanner_GetInterceptHeadingToPlan_c(TrajectoryAppData.pplanner,planID,nextWP,position);
            serializeReal(false,track,trajServiceResponse.buffer);
            SendSBMsg(trajServiceResponse);
        } else if(CHECKNAME((*msg),"ManeuverToIntercept")){

            char planID[10] = {0};
            int nextWP;
            double position[3];
            double cmdVelocity[3];

            b = deSerializeString(planID,b);
            b = deSerializeInt(false,&nextWP,b);
            b = deSerializeRealArray(position,b);

            PathPlanner_ManueverToIntercept_c(TrajectoryAppData.pplanner,planID,nextWP,position,cmdVelocity);
            serializeRealArray(3,cmdVelocity,trajServiceResponse.buffer);
            SendSBMsg(trajServiceResponse);
        }
    }else{
        trajServiceResponse.id = msg->id;
        trajServiceResponse.sType = _lookup_return_;
        if(CHECKNAME((*msg),"allowedXtrackDev")){
            double val;
            val = FlightData_GetAllowedXtracDeviation(TrajectoryAppData.fdata);
            serializeReal(false,val,trajServiceResponse.buffer);
            SendSBMsg(trajServiceResponse);
        }else if(CHECKNAME((*msg),"resolutionSpeed")){
            double val;
            val = FlightData_GetResolutionSpeed(TrajectoryAppData.fdata);
            serializeReal(false,val,trajServiceResponse.buffer);
            SendSBMsg(trajServiceResponse);
        }else if(CHECKNAME((*msg),"totalFences")){
            int val;
            val = FlightData_GetTotalFences(TrajectoryAppData.fdata);
            serializeInt(false,val,trajServiceResponse.buffer);
            SendSBMsg(trajServiceResponse);
        }else if(CHECKNAME((*msg),"totalTraffic")){
            int val;
            val = FlightData_GetTotalTraffic(TrajectoryAppData.fdata);
            serializeInt(false,val,trajServiceResponse.buffer);
            SendSBMsg(trajServiceResponse);
        }else if(CHECKNAME((*msg),"trafficResType")){
            int val;
            val = FlightData_GetTrafficResolutionType(TrajectoryAppData.fdata);
            serializeInt(false,val,trajServiceResponse.buffer);
            SendSBMsg(trajServiceResponse);
        }

    }

}