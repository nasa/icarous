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
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larmcfm.IO.SeparatedInput;
import com.MAVLink.common.*;
import java.io.*;

public class launch{
    
    public static void main(String args[]){

	boolean verbose   = false;
	String px4port    = null;
	String bcastgroup = null;
	String comport    = null;
	String radioport  = null;
	int sitlport      = 0;
	int bcastport     = 0;
	int comportin     = 0;
	int comportout    = 0;

	try{
	    FileReader in = new FileReader("params/icarous.txt");
	    SeparatedInput reader = new SeparatedInput(in);

	    reader.readLine();
	    ParameterData pData = reader.getParametersRef();	    	    
	}
	catch(FileNotFoundException e){
	    System.out.println("parameter file not found");
	}	
	

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
		comport    = args[++i];		
		comportin  = Integer.parseInt(args[++i]);
		comportout = Integer.parseInt(args[++i]);
	    }

	    else if(args[i].startsWith("--radio")){
		radioport = args[++i];
	    }

	    else if(args[i].startsWith("--bc")){
		bcastgroup = args[++i];
		bcastport  = Integer.parseInt(args[++i]);
	    }	    
	    
	    else if(args[i].startsWith("-")) {
		System.out.println("Invalid option "+args[i]);
		System.exit(0);
	    }
	}

		
	AircraftData FlightData    = new AircraftData(pData);
	
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

	Interface COMInt = null;
	if(radioport == null){
	    COMInt   = new Interface(Interface.SOCKET,
				     comport,
				     comportin,
				     comportout,
				     FlightData);
	}
	else{
	    COMInt   = new Interface(Interface.SERIAL,radioport, FlightData);
	    
	}

	
	Demo test = new Demo();
	
	Aircraft uasQuad  = new Aircraft(APInt,COMInt,FlightData,test,pData);

	uasQuad.error.setConsoleOutput(verbose);
	
	
	FMS fms_module           = new FMS("Flight management",uasQuad,pData);
	DAQ daq_module           = new DAQ("Data acquisition",uasQuad,pData);
	COM com_module           = new COM("Communications",uasQuad,pData);

	com_module.error.setConsoleOutput(verbose);
	test.error.setConsoleOutput(verbose);
		
	daq_module.start();

	msg_heartbeat msgHeartbeatAP = FlightData.Inbox.GetHeartbeat_AP();	
	while(msgHeartbeatAP == null){
	    msgHeartbeatAP = FlightData.Inbox.GetHeartbeat_AP();
	}

	System.out.println("Received heartbeat from AP");

	uasQuad.EnableDataStream(1);
	
	// Create broad cast thread if necessary
	if(bcastport > 0){

	    Interface BCASTInt = new Interface(Interface.SOCKET,
					       bcastgroup,
					       0,
					       bcastport,
					       FlightData);
	   
	    BCAST bcast_module       = new BCAST("Broadcast",uasQuad,BCASTInt);	
	    bcast_module.start();
		    
	}		
		
	com_module.start();	
	
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
