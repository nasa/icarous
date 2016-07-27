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
import com.MAVLink.common.*;
import java.io.*;



public class DebugGPS{
    
    public static void main(String args[]){

	boolean verbose   = false;
	String px4port = null;
	String bcastgroup = null;
	int sitlport      = 0;
	int bcastport     = 0;
	int comport       = 0;

	// Process input arguments
	for(int i=0;i<args.length && args[i].startsWith("-");i++){
	    if(args[i].startsWith("-v")){
		verbose = true;
	    }

	    else if(args[i].startsWith("--px4")){
		px4port = args[++i];
	    }

	    else if(args[i].startsWith("--sitl")){
		sitlport = Integer.parseInt(args[++i]);
	    }

	    else if(args[i].startsWith("--com")){
		comport = Integer.parseInt(args[++i]);		
	    }

	    else if(args[i].startsWith("--bc")){
		bcastgroup = args[++i];
		bcastport  = Integer.parseInt(args[++i]);
	    }
	}
	
	AircraftData FlightData    = new AircraftData();
	Interface APInt = null;
	
	if(sitlport > 0){
	    APInt        = new Interface(Interface.SOCKET,
						 null,
						 sitlport,
						 0,
						 FlightData);
	}else{	    
	   APInt         =  new Interface(Interface.SERIAL,px4port,FlightData);
	}	
					    	
			
	Aircraft uasQuad  = new Aircraft(APInt,null,FlightData,null);

	uasQuad.error.setConsoleOutput(verbose);	
	
	DAQ daq_module           = new DAQ("Data acquisition",uasQuad);

	daq_module.start();

	try{
	    Thread.sleep(1000);
	}catch(InterruptedException e){
	    System.out.println(e);
	}
	
	while(true){
	    msg_gps_raw_int msg = new msg_gps_raw_int();

	    msg = uasQuad.FlightData.Inbox.GpsRawInt();

	    System.out.format("\n\n***GPS status***\n");
	    System.out.format("Fix type           : %d\n",msg.fix_type);
	    System.out.format("lat                : %3.5f (degrees)\n",msg.lat/1E7);
	    System.out.format("lon                : %3.5f (degrees)\n",msg.lon/1E7);
	    System.out.format("alt                : %3.5f (degrees)\n",msg.alt/1E3);
	    System.out.format("Satellites visible : %d \n",msg.satellites_visible);

	    try{
		Thread.sleep(1000);
	    }catch(InterruptedException e){
		System.out.println(e);
	    }
	    
	}
	
    }

}
