//
// Created by swee on 1/1/18.
//

#include <CWrapper/TrafficMonitor_proxy.h>
#include <Icarous_msg.h>
#include "traffic.h"

void TrafficServiceHandler(service_t* msg){
    const char* b = msg->buffer;

    if(msg->sType == _command_) {
        trafficServiceResponse.id = msg->id;
        trafficServiceResponse.sType = _command_return_;

        if(CHECKNAME((*msg),"MonitorTraffic")){
            //OS_printf("received traffic monitor");
            double position[3];
            double velocity[3];
            bool bands;
            b = deSerializeBool(false,&bands,b);
            b = deSerializeRealArray(position,b);
            b = deSerializeRealArray(velocity,b);
            double resolution[3] = {0.0,0.0,0.0};
            double output[4];
            SwigObj bandwrapper;
            bandwrapper.obj = &trackBands;
            bool val = TrafficMonitor_MonitorTraffic(TrafficAppData.tfMonitor,bands,0.0,position,velocity,resolution,&bandwrapper);


            output[0] = val?1.0:-1.0;
            output[1] = resolution[0];
            output[2] = resolution[1];
            output[3] = resolution[2];
            serializeRealArray(4,output,trafficServiceResponse.buffer);
            SendSBMsg(trafficServiceResponse);

            //OS_printf("num bands: %d\n",trackBands.numBands);
            if(trackBands.numBands > 0) {
                SendSBMsg(trackBands);
            }

            //OS_printf("Traffic output:%d\n",output[0]);
        }else if(CHECKNAME((*msg),"CheckSafeToTurn")){
            double position[3];
            double velocity[3];
            double fromHeading;
            double toHeading;

            b = deSerializeRealArray(position,b);
            b = deSerializeRealArray(velocity,b);
            b = deSerializeReal(false,&fromHeading,b);
            b = deSerializeReal(false,&toHeading,b);

            bool val = TrafficMonitor_CheckSafeToTurn(TrafficAppData.tfMonitor,position,velocity,fromHeading,toHeading);
            serializeBool(false,val,trafficServiceResponse.buffer);
            //OS_printf("turn safety from %f to %f: %d\n",fromHeading,toHeading,val);
            SendSBMsg(trafficServiceResponse);
        }
    }

}