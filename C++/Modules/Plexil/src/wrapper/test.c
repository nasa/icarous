//
// Created by Swee Balachandran on 11/8/17.
//

#include <stdio.h>
#include <zconf.h>
#include "PlexilWrapper.h"

int main(int argc, char** argv){

    struct plexilExec* exec;
    struct plexilInterfaceAdapter* adap;

    plexil_init(argc,argv,&exec,&adap);

    sleep(3);

    while(1){

        plexil_run(exec);

        sleep(1);

        PlexilCommandMsg msg1;
        int n1 = plexil_getCommand(adap,&msg1);

        printf("Remaining commands %d\n",n1);

        PlexilCommandMsg msg2;
        int n2 = plexil_getLookup(adap,&msg2);

        printf("Remaining Looups %d\n",n1);

    }

    return 0;
}