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

public class FlightPlan{

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







