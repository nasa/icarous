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
import gov.nasa.larcfm.Util.ParameterData;

public class COM implements Runnable,ErrorReporter{

    public Thread t;
    public String threadName;
    public Aircraft UAS;
    public AircraftData FlightData;
    public Interface comIntf;
    public ErrorLog error;
    public MAVLinkMessages RcvdMessages;
    private ParameterData pData;
        
    public COM(String name,Aircraft uas,ParameterData pdata){
	threadName       = name;
	UAS              = uas;
	FlightData       = UAS.FlightData;
	comIntf          = UAS.comIntf;
	error            = new ErrorLog("COM     ");
	RcvdMessages     = FlightData.Inbox;
	pData            = pdata;
    }

    public void run(){

	msg_heartbeat msghb = new msg_heartbeat();
	
	msghb.type      = MAV_TYPE.MAV_TYPE_FIXED_WING;
	
	comIntf.SetTimeout(1);

	double time1 = UAS.timeCurrent;

	int count = 0;
	
	while(true){	    
	    
	    double time2 = UAS.timeCurrent;
	    
	    // Send AP heartbeat if available
	    msg_heartbeat msgHeartbeat_AP = RcvdMessages.GetHeartbeat_AP();
	    if(msgHeartbeat_AP != null){
		comIntf.Write(msgHeartbeat_AP);
		comIntf.Write(RcvdMessages.GpsRawInt());
		comIntf.Write(RcvdMessages.Attitude());
		comIntf.Write(RcvdMessages.GlobalPositionInt());
		comIntf.Write(RcvdMessages.MissionCurrent());
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
		UAS.EnableDataStream(0);
		synchronized(UAS.apIntf){		    
		    UAS.apIntf.Write(msgParamRequestList);		
		    msg_param_value msgParamValue = null;
		
		    while(msgParamValue  == null){
			UAS.apIntf.Read();
			msgParamValue = RcvdMessages.GetParamValue();
		    }

		    int param_count1 = msgParamValue.param_count;
		    
		    double time_param_read_start  = UAS.timeCurrent;
		    for(int i=0;i<param_count1;i++){
			//System.out.println("count:"+i);
			msgParamValue.param_count = param_count1;
			comIntf.Write(msgParamValue);		    
			msgParamValue = FlightData.Inbox.GetParamValue();
		    
			if(i<param_count1-1){
			    double time_elapsed = 0;			
			    while(msgParamValue  == null){
				UAS.apIntf.Read();
				msgParamValue = FlightData.Inbox.GetParamValue();
			    }						
			}
		    }
		  
		}
		System.out.println("Updated parameter list");
		UAS.EnableDataStream(1);
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
		    pData.set(ID,msgParamSet.param_value,pData.getUnit(ID));
		    System.out.println(ID+": "+pData.getValue(ID));
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

	    // Hangle commands
	    msg_command_long msgCommandLong = RcvdMessages.GetCommandLong();
	    if( msgCommandLong != null ){
		
		if(msgCommandLong.command == MAV_CMD.MAV_CMD_DO_FENCE_ENABLE){

		    int status;
		    status = FlightData.GetGeoFence(comIntf,msgCommandLong);
		    if(status == 1){
			error.addWarning("[" + timeLog + "] MSG: Geo fence update   ");
		    }
		}
		else if(msgCommandLong.command == MAV_CMD.MAV_CMD_MISSION_START){
		    if(msgCommandLong.param1 == 1){
			FlightData.startMission = 1;
			error.addWarning("[" + timeLog + "] MSG: Received Mission START");
		    }
		}

		if(msgCommandLong.command == MAV_CMD.MAV_CMD_SPATIAL_USER_1){
		    GenericObject obj = new GenericObject(0,(int)msgCommandLong.param1,
							  msgCommandLong.param5,msgCommandLong.param6,msgCommandLong.param7,
							  msgCommandLong.param2,msgCommandLong.param3,msgCommandLong.param4);

		    synchronized(FlightData.traffic){
			GenericObject.AddObject(FlightData.traffic,obj);
		    }

		}
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
