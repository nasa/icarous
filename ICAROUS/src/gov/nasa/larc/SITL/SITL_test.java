import gov.nasa.larc.ICAROUS.*;

public class SITL_test{

    public static void main(String args[]){
	AircraftData SharedData = new AircraftData(AircraftData.INIT_MESSAGES);
	ICAROUS_Interface SITL  = new ICAROUS_Interface(args[0],Integer.parseInt(args[1]),SharedData);
	FMS_Thread FMS    = new FMS_Thread("Flight management",SharedData,SITL);
	DAQ_Thread DAQ    = new DAQ_Thread("Data acquisition",SharedData,SITL);
	
	DAQ.start();
	FMS.start();

    }

}
