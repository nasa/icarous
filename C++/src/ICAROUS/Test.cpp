#include <stdio.h>
#include <thread>
#include <fstream>

#include "Communication.h"
#include "Interface.h"
#include "QuadFMS.h"
#include "ParameterData.h"
#include "SeparatedInput.h"


using namespace std;
using namespace larcfm;

int main(int argc,char* argv[]){
    
    
    printf("Testing interfaces\n");
    
    // Read parameters from file and get the parameter data container
    ifstream ConfigFile;
    SeparatedInput sepInputReader(&ConfigFile);
    ParameterData paramData;

    ConfigFile.open("params/icarous.txt");
    sepInputReader.readLine();
    paramData = sepInputReader.getParameters();

    MAVLinkMessages_t RcvdMessages;
    AircraftData_t FlightData(&RcvdMessages,&paramData);

    //SerialInterface_t apPort = SerialInterface("/dev/ttyO1",B57600,0,&RcvdMessages);
    SocketInterface_t SITL("127.0.0.1",14550,0,&RcvdMessages);
    SocketInterface_t COM("127.0.0.1",14552,14553,&RcvdMessages);

    Communication_t DAQ(&SITL,&COM,&FlightData);

    QuadFMS_t FMS(&SITL,&COM,&FlightData);

    FMS.SendStatusText("Starting ICAROUS");

    std::thread thread1(&Communication_t::GetPixhawkData,&DAQ);
    std::thread thread2(&Communication_t::GetGSData,&DAQ);
    std::thread thread3(&FlightManagementSystem_t::RunFMS,&FMS);
    
    //thread1.join();
    thread2.join();
    //thread3.join();
    

    return 0;
    

    /*
    MAVLinkInbox RcvdMessages;
    AircraftData FlightData(&RcvdMessages);
    SocketInterface SITL("127.0.0.1",14550,0,&FlightData);
    mavlink_message_t msg1,msg2;
    char buffer[500];
    mavlink_msg_heartbeat_pack(1, 200, &msg1, MAV_TYPE_HELICOPTER, MAV_AUTOPILOT_GENERIC, MAV_MODE_GUIDED_ARMED, 0, MAV_STATE_ACTIVE);
	
    mavlink_msg_sys_status_pack(1, 200, &msg2, 0, 0, 0, 500, 11000, -1, -1, 0, 0, 0, 0, 0, 0);



    std::queue<mavlink_message_t> msgList;
    msgList.push(msg1);
    msgList.push(msg2);
    SITL.msgQueue.push(msg1);

    unsigned int n = msgList.size();
    unsigned int n2 = SITL.msgQueue.size();
    */

}
