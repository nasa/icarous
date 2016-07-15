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
import com.MAVLink.*;

public class COM implements Runnable{

    public Thread t;
    public String threadName;
    public AircraftData FlightData;
    public Interface comIntf;
        
    public COM(String name,Aircraft UAS){
	threadName       = name;
	FlightData       = UAS.FlightData;
	comIntf          = UAS.comIntf;
    }

    public void run(){

	while(true){
	
	    Read();

	    // Handle new flight plan inputs
	    if(FlightData.Inbox.UnreadFlightPlanUpdate()){
		FlightData.Inbox.ReadFlightPlanUpdate();		
		FlightData.UpdateFlightPlan(comIntf);
	    }

	    // Handle geo fence messages
	    if(FlightData.Inbox.UnreadGeoFenceUpdate()){
		FlightData.Inbox.ReadGeoFenceUpdate();		
		FlightData.GetNewGeoFence(comIntf);
	    }
	    
	    // Handle traffic information
	    if(FlightData.Inbox.UnreadTraffic()){
		msg_pointofinterest msg1 = FlightData.Inbox.Pointofinterest();
		FlightData.Inbox.ReadTraffic();
		
		FlightData.traffic.AddObject(msg1);
		System.out.print("Received traffic information");

		msg_command_acknowledgement msg2 = new msg_command_acknowledgement();
		msg2.acktype = 2;
		msg2.value = 1;	       
		comIntf.Write(msg2);
	    }
	    
	    // Handle obstacle information
	    if(FlightData.Inbox.UnreadObstacle()){
		msg_pointofinterest msg1 = FlightData.Inbox.Pointofinterest();
		FlightData.Inbox.ReadObstacle();
		FlightData.obstacles.AddObject(msg1);
		System.out.print("Received obstacle information");
		
		msg_command_acknowledgement msg2 = new msg_command_acknowledgement();
		msg2.acktype = 3;
		msg2.value = 1;		
		comIntf.Write(msg2);
		
	    }

	    // Handle other points of interest (mission related)
	    if(FlightData.Inbox.UnreadOthers()){
		msg_pointofinterest msg1 = FlightData.Inbox.Pointofinterest();
		FlightData.Inbox.ReadOthers();
		FlightData.missionObj.AddObject(msg1);
		System.out.print("Received mission object");
		
		msg_command_acknowledgement msg2 = new msg_command_acknowledgement();
		msg2.acktype = 4;
		msg2.value = 1;
		comIntf.Write(msg2);
		
		
	    }

	    // Handling mission commands (start/stop)
	    if(FlightData.Inbox.UnreadMissionStart()){
		FlightData.Inbox.ReadMissionStart();		
		FlightData.startMission = FlightData.Inbox.MissionStartStop().missionStart;				
		System.out.println("Received mission start");
	     		
	    }

	  
	}
    }

    
    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }

    public void Read(){

	MAVLinkPacket RcvdPacket = comIntf.Read();
	FlightData.Inbox.decode_message(RcvdPacket);
	
    }

    public boolean CheckCOMHeartBeat(){

	FlightData.Inbox.ReadHeartbeat_COM();
	
	comIntf.SetTimeout(5000);
	FlightData.Inbox.decode_message(comIntf.Read());
	comIntf.SetTimeout(0);

	if(FlightData.Inbox.UnreadHeartbeat_COM()){
	    return true;
	}
	else{
	    return false;
	}
    }
    
}
