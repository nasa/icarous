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
	protected Interface apIntf;
	protected Interface gsIntf;
	public ErrorLog log;
	public AircraftData FlightData;
	public boolean devAllowed;
	public boolean debugDAA;
	public String debug_in;
	public String debug_out;
	public int currentMode;
	public boolean icarousActive;
   
	public FlightManagementSystem(String name,AircraftData fData,Interface ap, Interface gs){
		threadName       = name;
		FMSrunning       = true;
		fmsState         = FMS_STATE_t._IDLE_;
		FlightData       = fData;
		RcvdMessages     = FlightData.RcvdMessages;
		log              = new ErrorLog("FMS");
		apIntf           = ap;
		gsIntf           = gs;
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

		UpdateAircraftData();
		CheckMissionWaypointReached();
		CheckResetFlag();
		GetCurrentMode();

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

	public void UpdateAircraftData(){

		// Get GPS data from mavlink messages
		double lat,lon,alt;
		double vx,vy,vz;
		double bootTime;

		msg_global_position_int GPS = RcvdMessages.GetGlobalPositionInt();
		bootTime = (double) (GPS.time_boot_ms)/1E3;
		lat      = (double) (GPS.lat)/1.0E7;
		lon      = (double) (GPS.lon)/1.0E7;
		alt      = (double) (GPS.relative_alt)/1.0E3;
		vx       = (double) (GPS.vx)/1E2;
		vy       = (double) (GPS.vy)/1E2;
		vz       = (double) (GPS.vz)/1E2;
		
		Velocity V = Velocity.makeVxyz(vy,vx,"m/s",vz,"m/s");
		Position P = Position.makeLatLonAlt(lat,"degree",lon,"degree",alt,"m");	
		FlightData.acState.add(P,V,bootTime);

		// Get aircraft attitude information
		msg_attitude msg = RcvdMessages.GetAttitude();

		FlightData.roll  = msg.roll*180/Math.PI;
		FlightData.pitch = msg.pitch*180/Math.PI;
		FlightData.yaw   = msg.yaw*180/Math.PI;


		FlightData.heading = FlightData.acState.velocityLast().track("degree");

		if(FlightData.heading < 0){
			FlightData.heading = 360 + FlightData.heading;
		}

		FlightData.acTime = bootTime;
	}

	// Function to send commands to pixhawk
	public void SendCommand( int target_system,int target_component,int command,int confirmation,
			float param1,float param2,float param3,float param4,float param5,
			float param6,float param7){

		msg_command_long CommandLong  = new msg_command_long();

		CommandLong.target_system     = (short) target_system;
		CommandLong.target_component  = (short) target_component;
		CommandLong.command           = command;
		CommandLong.confirmation      = (short) confirmation;
		CommandLong.param1            = param1;
		CommandLong.param2            = param2;
		CommandLong.param3            = param3;
		CommandLong.param4            = param4;
		CommandLong.param5            = param5;
		CommandLong.param6            = param6;
		CommandLong.param7            = param7;

		apIntf.Write(CommandLong);

		try{
			Thread.sleep(100);
		}catch(InterruptedException e){
			System.out.println(e);
		}
	}

	// Yaw command
	public void SetYaw(boolean relative, double heading){
		int rel = relative?1:0;		
		SendCommand(0,0,MAV_CMD.MAV_CMD_CONDITION_YAW,0,
				(float)heading,0,1,rel,
				0,0,0);
	}

	// Position command
	public void SetGPSPos(double lat,double lon, double alt){

		msg_set_position_target_global_int msg= new msg_set_position_target_global_int();

		msg.time_boot_ms     = 0;
		msg.target_system    = 0;
		msg.target_component = 0;
		msg.coordinate_frame = MAV_FRAME.MAV_FRAME_GLOBAL_RELATIVE_ALT_INT;
		msg.type_mask        = 0b0000111111111000;
		msg.lat_int          = (int) (lat*1E7);
		msg.lon_int          = (int) (lon*1E7);
		msg.alt              = (float) alt;
		msg.vx               = 0;
		msg.vy               = 0;
		msg.vz               = 0;
		msg.afx              = 0;
		msg.afy              = 0;
		msg.afz              = 0;
		msg.yaw              = 0;
		msg.yaw_rate         = 0;

		apIntf.Write(msg);

		try{
			Thread.sleep(100);
		}catch(InterruptedException e){
			System.out.println(e);
		}
	}

	// Velocity command
	public void SetVelocity(double Vn,double Ve, double Vu){

		msg_set_position_target_local_ned msg= new msg_set_position_target_local_ned();

		msg.time_boot_ms     = 0;
		msg.target_system    = 0;
		msg.target_component = 0;
		msg.coordinate_frame = MAV_FRAME.MAV_FRAME_LOCAL_NED;	
		msg.type_mask        = 0b0000111111000111;
		msg.x                = 0;
		msg.y                = 0;
		msg.z                = 0;
		msg.vx               = (float)Vn;
		msg.vy               = (float)Ve;
		msg.vz               = (float)Vu;
		msg.afx              = 0;
		msg.afy              = 0;
		msg.afz              = 0;
		msg.yaw              = 0;
		msg.yaw_rate         = 0;

		apIntf.Write(msg);

		try{
			Thread.sleep(100);
		}
		catch(InterruptedException e){
			System.out.println(e);
		}
	}

	// Function to set mode
	public int SetMode(int modeid){

		msg_set_mode Mode = new msg_set_mode();
		Mode.target_system = (short) 0;
		Mode.base_mode     = (short) 1;
		Mode.custom_mode   = (long) modeid;

		apIntf.Write(Mode);

		try{
			Thread.sleep(200);
		}catch(InterruptedException e){
			System.out.println(e);
		}

		return 1;
	}
	
	public void SetMissionItem(int nextWP){
		msg_mission_set_current msgMission = new msg_mission_set_current();
        msgMission.target_system = 1;
        msgMission.target_component = 0;        
        msgMission.seq = nextWP;               
        for(int i=0;i<FlightData.WPMissionItemMapping.size();++i){
        	if (FlightData.WPMissionItemMapping.get(i).first == nextWP){
        		msgMission.seq = FlightData.WPMissionItemMapping.get(i).second;
        		break;
        	}
        }
        //System.out.println("Setting AP mission seq:"+msgMission.seq);
        apIntf.Write(msgMission);
	}

	// Check acknowledgement
	public int CheckAcknowledgement(int command){

		short status;

		msg_command_ack msgCommandAck = null;

		//System.out.println("Checking for ack for:"+command);
		while(msgCommandAck == null){	    
			msgCommandAck = RcvdMessages.GetCommandAck();
			if(msgCommandAck != null){
				//System.out.format("Got ack for %d = %d\n",msgCommandAck.command,msgCommandAck.result);
			}
			if( (msgCommandAck != null) && (msgCommandAck.command != command)){
				msgCommandAck = null;
			}
		}

		status = msgCommandAck.result;

		switch(status){

		case 0:
			//error.addWarning("Command Accepted");
			return 1;

		case MAV_CMD_ACK.MAV_CMD_ACK_OK:
			//error.addError("Command Accepted");	    
			return 1;

		case MAV_CMD_ACK.MAV_CMD_ACK_ERR_FAIL:
			log.addError("Command error: fail");	    
			return 0;

		case MAV_CMD_ACK.MAV_CMD_ACK_ERR_ACCESS_DENIED:
			log.addError("Command error: access denied");
			return 0;

		case MAV_CMD_ACK.MAV_CMD_ACK_ERR_NOT_SUPPORTED:
			log.addError("Command error: not supported");
			return 0;

		case MAV_CMD_ACK.MAV_CMD_ACK_ERR_COORDINATE_FRAME_NOT_SUPPORTED:
			log.addError("Command error: frame not supported");
			return 0;

		case MAV_CMD_ACK.MAV_CMD_ACK_ERR_COORDINATES_OUT_OF_RANGE:
			log.addError("Command error: coordinates out of range");
			return 0;

		case MAV_CMD_ACK.MAV_CMD_ACK_ERR_X_LAT_OUT_OF_RANGE:
			log.addError("Command error: x lat out of range");
			return 0;

		case MAV_CMD_ACK.MAV_CMD_ACK_ERR_Y_LON_OUT_OF_RANGE:
			log.addError("Command error: y lat out of range");
			return 0;

		case MAV_CMD_ACK.MAV_CMD_ACK_ERR_Z_ALT_OUT_OF_RANGE:
			log.addError("Command error: z alt out of range");
			return 0;

		case MAV_CMD_ACK.MAV_CMD_ACK_ENUM_END:
			log.addError("Command error: enum end");
			return 0;

		default:
			log.addError("Unrecognized result");
			return -1;

		}	   	    		
	}

	public void EnableDataStream(int option){

		msg_heartbeat msg1 = new msg_heartbeat();

		msg1.type      = MAV_TYPE.MAV_TYPE_ONBOARD_CONTROLLER;
		msg1.autopilot = MAV_AUTOPILOT.MAV_AUTOPILOT_GENERIC;
		msg1.sysid     = 255;

		apIntf.Write(msg1);

		msg_request_data_stream req = new msg_request_data_stream();
		req.req_message_rate = 4;
		req.req_stream_id    = MAV_DATA_STREAM.MAV_DATA_STREAM_ALL;
		req.start_stop       = (byte) option;
		req.target_system    = 1;
		req.target_component = 0;

		apIntf.Write(req);
	}
	
	public void GetCurrentMode(){
		msg_heartbeat msg = FlightData.RcvdMessages.GetHeartbeat_AP();
		currentMode =  (int)msg.custom_mode;
		
		if( (currentMode == ARDUPILOT_MODES.ALT_HOLD) || 
			(currentMode == ARDUPILOT_MODES.POSHOLD)){
			icarousActive = false;
		}else{
			icarousActive = true;
		}
	}

	public void SetSpeed(float speed){
		SendCommand(0,0,MAV_CMD.MAV_CMD_DO_CHANGE_SPEED,0,
				1,speed,0,0,0,0,0);
		
		FlightData.speed = speed;
	}

	public boolean CheckMissionWaypointReached(){

		boolean reached = false;
		msg_mission_item_reached msgMissionItemReached = FlightData.RcvdMessages.GetMissionItemReached();		
		if(msgMissionItemReached != null){
			//System.out.format("mission item reached:%d\n",msgMissionItemReached.seq);
			//System.out.println(FlightData.WaypointIndices);
			//System.out.println(FlightData.WaypointIndices.contains(msgMissionItemReached.seq));
			Iterator<Integer> iterator = FlightData.WaypointIndices.iterator();
		    while(iterator.hasNext()) {
		        Integer setElement = iterator.next();
		        if(msgMissionItemReached.seq >= setElement) {
		            iterator.remove();
		            reached = true;
		        }else{		        	
		        	break;
		        }
		    }
		}
		if(reached){
			//System.out.println("Incrementing next WP index");
			FlightData.nextMissionWP++;
			if(FlightData.nextMissionWP < FlightData.numMissionWP){
				float speed = FlightData.GetFlightPlanSpeed(FlightData.MissionPlan,FlightData.nextMissionWP);					
				//SetSpeed(speed);
			}
		}
		return reached;	
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

