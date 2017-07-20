/**
 * Communication module
 * This class enables listening and responding to requests from
 * other onboard applications.
 *
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
 *  ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE log FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, 
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

import com.MAVLink.common.*;
import com.MAVLink.enums.*;
import com.MAVLink.icarous.msg_kinematic_bands;
import com.MAVLink.*;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Velocity;

import java.io.*;

public class COM implements Runnable,ErrorReporter{

	public Thread t;
	public String threadName;
	public AircraftData FlightData;
	public ErrorLog log;
	public MAVLinkMessages RcvdMessages;
	private ParameterData pData;    
	private Interface apIntf;
	private Interface gsIntf;
	private int WPloaded;
	private boolean sgMsgRcvd1;
	private boolean sgMsgRcvd2;
	private int sfcount;
	Position acposrev;
	

	public COM(String name,Interface ap,Interface gs,AircraftData fData,ParameterData pdata){
		threadName       = name;
		apIntf           = ap;
		gsIntf           = gs;
		FlightData       = fData;
		log              = new ErrorLog("COM");
		RcvdMessages     = FlightData.RcvdMessages;
		pData            = pdata;
		WPloaded         = 0;
		sgMsgRcvd1       = false;
		sgMsgRcvd2       = false;
		acposrev         = Position.mkLatLonAlt(0, 0, 0);
		sfcount          = 0;
	}

	public void run(){

		gsIntf.SetTimeout(1);

		while(true){	        

			// Read data from COM interface
			gsIntf.Read();

			// Handle mission waypoints points
			HandleMissionCount();

			// Handle mission item
			HandleMissionItem();
			
			// Handle mission request list
			HandeMissionRequestList();
			
			// Handle mission request
			HandleMissionRequest();

			// Handle parameter requests
			HandleParamRequestList();

			// Handle parameter read requests
			HandleParamValue();

			// Handle parameter value
			HandleParamValue();

			// Handle parameter set
			HandleParamSet();

			// Handle long commands
			HandleCommandLong();

			// Handle int commands
			HandleCommandInt();

			//Handle mode changes
			HandleSetMode();

			//Handle GPS inject
			HandleGPSInjectData();
			
			//Handle kinematic bands
			HandleKinematicBands();

		}
	}

	public void start(){
		System.out.println("Starting "+threadName);
		t = new Thread(this,threadName);
		t.start();
	}    

	public void HandleMissionCount(){
		msg_mission_count msgMissionCount = RcvdMessages.GetMissionCount();	    
		if(msgMissionCount != null && msgMissionCount.target_system == 1){
			apIntf.Write(msgMissionCount);
			WPloaded = 0;
			FlightData.InputFlightPlan.clear();
		}
	}
	
	public void HandleMissionItem(){
		msg_mission_item msgMissionItem = RcvdMessages.GetMissionItem();
		if(msgMissionItem != null && msgMissionItem.target_system == 1){
			//System.out.println("received wp item:"+WPloaded);
			apIntf.Write(msgMissionItem);
			if(msgMissionItem.seq == WPloaded){
				FlightData.InputFlightPlan.add(msgMissionItem);
				WPloaded++;
			}
		}
	}
	
	public void HandleMissionRequest(){
		msg_mission_request msgMissionRequest = RcvdMessages.GetMissionRequest();
		if(msgMissionRequest != null && msgMissionRequest.target_system == 1){
			apIntf.Write(msgMissionRequest);
		}
	}

	public void HandeMissionRequestList(){
		msg_mission_request_list msgMissionRequestList = RcvdMessages.GetMissionRequestList();
		if(msgMissionRequestList != null && msgMissionRequestList.target_system == 1){
			apIntf.Write(msgMissionRequestList);
		}
	}

	public void HandleParamRequestList(){
		msg_param_request_list msgParamRequestList = RcvdMessages.GetParamRequestList();	    
		if(msgParamRequestList != null){
			log.addWarning("MSG: Handling parameter request list");
			apIntf.Write(msgParamRequestList);		
		}
	}

	public void HanldeParamRequestRead() {
		msg_param_request_read msgParamRequestRead = RcvdMessages.GetParamRequestRead();
		if(msgParamRequestRead != null){
			apIntf.Write(msgParamRequestRead);
		}
	}

	public void HandleParamValue(){
		msg_param_value msgParamValue = RcvdMessages.GetParamValue();
		if( msgParamValue != null){
			if(msgParamValue.sysid == 1){


			}
			else{
				apIntf.Write(msgParamValue);
			}				
		}
	}

	public void HandleParamSet(){
		msg_param_set msgParamSet = RcvdMessages.GetParamSet();
		if( msgParamSet != null ){
			String ID = new String(msgParamSet.param_id);		
			ID = ID.replaceAll("\0","");		
			msg_param_value msgParamValueIC = new msg_param_value();
			boolean icarous_parm = false;		

			switch (ID){
			
			case "SYS_ID":
			case "COMP_ID":
			case "AUTOPILOT_ID":
			case "ICHBEAT":
			case "HTHRESHOLD":
			case "VTHRESHOLD":
			case "HSTEPBACK":
			case "VSTEPBACK":
			case "STEPBACKTYPE":
			case "GRIDSIZE":
			case "BUFFER":
			case "LOOKAHEAD":
			case "PROXFACTOR":
			case "RES_SPEED":
			case "XTRK_GAIN":
			case "XTRK_DEV":
			case "TAKEOFF_ALT":
			case "MAX_CEILING":
			case "TRK_HEADING":
			case "TRK_DISTH":
			case "TRK_DISTV":
			case "CYL_RADIUS":
			case "CYL_HEIGHT":
			case "ALERT_TIME":
			case "EALERT_TIME":
			case "DAA_LOOKAHEAD":
			case "CHEAP_DAA":
			case "CHEAP_SEARCH":
			case "ALLOW_YAW":
				pData.set(ID,msgParamSet.param_value,pData.getUnit(ID));
				//System.out.println(ID+": "+pData.getValue(ID));
				icarous_parm  = true;
				break;
			default:
				apIntf.Write(msgParamSet);
				break;
			}

			if(icarous_parm){
				msgParamValueIC.param_id    =  msgParamSet.param_id;
				msgParamValueIC.param_value =  msgParamSet.param_value;
				gsIntf.Write(msgParamValueIC);
			}
		}
	}

	public void HandleCommandLong(){
		msg_command_long msgCommandLong = RcvdMessages.GetCommandLong();
		if( msgCommandLong != null ){		
			if(msgCommandLong.command == MAV_CMD.MAV_CMD_DO_FENCE_ENABLE){
				int status;
				status = FlightData.GetGeoFence(gsIntf,msgCommandLong);
				if(status == 1){
					log.addWarning("MSG: Geo fence update, #fences:"+FlightData.fenceList.size());
					gsIntf.SendStatusText("Received Fence");
				}
			}
			else if(msgCommandLong.command == MAV_CMD.MAV_CMD_MISSION_START){		    
				FlightData.startMission = (int) msgCommandLong.param1;
				log.addWarning("MSG: Received Mission START");
				gsIntf.SendStatusText("Starting mission");
				//System.out.println("Available FP size:"+FlightData.InputFlightPlan.size());
			}
			else if(msgCommandLong.command == MAV_CMD.MAV_CMD_SPATIAL_USER_1){
				GenericObject obj = new GenericObject(0,(int)msgCommandLong.param1,
						msgCommandLong.param5,msgCommandLong.param6,msgCommandLong.param7,
						msgCommandLong.param2,msgCommandLong.param3,msgCommandLong.param4);
				synchronized(FlightData.traffic){
					GenericObject.AddObject(FlightData.traffic,obj);
				}

			}
			else if(msgCommandLong.command == MAV_CMD.MAV_CMD_SPATIAL_USER_2){
				GenericObject obj = new GenericObject(1,(int)msgCommandLong.param1,
						msgCommandLong.param5,msgCommandLong.param6,msgCommandLong.param7,
						msgCommandLong.param2,msgCommandLong.param3,msgCommandLong.param4);
				synchronized(FlightData.missionObj){
					GenericObject.AddObject(FlightData.missionObj,obj);
				}

			}
			else if(msgCommandLong.command == MAV_CMD.MAV_CMD_USER_1){
				if(msgCommandLong.param1 == 1){
					synchronized(FlightData){
						log.addWarning("MSG: Resetting ICAROUS");
						gsIntf.SendStatusText("Resetting ICAROUS");
						FlightData.Reset();
					}
				}
				else if(msgCommandLong.param1 == 10){ // Safeguards gpio outputs					
					SafeguardHandler(msgCommandLong);
				}
			}		
			else{
				apIntf.Write(msgCommandLong); 
			}

		}
	}

	public void HandleCommandInt(){
		msg_command_int msgCommandInt = RcvdMessages.GetCommandInt();
		if(msgCommandInt != null){
			if(msgCommandInt.command == MAV_CMD.MAV_CMD_SPATIAL_USER_2){
				GenericObject obj = new GenericObject(1,(int)msgCommandInt.param1,
						(float) msgCommandInt.x*(float)1e-7,
						(float) msgCommandInt.y*(float)1e-7,
						msgCommandInt.z,
						msgCommandInt.param2,msgCommandInt.param3,msgCommandInt.param4);
				synchronized(FlightData.missionObj){
					GenericObject.AddObject(FlightData.missionObj,obj);
				}
			}

		}
	}

	public void HandleSetMode(){
		msg_set_mode msgSetMode = RcvdMessages.GetSetMode();
		if(msgSetMode != null){
			apIntf.Write(msgSetMode);
		}
	}

	public void HandleGPSInjectData(){
		// Handle GPS Inject RTCM messages
		msg_gps_inject_data msgGPSInjectData = RcvdMessages.GetGPSInjectData();
		if(msgGPSInjectData != null){
			apIntf.Write(msgGPSInjectData);
		}
	}
	
	public void HandleKinematicBands(){
		// Handle kinematic band messages
		msg_kinematic_bands msgKinematicBands = RcvdMessages.GetKinematicBands();
		if(msgKinematicBands != null){
			gsIntf.Write(msgKinematicBands);
		}
	}
	
	public void SafeguardHandler(msg_command_long msgCommandLong){
	       //System.out.println(msgCommandLong.param2);
		sfcount++;
		//System.out.format("SG signal %d:%d",sfcount,msgCommandLong.param2);
		if(msgCommandLong.param2 == 3){
			if(sgMsgRcvd1){
				Position currentPos = FlightData.acState.positionLast();
				double dist2pos = currentPos.distanceH(acposrev);				
				if(dist2pos < 1){
					msg_set_mode Mode = new msg_set_mode();
				    Mode.target_system = (short) 0;
				    Mode.base_mode     = (short) 1;
				    Mode.custom_mode   = (long) FlightManagementSystem.ARDUPILOT_MODES.AUTO;
				    apIntf.Write(Mode);
				    sgMsgRcvd1 = false;
				    gsIntf.SendStatusText("Switching to AUTO");
				}
			}
			
			if(sgMsgRcvd2){
				sgMsgRcvd2 = false;
			}
		}else if(msgCommandLong.param2 == 1){
		    // bounce back
			if(!sgMsgRcvd1){
				sgMsgRcvd1 = true;
				
				Position acpos = FlightData.acState.positionLast();
			    Velocity acvel = FlightData.acState.velocityLast();
			    
				// First, set mode to GUIDED
			    msg_set_mode Mode = new msg_set_mode();
			    Mode.target_system = (short) 0;
			    Mode.base_mode     = (short) 1;
			    Mode.custom_mode   = (long) FlightManagementSystem.ARDUPILOT_MODES.BRAKE;
			    
			    apIntf.Write(Mode);	
			    
			    try{
					Thread.sleep(500);
			    }catch(InterruptedException e){
					System.out.println(e);
			    }			    
			    
			    Mode.custom_mode   = (long) FlightManagementSystem.ARDUPILOT_MODES.GUIDED;
			    
			    apIntf.Write(Mode);					    					    
			    			    			    
			    Velocity acvelrev = Velocity.make(acvel.Scal(-1));
			    double trk = acvelrev.trk();
			    acposrev = acpos.linearDist2D(trk,15);
			    
			    
			    double lat = acposrev.latitude();
			    double lon = acposrev.longitude();
			    double alt = acposrev.alt();
			    
			    // No yawing					    
			    msg_command_long CommandLong  = new msg_command_long();
	
				CommandLong.target_system     = (short) 1;
				CommandLong.target_component  = (short) 0;
				CommandLong.command           = MAV_CMD.MAV_CMD_CONDITION_YAW;
				CommandLong.confirmation      = (short) 0;						
				CommandLong.param4            = 1;
				CommandLong.param5            = 1;
				
	
				apIntf.Write(CommandLong);
			    
				
				// Set position
			    msg_set_position_target_global_int msg= new msg_set_position_target_global_int();
			    msg.coordinate_frame = MAV_FRAME.MAV_FRAME_GLOBAL_RELATIVE_ALT_INT;
			    msg.type_mask        = 0b0000111111111000;
			    msg.lat_int          = (int) (lat*1E7);
			    msg.lon_int          = (int) (lon*1E7);
			    msg.alt              = (float) alt;
			    
			    apIntf.Write(msg);		    
			    System.out.println(FlightData.acTime +": received warning from safeguard");
			    gsIntf.SendStatusText("Safeguard warning");
			}		 		    					    					   
		}else if(msgCommandLong.param2 == 0 || msgCommandLong.param2 == 2 ){
		    // terminate
			if(!sgMsgRcvd2){
				sgMsgRcvd2 = true;
			    msg_set_mode Mode = new msg_set_mode();
			    Mode.target_system = (short) 0;
			    Mode.base_mode     = (short) 1;
			    Mode.custom_mode   = (long) FlightManagementSystem.ARDUPILOT_MODES.LAND;
			    
			    apIntf.Write(Mode);
			    System.out.println(FlightData.acTime + ": received terminate from safeguard");
			    gsIntf.SendStatusText("Safeguard terminate");
			    
			    try{
			    	Thread.sleep(200);
			    }catch(InterruptedException e){
			    	System.out.println(e);
			    }
			}
		}
	}
	

	public boolean hasError() {
		return log.hasError();
	}

	public boolean hasMessage() {
		return log.hasMessage();
	}

	public String getMessage() {
		return log.getMessage();
	}

	public String getMessageNoClear() {
		return log.getMessageNoClear();
	}

}
