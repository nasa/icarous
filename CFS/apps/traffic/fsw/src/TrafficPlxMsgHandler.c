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

        if(CHECK_NAME(msg->plxData,"MonitorTraffic")){

            double position[3];
            double velocity[3];
            b = deSerializeRealArray(position,b);
            b = deSerializeRealArray(velocity,b);
            double resolution[3] = {0.0,0.0,0.0};
            double output[4];
            bool val = TrafficMonitor_MonitorTraffic(TrafficAppData.tfMonitor,true,0.0,position,velocity,resolution);

            output[0] = val?1.0:-1.0;
            output[1] = resolution[0];
            output[2] = resolution[1];
            output[3] = resolution[2];
            serializeRealArray(4,output,trafficPlexilMsg.plxData.buffer);
            SendSBMsg(trafficPlexilMsg);
        }
    }

}