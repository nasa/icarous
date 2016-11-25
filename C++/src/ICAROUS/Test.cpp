#include <stdio.h>
#include "Interface.h"

int main(int argc,char* argv[]){

    printf("Testing interfaces\n");
    MAVLinkInbox RcvdMessages;

    Interface apPort = SerialInterface("/dev/ttyO1",B57600,0,&RcvdMessages);
    

    while(true){
        apPort.GetMAVLinkMsg();
    }

    return 0;
}