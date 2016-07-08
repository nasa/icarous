/**
 * ICAROUS Interface
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
import gov.nasa.larcfm.ICAROUS.*;

public class SITL_test{

    public static void main(String args[]){
	AircraftData SharedData  = new AircraftData(AircraftData.INIT_MESSAGES);
	
	ICAROUS_Interface SITL     = new ICAROUS_Interface(ICAROUS_Interface.SITL,Integer.parseInt(args[0]),SharedData);
	ICAROUS_Interface COMInt   = new ICAROUS_Interface(ICAROUS_Interface.COMBOX,Integer.parseInt(args[1]),SharedData);
	ICAROUS_Interface BCASTInt = new ICAROUS_Interface(ICAROUS_Interface.BROADCAST,"230.1.1.1",5555,SharedData);
	
	FMS_Thread FMS           = new FMS_Thread("Flight management",SharedData,SITL);
	DAQ_Thread DAQ           = new DAQ_Thread("Data acquisition",SharedData,SITL);
	COM_Thread COM           = new COM_Thread("Communications",SharedData,COMInt);
	BCAST_Thread BCAST       = new BCAST_Thread("Broadcast",SharedData,BCASTInt);

	// Initialized interfaces
	SITL.InitInterface(0);	
	COMInt.InitInterface(0);
	BCASTInt.InitInterface(0);

	DAQ.start();

	try{
	    Thread.sleep(1000);
	}catch(InterruptedException e){
	    System.out.println(e);
	}

	BCAST.start();
	
	try{
	    Thread.sleep(1000);
	}catch(InterruptedException e){
	    System.out.println(e);
	}

	
	COM.start();

	try{
	    Thread.sleep(1000);
	}catch(InterruptedException e){
	    System.out.println(e);
	}
	
	FMS.start();
	
    }

}
