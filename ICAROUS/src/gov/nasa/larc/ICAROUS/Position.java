package gov.nasa.larc.ICAROUS;

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
    float maxHorDev;
    float maxVerDev;

    public Waypoint(){
	super();
	id        =0;
	heading   =0.0f;
	maxHorDev =0.0f;
	maxVerDev =0.0f;
    }

    public Waypoint(int id_in,float lat_in,float lon_in,float alt_in,
		    float heading_in,float maxHorDev_in,float maxVerDev_in){
	super(lat_in,lon_in,alt_in);
	id    = id_in;
	heading   = heading_in;
        maxHorDev = maxHorDev_in;
	maxVerDev = maxVerDev_in;
    }

    public void UpdateWaypoint(int id_in,float lat_in,float lon_in,float alt_in,
			       float heading_in,float maxHorDev_in,float maxVerDev_in){
	id    = id_in;
	this.UpdatePosition(lat_in,lon_in,alt_in);
	heading   = heading_in;
        maxHorDev = maxHorDev_in;
	maxVerDev = maxVerDev_in;
	
    }
    
    
}

class Obstacle extends Position{

    int id;

    public Obstacle(int id_in,float lat_in, float lon_in, float alt_in){
	super(lat_in,lon_in,alt_in);
	id = id_in;
    }
    
}
