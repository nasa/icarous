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
Message ID: AHRS2(178)
--------------------------------------
%%~ Status of secondary AHRS filter if available
--------------------------------------
*/
public class AHRS2_class //implements Loggable
{
	public static final int msgID = 178;
	public float	 roll;	 	// Roll angle (rad)
	public float	 pitch;	 	// Pitch angle (rad)
	public float	 yaw;		 	// Yaw angle (rad)
	public float	 altitude; 	// Altitude (MSL)
	public int		 lat;		 	// Latitude in degrees * 1E7
	public int		 lng;		 	// Longitude in degrees * 1E7

	private packet rcvPacket;
	private packet sndPacket;

	public AHRS2_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public AHRS2_class(AHRS2_class o)
	{
		roll = o.roll;
		pitch = o.pitch;
		yaw = o.yaw;
		altitude = o.altitude;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4};
			// int[] javLen = {4, 4, 4, 4, 4, 4};

			roll		= rcvPacket.getFloat();
			pitch	= rcvPacket.getFloat();
			yaw		= rcvPacket.getFloat();
			altitude	= rcvPacket.getFloat();
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
					 ,altitude
					 ,lat
					 ,lng
					 );
	}

	public byte[] encode(
						 float v_roll
						,float v_pitch
						,float v_yaw
						,float v_altitude
						,int v_lat
						,int v_lng
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4};
		// int[] javLen = {4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_yaw);	// Add "yaw" parameter
		sndPacket.putFloat(v_altitude);	// Add "altitude" parameter
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
 				+ ", AHRS2_roll"
 				+ ", AHRS2_pitch"
 				+ ", AHRS2_yaw"
 				+ ", AHRS2_altitude"
 				+ ", AHRS2_lat"
 				+ ", AHRS2_lng"
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
 				+ ", " + altitude
 				+ ", " + lat
 				+ ", " + lng
				);
		return param;
	}
}
