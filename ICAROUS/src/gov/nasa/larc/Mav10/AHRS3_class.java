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
Message ID: AHRS3(182)
--------------------------------------
%%~ Status of third AHRS filter if available. This is for ANU research group (Ali and 
%%~ Sean)
--------------------------------------
*/
public class AHRS3_class //implements Loggable
{
	public static final int msgID = 182;
	public float	 roll;	 	// Roll angle (rad)
	public float	 pitch;	 	// Pitch angle (rad)
	public float	 yaw;		 	// Yaw angle (rad)
	public float	 altitude; 	// Altitude (MSL)
	public int		 lat;		 	// Latitude in degrees * 1E7
	public int		 lng;		 	// Longitude in degrees * 1E7
	public float	 v1;		 	// test variable1
	public float	 v2;		 	// test variable2
	public float	 v3;		 	// test variable3
	public float	 v4;		 	// test variable4

	private packet rcvPacket;
	private packet sndPacket;

	public AHRS3_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public AHRS3_class(AHRS3_class o)
	{
		roll = o.roll;
		pitch = o.pitch;
		yaw = o.yaw;
		altitude = o.altitude;
		lat = o.lat;
		lng = o.lng;
		v1 = o.v1;
		v2 = o.v2;
		v3 = o.v3;
		v4 = o.v4;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

			roll		= rcvPacket.getFloat();
			pitch	= rcvPacket.getFloat();
			yaw		= rcvPacket.getFloat();
			altitude	= rcvPacket.getFloat();
			lat		= rcvPacket.getInt();
			lng		= rcvPacket.getInt();
			v1		= rcvPacket.getFloat();
			v2		= rcvPacket.getFloat();
			v3		= rcvPacket.getFloat();
			v4		= rcvPacket.getFloat();
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
					 ,v1
					 ,v2
					 ,v3
					 ,v4
					 );
	}

	public byte[] encode(
						 float v_roll
						,float v_pitch
						,float v_yaw
						,float v_altitude
						,int v_lat
						,int v_lng
						,float v_v1
						,float v_v2
						,float v_v3
						,float v_v4
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_yaw);	// Add "yaw" parameter
		sndPacket.putFloat(v_altitude);	// Add "altitude" parameter
		sndPacket.putInt(v_lat);	// Add "lat" parameter
		sndPacket.putInt(v_lng);	// Add "lng" parameter
		sndPacket.putFloat(v_v1);	// Add "v1" parameter
		sndPacket.putFloat(v_v2);	// Add "v2" parameter
		sndPacket.putFloat(v_v3);	// Add "v3" parameter
		sndPacket.putFloat(v_v4);	// Add "v4" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", AHRS3_roll"
 				+ ", AHRS3_pitch"
 				+ ", AHRS3_yaw"
 				+ ", AHRS3_altitude"
 				+ ", AHRS3_lat"
 				+ ", AHRS3_lng"
 				+ ", AHRS3_v1"
 				+ ", AHRS3_v2"
 				+ ", AHRS3_v3"
 				+ ", AHRS3_v4"
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
 				+ ", " + v1
 				+ ", " + v2
 				+ ", " + v3
 				+ ", " + v4
				);
		return param;
	}
}
