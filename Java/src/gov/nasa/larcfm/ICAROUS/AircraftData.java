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

    boolean sendmsg;

    public FlightPlan NewFlightPlan;
    public FlightPlan CurrentFlightPlan;
    
    List Obstacles = new ArrayList(); // List for obstacles
    List Traffic   = new ArrayList(); // List for traffic information

    public int startMission = -1; // -1: last command executed, 0 - stop mission, 1 - start mission
    
    public AircraftData(boolean msg_requirement){
	if(msg_requirement){
	    RcvdMessages = new MAVLinkMessages();
	}

	aircraftPosition    = new Position();
	CurrentFlightPlan   = new FlightPlan();
	NewFlightPlan       = new FlightPlan();
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
    
}
