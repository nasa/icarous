package InterfaceTest;
import com.MAVLink.enums.*;

public class FMS_Thread implements Runnable{

    public Thread t;
    public String threadName;
    public MAVLinkMessages rcvdMessages;
    public AircraftData APState = new AircraftData();
    public AP_Interface AP;
    
    public FMS_Thread(String name,MAVLinkMessages SharedData, AP_Interface apInterface){
	threadName       = name;
	rcvdMessages     = SharedData;
	AP               = apInterface;	
    }

    public void run(){
		
	//Start the core flight management function
	this.GetAircraftStateInfo();
	this.FlightManagement();
    }

    
    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }

    public void GetAircraftStateInfo(){
	synchronized(rcvdData){
	    APState.CopyStateData(rcvdData);
	}
    }

    public void FlightManagement(){
	
	/*
	ICAROUS functionalities go in here.
	 - use synchronized codeblocks to access shared data.
	 - use Px4.SendCommand(targetsystem, targetcomponent, confirmation, command, 
                                param1,...,param7)
	   to send commands to the autopilot.
	*/

	/* --------- Example Mission ---------- */
	
	// Set mode to guided
	
	
	// Arm the throttles
	AP.SendCommand(0,0,0,MAV_CMD.MAV_CMD_COMPONENT_ARM_DISARM,
		       1,0,0,0,0,0,0);

	
    }

    
}
