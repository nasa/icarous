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
		comIntf.Write(msghb);
	    }

	    
	    comIntf.Read();
	    

	    String timeLog = UAS.timeLog;

	    if(FlightData.Inbox.UnreadMissionCount()){
		FlightData.Inbox.ReadMissionCount();
		msg_mission_count msgMissionCount = FlightData.Inbox.MissionCount();
		FlightData.GetWaypoints(comIntf,0,0,msgMissionCount.count,FlightData.InputFlightPlan);
		error.addWarning("[" + timeLog + "] MSG: Got waypoints");
	    }

	    if(FlightData.Inbox.UnreadMissionRequestList()){
		FlightData.Inbox.ReadMissionRequestList();

		if(FlightData.InputFlightPlan.size() > 0){
		    FlightData.SendWaypoints(comIntf,FlightData.InputFlightPlan);
		}
		error.addWarning("[" + timeLog + "] MSG: Sent waypoints");
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
