package gov.nasa.larc.ICAROUS;

import java.util.*;

public class Position{

    float lat;
    float lon;
    float alt;

    public Position(){
	lat   = 0.0f;
	lon   = 0.0f;
	alt   = 0.0f;
    }

    public Position(float lat_in,float lon_in,float alt_in){
	lat   = lat_in;
	lon   = lon_in;
	alt   = alt_in;
    }

    public void UpdatePosition(float lat_in,float lon_in,float alt_in){
	lat   = lat_in;
	lon   = lon_in;
	alt   = alt_in;
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

    public Waypoint(int id_in,float lat_in,float lon_in,float alt_in,
		    float heading_in){
	super(lat_in,lon_in,alt_in);
	id    = id_in;
	heading   = heading_in;
    }
        
}

class Obstacle extends Position{

    int id;

    public Obstacle(int id_in,float lat_in, float lon_in, float alt_in){
	super(lat_in,lon_in,alt_in);
	id = id_in;
    }
    
}

class FlightPlan{

    public List wayPoints;
    public long numWayPoints;
    public float maxHorDev;
    public float maxVerDev;
    public float standOffDist;
    public Iterator wpIt = null;

    public FlightPlan(){
	wayPoints    = new ArrayList();
    }
    
    public void FlightPlanInfo(long num,float HorDev, float VerDev, float standDist){

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
}







