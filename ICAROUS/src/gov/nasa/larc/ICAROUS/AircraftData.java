package gov.nasa.larc.ICAROUS;

public class AircraftData{

    public static boolean INIT_MESSAGES = true;
    public static boolean NO_MESSAGES   = false;
    
    public MAVLinkMessages RcvdMessages;

    // Aircraft attitude
    public float roll;
    public float pitch;
    public float yaw;

    // Aircraft euler rates
    public float roll_rate;
    public float pitch_rate;
    public float yaw_rate;

    // Aircraft angular rates
    public float p;
    public float q;
    public float r;

    // Aircraft velocity components in the body frame
    public float u;
    public float v;
    public float w;

    // Aircraft position (GPS)
    public float lat;
    public float lon;
    public float alt;

    // Angle of attack, sideslip and airspeed
    public float aoa;
    public float sideslip;
    public float airspeed;

    public AircraftData(boolean msg_requirement){
	if(msg_requirement){
	    RcvdMessages = new MAVLinkMessages();
	}
    }

    public void CopyAircraftStateInfo(AircraftData Input){

	roll = Input.roll;
	pitch = Input.pitch;
	yaw = Input.pitch;

	roll_rate = Input.roll_rate;
	pitch_rate = Input.pitch_rate;
	yaw_rate = Input.yaw_rate;

	p = Input.p;
	q = Input.q;
	r = Input.r;

	u = Input.u;
	v = Input.v;
	w = Input.w;

	lat = Input.lat;
	lon = Input.lon;
	alt = Input.alt;

	aoa = Input.aoa;
	sideslip = Input.sideslip;
	airspeed = Input.airspeed;
    }

    public void GetDataFromMessages(){

    }

}
