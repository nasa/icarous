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
Message ID: SIM_STATE(108)
--------------------------------------
%%~ Status of simulation environment, if used
--------------------------------------
*/
public class SIM_STATE_class //implements Loggable
{
	public static final int msgID = 108;
	public float	 q1;			 	// True attitude quaternion component 1, w (1 in null-rotation)
	public float	 q2;			 	// True attitude quaternion component 2, x (0 in null-rotation)
	public float	 q3;			 	// True attitude quaternion component 3, y (0 in null-rotation)
	public float	 q4;			 	// True attitude quaternion component 4, z (0 in null-rotation)
	public float	 roll;		 	// Attitude roll expressed as Euler angles, not recommended except for human-readable outputs
	public float	 pitch;		 	// Attitude pitch expressed as Euler angles, not recommended except for human-readable outputs
	public float	 yaw;			 	// Attitude yaw expressed as Euler angles, not recommended except for human-readable outputs
	public float	 xacc;		 	// X acceleration m/s/s
	public float	 yacc;		 	// Y acceleration m/s/s
	public float	 zacc;		 	// Z acceleration m/s/s
	public float	 xgyro;		 	// Angular speed around X axis rad/s
	public float	 ygyro;		 	// Angular speed around Y axis rad/s
	public float	 zgyro;		 	// Angular speed around Z axis rad/s
	public float	 lat;			 	// Latitude in degrees
	public float	 lon;			 	// Longitude in degrees
	public float	 alt;			 	// Altitude in meters
	public float	 std_dev_horz; 	// Horizontal position standard deviation
	public float	 std_dev_vert; 	// Vertical position standard deviation
	public float	 vn;			 	// True velocity in m/s in NORTH direction in earth-fixed NED frame
	public float	 ve;			 	// True velocity in m/s in EAST direction in earth-fixed NED frame
	public float	 vd;			 	// True velocity in m/s in DOWN direction in earth-fixed NED frame

	private packet rcvPacket;
	private packet sndPacket;

	public SIM_STATE_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public SIM_STATE_class(SIM_STATE_class o)
	{
		q1 = o.q1;
		q2 = o.q2;
		q3 = o.q3;
		q4 = o.q4;
		roll = o.roll;
		pitch = o.pitch;
		yaw = o.yaw;
		xacc = o.xacc;
		yacc = o.yacc;
		zacc = o.zacc;
		xgyro = o.xgyro;
		ygyro = o.ygyro;
		zgyro = o.zgyro;
		lat = o.lat;
		lon = o.lon;
		alt = o.alt;
		std_dev_horz = o.std_dev_horz;
		std_dev_vert = o.std_dev_vert;
		vn = o.vn;
		ve = o.ve;
		vd = o.vd;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

			q1			= rcvPacket.getFloat();
			q2			= rcvPacket.getFloat();
			q3			= rcvPacket.getFloat();
			q4			= rcvPacket.getFloat();
			roll			= rcvPacket.getFloat();
			pitch		= rcvPacket.getFloat();
			yaw			= rcvPacket.getFloat();
			xacc			= rcvPacket.getFloat();
			yacc			= rcvPacket.getFloat();
			zacc			= rcvPacket.getFloat();
			xgyro		= rcvPacket.getFloat();
			ygyro		= rcvPacket.getFloat();
			zgyro		= rcvPacket.getFloat();
			lat			= rcvPacket.getFloat();
			lon			= rcvPacket.getFloat();
			alt			= rcvPacket.getFloat();
			std_dev_horz	= rcvPacket.getFloat();
			std_dev_vert	= rcvPacket.getFloat();
			vn			= rcvPacket.getFloat();
			ve			= rcvPacket.getFloat();
			vd			= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  q1
					 ,q2
					 ,q3
					 ,q4
					 ,roll
					 ,pitch
					 ,yaw
					 ,xacc
					 ,yacc
					 ,zacc
					 ,xgyro
					 ,ygyro
					 ,zgyro
					 ,lat
					 ,lon
					 ,alt
					 ,std_dev_horz
					 ,std_dev_vert
					 ,vn
					 ,ve
					 ,vd
					 );
	}

	public byte[] encode(
						 float v_q1
						,float v_q2
						,float v_q3
						,float v_q4
						,float v_roll
						,float v_pitch
						,float v_yaw
						,float v_xacc
						,float v_yacc
						,float v_zacc
						,float v_xgyro
						,float v_ygyro
						,float v_zgyro
						,float v_lat
						,float v_lon
						,float v_alt
						,float v_std_dev_horz
						,float v_std_dev_vert
						,float v_vn
						,float v_ve
						,float v_vd
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_q1);	// Add "q1" parameter
		sndPacket.putFloat(v_q2);	// Add "q2" parameter
		sndPacket.putFloat(v_q3);	// Add "q3" parameter
		sndPacket.putFloat(v_q4);	// Add "q4" parameter
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_yaw);	// Add "yaw" parameter
		sndPacket.putFloat(v_xacc);	// Add "xacc" parameter
		sndPacket.putFloat(v_yacc);	// Add "yacc" parameter
		sndPacket.putFloat(v_zacc);	// Add "zacc" parameter
		sndPacket.putFloat(v_xgyro);	// Add "xgyro" parameter
		sndPacket.putFloat(v_ygyro);	// Add "ygyro" parameter
		sndPacket.putFloat(v_zgyro);	// Add "zgyro" parameter
		sndPacket.putFloat(v_lat);	// Add "lat" parameter
		sndPacket.putFloat(v_lon);	// Add "lon" parameter
		sndPacket.putFloat(v_alt);	// Add "alt" parameter
		sndPacket.putFloat(v_std_dev_horz);	// Add "std_dev_horz" parameter
		sndPacket.putFloat(v_std_dev_vert);	// Add "std_dev_vert" parameter
		sndPacket.putFloat(v_vn);	// Add "vn" parameter
		sndPacket.putFloat(v_ve);	// Add "ve" parameter
		sndPacket.putFloat(v_vd);	// Add "vd" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SIM_STATE_q1"
 				+ ", SIM_STATE_q2"
 				+ ", SIM_STATE_q3"
 				+ ", SIM_STATE_q4"
 				+ ", SIM_STATE_roll"
 				+ ", SIM_STATE_pitch"
 				+ ", SIM_STATE_yaw"
 				+ ", SIM_STATE_xacc"
 				+ ", SIM_STATE_yacc"
 				+ ", SIM_STATE_zacc"
 				+ ", SIM_STATE_xgyro"
 				+ ", SIM_STATE_ygyro"
 				+ ", SIM_STATE_zgyro"
 				+ ", SIM_STATE_lat"
 				+ ", SIM_STATE_lon"
 				+ ", SIM_STATE_alt"
 				+ ", SIM_STATE_std_dev_horz"
 				+ ", SIM_STATE_std_dev_vert"
 				+ ", SIM_STATE_vn"
 				+ ", SIM_STATE_ve"
 				+ ", SIM_STATE_vd"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + q1
 				+ ", " + q2
 				+ ", " + q3
 				+ ", " + q4
 				+ ", " + roll
 				+ ", " + pitch
 				+ ", " + yaw
 				+ ", " + xacc
 				+ ", " + yacc
 				+ ", " + zacc
 				+ ", " + xgyro
 				+ ", " + ygyro
 				+ ", " + zgyro
 				+ ", " + lat
 				+ ", " + lon
 				+ ", " + alt
 				+ ", " + std_dev_horz
 				+ ", " + std_dev_vert
 				+ ", " + vn
 				+ ", " + ve
 				+ ", " + vd
				);
		return param;
	}
}
