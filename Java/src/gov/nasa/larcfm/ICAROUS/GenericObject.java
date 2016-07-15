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


class GenericObject{

    int id;
    int type;
    Position pos;
    float vx;
    float vy;
    float vz;
    float orientation;

    public GenericObject(int id_in,int type_in,
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

    public static void AddObject(List<GenericObject> objectList,msg_pointofinterest msg){

	GenericObject obj = new GenericObject(msg.index,msg.subtype,
					 msg.latx,msg.lony,msg.altz,msg.heading,
					 msg.vx,msg.vy,msg.vz);
	
	objectList.add(obj);
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

