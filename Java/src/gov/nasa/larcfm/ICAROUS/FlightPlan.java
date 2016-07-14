/**
 * FlightPlan
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



class Waypoint{

    int id;
    Position pos;
    float heading;

    public Waypoint(){
	pos = new Position();
	id        =0;
	heading   =0.0f;
    }

    public Waypoint(int id_in,float lat_in,float lon_in,float altmsl_in,
		    float heading_in){
	pos = new Position(lat_in,lon_in,altmsl_in);
	id        = id_in;
	heading   = heading_in;
    }

    public Waypoint(Waypoint wp){
	pos = new Position(wp.pos.lat,wp.pos.lon,wp.pos.alt_msl,wp.pos.alt_agl);
	id = wp.id;
	heading = wp.heading; 

    }
        
}

public class FlightPlan{

    public enum FP_READ_COM{
	FP_INFO, FP_WAYPT_INFO, FP_ACK_FAIL,FP_ACK_SUCCESS
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

    public double[] Distance2Waypoint(Position currentPos,Position next_wp){

	double Dist[] = new double[2];
	
	double lat1 = currentPos.lat;
	double lon1 = currentPos.lon;

	double lat2 = next_wp.lat;
	double lon2 = next_wp.lon;

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

