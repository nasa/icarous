/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//MavLink 1.0

//package gov.nasa.larc.AP;
//import gov.nasa.larc.serial.Loggable;

/**
Message ID: HIL_STATE_QUATERNION(115)
--------------------------------------
%%~ Sent from simulation to autopilot, avoids in contrast to HIL_STATE singularities. 
%%~ This packet is useful for high throughput applications such as hardware in the 
%%~ loop simulations.
--------------------------------------
*/
public class HIL_STATE_QUATERNION_class //implements Loggable
{
	public static final int msgID = 115;
	public long	 time_usec;			 	// Timestamp (microseconds since UNIX epoch or microseconds since system boot)
	public float[]	 attitude_quaternion = new float[4];		// Vehicle attitude expressed as normalized quaternion in w, x, y, z order (with 1 0 0 0 being the null-rotation)
	public float	 rollspeed;			 	// Body frame roll / phi angular speed (rad/s)
	public float	 pitchspeed;			 	// Body frame pitch / theta angular speed (rad/s)
	public float	 yawspeed;			 	// Body frame yaw / psi angular speed (rad/s)
	public int		 lat;					 	// Latitude, expressed as * 1E7
	public int		 lon;					 	// Longitude, expressed as * 1E7
	public int		 alt;					 	// Altitude in meters, expressed as * 1000 (millimeters)
	public short	 vx;					 	// Ground X Speed (Latitude), expressed as m/s * 100
	public short	 vy;					 	// Ground Y Speed (Longitude), expressed as m/s * 100
	public short	 vz;					 	// Ground Z Speed (Altitude), expressed as m/s * 100
	public int		 ind_airspeed;		 	// Indicated airspeed, expressed as m/s * 100
	public int		 true_airspeed;		 	// True airspeed, expressed as m/s * 100
	public short	 xacc;				 	// X acceleration (mg)
	public short	 yacc;				 	// Y acceleration (mg)
	public short	 zacc;				 	// Z acceleration (mg)

	private packet rcvPacket;
	private packet sndPacket;

	public HIL_STATE_QUATERNION_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public HIL_STATE_QUATERNION_class(HIL_STATE_QUATERNION_class o)
	{
		time_usec = o.time_usec;
		attitude_quaternion = o.attitude_quaternion;
		rollspeed = o.rollspeed;
		pitchspeed = o.pitchspeed;
		yawspeed = o.yawspeed;
		lat = o.lat;
		lon = o.lon;
		alt = o.alt;
		vx = o.vx;
		vy = o.vy;
		vz = o.vz;
		ind_airspeed = o.ind_airspeed;
		true_airspeed = o.true_airspeed;
		xacc = o.xacc;
		yacc = o.yacc;
		zacc = o.zacc;
	}

	public boolean parse(byte[] b)
	{
		return parse(b, false);
	}

