package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_Position extends IcarousMessages {
	
	public static final int ICAROUS_MSG_ID_POSITION = 7;	
	public double time_gps;
	public double latitude;
	public double longitude;
	public double altitude_abs;
	public double altitude_rel;
	public double vx;
	public double vy;
	public double vz;
	public double hdop;
	public double vdop;
	public int numSats;
	
	public msg_Position(){
		msgid = ICAROUS_MSG_ID_POSITION;
	}
}
