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

    public static boolean INIT_MESSAGES = true;
    public static boolean NO_MESSAGES   = false;
    
    public MAVLinkMessages RcvdMessages;

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

    // Aircraft position (GPS)
    public Position aircraftPosition;

    // Angle of attack, sideslip and airspeed
    public double aoa;
    public double sideslip;
    public double airspeed;
    
    public FlightPlan NewFlightPlan;
    public FlightPlan CurrentFlightPlan;

    public GEOFENCES listOfFences;
    
    // List for obstacles
    // List for traffic information

    public int startMission = -1; // -1: last command executed, 0 - stop mission, 1 - start mission
    
    public AircraftData(boolean msg_requirement){
	if(msg_requirement){
	    RcvdMessages = new MAVLinkMessages();
	}

	aircraftPosition    = new Position();
	CurrentFlightPlan   = new FlightPlan();
	listOfFences        = new GEOFENCES();
    }

    public void CopyAircraftStateInfo(AircraftData Input){

	roll  = Input.roll;
	pitch = Input.pitch;
	yaw   = Input.yaw;

	roll_rate  = Input.roll_rate;
	pitch_rate = Input.pitch_rate;
	yaw_rate   = Input.yaw_rate;

	p = Input.p;
	q = Input.q;
	r = Input.r;

	u = Input.u;
	v = Input.v;
	w = Input.w;

	aircraftPosition.UpdatePosition(Input.aircraftPosition.lat,
					Input.aircraftPosition.lon,
					Input.aircraftPosition.alt_msl,
					Input.aircraftPosition.alt_agl);

	aoa      = Input.aoa;
	sideslip = Input.sideslip;
	airspeed = Input.airspeed;
    }

    public void GetDataFromMessages(){

	double lat = (double) (RcvdMessages.msgGlobalPositionInt.lat)/1.0E7;
	double lon = (double) (RcvdMessages.msgGlobalPositionInt.lon)/1.0E7;
	double alt_msl = (double) (RcvdMessages.msgGlobalPositionInt.alt)/1.0E3;
	double alt_agl = (double) (RcvdMessages.msgGlobalPositionInt.relative_alt)/1.0E3;

	aircraftPosition.UpdatePosition((float)lat,(float)lon,(float)alt_msl,(float)alt_agl);
    }

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

    public void UpdatePosition(float lat_in,float lon_in,float altmsl_in,float altagl_in){
	lat   = lat_in;
	lon   = lon_in;
	alt_msl   = altmsl_in;
	alt_agl   = altagl_in;
    }

}

class Waypoint extends Position{

    int id;
    float heading;

    public Waypoint(){
	super();
	id        =0;
	heading   =0.0f;
    }

    public Waypoint(int id_in,float lat_in,float lon_in,float altmsl_in,
		    float heading_in){
	super(lat_in,lon_in,altmsl_in);
	id        = id_in;
	heading   = heading_in;
    }

    public Waypoint(Waypoint wp){
	super(wp.lat,wp.lon,wp.alt_msl,wp.alt_agl);
	id = wp.id;
	heading = wp.heading; 

    }
        
}

class Obstacle extends Position{

    int id;

    public Obstacle(int id_in,float lat_in, float lon_in, float altmsl_in){
	super(lat_in,lon_in,altmsl_in);
	id = id_in;
    }
    
}

class FlightPlan{

    public enum FP_READ_COM{
	FP_INFO, FP_WAYPT_INFO, FP_ACK_FAIL,FP_ACK_SUCCESS
    }

    public enum FP_WRITE_AP{
	FP_CLR, FP_SEND_COUNT, FP_SEND_WP
    }
    
    public List<Waypoint> wayPoints;
    public int numWayPoints;
    public float maxHorDev;
    public float maxVerDev;
    public float standOffDist;
    public int nextWaypoint;
    public Iterator wpIt = null;

    public FlightPlan(){
	wayPoints    = new ArrayList<Waypoint>();
    }
    
