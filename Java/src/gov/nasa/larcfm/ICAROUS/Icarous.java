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
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.ICAROUS.Interface.Interface;
import gov.nasa.larcfm.ICAROUS.Messages.msg_ArgCmds;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Attitude;
import gov.nasa.larcfm.ICAROUS.Messages.msg_CmdAck;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Geofence;
import gov.nasa.larcfm.ICAROUS.Messages.msg_MissionItemReached;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Object;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Position;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Visbands;
import gov.nasa.larcfm.ICAROUS.Messages.msg_Waypoint;
import gov.nasa.larcfm.IO.SeparatedInput;
import com.MAVLink.common.*;
import java.io.*;
import java.util.*;
import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

public class Icarous{

	public static final String VERSION = "1.2.3";

    private boolean verbose;
    private String mode;
	private String inputfile;	
	private ParameterData pData;
	private AircraftData FlightData;	
	private QuadFMS fms_module;	
	//private BCAST bcast_module;
	private Mission Task;
	private boolean debugDAA;
	public String sitlhost;
	public String px4port;	
	public String comport;
	public String radioport;	
	public int px4baud;
	public int radiobaud;
	public int sitlport;	
	public int comportin;
	public int comportout;
	
	public class IcarousMode{
		public static final int _ACTIVE_ = 0;
		public static final int _PASSIVE_ = 1;
	}

	public Icarous(String args[],Mission task){
		verbose    = false;
		debugDAA   = false;
		sitlhost   = null;
		px4port    = null;		
		comport    = null;
		radioport  = null;
		mode       = null;
		px4baud   = 0;
		radiobaud = 0;
		sitlport   = 0;		
		comportin  = 0;
		comportout = 0;		
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
		fms_module    = new QuadFMS(FlightData, task, pData);
	}

	public static String release() {
		return "ICAROUSj V-"+VERSION+
				"-FormalATM-"+Constants.version+" (July-28-2017)"; 
	}

	public void Run(Interface AP,Interface GS){
		while(true){
			fms_module.run();			
			OutputToAP(AP);
			OutputToGS(GS);
		}
	}
	
	
	public void InputStartMission(int param){
		FlightData.SetStartMissionFlag(param);
	}
	
	public void InputResetIcarous(){
		FlightData.Reset();
		fms_module.Reset();
	}
	
	public void InputClearFlightPlan(){
		FlightData.InputFlightPlan.clear();
	}
		
	public void InputFlightPlanData(msg_Waypoint waypoint){
		FlightData.AddMissionItem(waypoint);
	}
	
	public void InputGeofenceData(msg_Geofence geofence){
		FlightData.AddGeofence(geofence);
	}
	
	public void InputPosition(msg_Position position){
		double lat,lon,alt;
		double vx,vy,vz;
		double bootTime;
		
		bootTime = (double) position.time_gps;
		lat      = (double) position.latitude;
		lon      = (double) position.longitude;
		alt      = (double) position.altitude_rel;
		vx       = (double) position.vx;
		vy       = (double) position.vy;
		vz       = (double) position.vz;
		
		Velocity V = Velocity.makeVxyz(vy,vx,"m/s",vz,"m/s");
		Position P = Position.makeLatLonAlt(lat,"degree",lon,"degree",alt,"m");	
		FlightData.acState.add(P,V,bootTime);
		FlightData.acTime = bootTime;
		
	}
	
	public void InputAttitude(msg_Attitude attitude){
		// Get aircraft attitude information
		
		FlightData.roll  = attitude.roll;
		FlightData.pitch = attitude.pitch;
		FlightData.yaw   = attitude.yaw;
		
		
		FlightData.heading = FlightData.acState.velocityLast().track("degree");
		
		if(FlightData.heading < 0){
			FlightData.heading = 360 + FlightData.heading;
		}
	}	
	
	public void InputMissionItemReached(msg_MissionItemReached misssionItemReached){
		FlightData.nextMissionWP++;
	}
	
	public void InputTraffic(msg_Object traffic){
		FlightData.AddTraffic(traffic.index, traffic.latitude, traffic.longitude, traffic.altiude,
				traffic.vx, traffic.vy, traffic.vz);
	}
	
	public void InputAck(msg_CmdAck ack){
		FlightData.InputAck(ack);
	}
	
	public msg_ArgCmds OutputCommand(){
		if(FlightData.outputList.size() > 0){
			return FlightData.outputList.remove();
		}else{
			return null;
		}
	}
	
	public msg_Visbands OutputKinematicBands(){
		return FlightData.visBands;
	}
	
	public void OutputToAP(Interface iface){
		msg_ArgCmds cmd = null;
		do{
			cmd = OutputCommand();
			if (cmd != null){
				iface.SendData(cmd);
			}
		}while(cmd != null);
	}
	
	public void OutputToGS(Interface iface){
		msg_Visbands msg = FlightData.visBands;
		FlightData.visBands = null;		
		if(msg!= null){
			iface.SendData(msg);
		}		
	}
	
}// end of class
