//
// Created by research133 on 1/2/18.
//

#include <Icarous_msg.h>
#include "geofence.h"

void GeoPlxMsgHandler(service_t* msg){

    msg = (service_t*)geofenceAppData.Geofence_MsgPtr;
    const char* b = msg->buffer;

    gfServiceResponse.sType = _command_return_;
    gfServiceResponse.id = msg->id;

    if(CHECKNAME((*msg),"CheckFenceViolation")){
        double position[3];
        double velocity[3];
        b = deSerializeRealArray(position,b);
        b = deSerializeRealArray(velocity,b);
        GeofenceMonitor_CheckViolation(geofenceAppData.gfMonitor,position,velocity[0],velocity[1],velocity[2]);
        bool conflicts[2];
        GeofenceMonitor_GetConflictStatus(geofenceAppData.gfMonitor,conflicts);
        serializeBoolArray(2,conflicts,gfServiceResponse.buffer);
        SendSBMsg(gfServiceResponse);

        status_t statusMsg;
        CFE_SB_InitMsg(&statusMsg,ICAROUS_STATUS_MID,sizeof(statusMsg),TRUE);
        if(conflicts[0])
            memcpy(statusMsg.buffer,"IC:Keep out conflict",20);

        if(conflicts[1])
            memcpy(statusMsg.buffer,"IC:Keep in conflict",19);

        SendSBMsg(statusMsg);

    }else if(CHECKNAME((*msg),"CheckDirectPathFeasibility")){
        double fromPosition[3];
        double toPosition[3];
        b = deSerializeRealArray(fromPosition,b);
        b = deSerializeRealArray(toPosition,b);
        bool status = GeofenceMonitor_CheckWPFeasibility(geofenceAppData.gfMonitor,fromPosition,toPosition);
        serializeBool(false,status,gfServiceResponse.buffer);
        SendSBMsg(gfServiceResponse);
        //OS_printf("Path feasibility:%d\n",status);
    }else if(CHECKNAME((*msg),"GetRecoveryPosition")){
        double currentPos[3];
        b = deSerializeRealArray(currentPos,b);
        double recoveryPosition[3];
        GeofenceMonitor_GetClosestRecoveryPoint(geofenceAppData.gfMonitor,currentPos,recoveryPosition);
        serializeRealArray(3,recoveryPosition,gfServiceResponse.buffer);
        //OS_printf("Recovery position: %f,%f,%f\n",recoveryPosition[0],recoveryPosition[1],recoveryPosition[2]);
        SendSBMsg(gfServiceResponse);
    }

}