/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: GLOBAL_POSITION(33)
--------------------------------------
%%~ The filtered global position (e.g. fused GPS and accelerometers). Coordinate frame 
%%~ is right-handed, Z-axis up (GPS frame)
--------------------------------------
*/
public class GLOBAL_POSITION_class implements Loggable
{
	public static final int msgID = 33;
	public long	 usec; 	// Timestamp (microseconds since unix epoch)
	public float	 lat;	 	// Latitude, in degrees
	public float	 lon;	 	// Longitude, in degrees
	public float	 alt;	 	// Absolute altitude, in meters
	public float	 vx;	 	// X Speed (in Latitude direction, positive: going north)
	public float	 vy;	 	// Y Speed (in Longitude direction, positive: going east)
	public float	 vz;	 	// Z Speed (in Altitude direction, positive: going up)

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public GLOBAL_POSITION_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public GLOBAL_POSITION_class(GLOBAL_POSITION_class o)
	{
		usec = o.usec;
		lat = o.lat;
		lon = o.lon;
		alt = o.alt;
		vx = o.vx;
		vy = o.vy;
		vz = o.vz;
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

			// int[] mavLen = {8, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4};

			usec	= rcvPacket.getLong();
			lat	= rcvPacket.getFloat();
			lon	= rcvPacket.getFloat();
			alt	= rcvPacket.getFloat();
			vx	= rcvPacket.getFloat();
			vy	= rcvPacket.getFloat();
			vz	= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  usec
					 ,lat
					 ,lon
					 ,alt
					 ,vx
					 ,vy
					 ,vz
					 );
	}

	public byte[] encode(
						 long v_usec
						,float v_lat
						,float v_lon
						,float v_alt
						,float v_vx
						,float v_vy
						,float v_vz
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_usec);	// Add "usec" parameter
		sndPacket.putFloat(v_lat);	// Add "lat" parameter
		sndPacket.putFloat(v_lon);	// Add "lon" parameter
		sndPacket.putFloat(v_alt);	// Add "alt" parameter
		sndPacket.putFloat(v_vx);	// Add "vx" parameter
		sndPacket.putFloat(v_vy);	// Add "vy" parameter
		sndPacket.putFloat(v_vz);	// Add "vz" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GLOBAL_POSITION_usec"
 				+ ", GLOBAL_POSITION_lat"
 				+ ", GLOBAL_POSITION_lon"
 				+ ", GLOBAL_POSITION_alt"
 				+ ", GLOBAL_POSITION_vx"
 				+ ", GLOBAL_POSITION_vy"
 				+ ", GLOBAL_POSITION_vz"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + usec
 				+ ", " + lat
 				+ ", " + lon
 				+ ", " + alt
 				+ ", " + vx
 				+ ", " + vy
 				+ ", " + vz
				);
		return param;
	}
}