	public boolean parse(byte[] b, boolean valid)
	{
		rcvPacket.load(b);

		boolean pstatus = valid || rcvPacket.isPacket();
		if (pstatus)
		{
			rcvPacket.updateSeqNum();

			// int[] mavLen = {8, 16, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2};
			// int[] javLen = {8, 16, 4, 4, 4, 4, 4, 4, 2, 2, 2, 4, 4, 2, 2, 2};

			time_usec			= rcvPacket.getLong();
			rcvPacket.getByte(attitude_quaternion, 0, 4);
			rollspeed			= rcvPacket.getFloat();
			pitchspeed			= rcvPacket.getFloat();
			yawspeed				= rcvPacket.getFloat();
			lat					= rcvPacket.getInt();
			lon					= rcvPacket.getInt();
			alt					= rcvPacket.getInt();
			vx					= rcvPacket.getShort();
			vy					= rcvPacket.getShort();
			vz					= rcvPacket.getShort();
			ind_airspeed			= rcvPacket.getIntS();
			true_airspeed		= rcvPacket.getIntS();
			xacc					= rcvPacket.getShort();
			yacc					= rcvPacket.getShort();
			zacc					= rcvPacket.getShort();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_usec
					 ,attitude_quaternion
					 ,rollspeed
					 ,pitchspeed
					 ,yawspeed
					 ,lat
					 ,lon
					 ,alt
					 ,vx
					 ,vy
					 ,vz
					 ,ind_airspeed
					 ,true_airspeed
					 ,xacc
					 ,yacc
					 ,zacc
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,float[] v_attitude_quaternion
						,float v_rollspeed
						,float v_pitchspeed
						,float v_yawspeed
						,int v_lat
						,int v_lon
						,int v_alt
						,short v_vx
						,short v_vy
						,short v_vz
						,int v_ind_airspeed
						,int v_true_airspeed
						,short v_xacc
						,short v_yacc
						,short v_zacc
						)
	{
		// int[] mavLen = {8, 16, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2};
		// int[] javLen = {8, 16, 4, 4, 4, 4, 4, 4, 2, 2, 2, 4, 4, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
		sndPacket.putByte(v_attitude_quaternion,0,4);	// Add "attitude_quaternion" parameter
		sndPacket.putFloat(v_rollspeed);	// Add "rollspeed" parameter
		sndPacket.putFloat(v_pitchspeed);	// Add "pitchspeed" parameter
		sndPacket.putFloat(v_yawspeed);	// Add "yawspeed" parameter
		sndPacket.putInt(v_lat);	// Add "lat" parameter
		sndPacket.putInt(v_lon);	// Add "lon" parameter
		sndPacket.putInt(v_alt);	// Add "alt" parameter
		sndPacket.putShort(v_vx);	// Add "vx" parameter
		sndPacket.putShort(v_vy);	// Add "vy" parameter
		sndPacket.putShort(v_vz);	// Add "vz" parameter
		sndPacket.putShortI(v_ind_airspeed);	// Add "ind_airspeed" parameter
		sndPacket.putShortI(v_true_airspeed);	// Add "true_airspeed" parameter
		sndPacket.putShort(v_xacc);	// Add "xacc" parameter
		sndPacket.putShort(v_yacc);	// Add "yacc" parameter
		sndPacket.putShort(v_zacc);	// Add "zacc" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", HIL_STATE_QUATERNION_time_usec"
 				+ ", HIL_STATE_QUATERNION_attitude_quaternion"
 				+ ", HIL_STATE_QUATERNION_rollspeed"
 				+ ", HIL_STATE_QUATERNION_pitchspeed"
 				+ ", HIL_STATE_QUATERNION_yawspeed"
 				+ ", HIL_STATE_QUATERNION_lat"
 				+ ", HIL_STATE_QUATERNION_lon"
 				+ ", HIL_STATE_QUATERNION_alt"
 				+ ", HIL_STATE_QUATERNION_vx"
 				+ ", HIL_STATE_QUATERNION_vy"
 				+ ", HIL_STATE_QUATERNION_vz"
 				+ ", HIL_STATE_QUATERNION_ind_airspeed"
 				+ ", HIL_STATE_QUATERNION_true_airspeed"
 				+ ", HIL_STATE_QUATERNION_xacc"
 				+ ", HIL_STATE_QUATERNION_yacc"
 				+ ", HIL_STATE_QUATERNION_zacc"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
 				+ ", " + attitude_quaternion
 				+ ", " + rollspeed
 				+ ", " + pitchspeed
 				+ ", " + yawspeed
 				+ ", " + lat
 				+ ", " + lon
 				+ ", " + alt
 				+ ", " + vx
 				+ ", " + vy
 				+ ", " + vz
 				+ ", " + ind_airspeed
 				+ ", " + true_airspeed
 				+ ", " + xacc
 				+ ", " + yacc
 				+ ", " + zacc
				);
		return param;
	}
}
