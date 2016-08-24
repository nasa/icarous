/**
 * GenericObject class
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
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Velocity;

public class GenericObject{

    public int id;
    public int type;
    public Position pos;
    public Velocity vel;
    public double orientation;

    public GenericObject(int type_in,int id_in,
			 float lat_in, float lon_in, float altmsl_in,			 
			 float vx_in, float vy_in,float vz_in){
	id   = id_in;
	type = type_in;
	pos  = Position.makeLatLonAlt(lat_in,"degree",lon_in,"degree",altmsl_in,"m");
	vel  = Velocity.makeVxyz(vy_in,vx_in,vz_in);		
	orientation = vel.trk();
	
    }

    public boolean isEqual(GenericObject obj,boolean update){

	if(id == obj.id){
	    if(update){
		pos = obj.pos.copy();
		vel = obj.vel.mkAddTrk(0);
		orientation = obj.orientation;
	    }
	    return true;
	}else{
	    return false;
	}
	
    }
	
    

    public static void AddObject(List<GenericObject> objectList,GenericObject obj){

	boolean inList = false;
	for(int i=0;i<objectList.size();i++){
	    if( objectList.get(i).isEqual(obj,true)){ 
		inList = true;
		break;
	    }
	}

	if(!inList){
	    objectList.add(obj);
	}
	
    }

    public static void RemoveObject(List<GenericObject> objectList, int id){

	GenericObject obj;
	Iterator Itr = objectList.iterator();
	
	while(Itr.hasNext()){
	    obj = (GenericObject) Itr.next();
	    
	    if(obj.id == id){
		Itr.remove();		
		break;   
	    }
	}
    }
    
}

