import gov.nasa.larc.ICAROUS.*;

public class ICAROUS{

    public static void main(String args[]){
	MAVLinkMessages RcvdMessages = new MAVLinkMessages();
	AP_Interface SITL = new AP_Interface(args[0],Integer.parseInt(args[1]),RcvdMessages);
	FMS_Thread FMS    = new FMS_Thread("Flight management",RcvdMessages,SITL);
	DAQ_Thread DAQ    = new DAQ_Thread("Data acquisition",RcvdMessages,SITL);
	//COM_Thread COM  = new COM_Thread("Communications",Rcvdmessages,SITL);
	
	DAQ.start();
	FMS.start();
	//COM.start();

    }

}
