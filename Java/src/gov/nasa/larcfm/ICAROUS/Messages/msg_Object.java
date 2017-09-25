package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_Object extends IcarousMessages {
	
	public static final int ICAROUS_MSG_ID_OBJECT = 6;	
	public int type;
	public int index;
	public float latitude;
	public float longitude;
	public float altiude;
	public float vx;
	public float vy;
	public float vz;
	
	public msg_Object(){
		msgid = ICAROUS_MSG_ID_OBJECT;
	}
}
