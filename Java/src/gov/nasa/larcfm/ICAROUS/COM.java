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

import com.MAVLink.icarous.*;
import com.MAVLink.common.*;
import com.MAVLink.enums.*;
import com.MAVLink.*;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.ParameterData;
import java.io.*;

public class COM implements Runnable,ErrorReporter{

    private class TIMERS{
	public double heartbeat_AP;
	public double gps_raw_int;
	public double sys_status;
	public double system_time;
	public double raw_imu;
	public double scaled_pressure;
	public double attitude;
	public double local_position_ned;
	public double global_position_int;
	public double rc_channels_raw;
	public double servo_output_raw;
	public double mission_current;
	public double nav_controller_output;
	public double rc_channels;
	public double vfr_hud;
	public double scaled_imu2;
	public double power_status;
	public double terrain_report;
	public double sensor_offsets;
	public double meminfo;
	public double ahrs;
	public double simstate;
	public double hwstatus;
	public double ahrs2;
	public double ahrs3;
	public double ekf_status_report;
	public double vibration;			
    }
    
    public Thread t;
    public String threadName;
    public Aircraft UAS;
    public AircraftData FlightData;
    public Interface comIntf;
    public ErrorLog error;
    public MAVLinkMessages RcvdMessages;
    private ParameterData pData;
    private TIMERS msgScheduler = new TIMERS();
    private boolean pauseDataStream;
    private int paramCount;
    private int AP_ID;
    private int logCount;
            
    public COM(String name,Aircraft uas,ParameterData pdata){
	threadName       = name;
	UAS              = uas;
	FlightData       = UAS.FlightData;
	comIntf          = UAS.comIntf;
	error            = new ErrorLog("COM     ");
	RcvdMessages     = FlightData.Inbox;
	pData            = pdata;
	pauseDataStream  = false;
	paramCount       = 0;
	AP_ID            = pdata.getInt("AUTOPILOT_ID");
	logCount         = 1;
    }

