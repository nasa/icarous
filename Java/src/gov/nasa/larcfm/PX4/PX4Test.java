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

import com.MAVLink.*;
import com.MAVLink.common.*;


class PowerLineInspection implements Mission{

	public PowerLineInspection(){

	}
	
	public int Execute(Aircraft UAS){

	    //Do nothing
	    return 0;
	}
}

public class PX4Test{

    
    
    public static void main(String args[]){
	AircraftData FlightData    = new AircraftData();
	
	
	Interface PX4Int    = new Interface(Interface.SERIAL,args[0],FlightData);

	//Interface PX4Int    = new Interface(Interface.SOCKET,
	//				    null,
	//				    Integer.parseInt(args[0]),
	//				    0);
					    
	
	Interface COMInt   = new Interface(Interface.SOCKET,
					   null,
					   Integer.parseInt(args[1]),
					   0,
					   FlightData);
	
	Interface BCASTInt = new Interface(Interface.SOCKET,
					   "230.1.1.1",
					   0,
					   5555,
					   FlightData);

	PowerLineInspection test = new PowerLineInspection();
	
	Aircraft uasQuad  = new Aircraft(PX4Int,COMInt,FlightData,test);

	
	
	uasQuad.error.setConsoleOutput(true);
	
	
	//FMS fms_module           = new FMS("Flight management",uasQuad);
	DAQ daq_module             = new DAQ("Data acquisition",uasQuad);
	//COM com_module           = new COM("Communications",uasQuad);
	//BCAST bcast_module       = new BCAST("Broadcast",uasQuad,BCASTInt);

	//com_module.error.setConsoleOutput(true);

	uasQuad.EnableDataStream();
	
	while(!uasQuad.fsam.CheckAPHeartBeat()){
	    
	}
	
	System.out.println("Received heartbeat from AP");

	//uasQuad.EnableDataStream();
	
	//while(!com_module.CheckCOMHeartBeat()){
	    
	//}

	//System.out.println("Received heartbeat from COM");
	
	daq_module.start();	    
	
	while(true){

	    uasQuad.FlightData.GetAttitude();
	  
	    System.out.format("%f,%f,%f\n",uasQuad.FlightData.roll,uasQuad.FlightData.pitch,uasQuad.FlightData.yaw);
	 
	  
	  
	}
	    
	
	//fms_module.start();
	
    }

}
