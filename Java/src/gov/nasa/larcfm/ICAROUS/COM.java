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
 */
package gov.nasa.larcfm.ICAROUS;

import com.MAVLink.icarous.*;
import com.MAVLink.common.*;
import com.MAVLink.enums.*;
import com.MAVLink.*;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;

public class COM implements Runnable,ErrorReporter{

    public Thread t;
    public String threadName;
    public Aircraft UAS;
    public AircraftData FlightData;
    public Interface comIntf;
    public ErrorLog error;
    
        
    public COM(String name,Aircraft uas){
	threadName       = name;
	UAS              = uas;
	FlightData       = UAS.FlightData;
	comIntf          = UAS.comIntf;
	error            = new ErrorLog("COM     ");
    }

    public void run(){

	msg_heartbeat msghb = new msg_heartbeat();
	
	msghb.type      = MAV_TYPE.MAV_TYPE_ONBOARD_CONTROLLER;
	msghb.autopilot = MAV_AUTOPILOT.MAV_AUTOPILOT_GENERIC;
	
	comIntf.SetTimeout(500);

	double time1 = UAS.timeCurrent;
	
	while(true){	    

	    
	    double time2 = UAS.timeCurrent;
	    //System.out.println(time2-time1);
	    if( (time2 - time1)/1E9 >= 2){
		time1 = time2;
		//System.out.println("Sending heartbeat\n");
		if(FlightData.Inbox.UnreadHeartbeat_AP()){
		    FlightData.Inbox.ReadHeartbeat_AP(); 
		    comIntf.Write(FlightData.Inbox.Heartbeat_AP());
		    comIntf.Write(FlightData.Inbox.msgSysStatus);
		    comIntf.Write(FlightData.Inbox.msgSystemTime);
		    //comIntf.Write(FlightData.Inbox.msgStatustext);
		    comIntf.Write(FlightData.Inbox.msgRawImu);
		    comIntf.Write(FlightData.Inbox.msgScaledPressure);
		    comIntf.Write(FlightData.Inbox.msgAttitude);
		    comIntf.Write(FlightData.Inbox.msgLocalPositionNed);
		    comIntf.Write(FlightData.Inbox.msgGlobalPositionInt);
		    comIntf.Write(FlightData.Inbox.msgRcChannelsRaw);
		    comIntf.Write(FlightData.Inbox.msgServoOutputRaw);
		    comIntf.Write(FlightData.Inbox.msgMissionCurrent);
		    comIntf.Write(FlightData.Inbox.msgNavControllerOutput);
		    comIntf.Write(FlightData.Inbox.msgRcChannels);
		    comIntf.Write(FlightData.Inbox.msgVfrHud);
		    comIntf.Write(FlightData.Inbox.msgScaledImu2);
		    comIntf.Write(FlightData.Inbox.msgPowerStatus);
		    comIntf.Write(FlightData.Inbox.msgTerrainRequest);
		    comIntf.Write(FlightData.Inbox.msgTerrainReport);
		    comIntf.Write(FlightData.Inbox.msgSimState);
		    comIntf.Write(FlightData.Inbox.msgVibration);
		    //comIntf.Write(FlightData.Inbox.msgGpsStatus);
		    comIntf.Write(FlightData.Inbox.msgGpsRawInt);
		    comIntf.Write(FlightData.Inbox.msgStatustext);
		    comIntf.Write(FlightData.Inbox.msgMissionItem);
		    comIntf.Write(FlightData.Inbox.msgMissionCount);
		    comIntf.Write(FlightData.Inbox.msgSimstate);
		    comIntf.Write(FlightData.Inbox.msgAhrs);
		    comIntf.Write(FlightData.Inbox.msgAhrs2);
		    comIntf.Write(FlightData.Inbox.msgAhrs3);
		    comIntf.Write(FlightData.Inbox.msgEkfStatusReport);
		    comIntf.Write(FlightData.Inbox.msgHwstatus);
		    comIntf.Write(FlightData.Inbox.msgMeminfo);
		    comIntf.Write(FlightData.Inbox.msgParamValue);
		}
	    }
	    
	    
	    comIntf.Read();
	    
	    
	    String timeLog = UAS.timeLog;

	    // Handle mission waypoints
	    if(FlightData.Inbox.UnreadMissionCount()){
		FlightData.Inbox.ReadMissionCount();
		msg_mission_count msgMissionCount = FlightData.Inbox.MissionCount();
		FlightData.GetWaypoints(comIntf,0,0,msgMissionCount.count,FlightData.InputFlightPlan);
		error.addWarning("[" + timeLog + "] MSG: Got waypoints");
	    }

	    // Handle mission request list
	    if(FlightData.Inbox.UnreadMissionRequestList()){
		FlightData.Inbox.ReadMissionRequestList();

		if(FlightData.InputFlightPlan.size() > 0){
		    FlightData.SendWaypoints(comIntf,FlightData.InputFlightPlan);
		}
		else{
		    msg_mission_ack MsgMissionAck = new msg_mission_ack();

		    MsgMissionAck.type = MAV_MISSION_RESULT.MAV_MISSION_ERROR;
		}
		//error.addWarning("[" + timeLog + "] MSG: Sent waypoints");
	    }

	    // Handle parameter requests
	    
	    if(FlightData.Inbox.UnreadParamRequestList()){
		FlightData.Inbox.ReadParamRequestList();
		System.out.println("Received parameter request list");
		msg_param_request_list MsgParamRequestList = FlightData.Inbox.ParamRequestList();

		
		MsgParamRequestList.target_system = 0;
		MsgParamRequestList.target_component = 0;
		System.out.println(MsgParamRequestList.target_system);
		System.out.println(MsgParamRequestList.target_component);
		UAS.apIntf.Write(MsgParamRequestList);

		
		System.out.println("Wrote request list");
		while(!FlightData.Inbox.UnreadParamValue()){
		    UAS.apIntf.Read();
		}
		
		FlightData.Inbox.ReadParamValue();

		msg_param_value MsgParamValue = FlightData.Inbox.ParamValue();
		int param_count = MsgParamValue.param_count;

		double time_param_read_start  = UAS.timeCurrent;
		for(int i=0;i<param_count;i++){
		    
		    comIntf.Write(MsgParamValue);

		    //System.out.println("Wrote parameter:"+i);
		    		    
		    if(i<param_count-1){
			double time_elapsed = 0;
			while(!FlightData.Inbox.UnreadParamValue())
			    {
				double time_param_read_now = UAS.timeCurrent;
				time_elapsed = (time_param_read_now - time_param_read_start)/1E9;		    
				UAS.apIntf.Read();
				if(time_elapsed > 1){
				    
				    break;
				}
			    }
			
			FlightData.Inbox.ReadParamValue();
		
			MsgParamValue = FlightData.Inbox.ParamValue();
		    }
		}

		System.out.println("Received parameter\n");
		
		
	    }

	    if(FlightData.Inbox.UnreadParamRequestRead()){
		FlightData.Inbox.ReadParamRequestRead();

		msg_param_request_read MsgParamRequestRead = FlightData.Inbox.ParamRequestRead();
		MsgParamRequestRead.target_system = 0;
		MsgParamRequestRead.target_component = 0;
		//System.out.println("Requesting:"+MsgParamRequestRead.param_index);
		UAS.apIntf.Write(MsgParamRequestRead);
	    }

	    if(FlightData.Inbox.UnreadParamValue()){
		FlightData.Inbox.ReadParamValue();
		msg_param_value MsgParamValue = FlightData.Inbox.ParamValue();
		comIntf.Write(MsgParamValue);
		//System.out.println("Wrote param value outside");
	    }
	    
	    
	    // Handle new flight plan inputs
	    if(FlightData.Inbox.UnreadFlightPlanUpdate()){
		FlightData.Inbox.ReadFlightPlanUpdate();		
		FlightData.UpdateFlightPlan(comIntf);
		error.addWarning("[" + timeLog + "] MSG: Flight plan update");
	    }

	    // Handle geo fence messages
	    if(FlightData.Inbox.UnreadGeoFenceUpdate()){
		FlightData.Inbox.ReadGeoFenceUpdate();		
		FlightData.GetNewGeoFence(comIntf);
		error.addWarning("[" + timeLog + "] MSG: Geo fence update   ");
	    }
	    
	    // Handle traffic information
	    if(FlightData.Inbox.UnreadTraffic()){
		msg_pointofinterest msg1 = FlightData.Inbox.Pointofinterest();
		FlightData.Inbox.ReadTraffic();
		
		GenericObject.AddObject(FlightData.traffic,msg1);
	        error.addWarning("[" + timeLog + "] MSG: Traffic update");

		msg_command_acknowledgement msg2 = new msg_command_acknowledgement();
		msg2.acktype = 2;
		msg2.value = 1;	       
		comIntf.Write(msg2);
	    }
	    
	    // Handle obstacle information
	    if(FlightData.Inbox.UnreadObstacle()){
		msg_pointofinterest msg1 = FlightData.Inbox.Pointofinterest();
		FlightData.Inbox.ReadObstacle();

		GenericObject.AddObject(FlightData.obstacles,msg1);
		error.addWarning("[" + timeLog + "] MSG: Obstacle update");
		
		msg_command_acknowledgement msg2 = new msg_command_acknowledgement();
		msg2.acktype = 3;
		msg2.value = 1;		
		comIntf.Write(msg2);
		
	    }

	    // Handle other points of interest (mission related)
	    if(FlightData.Inbox.UnreadOthers()){
		msg_pointofinterest msg1 = FlightData.Inbox.Pointofinterest();
		FlightData.Inbox.ReadOthers();

		GenericObject.AddObject(FlightData.missionObj,msg1);
		error.addWarning("[" + timeLog + "] MSG: Mission object update");
		
		msg_command_acknowledgement msg2 = new msg_command_acknowledgement();
		msg2.acktype = 4;
		msg2.value = 1;
		comIntf.Write(msg2);
		
		
	    }

	    // Handling mission commands (start/stop)
	    if(FlightData.Inbox.UnreadMissionStart()){
		FlightData.Inbox.ReadMissionStart();		
		FlightData.startMission = FlightData.Inbox.MissionStartStop().missionStart;				
		error.addWarning("[" + timeLog + "] MSG: Received Mission START/STOP");
	     		
	    }

	    
	}
    }

    
    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }



    public boolean CheckCOMHeartBeat(){

	FlightData.Inbox.ReadHeartbeat_COM();
	
	comIntf.Read();
	
	if(FlightData.Inbox.UnreadHeartbeat_COM()){
	    return true;
	}
	else{
	    return false;
	}
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
