/**
 * Flight Management System (FMS)
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
import com.MAVLink.enums.*;
import com.MAVLink.icarous.msg_kinematic_bands;
import com.MAVLink.common.*;

import gov.nasa.larcfm.ICAROUS.Messages.msg_ArgCmds;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;
import java.util.*;
import java.io.*;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

public class FlightManagementSystem implements Runnable,ErrorReporter{

	public class ARDUPILOT_MODES{
		public static final int STABILIZE =     0;  // manual airframe angle with manual throttle
		public static final int ACRO =          1;  // manual body-frame angular rate with manual throttle
		public static final int ALT_HOLD =      2;  // manual airframe angle with automatic throttle
		public static final int AUTO =          3;  // fully automatic waypoint control using mission commands
		public static final int GUIDED =        4;  // fully automatic fly to coordinate or fly at velocity/direction using GCS immediate commands
		public static final int LOITER =        5;  // automatic horizontal acceleration with automatic throttle
		public static final int RTL =           6;  // automatic return to launching point
		public static final int CIRCLE =        7;  // automatic circular flight with automatic throttle
		public static final int LAND =          9;  // automatic landing with horizontal position control
		public static final int DRIFT =        11;  // semi-automous position, yaw and throttle control
		public static final int SPORT =        13;  // manual earth-frame angular rate control with manual throttle
		public static final int FLIP =         14;  // automatically flip the vehicle on the roll axis
		public static final int AUTOTUNE =     15;  // automatically tune the vehicle's roll and pitch gains
		public static final int POSHOLD =      16;  // automatic position hold with manual override, with automatic throttle
		public static final int BRAKE =        17;  // full-brake using inertial/GPS system, no pilot input
		public static final int THROW =        18;  // throw to launch mode using inertial/GPS system, no pilot input
		public static final int AVOID_ADSB =   19;  // automatic avoidance of obstacles in the macro scale - e.g. full-sized aircraft
		public static final int GUIDED_NOGPS = 20;  // guided mode but only accepts attitude and altitude
	}

	public enum FMS_STATE_t{
		_IDLE_, _TAKEOFF_, _CLIMB_, _CRUISE_, _DESCEND_, _LAND_, _TERMINATE_
	}

	protected Thread t;
	protected String threadName;    
	protected MAVLinkMessages RcvdMessages;
	protected FMS_STATE_t fmsState;
	protected boolean FMSrunning;
	public ErrorLog log;
	public AircraftData FlightData;
	public boolean devAllowed;
	public boolean debugDAA;
	public String debug_in;
	public String debug_out;
	public int currentMode;
	public boolean icarousActive;
   
	public FlightManagementSystem(String name,AircraftData fData){
		threadName       = name;
		FMSrunning       = true;
		fmsState         = FMS_STATE_t._IDLE_;
		FlightData       = fData;
		RcvdMessages     = FlightData.RcvdMessages;
		log              = new ErrorLog("FMS");
		devAllowed       = false;
		debugDAA         = false;
		debug_in         = "";
		debug_out 	     = "";
		currentMode      = 0;
		icarousActive    = true;
	}

	public void run(){	
		while(FMSrunning){
			FlightManagement();
		}
	}

	public void start(){
		System.out.println("Starting "+threadName);
		t = new Thread(this,threadName);
		t.start();
	}

	public synchronized boolean isFMSrunning(){
		return FMSrunning;
	}

	public synchronized FMS_STATE_t getFMSstate(){
		return fmsState;
	}

	public void FlightManagement(){
		switch(fmsState){

		case _IDLE_:
			IDLE();
			break;

		case _TAKEOFF_:
			TAKEOFF();
			break;

		case _CLIMB_:
			CLIMB();
			break;

		case _CRUISE_:
			CRUISE();
			break;

		case _DESCEND_:
			DESCEND();
			break;

		case _LAND_:
			LAND();
			break;
		
		case _TERMINATE_: // [CAM] Added this case since it was missing
			break;
		}
	}
	
	public void StartTakeoff(float alt){
		msg_ArgCmds cmd = new msg_ArgCmds();
		cmd.name = msg_ArgCmds.command_name._TAKEOFF_;
		cmd.param1 = alt;
		FlightData.outputList.add(cmd);
	}
	
	public void StartLand(){
		msg_ArgCmds cmd = new msg_ArgCmds();
		cmd.name = msg_ArgCmds.command_name._LAND_;
		Position currPosition = FlightData.acState.positionLast();
		cmd.param5 = (float)currPosition.latitude();
		cmd.param6 = (float)currPosition.longitude();
		cmd.param7 = (float)0.0;
		FlightData.outputList.add(cmd);
	}
	
	public void ArmThrottles(boolean arm){
				
		int c=arm?1:0;
		msg_ArgCmds cmd = new msg_ArgCmds(); 
		cmd.name = msg_ArgCmds.command_name._ARM_;
		cmd.param1 = c;
		FlightData.outputList.add(cmd);		
	}
	
	// Yaw command
	public void SetYaw(boolean relative, double heading){
		int rel = relative?1:0;
		msg_ArgCmds cmd = new msg_ArgCmds();
		cmd.name = msg_ArgCmds.command_name._SETYAW_;
		cmd.param1 = (float)heading;
		cmd.param2 = 0;
		cmd.param3 = 1;
		cmd.param4 = rel;
		FlightData.outputList.add(cmd);		
	}

	// Position command
	public void SetGPSPos(double lat,double lon, double alt){

		msg_ArgCmds cmd = new msg_ArgCmds();
		cmd.name = msg_ArgCmds.command_name._SETPOS_;
		cmd.param1 = (float)lat;
		cmd.param2 = (float)lon;
		cmd.param3 = (float)alt;
		FlightData.outputList.add(cmd);
	}

	// Velocity command
	public void SetVelocity(double Vn,double Ve, double Vu){

		msg_ArgCmds cmd = new msg_ArgCmds();
		cmd.name = msg_ArgCmds.command_name._SETVEL_;
		cmd.param1 = (float)Vn;
		cmd.param2 = (float)Ve;
		cmd.param3 = (float)Vu;
		FlightData.outputList.add(cmd);
	}

	// Function to set mode
	public void SetMode(int modeid){

		msg_ArgCmds cmd = new msg_ArgCmds();
		cmd.name = msg_ArgCmds.command_name._SETMODE_;
		cmd.param1 = modeid;
		FlightData.outputList.add(cmd);
	}
	
	public void SetMissionItem(int nextWP){
		msg_ArgCmds cmd = new msg_ArgCmds();
		cmd.name = msg_ArgCmds.command_name._GOTOWP_;
		cmd.param1 = nextWP;
		FlightData.outputList.add(cmd);
	}
	
	// Check acknowledgement
	public boolean CheckAcknowledgement(int command){
		return FlightData.CheckAck(command);
	}

	public void GetCurrentMode(){
		//TODO: implement this
	}

	public void SetSpeed(float speed){
		msg_ArgCmds cmd = new msg_ArgCmds();
		cmd.name = msg_ArgCmds.command_name._SETSPEED_;
		cmd.param1 = speed;
		FlightData.outputList.add(cmd);		
		FlightData.speed = speed;
	}

	public void IDLE(){
		if(FlightData.startMission == 0){		
			if(FlightData.InputFlightPlan.size() > 0){
				Preflight();
				fmsState = FMS_STATE_t._TAKEOFF_;
			}
			else{
				log.addError("MSG: No flight plan loaded");
			}
			FlightData.startMission = -1;
		}
		else if( FlightData.startMission > 0 && FlightData.startMission < FlightData.InputFlightPlan.size()){		
			if(FlightData.InputFlightPlan.size() > 0){
				Preflight();
				fmsState = FMS_STATE_t._CRUISE_;
				FlightData.nextMissionWP = FlightData.startMission;
				log.addWarning("MSG: Starting mission to waypoint:"+FlightData.nextMissionWP);
			}
			else{
				log.addError("MSG: No flight plan loaded");
			}
			FlightData.startMission = -1;
		}
	}

	public void Preflight(){
		FlightData.ConstructFlightPlan();
	}

	public double getApproxElapsedPlanTime(Plan currentPlan,int nextWP){
		Plan FP = currentPlan;

		Position pos          =  FlightData.acState.positionLast();
		double legDistance    = FP.pathDistance(nextWP - 1);
		double legTime        = FP.time(nextWP) - FP.time(nextWP-1);
		double lastWPDistance = FP.point(nextWP-1).position().distanceH(pos);
		double currentTime    = FP.time(nextWP-1) + legTime/legDistance * lastWPDistance;

		return currentTime;
	}

	public void CheckResetFlag(){
		if(FlightData.reset){
			FlightData.reset = false;
			Reset();
			if (debugDAA) {	
				PrintWriter debug_fin = new PrintWriter(System.out);
				PrintWriter debug_fout = new PrintWriter(System.out);
				try {
					System.out.println("Writing to file");
					DateFormat df = new SimpleDateFormat("yyyy-MM-dd-hh_mm_ss");
					Date date = new Date();
					debug_fin = new PrintWriter(new BufferedWriter(new FileWriter("Icarous-"+df.format(date)+".login")));
					debug_fout = new PrintWriter(new BufferedWriter(new FileWriter("Icarous-"+df.format(date)+".logout")));
				}
				catch (Exception e) {
					System.out.println("ERROR: "+e);
				}
				debug_fin.print(debug_in);
				debug_fin.close();
				debug_fout.print(debug_out);
				debug_fout.close();
				debug_in = "";
				debug_out = "";
			}
		}
	}

	public void TAKEOFF(){};
	public void CLIMB(){};
	public void CRUISE(){};
	public void DESCEND(){};
	public void LAND(){};
	public void TRACKING(Position target){};
	public void Reset(){};

	@Override
	public boolean hasError() {
		
		return log.hasError();
	}

	@Override
	public boolean hasMessage() {
		
		return log.hasMessage();
	}

	@Override
	public String getMessage() {
		
		return log.getMessage();
	}

	@Override
	public String getMessageNoClear() {
		
		return log.getMessageNoClear();
	};
}

