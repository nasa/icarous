/**
 * ICAROUS Interface
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ICAROUS;

import java.util.*;
import java.lang.*;
import com.MAVLink.common.*;
import com.MAVLink.icarous.*;
import com.MAVLink.enums.*;


public class AircraftData{

    public enum FP_WRITE_AP{
	FP_CLR, FP_SEND_COUNT, FP_SEND_WP
    }

    public enum FP_READ_COM{
	FP_INFO, FP_WAYPT_INFO, FP_ACK_FAIL,FP_ACK_SUCCESS
    }

    
    public MAVLinkMessages Inbox;
    
    // Aircraft attitude
    public double roll;
    public double pitch;
    public double yaw;

    // Aircraft euler rates
    public double roll_rate;
    public double pitch_rate;
    public double yaw_rate;

    // Aircraft angular rates
    public double p;
    public double q;
    public double r;

    // Aircraft velocity components in the body frame
    public double u;
    public double v;
    public double w;

    // Aircraft's current position (GPS)
    public Position currPosition;

    // Angle of attack, sideslip and airspeed
    public double aoa;
    public double sideslip;
    public double airspeed;
    
    public FlightPlan NewFlightPlan;
    public FlightPlan CurrentFlightPlan;

    public GEOFENCES listOfFences;
    
    // List for obstacles
    public ObjectList obstacles;
    public ObjectList traffic;
    public ObjectList missionObj;
    
    // List for traffic information
    

    public int startMission = -1; // -1: last command executed, 0 - stop mission, 1 - start mission
    
    public AircraftData(){

	Inbox               = new MAVLinkMessages();
	currPosition        = new Position();
	CurrentFlightPlan   = new FlightPlan();
	listOfFences        = new GEOFENCES();
	obstacles           = new ObjectList();
	traffic             = new ObjectList();
	missionObj          = new ObjectList();
    }

    public void GetGPSdata(){

	double lat,lon,alt_msl,alt_agl;

	msg_global_position_int GPS = Inbox.GlobalPositionInt();
	
	lat = (double) (GPS.lat)/1.0E7;
	lon = (double) (GPS.lon)/1.0E7;
	alt_msl = (double) (GPS.alt)/1.0E3;
	alt_agl = (double) (GPS.relative_alt)/1.0E3;
	

	currPosition.UpdatePosition((float)lat,(float)lon,(float)alt_msl,(float)alt_agl);
    }

    // Function to send a flight plan to pixhawk
    public void SendFlightPlanToAP(Interface Intf){

	FP_WRITE_AP state = FP_WRITE_AP.FP_CLR;
	
	msg_mission_count msgMissionCount = new msg_mission_count();
	msg_mission_item msgMissionItem   = new msg_mission_item();
	msg_mission_clear_all msgMissionClearAll = new msg_mission_clear_all();
	boolean writeComplete = false;
	int count = 0;

	// Copy new flight into current flight plan
	CurrentFlightPlan.Copy(NewFlightPlan);

	msgMissionCount.target_system    = 0;
	msgMissionCount.target_component = 0;
	msgMissionItem.target_system     = 0;
	msgMissionItem.target_component  = 0;

	msgMissionClearAll.target_system    = 0;
	msgMissionClearAll.target_component = 0;
		
	while(!writeComplete){

	    switch(state){

	    case FP_CLR:
		Intf.Write(msgMissionClearAll);
		System.out.println("Cleared mission on AP");
		state = FP_WRITE_AP.FP_SEND_COUNT;
		
		break;

	    case FP_SEND_COUNT:
		
		msgMissionCount.count = CurrentFlightPlan.numWayPoints;

		Intf.Write(msgMissionCount);
		System.out.println("Wrote mission count: "+msgMissionCount.count);
		state = FP_WRITE_AP.FP_SEND_WP;
		break;
	    
	    case FP_SEND_WP:

		Inbox.decode_message(Intf.Read());
		
		try{
		    Thread.sleep(50);
		}
		catch(InterruptedException e){
		    System.out.println(e);
		}
		
		if(Inbox.UnreadMissionRequest()){
		    
		    Inbox.ReadMissionRequest();

		    int seq = Inbox.MissionRequest().seq;
		    
		    System.out.println("Received mission request: "+ seq );
		    
		    msgMissionItem.seq     = seq;
		    msgMissionItem.frame   = MAV_FRAME.MAV_FRAME_GLOBAL;
		    msgMissionItem.command = MAV_CMD.MAV_CMD_NAV_WAYPOINT;
		    msgMissionItem.current = 0;
		    msgMissionItem.autocontinue = 0;
		    msgMissionItem.param1  = 0.0f;
		    msgMissionItem.param2  = 0.0f;
		    msgMissionItem.param3  = 0.0f;
		    msgMissionItem.param4  = CurrentFlightPlan.GetWaypoint(msgMissionItem.seq).heading;
		    msgMissionItem.x       = CurrentFlightPlan.GetWaypoint(msgMissionItem.seq).pos.lat;
		    msgMissionItem.y       = CurrentFlightPlan.GetWaypoint(msgMissionItem.seq).pos.lon;
		    msgMissionItem.z       = CurrentFlightPlan.GetWaypoint(msgMissionItem.seq).pos.alt_msl;
		    
		    Intf.Write(msgMissionItem);
		    System.out.println("Wrote mission item");
		    count++;
		}
		
		if(Inbox.UnreadMissionAck()){

		    Inbox.ReadMissionAck();
		    
		    System.out.println("Received acknowledgement - type: "+Inbox.MissionAck().type);
		    
		    if(Inbox.MissionAck().type == 0){
			if(count == CurrentFlightPlan.numWayPoints){
			    System.out.println("Waypoints sent successfully");
			    writeComplete = true;
			}
			
		    }
		    else{
			state = FP_WRITE_AP.FP_CLR;
			System.out.println("Error in writing mission to AP");
		    }
		}
	    } // end of switch case
	}//end of while	
    }//end of function

    // Function to update new flight plan received from combox
    public void UpdateFlightPlan(Interface Intf){
	
	  boolean getFP        = true;
	  FP_READ_COM state    = FP_READ_COM.FP_INFO;
	  int count            = 0;

	  NewFlightPlan = new FlightPlan();
	  
	  while(getFP){
	      switch(state){
		  
	      case FP_INFO:
		  
		  msg_flightplan_info msg1 = Inbox.FlightplanInfo();

		  NewFlightPlan.FlightPlanInfo(msg1.numWaypoints,msg1.maxHorDev,msg1.maxVerDev,msg1.standoffDist);
		  
		  state = FP_READ_COM.FP_WAYPT_INFO;

		  System.out.println("Received flight plan info, Reading " +msg1.numWaypoints+" waypoints");
		  
		  break;
		  
	      case FP_WAYPT_INFO:

		  Intf.SetTimeout(500);
		  Inbox.decode_message(Intf.Read());
		  Intf.SetTimeout(0);

		  if(Inbox.UnreadWaypoint()){
		      Inbox.ReadWaypoint();
		      msg_pointofinterest msg2 = Inbox.Pointofinterest();
		      
		      if(msg2.id == 0 && msg2.index != count){
		      
			  state  = FP_READ_COM.FP_ACK_FAIL;
			  break;
		      }
		      
		      Waypoint wp          = new Waypoint(msg2.index,msg2.latx,msg2.lony,msg2.altz,msg2.heading);
		      
		      System.out.println("waypoint:"+count+" lat:"+wp.pos.lat+" lon:"+wp.pos.lon);
		      NewFlightPlan.AddWaypoints(count,wp);
		      
		      if(count == (NewFlightPlan.numWayPoints-1)){
			  state  = FP_READ_COM.FP_ACK_SUCCESS;
			  
		      }
		      else{
			  count++;
			  System.out.println("Receiving next waypoint");
		      }
		      
		  break;
		      
		  }
		  else{
		      state  = FP_READ_COM.FP_ACK_FAIL;
		      break;
		  }

		  

	      case FP_ACK_SUCCESS:

		  msg_command_acknowledgement msg3 = new msg_command_acknowledgement();

		  msg3.acktype = 0;
		  		 
		  getFP = false;

		  // Send acknowledgment
		  msg3.value = 1;
		  
		  Intf.Write(msg3);

		  System.out.println("Waypoints received successfully");
		  
		  break;  
		  
	      case FP_ACK_FAIL:

		  msg_command_acknowledgement msg4 = new msg_command_acknowledgement();

		  msg4.acktype = 0;		  
		  
		  getFP = false;

		  // Send acknowledgment
		  msg4.value = 0;
		  
		  Intf.Write(msg4);
		  
		  break;
		  
	      } // end of switch case
	  }// end of while
    }//end of function

    
}

class Position{

    float lat;
    float lon;
    float alt_msl;
    float alt_agl;

    public Position(){
	lat   = 0.0f;
	lon   = 0.0f;
	alt_msl   = 0.0f;
	alt_agl   = 0.0f;
    }

    public Position(float lat_in,float lon_in,float altmsl_in){
	lat   = lat_in;
	lon   = lon_in;
	alt_msl   = altmsl_in;
    }

    public Position(float lat_in,float lon_in,float altmsl_in,float altagl_in){
	lat     = lat_in;
	lon     = lon_in;
	alt_msl   = altmsl_in;
	alt_agl   = altagl_in;
    }

    public void UpdatePosition(float lat_in,float lon_in,float altmsl_in){
	lat   = lat_in;
	lon   = lon_in;
	alt_msl   = altmsl_in;
    }
    
    public void UpdatePosition(float lat_in,float lon_in,float altmsl_in,float altagl_in){
	lat   = lat_in;
	lon   = lon_in;
	alt_msl   = altmsl_in;
	alt_agl   = altagl_in;
    }

}



class Obstacle{

    int id;
    int type;
    Position pos;
    float vx;
    float vy;
    float vz;
    float orientation;

    public Obstacle(int id_in,int type_in,
		    float lat_in, float lon_in, float altmsl_in,
		    float orient_in,
		    float vx_in, float vy_in,float vz_in){
	pos = new Position(lat_in,lon_in,altmsl_in);
	id = id_in;
	type = type_in;
	orientation = orient_in;
	vx = vx_in;
	vy = vy_in;
	vz = vz_in;
    }
    
}

class ObjectList{

    public List<Obstacle> listofobjects;
    public int numObjects;

    public ObjectList(){
	numObjects = 0;
	listofobjects = new ArrayList<Obstacle>();
    }

    public void AddObject(int id,int type,
			  float lat, float lon, float altmsl,
			  float orient,float vx,float vy, float vz){
	Obstacle obj = new Obstacle(id,type,
				    lat,lon,altmsl,
				    orient,vx,vy,vz);
	listofobjects.add(obj);
	System.out.println("Adding obstacle "+id+" at lat:"+lat+" lon: "+lon);
    }

    public void AddObject(msg_pointofinterest msg){

	this.AddObject(msg.index,msg.subtype,msg.latx,msg.lony,msg.altz,msg.heading,msg.vx,msg.vy,msg.vz);

    }

    public void RemoveObject(int id){

	Obstacle obj;
	Iterator Itr = listofobjects.iterator();
	
	while(Itr.hasNext()){
	    obj = (Obstacle) Itr.next();
	    
	    if(obj.id == id){
		Itr.remove();
		numObjects = listofobjects.size();
		break;   
	    }
	}
    }
}


