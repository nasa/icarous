//
// Created by Swee Balachandran on 11/8/17.
//

#include <stdio.h>
#include <zconf.h>
#include <memory.h>
#include "PlexilWrapper.h"

int main(int argc, char** argv){

    struct plexilExec* exec;
    struct plexilInterfaceAdapter* adap;

    plexil_init(argc,argv,&exec,&adap);


    bool bval1 = true;
    bool bval2 = false;

    int32_t ival1 = 7;
    double dval = 3.142;

    bool bvalArray[2] = {false,true};
    bool bvalArrayOut[2];

    int32_t ivalArray[5] = {5,4,3,2,1};
    int32_t ivalArrayOut[5];

    double dvalArray[3] = {3.12,2.236,1.5};
    double dvalArrayOut[3];

    char buffer[250];

    char* b = buffer;

    b = serializeBool(bval1,b);
    b = serializeBool(bval2,b);
    b = serializeInt(ival1,b);
    b = serializeReal(dval,b);
    b = serializeBoolArray(2,bvalArray,b);
    b = serializeIntArray(5,ivalArray,b);
    b = serializeRealArray(3,dvalArray,b);

    char bufferCpy[250];
    memcpy(bufferCpy,buffer,250);

    const char* o = bufferCpy;
    bool bval1out,bval2out;
    int32_t ivalout;
    double dvalout;

    o = deSerializeBool(&bval1out,o);
    o = deSerializeBool(&bval2out,o);
    o = deSerializeInt(&ivalout,o);
    o = deSerializeReal(&dvalout,o);
    o = deSerializeBoolArray(bvalArrayOut,o);
    o = deSerializeIntArray(ivalArrayOut,o);
    o = deSerializeRealArray(dvalArrayOut,o);

    printf("boo1 %d, bool2 %d, boolArray %d,%d\n",bval1out,bval2out,bvalArrayOut[0],bvalArrayOut[1]);
    printf("int %d, int array: %d,%d,%d,%d,%d\n",ivalout,ivalArrayOut[0],ivalArrayOut[1],ivalArrayOut[2],ivalArrayOut[3],ivalArrayOut[4]);
    printf("double %lf,double array %lf,%lf,%lf\n",dvalout,dvalArrayOut[0],dvalArrayOut[1],dvalArrayOut[2]);

    sleep(3);

    while(1){

        plexil_run(exec);

        sleep(1);

        PlexilMsg msg1;
        int n1 = plexil_getCommand(adap,&msg1);

        printf("Remaining commands %d\n",n1);

        PlexilMsg msg2;
        int n2 = plexil_getLookup(adap,&msg2);

        printf("Remaining Looups %d\n",n1);

    }

    return 0;
}