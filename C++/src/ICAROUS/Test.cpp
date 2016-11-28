#include <stdio.h>
#include "Interface.h"
#include "DAQ.h"

using namespace std;

int main(int argc,char* argv[]){

    printf("Testing interfaces\n");
    MAVLinkInbox RcvdMessages;

    //Interface apPort = SerialInterface("/dev/ttyO1",B57600,0,&RcvdMessages);
    
    SocketInterface SITL("127.0.0.1",14550,0,&RcvdMessages);
    SocketInterface COM("127.0.0.1",14552,14553,&RcvdMessages);

    DataAcquisition DAQ(&SITL,&COM,&RcvdMessages);

    DAQ.RunDAQ();

    return 0;
}