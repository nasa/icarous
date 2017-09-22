package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_Object extends IcarousMessages {
	
	public static final int ICAROUS_MSG_ID_OBJECT = 6;	
	int type;
	int index;
	float latitude;
	float longitude;
	float altiude;
	float vx;
	float vy;
	float vz;
	
	public msg_Object(){
		msgid = ICAROUS_MSG_ID_OBJECT;
	}
}
