package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_Waypoint extends IcarousMessages {
	
	public int totalWaypoints;
	public int waypointIndex;
	public float latitude;
	public float longitude;
	public float altitude;
	public float speed;
	
	public msg_Waypoint(int totalWP,int indexWP,float lat,float lon,float alt,float spd){
		totalWaypoints = totalWP;
		waypointIndex = indexWP;
		latitude = lat;
		longitude = lon;
		altitude = alt;
		speed = spd;
	}
}
