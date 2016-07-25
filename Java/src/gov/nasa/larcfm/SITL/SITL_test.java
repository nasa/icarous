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
import gov.nasa.larcfm.MISSION.*;
import java.io.*;

public class SITL_test{

    
    
    public static void main(String args[]){
	AircraftData FlightData    = new AircraftData();
	
	
	Interface SITLInt    = new Interface(Interface.SOCKET,
					   null,
					   Integer.parseInt(args[0]),
					   0,
					   FlightData);
	
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

	Demo test = new Demo();
	
	Aircraft uasQuad  = new Aircraft(SITLInt,COMInt,FlightData,test);

	uasQuad.error.setConsoleOutput(true);
	
	
	FMS fms_module           = new FMS("Flight management",uasQuad);
	DAQ daq_module           = new DAQ("Data acquisition",uasQuad);
	COM com_module           = new COM("Communications",uasQuad);
	BCAST bcast_module       = new BCAST("Broadcast",uasQuad,BCASTInt);

	com_module.error.setConsoleOutput(true);
	test.error.setConsoleOutput(true);
	
	while(!uasQuad.fsam.CheckAPHeartBeat()){
	    
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

	while(fms_module.isFMSrunning()){
	    // DO nothing
	}

	System.out.println("Creating log file");
	try{
	    FileWriter writer = new FileWriter("SITLLog.log");
	    writer.write(uasQuad.getMessage());
	    writer.write(com_module.getMessage());
	    writer.write(test.getMessage());
	    writer.close();
	}
	catch(IOException e){
	    System.out.println(e);
	}
	
    }

}
