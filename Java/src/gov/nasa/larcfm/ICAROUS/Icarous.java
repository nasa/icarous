/**
 * Main entry point for ICAROUS
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 * Notices:
 *  Copyright 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration. 
 *  All rights reserved.
 *     
 * Disclaimers:
 *  No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, 
 *  IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY
 *  IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, 
 *  ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, 
 *  WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT 
 *  AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS 
 *  RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND 
 *  LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
 *
 * Waiver and Indemnity:  
 *   RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, 
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE 
 *   RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES,
 *   EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, 
 *   RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, 
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  
 *   RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
 */

package gov.nasa.larcfm.ICAROUS;
import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Attitude;
import gov.nasa.larcfm.ICAROUS.Messages.msg_CmdAck;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Geofence;
import gov.nasa.larcfm.ICAROUS.Messages.msg_MissionItemReached;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Object;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Position;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Waypoint;
import gov.nasa.larcfm.IO.SeparatedInput;
import com.MAVLink.common.*;
import java.io.*;
import java.util.*;
import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

public class Icarous{

	public static final String VERSION = "1.2.2";

        private boolean verbose;
	private String sitlhost;
	private String px4port;
	private String bcastgroup;
	private String comport;
	private String radioport;
	private String mode;
	private String inputfile;
	private int px4baud;
	private int radiobaud;
	private int sitlport;
	private int bcastport;
	private int comportin;
	private int comportout;
	private ParameterData pData;
	private AircraftData FlightData;
	private Interface1 APInt;
	private Interface1 COMInt;
	private Interface1 BCASTInt;
	private QuadFMS fms_module;
	private DAQ daq_module;
	private COM com_module;
	//private BCAST bcast_module;
	private Mission Task;
	private boolean debugDAA;

	public Icarous(String args[],Mission task){
		verbose    = false;
		debugDAA   = false;
		sitlhost   = null;
		px4port    = null;
		bcastgroup = null;
		comport    = null;
		radioport  = null;
		mode       = null;
		px4baud   = 0;
		radiobaud = 0;
		sitlport   = 0;
		bcastport  = 0;
		comportin  = 0;
		comportout = 0;
		APInt      = null;
		COMInt     = null;
		BCASTInt   = null;
		Task       = task;

		// Read in initial value for all parameters (Note this can also be set from
		// the ground station later.		
		// Process input arguments
		for(int i=0;i<args.length && args[i].startsWith("-");++i){
			if(args[i].startsWith("-v")){
				verbose = true;
			}

			else if(args[i].startsWith("--px4")){
				px4port = args[++i];
				px4baud = Integer.parseInt(args[++i]);
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
				radiobaud = Integer.parseInt(args[++i]);
			}

			else if(args[i].startsWith("--bc")){
				bcastgroup = args[++i];
				bcastport  = Integer.parseInt(args[++i]);
			}	    

			else if(args[i].startsWith("--mode")){
				mode = args[++i];
			}
			
			else if(args[i].startsWith("--config")){
				inputfile = args[++i];
			}

			else if(args[i].startsWith("--debug")){
				debugDAA = true;
			}

			else if(args[i].startsWith("-")) {
				System.out.println("Invalid option "+args[i]);
				System.exit(0);
			}
			

		}
		
		pData = null;
		try{
			System.out.println("Getting data from: "+inputfile);
			FileReader in = new FileReader(inputfile);
			SeparatedInput reader = new SeparatedInput(in);

			reader.readLine();
			pData = reader.getParametersRef();	    	    
		}
		catch(FileNotFoundException e){
			System.out.println("parameter file not found");
		}	

		System.out.println("ICAROUS Release: "+release());

		FlightData    = new AircraftData(pData);


		if(sitlport > 0){
			APInt        = new Interface1(Interface1.SOCKET,
					sitlhost,
					sitlport,
					0,
					FlightData);


		}else{	    
			APInt         =  new Interface1(Interface1.SERIAL,px4port,px4baud,FlightData);
		}


		if(radioport == null){
			COMInt   = new Interface1(Interface1.SOCKET,
					comport,
					comportin,
					comportout,
					FlightData);
		}
		else{
			COMInt   = new Interface1(Interface1.SERIAL,radioport,radiobaud,FlightData);

		}

		COMInt.SendStatusText("ICAROUS version: "+VERSION);

		fms_module  = new QuadFMS(APInt,COMInt,FlightData,Task,pData);
		daq_module  = new DAQ("Data acquisition",FlightData,APInt,COMInt);
		com_module  = new COM("Communications",APInt,COMInt,FlightData,pData);

		fms_module.log.setConsoleOutput(verbose);
		com_module.log.setConsoleOutput(verbose);
		fms_module.debugDAA = debugDAA;

		COMInt.SendStatusText("IC: DAQ, FMS, COM Initialized");
	}

	public static String release() {
		return "ICAROUSj V-"+VERSION+
				"-FormalATM-"+Constants.version+" (July-28-2017)"; 
	}

	public void run(){
		if(mode.equals("passthrough")){
			System.out.println("ICAROUS pass through mode");
			while(true){
				Interface1.PassThrough(APInt,COMInt);
			}
		}	
		else{

			daq_module.start();
			com_module.start();
			
			msg_heartbeat msgHeartbeatAP = FlightData.RcvdMessages.GetHeartbeat_AP();	
			while(msgHeartbeatAP == null){
				msgHeartbeatAP = FlightData.RcvdMessages.GetHeartbeat_AP();
			}

			System.out.println("Received heartbeat from AP");
			COMInt.SendStatusText("IC:Connected to Pixhawk");

			fms_module.EnableDataStream(1);

			

			if(bcastport > 0){
				//bcast_module.start();
			}

			if(mode.equals("passive")){

				System.out.println("ICAROUS passive mode");
				COMInt.SendStatusText("IC:ICAROUS is passive");
				while(true){
					fms_module.UpdateAircraftData();
					fms_module.Monitor();
				}
			}
			else{
				System.out.println("ICAROUS active mode");
				COMInt.SendStatusText("IC:ICAROUS is active");
				fms_module.start();
				while(fms_module.isFMSrunning()){
					// DO nothing
				}					
			} // end of mode else (passive)
		} // end of mode else (passthrough)
		// [CAM] Printing debug information
	}// end of run
	
	public void InputStartMission(int param){
		
	}
	
	public void InputResetIcarous(){
		
	}
	
	public void InputClearFlightPlan(){
		
	}
	
	public void InputParamTable(ParameterData pData){
		
	}
	
	public void InputFlightPlanData(msg_Waypoint waypoint){
		
	}
	
	public void InputGeofenceData(msg_Geofence geofence){
		
	}
	
	public void InputPosition(msg_Position position){
		
	}
	
	public void InputAttitude(msg_Attitude attitude){
		
	}
	
	public void InputMissionItemReached(msg_MissionItemReached misssionItemReached){
		
	}
	
	public void InputTraffic(msg_Object traffic){
		
		
	}
	
	public void InputAck(msg_CmdAck ack){
		
	}
	
}// end of class
