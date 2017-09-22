package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_Geofence extends IcarousMessages {
	public static final int ICAROUS_MSG_ID_GEOFENCE = 4;	
	public int index;
	public int type;
	public int totalVertices;
	public int vertexIndex;
	public float latitude;
	public float longitude;
	public float floor;
	public float ceiling;
	
	public msg_Geofence(){
		msgid = ICAROUS_MSG_ID_GEOFENCE;
	}
}
