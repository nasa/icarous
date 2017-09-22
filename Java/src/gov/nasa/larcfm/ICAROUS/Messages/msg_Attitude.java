package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_Attitude extends IcarousMessages {
	
	public static final int ICAROUS_MSG_ID_ATTITUDE = 2;
		
	public float roll;
	public float pitch;
	public float yaw;
	
	public msg_Attitude(){
		msgid = ICAROUS_MSG_ID_ATTITUDE;
	}
	
}
