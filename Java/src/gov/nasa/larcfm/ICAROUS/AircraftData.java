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
    public ObjectList obstacles;
    public ObjectList traffic;
    public ObjectList missionObj;
    
    // List for traffic information
    

    public int startMission = -1; // -1: last command executed, 0 - stop mission, 1 - start mission
    
    public AircraftData(boolean msg_requirement){

	RcvdMessages        = new MAVLinkMessages();
	aircraftPosition    = new Position();
	CurrentFlightPlan   = new FlightPlan();
	listOfFences        = new GEOFENCES();
	obstacles           = new ObjectList();
	traffic             = new ObjectList();
	missionObj          = new ObjectList();
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


