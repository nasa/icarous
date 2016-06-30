import gov.nasa.larc.ICAROUS.*;

public class TestSerial{
    public static void main(String args[]){
	AircraftData SharedData    = new AircraftData(AircraftData.INIT_MESSAGES);
	ICAROUS_Interface Pixhawk  = new ICAROUS_Interface(args[0],SharedData);
	FMS_Thread FMS             = new FMS_Thread("Flight management",SharedData,Pixhawk);
	DAQ_Thread DAQ             = new DAQ_Thread("Data acquisition",SharedData,Pixhawk);
	
	DAQ.start();
	FMS.start();

    }
}

