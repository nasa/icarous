/**
 * Software in the loop test
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

public class launch{
    
    public static void main(String args[]){

	boolean verbose   = false;
	String px4port    = null;
	String bcastgroup = null;
	String paraminput = null
	int sitlport      = 0;
	int bcastport     = 0;
	int comport       = 0;
	

	// Process input arguments
	for(int i=0;i<args.length && args[i].startsWith("-");++i){
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

	    else if(args[i].startsWith("--param")){
		paraminput = args[++i];
	    }
	}

	//FileReader in = new FileReader(paraminput);
	//SeparatedInput reader = new SeparatedInput(in);
	
	//reader.readLine();
	//ParameterData parameters = reader.getParametersRef();
	
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
	
	Interface COMInt   = new Interface(Interface.SOCKET,
					   null,
					   comport,
					   0,
					   FlightData);
	
	Interface BCASTInt = new Interface(Interface.SOCKET,
					   bcastgroup,
					   0,
					   bcastport,
					   FlightData);

	Demo test = new Demo();
	
	Aircraft uasQuad  = new Aircraft(APInt,COMInt,FlightData,test);

	uasQuad.error.setConsoleOutput(verbose);
	
	
	FMS fms_module           = new FMS("Flight management",uasQuad);
	DAQ daq_module           = new DAQ("Data acquisition",uasQuad);
	COM com_module           = new COM("Communications",uasQuad);
	BCAST bcast_module       = new BCAST("Broadcast",uasQuad,BCASTInt);

	com_module.error.setConsoleOutput(verbose);
	test.error.setConsoleOutput(verbose);

	uasQuad.EnableDataStream();
	
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
