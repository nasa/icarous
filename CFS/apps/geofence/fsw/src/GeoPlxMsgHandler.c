//
// Created by research133 on 1/2/18.
//

#include <Plexil_msg.h>
#include <cfs-data-format.hh>
#include "geofence.h"

void GeoPlxMsgHandler(plexil_interface_t* msg){

    msg = (plexil_interface_t*)geofenceAppData.Geofence_MsgPtr;
    const char* b = msg->plxData.buffer;

    gfPlexilMsg.plxData.mType = _COMMAND_RETURN_;
    gfPlexilMsg.plxData.id = msg->plxData.id;

    if(CHECK_NAME(msg->plxData,"CheckFenceViolation")){
        double position[3];
        double velocity[3];
        b = deSerializeRealArray(position,b);
        b = deSerializeRealArray(velocity,b);
        GeofenceMonitor_CheckViolation(geofenceAppData.gfMonitor,position,velocity[0],velocity[1],velocity[2]);
        bool conflicts[2];
        GeofenceMonitor_GetConflictStatus(geofenceAppData.gfMonitor,conflicts);
        serializeBoolArray(2,conflicts,gfPlexilMsg.plxData.buffer);
        SendSBMsg(gfPlexilMsg);
    }else if(CHECK_NAME(msg->plxData,"CheckDirectPathFeasibility")){
        OS_printf("Received WP feasbilility check command from plexil\n");
        double fromPosition[3];
        double toPosition[3];
        b = deSerializeRealArray(fromPosition,b);
        b = deSerializeRealArray(toPosition,b);
        OS_printf("From position: %f,%f,%f\n",fromPosition[0],fromPosition[1],fromPosition[2]);
        OS_printf("To position: %f,%f,%f\n",toPosition[0],toPosition[1],toPosition[2]);
        bool status = GeofenceMonitor_CheckWPFeasibility(geofenceAppData.gfMonitor,fromPosition,toPosition);
        serializeBool(false,status,gfPlexilMsg.plxData.buffer);
        SendSBMsg(gfPlexilMsg);
        OS_printf("Path feasibility:%d\n",status);
    }else if(CHECK_NAME(msg->plxData,"GetRecoveryPosition")){
        OS_printf("Received recovery position command from plexil\n");
        double currentPos[3];
        b = deSerializeRealArray(currentPos,b);
        double recoveryPosition[3];
        GeofenceMonitor_GetClosestRecoveryPoint(geofenceAppData.gfMonitor,currentPos,recoveryPosition);
        serializeRealArray(3,recoveryPosition,gfPlexilMsg.plxData.buffer);
        OS_printf("Recovery position: %f,%f,%f\n",recoveryPosition[0],recoveryPosition[1],recoveryPosition[2]);
        SendSBMsg(gfPlexilMsg);
    }

}