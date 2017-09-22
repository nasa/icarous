package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_MissionItemReached extends IcarousMessages {
	public static final int ICAROUS_MSG_MISSION_ITEM_REACHED = 5;	
	public int reachedWaypoint;
	
	public msg_MissionItemReached(int n){
		msgid = ICAROUS_MSG_MISSION_ITEM_REACHED;
		reachedWaypoint = n;
	}
}