    public void FlightPlanInfo(int num,float HorDev, float VerDev, float standDist){

	numWayPoints = num;
	maxHorDev    = HorDev;
	maxVerDev    = VerDev;
	standOffDist = standDist;
    }

    public void AddWaypoints(int index,Waypoint wp){
	wayPoints.add(index,wp);

	if(wpIt != null){
	    wpIt = wayPoints.iterator();
	}
    }

    public Waypoint GetWaypoint(int index){
	return (Waypoint) wayPoints.get(index);
    }

    public void Copy(FlightPlan newFP){

	numWayPoints = newFP.numWayPoints;
	maxHorDev    = newFP.maxHorDev;
	maxVerDev    = newFP.maxVerDev;
	standOffDist = newFP.standOffDist;

	for(int i=0;i<numWayPoints;i++){
	    Waypoint wp = new Waypoint(newFP.GetWaypoint(i));
	    this.AddWaypoints(i,wp);
	}
    }

    public double[] Distance2Waypoint(Position currentPos,Waypoint wp){

	double Dist[] = new double[2];
	
	double lat1 = currentPos.lat;
	double lon1 = currentPos.lon;

	double lat2 = wp.lat;
	double lon2 = wp.lon;

	double d2r  = Math.PI/180;
	double r2d  = 180/Math.PI;
	
	double R    = 6371.0f;                                                           
	double dLat = (lat2-lat1)*d2r;
	double dLon = (lon2-lon1)*d2r;
	lat1 = lat1*d2r;
	lat2 = lat2*d2r;
	
	double a    = Math.sin(dLat/2) * Math.sin(dLat/2) + Math.sin(dLon/2) * Math.sin(dLon/2) * Math.cos(lat1) * Math.cos(lat2);
	double c    = 2 * Math.atan2(Math.sqrt(a),Math.sqrt(1-a));
	double D    = R * c;
	
	double y = Math.sin(dLon) * Math.cos(lat2);
	double x = Math.cos(lat1)*Math.sin(lat2) - Math.sin(lat1)*Math.cos(lat2)*Math.cos(dLon);
	double H = Math.atan2(y, x)*r2d;

	Dist[0] = D;
	Dist[1] = H;

	return Dist;
	
    }

