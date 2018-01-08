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
            //OS_printf("received traffic monitor");
            double position[3];
            double velocity[3];
            b = deSerializeRealArray(position,b);
            b = deSerializeRealArray(velocity,b);
            double resolution[3] = {0.0,0.0,0.0};
            double output[4];
            bool val = TrafficMonitor_MonitorTraffic(TrafficAppData.tfMonitor,false,0.0,position,velocity,resolution);

            output[0] = val?1.0:-1.0;
            output[1] = resolution[0];
            output[2] = resolution[1];
            output[3] = resolution[2];
            serializeRealArray(4,output,trafficPlexilMsg.plxData.buffer);
            SendSBMsg(trafficPlexilMsg);
            //OS_printf("Traffic output:%d\n",output[0]);
        }else if(CHECK_NAME(msg->plxData,"CheckSafeToTurn")){
            double position[3];
            double velocity[3];
            double fromHeading;
            double toHeading;

            b = deSerializeRealArray(position,b);
            b = deSerializeRealArray(velocity,b);
            b = deSerializeReal(false,&fromHeading,b);
            b = deSerializeReal(false,&toHeading,b);

            bool val = TrafficMonitor_CheckSafeToTurn(TrafficAppData.tfMonitor,position,velocity,fromHeading,toHeading);
            serializeBool(false,val,trafficPlexilMsg.plxData.buffer);
            //OS_printf("turn safety from %f to %f: %d\n",fromHeading,toHeading,val);
            SendSBMsg(trafficPlexilMsg);
        }
    }

}