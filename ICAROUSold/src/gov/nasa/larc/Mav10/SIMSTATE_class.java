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
Message ID: SIMSTATE(164)
--------------------------------------
%%~ Status of simulation environment, if used
--------------------------------------
*/
public class SIMSTATE_class //implements Loggable
{
	public static final int msgID = 164;
	public float	 roll;	 	// Roll angle (rad)
	public float	 pitch;	 	// Pitch angle (rad)
	public float	 yaw;		 	// Yaw angle (rad)
	public float	 xacc;	 	// X acceleration m/s/s
	public float	 yacc;	 	// Y acceleration m/s/s
	public float	 zacc;	 	// Z acceleration m/s/s
	public float	 xgyro;	 	// Angular speed around X axis rad/s
	public float	 ygyro;	 	// Angular speed around Y axis rad/s
	public float	 zgyro;	 	// Angular speed around Z axis rad/s
	public int		 lat;		 	// Latitude in degrees * 1E7
	public int		 lng;		 	// Longitude in degrees * 1E7

	private packet rcvPacket;
	private packet sndPacket;

	public SIMSTATE_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public SIMSTATE_class(SIMSTATE_class o)
	{
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
		lng = o.lng;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

			roll		= rcvPacket.getFloat();
			pitch	= rcvPacket.getFloat();
			yaw		= rcvPacket.getFloat();
			xacc		= rcvPacket.getFloat();
			yacc		= rcvPacket.getFloat();
			zacc		= rcvPacket.getFloat();
			xgyro	= rcvPacket.getFloat();
			ygyro	= rcvPacket.getFloat();
			zgyro	= rcvPacket.getFloat();
			lat		= rcvPacket.getInt();
			lng		= rcvPacket.getInt();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  roll
					 ,pitch
					 ,yaw
					 ,xacc
					 ,yacc
					 ,zacc
					 ,xgyro
					 ,ygyro
					 ,zgyro
					 ,lat
					 ,lng
					 );
	}

	public byte[] encode(
						 float v_roll
						,float v_pitch
						,float v_yaw
						,float v_xacc
						,float v_yacc
						,float v_zacc
						,float v_xgyro
						,float v_ygyro
						,float v_zgyro
						,int v_lat
						,int v_lng
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_yaw);	// Add "yaw" parameter
		sndPacket.putFloat(v_xacc);	// Add "xacc" parameter
		sndPacket.putFloat(v_yacc);	// Add "yacc" parameter
		sndPacket.putFloat(v_zacc);	// Add "zacc" parameter
		sndPacket.putFloat(v_xgyro);	// Add "xgyro" parameter
		sndPacket.putFloat(v_ygyro);	// Add "ygyro" parameter
		sndPacket.putFloat(v_zgyro);	// Add "zgyro" parameter
		sndPacket.putInt(v_lat);	// Add "lat" parameter
		sndPacket.putInt(v_lng);	// Add "lng" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SIMSTATE_roll"
 				+ ", SIMSTATE_pitch"
 				+ ", SIMSTATE_yaw"
 				+ ", SIMSTATE_xacc"
 				+ ", SIMSTATE_yacc"
 				+ ", SIMSTATE_zacc"
 				+ ", SIMSTATE_xgyro"
 				+ ", SIMSTATE_ygyro"
 				+ ", SIMSTATE_zgyro"
 				+ ", SIMSTATE_lat"
 				+ ", SIMSTATE_lng"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
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
 				+ ", " + lng
				);
		return param;
	}
}
