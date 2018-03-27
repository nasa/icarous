//
// Created by Swee Balachandran on 11/8/17.
//

#include <stdio.h>
#include <zconf.h>
#include <memory.h>
#include <interfaces/CfsAdapter/cfs-data-format.hh>
#include "PlexilWrapper.h"
#include "Serialize.h"

int main(int argc, char** argv){

    struct plexilExec* exec;
    struct plexilInterfaceAdapter* adap;

    plexil_init(argc,argv,&exec,&adap);

    while(1){

        plexil_run(exec);

        PlexilMsg msg1,msg2;
        int n1 = plexil_getCommand(adap,&msg1);
        int n2 = plexil_getLookup(adap,&msg2);

        if(n1>=0) {
            if (CHECK_NAME(msg1, "ReceiveTrue")) {
                PlexilMsg reply;
                reply.mType = _COMMAND_RETURN_;
                reply.id = msg1.id;
                bool val = true;
                serializeBool(false, val, reply.buffer);
                plexil_return(adap, &reply);
            } else if (CHECK_NAME(msg1, "ReceiveFalse")) {
                PlexilMsg reply;
                reply.mType = _COMMAND_RETURN_;
                reply.id = msg1.id;
                bool val = false;
                serializeBool(false, val, reply.buffer);
                plexil_return(adap, &reply);
            } else if (CHECK_NAME(msg1, "SendBool")) {
                bool arg1;
                bool arg2[3];
                char *b = msg1.buffer;
                b = deSerializeBool(false, &arg1, b);
                b = deSerializeBoolArray(arg2, b);
                printf("Received bool arg1: %d, arg2: %d,%d,%d\n", arg1, arg2[0], arg2[1], arg2[2]);

                PlexilMsg reply;
                reply.mType = _COMMAND_RETURN_;
                reply.id = msg1.id;
                bool val = true;
                serializeBool(false, val, reply.buffer);
                plexil_return(adap, &reply);
            } else if (CHECK_NAME(msg1, "SendInt")) {
                int32_t arg1;
                int32_t arg2[2];
                char *b = msg1.buffer;
                b = deSerializeInt(false, &arg1, b);
                b = deSerializeIntArray(arg2, b);
                printf("Received int arg1: %d, arg2: %d,%d\n", arg1, arg2[0], arg2[1]);

                PlexilMsg reply;
                reply.mType = _COMMAND_RETURN_;
                reply.id = msg1.id;
                int32_t val = arg1 + 1;
                serializeInt(false, val, reply.buffer);
                plexil_return(adap, &reply);

            } else if (CHECK_NAME(msg1, "SendReal")) {
                double arg1;
                double arg2[3];
                char *b = msg1.buffer;
                b = deSerializeReal(false, &arg1, b);
                b = deSerializeRealArray(arg2, b);
                printf("Received int arg1: %lf, arg2: %lf,%lf,%lf\n", arg1, arg2[0], arg2[1], arg2[2]);

                PlexilMsg reply;
                reply.mType = _COMMAND_RETURN_;
                reply.id = msg1.id;
                double val = arg1 + 1;
                serializeReal(false, val, reply.buffer);
                plexil_return(adap, &reply);
            } else if (CHECK_NAME(msg1,"SendString")){
                char string[25];
                memset(string,0,25);
                char* b = msg1.buffer;
                b = deSerializeString(string,b);
                printf("Received string: %s\n",string);
            }
        }

        if(n2>=0){
            if (CHECK_NAME(msg2, "position")) {
                double position[3] = {1.5, 2.3, 1.45};
                PlexilMsg reply;
                reply.mType = _LOOKUP_RETURN_;
                reply.id = msg1.id;
                serializeRealArray(3, position, reply.buffer);
                plexil_return(adap, &reply);
            }
        }

    }

    return 0;
}