    // Function to send a flight plan to pixhawk
    public void SendFlightPlanToAP(ICAROUS_Interface Intf){

	AircraftData SharedData = Intf.SharedData;
	MAVLinkMessages RcvdMessages = SharedData.RcvdMessages;
	
	FP_WRITE_AP state = FP_WRITE_AP.FP_CLR;
	
	msg_mission_count msgMissionCount = new msg_mission_count();
	msg_mission_item msgMissionItem   = new msg_mission_item();
	msg_mission_clear_all msgMissionClearAll = new msg_mission_clear_all();
	boolean writeComplete = false;
	int count = 0;

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
		
		msgMissionCount.count = numWayPoints;

		Intf.Write(msgMissionCount);
		System.out.println("Wrote mission count: "+numWayPoints);
		state = FP_WRITE_AP.FP_SEND_WP;
		break;
	    
	    case FP_SEND_WP:

		Intf.Read();
		
		try{
		    Thread.sleep(50);
		}
		catch(InterruptedException e){
		    System.out.println(e);
		}
		
		if(RcvdMessages.RcvdMissionRequest == 1){
		    synchronized(SharedData){
			RcvdMessages.RcvdMissionRequest = 0;
		    }

		    System.out.println("Received mission request: "+ RcvdMessages.msgMissionRequest.seq);
		    
		    msgMissionItem.seq     = RcvdMessages.msgMissionRequest.seq;
		    msgMissionItem.frame   = MAV_FRAME.MAV_FRAME_GLOBAL;
		    msgMissionItem.command = MAV_CMD.MAV_CMD_NAV_WAYPOINT;
		    msgMissionItem.current = 0;
		    msgMissionItem.autocontinue = 0;
		    msgMissionItem.param1  = 0.0f;
		    msgMissionItem.param2  = 0.0f;
		    msgMissionItem.param3  = 0.0f;
		    msgMissionItem.param4  = GetWaypoint(msgMissionItem.seq).heading;
		    msgMissionItem.x       = GetWaypoint(msgMissionItem.seq).lat;
		    msgMissionItem.y       = GetWaypoint(msgMissionItem.seq).lon;
		    msgMissionItem.z       = GetWaypoint(msgMissionItem.seq).alt_msl;
		    
		    Intf.Write(msgMissionItem);
		    System.out.println("Wrote mission item");
		    count++;
		}
		
		if(RcvdMessages.RcvdMissionAck == 1){

		    synchronized(SharedData){
			RcvdMessages.RcvdMissionAck = 0;
		    }
		    
		    System.out.println("Received acknowledgement - type: "+RcvdMessages.msgMissionAck.type);
		    
		    if(RcvdMessages.msgMissionAck.type == 0){
			if(count == numWayPoints){
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
    public void UpdateFlightPlan(ICAROUS_Interface Intf){
	
	  boolean getFP        = true;
	  FP_READ_COM state1   = FP_READ_COM.FP_INFO;
	  int count            = 0;
	  AircraftData SharedData = Intf.SharedData;
	  
	  while(getFP){
	      switch(state1){
		  
	      case FP_INFO:
		  
		  msg_flightplan_info msg1 = SharedData.RcvdMessages.msgFlightplanInfo;

		  this.FlightPlanInfo(msg1.numWaypoints,msg1.maxHorDev,msg1.maxVerDev,msg1.standoffDist);
		  
		  state1 = FP_READ_COM.FP_WAYPT_INFO;

		  System.out.println("Received flight plan info, Reading " +msg1.numWaypoints+" waypoints");
		  
		  break;
		  
	      case FP_WAYPT_INFO:

		  Intf.SetTimeout(500);
		  Intf.Read();
		  Intf.SetTimeout(0);

		  if(SharedData.RcvdMessages.RcvdWaypoint == 1){
		      msg_pointofinterest msg2 = SharedData.RcvdMessages.msgPointofinterest;
		      SharedData.RcvdMessages.RcvdWaypoint = 0;

		      if(msg2.id == 0 && msg2.index != count){
		      
			  state1  = FP_READ_COM.FP_ACK_FAIL;
			  break;
		      }
		      
		      Waypoint wp          = new Waypoint(msg2.index,msg2.lat,msg2.lon,msg2.alt,msg2.heading);
		      
		      System.out.println("waypoint:"+count+" lat:"+wp.lat+" lon:"+wp.lon);
		      this.AddWaypoints(count,wp);
		      
		      if(count == (this.numWayPoints-1)){
			  state1  = FP_READ_COM.FP_ACK_SUCCESS;
			  
		      }
		      else{
			  count++;
			  System.out.println("Receiving next waypoint");
		      }
		      
		  break;
		      
		  }
		  else{
		      state1  = FP_READ_COM.FP_ACK_FAIL;
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


class GeoFence extends Position{

    public int Type;
    public int ID;
    public int numVertices;
    public double floor;
    public double ceiling;
    
    List<Position> Vertices;

    public GeoFence(int IDIn,int TypeIn,int numVerticesIn,double floorIn,double ceilingIn){
	Vertices = new ArrayList<Position>();
	Type     = TypeIn;
	ID       = IDIn;
	numVertices = numVerticesIn;
	floor     = floorIn;
	ceiling   = ceilingIn;
    }

    public void AddVertex(int index,float lat,float lon){
	Position pos = new Position(lat,lon,0);
	Vertices.add(index,pos);
    }

    public void print(){
	System.out.println("Type: "+Type);
	System.out.println("ID:" + ID);
	System.out.println("Num vertices:"+numVertices);
	System.out.println("Floor:"+floor);
	System.out.println("Ceiling:"+ceiling);
	System.out.println("Vertices information");
	for(int i=0;i<numVertices;i++){
	    Position vertex = Vertices.get(i);
	    System.out.println("Lat :"+vertex.lat);
	    System.out.println("Lon :"+vertex.lon);
	}

    }
    
}

class GEOFENCES{

    enum FENCESTATE{
	IDLE,INFO,VERTICES,ACK_FAIL,ACK_SUCCESS,UPDATE,REMOVE
    }
    
    public List<GeoFence> fenceList;
    FENCESTATE state;
    int numFences;
    
    public GEOFENCES(){
	fenceList  = new ArrayList<GeoFence>();
	state = FENCESTATE.IDLE;
	numFences = 0;
    }

    public GeoFence GetGeoFence(int index){

	int size = fenceList.size();
	
	if(index > 0){
	    return (GeoFence) fenceList.get(index);
	}
	else{
	    return (GeoFence) fenceList.get(size-1);
	}
    }


    public void GetNewGeoFence(ICAROUS_Interface Intf){

	GeoFence fence1 = null;
	int count = 0;
	boolean getfence = true;

	MAVLinkMessages RcvdMessages = Intf.SharedData.RcvdMessages;
	
	msg_geofence_info msg1 = RcvdMessages.msgGeofenceInfo;;

	if(msg1.msgType == 0){
	    state = FENCESTATE.INFO;
	    System.out.println("Adding fence");
	}
	else{
	    state = FENCESTATE.REMOVE;
	    System.out.println("Removing fence");
	}
	
	while(getfence){    

	    switch(state){
	    
	    case INFO:
	    
		fence1 = new GeoFence(msg1.fenceID,msg1.fenceType,msg1.numVertices,msg1.fenceFloor,msg1.fenceCeiling);
		System.out.println("Received geofence information: "+msg1.fenceCeiling);
		state = FENCESTATE.VERTICES;
		break;

	    case VERTICES:

		Intf.SetTimeout(500);
		Intf.Read();
		Intf.SetTimeout(0);
		
		if(RcvdMessages.RcvdVertex == 1){
		    msg_pointofinterest msg2 = RcvdMessages.msgPointofinterest;
		    RcvdMessages.RcvdVertex = 0;
		
		    if(msg2.id == 1 && msg2.index != count){
		      
			state  =  FENCESTATE.ACK_FAIL;
			break;
		    }

		    System.out.println("Adding vertex :"+count);
		    fence1.AddVertex(msg2.index,msg2.lat,msg2.lon);
		    count++;

		    
		    if(count == fence1.numVertices){
			state = FENCESTATE.UPDATE;
			break;
		    }
		}

		break;

	    case UPDATE:

		fenceList.add(fence1);
		GeoFence gf = this.GetGeoFence(-1);
		gf.print();
		System.out.println("Updated fence list");
		numFences = fenceList.size();
		System.out.println("Total fences in ICAROUS:"+numFences);
		state = FENCESTATE.ACK_SUCCESS;
		
		break;

	    case REMOVE:

		Iterator Itr = fenceList.iterator();

		while(Itr.hasNext()){
		    GeoFence f1 = (GeoFence) Itr.next();

		    if(f1.ID == msg1.fenceID){
			Itr.remove();
			numFences = fenceList.size();
			System.out.println("Total fences in ICAROUS:"+numFences);
			break;   
		    }
		}

		break;

	       
	    case ACK_FAIL:

		msg_command_acknowledgement msg4 = new msg_command_acknowledgement();

		msg4.acktype = 1;		  
		
		getfence = false;
		
		// Send acknowledgment
		msg4.value = 0;
		
		Intf.Write(msg4);
		
		break;

	    case ACK_SUCCESS:

		msg_command_acknowledgement msg5 = new msg_command_acknowledgement();

		msg5.acktype = 1;		  
		
		getfence = false;
		
		// Send acknowledgment
		msg5.value = 1;
		
		Intf.Write(msg5);

		state = FENCESTATE.UPDATE;
		
		break;
		
	    }//end of switch
	}//end of while
	    
    }//end of function
    
    
    
    
}
