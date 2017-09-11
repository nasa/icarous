#include <IpcIcarousAdapter.h>
#include "Icarous.h"
#include "MAVLinkInterface.h"


int main(int argc,char* argv[]) {

    // Pointers to Autopilot and ground station ports
    Icarous_t ICAROUS(argc, argv);

    // Initialize the FMS
    ICAROUS.Initialize();


    // Mavlink interface
    ArduPilotInterface_t apInterface(&ICAROUS);
    if(ICAROUS.px4baud > 0){
        apInterface.ConfigurePorts(ICAROUS.px4port,ICAROUS.px4baud);
    }else{
        apInterface.ConfigurePorts(ICAROUS.sitlhost,ICAROUS.sitlin,ICAROUS.sitlout);
    }

    MAVProxyInterface_t gsInterface(&ICAROUS);
    if(ICAROUS.radiobaud > 0){
        gsInterface.ConfigurePorts(ICAROUS.gsradio,ICAROUS.radiobaud);
    }else{
        gsInterface.ConfigurePorts(ICAROUS.gshost,ICAROUS.gsin,ICAROUS.gsout);
    }

    apInterface.SetPipe(&gsInterface);
    gsInterface.SetPipe(&apInterface);

    std::string centralhost("localhost:1381");
    IpcIcarousAdapter ipcAdapter(centralhost,&ICAROUS);

    // Launch a thread to read data from AP
    std::thread thread1(&ArduPilotInterface_t::GetData, &apInterface);

    // Launch a thread to read data from ground station
    std::thread thread2(&MAVProxyInterface_t::GetData, &gsInterface);

    ICAROUS.Run(&apInterface,&gsInterface);

	thread1.join();
	thread2.join();


    return 0;
}

