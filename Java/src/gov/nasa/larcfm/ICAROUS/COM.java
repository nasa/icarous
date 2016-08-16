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
    public MAVLinkMessages RcvdMessages;
    
        
    public COM(String name,Aircraft uas){
	threadName       = name;
	UAS              = uas;
	FlightData       = UAS.FlightData;
	comIntf          = UAS.comIntf;
	error            = new ErrorLog("COM     ");
	RcvdMessages     = FlightData.Inbox;
    }

    public void run(){

	msg_heartbeat msghb = new msg_heartbeat();
	
	msghb.type      = MAV_TYPE.MAV_TYPE_FIXED_WING;
	
	comIntf.SetTimeout(100);

	double time1 = UAS.timeCurrent;

	int count = 0;
	
	while(true){	    
	    
	    double time2 = UAS.timeCurrent;
	    
	    // Send AP heartbeat if available
	    msg_heartbeat msgHeartbeat_AP = RcvdMessages.GetHeartbeat_AP();
	    if(msgHeartbeat_AP != null){
		comIntf.Write(msgHeartbeat_AP);		    
	    }
	    	    	    
	    comIntf.Read();
	    	    
	    String timeLog = UAS.timeLog;

	    // Handle mission waypoints / geofence points
	    msg_mission_count msgMissionCount = RcvdMessages.GetMissionCount();	    
	    if(msgMissionCount != null){		
		//System.out.println("Handling new waypoints");
		FlightData.GetWaypoints(comIntf,0,0,msgMissionCount.count,FlightData.InputFlightPlan);
		error.addWarning("[" + timeLog + "] MSG: Got waypoints");				
	    }

	    // Handle mission request list
	    msg_mission_request_list msgMissionRequestList = RcvdMessages.GetMissionRequestList();
	    if(msgMissionRequestList != null){
		System.out.println("Handling mission list request");
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
	    msg_param_request_list msgParamRequestList = RcvdMessages.GetParamRequestList();
	    if(msgParamRequestList != null){
		System.out.println("Handling parameter request list");		
		UAS.apIntf.Write(msgParamRequestList);		
		msg_param_value msgParamValue = null;
		
		while(msgParamValue  == null){
		    UAS.apIntf.Read();
		    msgParamValue = RcvdMessages.GetParamValue();
		}
						
		int param_count = msgParamValue.param_count;

		double time_param_read_start  = UAS.timeCurrent;
		for(int i=0;i<param_count;i++){
		    //System.out.println("count:"+i);
		    comIntf.Write(msgParamValue);		    
		    msgParamValue = FlightData.Inbox.GetParamValue();
		    
		    if(i<param_count-1){
			double time_elapsed = 0;			
			while(msgParamValue  == null){
			    UAS.apIntf.Read();
			    msgParamValue = FlightData.Inbox.GetParamValue();
		        }						
		    }
		}	
		
	    }

	    // Handle parameter read requests
	    msg_param_request_read msgParamRequestRead = RcvdMessages.GetParamRequestRead();
	    if(msgParamRequestRead != null){
		UAS.apIntf.Write(msgParamRequestRead);
	    }

	    // Handle parameter value
	    msg_param_value msgParamValue = RcvdMessages.GetParamValue();
	    if( msgParamValue != null){
		// Handle parameter value

		//System.out.println("Handle parameter value");
		if(msgParamValue.sysid == 1){
		    //System.out.printf("Received parameter from AP");
		    comIntf.Write(msgParamValue);		
		}
		else{
		    UAS.apIntf.Write(msgParamValue);
		    //System.out.println("Writing to AP");
		}

		
		
	    }

	    // Handle parameter set
	    msg_param_set msgParamSet = RcvdMessages.GetParamSet();
	    if( msgParamSet != null ){			
		UAS.apIntf.Write(msgParamSet);		
	    }

	    // Hangle commands
	    msg_command_long msgCommandLong = RcvdMessages.GetCommandLong();
	    if( msgCommandLong != null ){
		
		if(msgCommandLong.command == MAV_CMD.MAV_CMD_DO_FENCE_ENABLE){
		    FlightData.GetGeoFence(comIntf,msgCommandLong);
		}
		else if(msgCommandLong.command == MAV_CMD.MAV_CMD_MISSION_START){
		    if(msgCommandLong.param1 == 1){
			FlightData.startMission = 1;
			error.addWarning("[" + timeLog + "] MSG: Received Mission START");
		    }
		}
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
