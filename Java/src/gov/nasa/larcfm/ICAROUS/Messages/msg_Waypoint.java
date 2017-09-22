package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_Waypoint extends IcarousMessages {
	
	public static final int ICAROUS_MSG_WAYPOINT = 9;	
	public int totalWaypoints;
	public int waypointIndex;
	public float latitude;
	public float longitude;
	public float altitude;
	public float speed;
	
	public msg_Waypoint(int totalWP,int indexWP,float lat,float lon,float alt,float spd){
		msgid = ICAROUS_MSG_WAYPOINT;
		totalWaypoints = totalWP;
		waypointIndex = indexWP;
		latitude = lat;
		longitude = lon;
		altitude = alt;
		speed = spd;
	}
}
