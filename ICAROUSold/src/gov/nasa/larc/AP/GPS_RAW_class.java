/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: GPS_RAW(32)
--------------------------------------
%%~ The global position, as returned by the Global Positioning System (GPS). This is 
%%~ NOT the global position estimate of the sytem, but rather a RAW sensor value. See 
%%~ message GLOBAL_POSITION for the global position estimate. Coordinate frame is 
%%~ right-handed, Z-axis up (GPS frame)
--------------------------------------
*/
public class GPS_RAW_class implements Loggable
{
	public static final int msgID = 32;
	public long	 usec;	 	// Timestamp (microseconds since UNIX epoch or microseconds since system boot)
	public short	 fix_type; 	// 0-1: no fix, 2: 2D fix, 3: 3D fix. Some applications will not use the value of this field unless it is at least two, so always correctly fill in the fix.
	public float	 lat;		 	// Latitude in degrees
	public float	 lon;		 	// Longitude in degrees
	public float	 alt;		 	// Altitude in meters
	public float	 eph;		 	// GPS HDOP
	public float	 epv;		 	// GPS VDOP
	public float	 v;		 	// GPS ground speed
	public float	 hdg;		 	// Compass heading in degrees, 0..360 degrees

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public GPS_RAW_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public GPS_RAW_class(GPS_RAW_class o)
	{
		usec = o.usec;
		fix_type = o.fix_type;
		lat = o.lat;
		lon = o.lon;
		alt = o.alt;
		eph = o.eph;
		epv = o.epv;
		v = o.v;
		hdg = o.hdg;
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

			// int[] mavLen = {8, 1, 4, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {8, 2, 4, 4, 4, 4, 4, 4, 4};

			usec		= rcvPacket.getLong();
			fix_type	= rcvPacket.getShortB();
			lat		= rcvPacket.getFloat();
			lon		= rcvPacket.getFloat();
			alt		= rcvPacket.getFloat();
			eph		= rcvPacket.getFloat();
			epv		= rcvPacket.getFloat();
			v		= rcvPacket.getFloat();
			hdg		= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  usec
					 ,fix_type
					 ,lat
					 ,lon
					 ,alt
					 ,eph
					 ,epv
					 ,v
					 ,hdg
					 );
	}

	public byte[] encode(
						 long v_usec
						,short v_fix_type
						,float v_lat
						,float v_lon
						,float v_alt
						,float v_eph
						,float v_epv
						,float v_v
						,float v_hdg
						)
	{
		// int[] mavLen = {8, 1, 4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {8, 2, 4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_usec);	// Add "usec" parameter
		sndPacket.putByteS(v_fix_type);	// Add "fix_type" parameter
		sndPacket.putFloat(v_lat);	// Add "lat" parameter
		sndPacket.putFloat(v_lon);	// Add "lon" parameter
		sndPacket.putFloat(v_alt);	// Add "alt" parameter
		sndPacket.putFloat(v_eph);	// Add "eph" parameter
		sndPacket.putFloat(v_epv);	// Add "epv" parameter
		sndPacket.putFloat(v_v);	// Add "v" parameter
		sndPacket.putFloat(v_hdg);	// Add "hdg" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GPS_RAW_usec"
 				+ ", GPS_RAW_fix_type"
 				+ ", GPS_RAW_lat"
 				+ ", GPS_RAW_lon"
 				+ ", GPS_RAW_alt"
 				+ ", GPS_RAW_eph"
 				+ ", GPS_RAW_epv"
 				+ ", GPS_RAW_v"
 				+ ", GPS_RAW_hdg"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + usec
 				+ ", " + fix_type
 				+ ", " + lat
 				+ ", " + lon
 				+ ", " + alt
 				+ ", " + eph
 				+ ", " + epv
 				+ ", " + v
 				+ ", " + hdg
				);
		return param;
	}
}