    public void run(){
	
	comIntf.SetTimeout(1);
	double time1 = UAS.timeCurrent;
	
	while(true){	    
	    
	    double time2 = (float)System.nanoTime()/1E9;

	    // Send messages to ground station (same as pixhawk output messages)
	    sendMsg2Gs(time2);

	    // Read data from COM interface
	    comIntf.Read();
	    	    
	    String timeLog = UAS.timeLog; // String of current time for logging

	    // Handle messages from safeguard
	    msg_safeguard msgSafeguard = RcvdMessages.GetSafeguard();
	    if(msgSafeguard != null){
		if( msgSafeguard.value == 1){
		    error.addWarning("[" + timeLog + "] MSG: Got SAFEGUARD flag");
		}
	    }

	    // Handle mission waypoints points
	    msg_mission_count msgMissionCount = RcvdMessages.GetMissionCount();	    
	    if(msgMissionCount != null){
		    int status = FlightData.GetWaypoints(comIntf,0,0,msgMissionCount.count,FlightData.InputFlightPlan);		
		    if(status == 1){
			error.addWarning("[" + timeLog + "] MSG: Got waypoints");
			UAS.EnableDataStream(0);		
			comIntf.Write(FlightData.SendFlightPlanToAP(UAS.apIntf));
			UAS.EnableDataStream(1);
		    }
	    }

	    // Handle mission request list
	    msg_mission_request_list msgMissionRequestList = RcvdMessages.GetMissionRequestList();
	    if(msgMissionRequestList != null){
		if(FlightData.InputFlightPlan.size() > 0){
		    FlightData.SendWaypoints(comIntf,FlightData.InputFlightPlan);
		}
		else{
		    msg_mission_ack MsgMissionAck = new msg_mission_ack();
		    MsgMissionAck.type = MAV_MISSION_RESULT.MAV_MISSION_ERROR;
		}
	    }

	    // Handle parameter requests
	    msg_param_request_list msgParamRequestList = RcvdMessages.GetParamRequestList();	    
	    if(msgParamRequestList != null){
		error.addWarning("[" + timeLog + "] MSG: Handling parameter request list");
		UAS.apIntf.Write(msgParamRequestList);
		pauseDataStream = true;
		paramCount = 0;		
	    }

	    // Handle parameter read requests
	    msg_param_request_read msgParamRequestRead = RcvdMessages.GetParamRequestRead();
	    if(msgParamRequestRead != null){
		UAS.apIntf.Write(msgParamRequestRead);
	    }

	    // Handle parameter value
	    msg_param_value msgParamValue = RcvdMessages.GetParamValue();
	    if( msgParamValue != null){
		if(msgParamValue.sysid == 1){
		    comIntf.Write(msgParamValue);
		    paramCount++;
		    if(paramCount == msgParamValue.param_count){
			pauseDataStream = false;
		    }		    
		}
		else{
		    UAS.apIntf.Write(msgParamValue);
		}				
	    }

	    // Handle parameter set
	    msg_param_set msgParamSet = RcvdMessages.GetParamSet();
	    if( msgParamSet != null ){
		String ID = new String(msgParamSet.param_id);		
		ID = ID.replaceAll("\0","");		
		msg_param_value msgParamValueIC = new msg_param_value();
		boolean icarous_parm = false;		
		
		switch (ID){

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
		case "STANDOFF":
		case "TAKEOFF_ALT":
		    pData.set(ID,msgParamSet.param_value,pData.getUnit(ID));
		    //System.out.println(ID+": "+pData.getValue(ID));
		    icarous_parm  = true;
		    break;
		default:
		    UAS.apIntf.Write(msgParamSet);
		    break;
		}
		
		if(icarous_parm){
		    msgParamValueIC.param_id    =  msgParamSet.param_id;
		    msgParamValueIC.param_value =  msgParamSet.param_value;
		    comIntf.Write(msgParamValueIC);
		}
	    }

	    // Handle commands
	    msg_command_long msgCommandLong = RcvdMessages.GetCommandLong();
	    if( msgCommandLong != null ){		
		if(msgCommandLong.command == MAV_CMD.MAV_CMD_DO_FENCE_ENABLE){
		    int status;
		    status = FlightData.GetGeoFence(comIntf,msgCommandLong);
		    if(status == 1){
			error.addWarning("[" + timeLog + "] MSG: Geo fence update, #fences:"+FlightData.fenceList.size());
		    }
		}
		else if(msgCommandLong.command == MAV_CMD.MAV_CMD_MISSION_START){		    
		    FlightData.startMission = (int) msgCommandLong.param1;
		    error.addWarning("[" + timeLog + "] MSG: Received Mission START");		    
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
			synchronized(UAS){
			    error.addWarning("[" + timeLog + "] MSG: Resetting ICAROUS");
			    UAS.IcarousReset = true;
			}
		    }
		    if(msgCommandLong.param2 == 1){
			try{
			    error.addWarning("[" + timeLog + "] MSG: Dumping to log "+logCount);
			    FileWriter writer = new FileWriter("FlightLog"+logCount+".log");
			    writer.write(UAS.getMessage());
			    writer.write(getMessage());		   
			    writer.close();
			    logCount++;
			}
			catch(IOException e){
			    System.out.println(e);
			}
		    }
		}		
		else{
		   UAS.apIntf.Write(msgCommandLong); 
		}

	    }

	    msg_command_int msgCommandInt = RcvdMessages.GetCommandInt();

	    if(msgCommandInt != null){
		if(msgCommandInt.command == MAV_CMD.MAV_CMD_SPATIAL_USER_2){
		    GenericObject obj = new GenericObject(1,(int)msgCommandInt.param1,
							  (float) msgCommandInt.x*(float)1e-7, (float) msgCommandInt.y*(float)1e-7,msgCommandInt.z,
							  msgCommandInt.param2,msgCommandInt.param3,msgCommandInt.param4);
		    synchronized(FlightData.missionObj){
			GenericObject.AddObject(FlightData.missionObj,obj);
		    }
		}
		
	    }
	    	    
	    //Handle mode changes
	    msg_set_mode msgSetMode = RcvdMessages.GetSetMode();
	    if(msgSetMode != null){
		UAS.apIntf.Write(msgSetMode);
	    }

	    // Handle GPS Inject RTCM messages
	    msg_gps_inject_data msgGPSInjectData = RcvdMessages.GetGPSInjectData();
	    if(msgGPSInjectData != null){
		UAS.apIntf.Write(msgGPSInjectData);
	    }
	}
    }

    public void sendMsg2Gs(double time){

	if(time - msgScheduler.heartbeat_AP >= 1){	    
	    comIntf.Write(RcvdMessages.GetHeartbeat_AP());
	    msgScheduler.heartbeat_AP = time;
	}

	if(!pauseDataStream){
	    if(time - msgScheduler.gps_raw_int >= 1/5.0){
		comIntf.Write(RcvdMessages.GetGpsRawInt());
		msgScheduler.gps_raw_int = time;
	    }

	    if(time - msgScheduler.sys_status >= 1/2.0){
		comIntf.Write(RcvdMessages.GetSysStatus());
		msgScheduler.sys_status = time;
	    }

	    if(time - msgScheduler.system_time >= 1/2.0){
		comIntf.Write(RcvdMessages.GetSystemTime());
		msgScheduler.system_time = time;
	    }

	    if(time - msgScheduler.raw_imu >= 1/5.0){
		comIntf.Write(RcvdMessages.GetRawImu());
		msgScheduler.raw_imu = time;
	    }

	    if(time - msgScheduler.scaled_pressure >= 1/2.5){
		comIntf.Write(RcvdMessages.GetScaledPressure());
		msgScheduler.scaled_pressure = time;
	    }

	    if(time - msgScheduler.attitude >= 1/5.0){
		comIntf.Write(RcvdMessages.GetAttitude());
		msgScheduler.attitude = time;
	    }

	    if(time - msgScheduler.local_position_ned >= 1/5.0){
		comIntf.Write(RcvdMessages.GetLocalPositionNed());
		msgScheduler.local_position_ned = time;
	    }

	    if(time - msgScheduler.global_position_int >= 1/5.0){
		comIntf.Write(RcvdMessages.GetGlobalPositionInt());
		msgScheduler.global_position_int = time;
	    }
	
	    if(time - msgScheduler.rc_channels_raw >= 1/2.5){
		comIntf.Write(RcvdMessages.GetRcChannelsRaw());
		msgScheduler.rc_channels_raw = time;
	    }

	    if(time - msgScheduler.servo_output_raw >= 1/2.5){
		comIntf.Write(RcvdMessages.GetServoOutputRaw());
		msgScheduler.servo_output_raw = time;
	    }

	    if(time - msgScheduler.mission_current >= 1/2.0){
		comIntf.Write(RcvdMessages.GetMissionCurrent());
		msgScheduler.mission_current = time;
	    }

	    if(time - msgScheduler.nav_controller_output >= 1/2.5){
		comIntf.Write(RcvdMessages.GetNavControllerOutput());
		msgScheduler.nav_controller_output = time;
	    }

	    if(time - msgScheduler.rc_channels >= 1/2.5){
		comIntf.Write(RcvdMessages.GetRcChannels());
		msgScheduler.rc_channels = time;
	    }

	    if(time - msgScheduler.vfr_hud >= 1/6.0){
		comIntf.Write(RcvdMessages.GetVfrHud());
		msgScheduler.vfr_hud = time;
	    }

	    if(time - msgScheduler.scaled_imu2 >= 1/3.0){
		comIntf.Write(RcvdMessages.GetScaledImu2());
		msgScheduler.scaled_imu2 = time;
	    }

	    if(time - msgScheduler.power_status >= 1/2.0){
		comIntf.Write(RcvdMessages.GetPowerStatus());
		msgScheduler.power_status = time;
	    }

	    if(time - msgScheduler.terrain_report >= 1/2.0){
		comIntf.Write(RcvdMessages.GetTerrainReport());
		msgScheduler.terrain_report = time;
	    }

	    if(time - msgScheduler.sensor_offsets >= 1/2.0){
		comIntf.Write(RcvdMessages.GetSensorOffsets());
		msgScheduler.sensor_offsets = time;
	    }

	    if(time - msgScheduler.meminfo >= 1/2.5){
		comIntf.Write(RcvdMessages.GetMeminfo());
		msgScheduler.meminfo = time;
	    }

	    if(time - msgScheduler.ahrs >= 1/3.0){
		comIntf.Write(RcvdMessages.GetAhrs());
		msgScheduler.ahrs = time;
	    }

	    if(time - msgScheduler.ahrs2 >= 1/9.0){
		comIntf.Write(RcvdMessages.GetAhrs2());
		msgScheduler.ahrs2 = time;
	    }

	    if(time - msgScheduler.ahrs3 >= 1/9.0){
		comIntf.Write(RcvdMessages.GetAhrs3());
		msgScheduler.ahrs3 = time;
	    }

	    if(time - msgScheduler.simstate >= 1/7.0){
		comIntf.Write(RcvdMessages.GetSimstate());
		msgScheduler.simstate = time;
	    }

	    if(time - msgScheduler.hwstatus >= 1/2.5){
		comIntf.Write(RcvdMessages.GetHwstatus());
		msgScheduler.hwstatus = time;
	    }

	    if(time - msgScheduler.ekf_status_report >= 1/2.5){
		comIntf.Write(RcvdMessages.GetEkfStatusReport());
		msgScheduler.ekf_status_report = time;
	    }

	    if(time - msgScheduler.vibration >= 1/2.5){
		comIntf.Write(RcvdMessages.GetVibration());
		msgScheduler.vibration = time;
	    }

	    msg_command_ack msgCommandAck = UAS.GetCmdError();
	    if(msgCommandAck != null){
		//System.out.println("Received ack");
		comIntf.Write(msgCommandAck);
	    }
	}
    }
    
    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }    

    public boolean hasError() {
	return error.hasError();
    }
    
    public boolean hasMessage() {
	return error.hasMessage();
    }
    
    public String getMessage() {
	return error.getMessage();
    }
    
    public String getMessageNoClear() {
	return error.getMessageNoClear();
    }
    
}
