/**
 * Obstacle and ObjectList
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
import com.MAVLink.icarous.*;


class Obstacle extends Conflict{

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
