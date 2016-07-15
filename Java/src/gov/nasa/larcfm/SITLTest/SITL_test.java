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
	AircraftData FlightData    = new AircraftData();
	
	
	Interface SITLInt    = new Interface(Interface.SOCKET,
					   null,
					   Integer.parseInt(args[0]),
					   0);
	
	Interface COMInt   = new Interface(Interface.SOCKET,
					   null,
					   Integer.parseInt(args[1]),
					   0);
	
	Interface BCASTInt = new Interface(Interface.SOCKET,
					   "230.1.1.1",
					   0,
					   5555);

	Aircraft uasQuad  = new Aircraft(SITLInt,COMInt,FlightData);
	
	FMS fms_module           = new FMS("Flight management",uasQuad);
	DAQ daq_module           = new DAQ("Data acquisition",uasQuad);
	COM com_module           = new COM("Communications",uasQuad);
	BCAST bcast_module       = new BCAST("Broadcast",uasQuad,BCASTInt);

	
	

	while(!uasQuad.CheckAPHeartBeat()){
	    
	}

	System.out.println("Received heartbeat from AP");

	while(!com_module.CheckCOMHeartBeat()){
	    
	}

	System.out.println("Received heartbeat from COM");
	    
	daq_module.start();
	    

	try{
	    Thread.sleep(1000);
	}catch(InterruptedException e){
	    System.out.println(e);
	}

	bcast_module.start();
	
	try{
	    Thread.sleep(1000);
	}catch(InterruptedException e){
	    System.out.println(e);
	}

	
	com_module.start();

	try{
	    Thread.sleep(1000);
	}catch(InterruptedException e){
	    System.out.println(e);
	}
	
	fms_module.start();
	
    }

}
