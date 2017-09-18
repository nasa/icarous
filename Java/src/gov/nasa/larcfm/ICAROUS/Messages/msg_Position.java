package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_Position extends IcarousMessages {
	double time_gps;
	double latitude;
	double longitude;
	double altitude_abs;
	double altitude_rel;
	double vx;
	double vy;
	double vz;
	double hdop;
	double vdop;
	int numSats;
}
