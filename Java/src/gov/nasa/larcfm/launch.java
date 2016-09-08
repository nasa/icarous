/**
 * Main entry point for ICAROUS
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
import gov.nasa.larcfm.IO.SeparatedInput;
import com.MAVLink.common.*;
import java.io.*;

public class launch{
    
    public static void main(String args[]){

	boolean verbose   = false;
	String sitlhost   = null;
	String px4port    = null;
	String bcastgroup = null;
	String comport    = null;
	String radioport  = null;
	String mode       = null;
	int sitlport      = 0;
	int bcastport     = 0;
	int comportin     = 0;
	int comportout    = 0;

	// Read in initial value for all parameters (Note this can also be set from
	// the ground station later.
	ParameterData pData = null;
	try{
	    FileReader in = new FileReader("params/icarous.txt");
	    SeparatedInput reader = new SeparatedInput(in);

	    reader.readLine();
	    pData = reader.getParametersRef();	    	    
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
		sitlhost = args[++i];
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

	    else if(args[i].startsWith("--mode")){
		mode = args[++i];
	    }
	}

		
	AircraftData FlightData    = new AircraftData(pData);
	
	Interface APInt = null;	
	if(sitlport > 0){
	    APInt        = new Interface(Interface.SOCKET,
						 sitlhost,
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

	if(mode == "passthrough"){
	    System.out.println("ICAROUS pass through mode");
	    while(true){
		Interface.PassThrough(APInt,COMInt);
	    }
	}	
	else{
		
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
	   
		BCAST bcast_module       = new BCAST("Broadcast",uasQuad,BCASTInt,pData);	
		bcast_module.start();
		    
	    }
	    
	    com_module.start();	

	    if(mode == "passive"){

		System.out.println("ICAROUS passive mode");
		while(true){
		    uasQuad.fsam.Monitor();
		}
	    }
	    else{
		System.out.println("ICAROUS active mode");
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
	    } // end of mode else (passive)
	}// end of mode else (passthrough)
    }// end of main    
}// end of class
