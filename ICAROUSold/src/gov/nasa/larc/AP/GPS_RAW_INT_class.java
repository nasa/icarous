/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: GPS_RAW_INT(25)
--------------------------------------
%%~ The global position, as returned by the Global Positioning System (GPS). This is 
%%~ NOT the global position estimate of the sytem, but rather a RAW sensor value. See 
%%~ message GLOBAL_POSITION for the global position estimate. Coordinate frame is 
%%~ right-handed, Z-axis up (GPS frame)
--------------------------------------
*/
public class GPS_RAW_INT_class implements Loggable
{
	public static final int msgID = 25;
	public long	 usec;	 	// Timestamp (microseconds since UNIX epoch or microseconds since system boot)
	public short	 fix_type; 	// 0-1: no fix, 2: 2D fix, 3: 3D fix. Some applications will not use the value of this field unless it is at least two, so always correctly fill in the fix.
	public int		 lat;		 	// Latitude in 1E7 degrees
	public int		 lon;		 	// Longitude in 1E7 degrees
	public int		 alt;		 	// Altitude in 1E3 meters (millimeters)
	public float	 eph;		 	// GPS HDOP
	public float	 epv;		 	// GPS VDOP
	public float	 v;		 	// GPS ground speed (m/s)
	public float	 hdg;		 	// Compass heading in degrees, 0..360 degrees

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public GPS_RAW_INT_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public GPS_RAW_INT_class(GPS_RAW_INT_class o)
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
			lat		= rcvPacket.getInt();
			lon		= rcvPacket.getInt();
			alt		= rcvPacket.getInt();
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
						,int v_lat
						,int v_lon
						,int v_alt
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
		sndPacket.putInt(v_lat);	// Add "lat" parameter
		sndPacket.putInt(v_lon);	// Add "lon" parameter
		sndPacket.putInt(v_alt);	// Add "alt" parameter
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
 				+ ", GPS_RAW_INT_usec"
 				+ ", GPS_RAW_INT_fix_type"
 				+ ", GPS_RAW_INT_lat"
 				+ ", GPS_RAW_INT_lon"
 				+ ", GPS_RAW_INT_alt"
 				+ ", GPS_RAW_INT_eph"
 				+ ", GPS_RAW_INT_epv"
 				+ ", GPS_RAW_INT_v"
 				+ ", GPS_RAW_INT_hdg"
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
