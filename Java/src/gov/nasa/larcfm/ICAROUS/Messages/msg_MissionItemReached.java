package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_MissionItemReached extends IcarousMessages {
	public int reachedWaypoint;
	
	public msg_MissionItemReached(int n){
		reachedWaypoint = n;
	}
}
