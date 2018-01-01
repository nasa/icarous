//
// Created by swee on 1/1/18.
//
#include <Plexil_msg.h>
#include "traffic.h"

void TrafficPlxMsgHandler(plexil_interface_t* msg){
    char* b = msg->plxData.buffer;

    if(msg->plxData.mType == _COMMAND_) {
        trafficPlexilMsg.plxData.id = msg->plxData.id;
        trafficPlexilMsg.plxData.mType = _COMMAND_RETURN_;

    }

}