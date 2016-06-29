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
Message ID: GPS_RAW_INT(24)
--------------------------------------
%%~ The global position, as returned by the Global Positioning System (GPS). This is 
%%~                 NOT the global position estimate of the system, but rather a RAW 
%%~ sensor value. See message GLOBAL_POSITION for the global position estimate. Coordinate 
%%~ frame is right-handed, Z-axis up (GPS frame).
--------------------------------------
*/
public class GPS_RAW_INT_class //implements Loggable
{
	public static final int msgID = 24;
	public long	 time_usec;			 	// Timestamp (microseconds since UNIX epoch or microseconds since system boot)
	public int		 lat;					 	// Latitude (WGS84), in degrees * 1E7
	public int		 lon;					 	// Longitude (WGS84), in degrees * 1E7
	public int		 alt;					 	// Altitude (AMSL, NOT WGS84), in meters * 1000 (positive for up). Note that virtually all GPS modules provide the AMSL altitude in addition to the WGS84 altitude.
	public int		 eph;					 	// GPS HDOP horizontal dilution of position in cm (m*100). If unknown, set to: UINT16_MAX
	public int		 epv;					 	// GPS VDOP vertical dilution of position in cm (m*100). If unknown, set to: UINT16_MAX
	public int		 vel;					 	// GPS ground speed (m/s * 100). If unknown, set to: UINT16_MAX
	public int		 cog;					 	// Course over ground (NOT heading, but direction of movement) in degrees * 100, 0.0..359.99 degrees. If unknown, set to: UINT16_MAX
	public short	 fix_type;			 	// 0-1: no fix, 2: 2D fix, 3: 3D fix, 4: DGPS, 5: RTK. Some applications will not use the value of this field unless it is at least two, so always correctly fill in the fix.
	public short	 satellites_visible;	 	// Number of satellites visible. If unknown, set to 255

	private packet rcvPacket;
	private packet sndPacket;

	public GPS_RAW_INT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public GPS_RAW_INT_class(GPS_RAW_INT_class o)
	{
		time_usec = o.time_usec;
		lat = o.lat;
		lon = o.lon;
		alt = o.alt;
		eph = o.eph;
		epv = o.epv;
		vel = o.vel;
		cog = o.cog;
		fix_type = o.fix_type;
		satellites_visible = o.satellites_visible;
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

			// int[] mavLen = {8, 4, 4, 4, 2, 2, 2, 2, 1, 1};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 2, 2};

			time_usec			= rcvPacket.getLong();
			lat					= rcvPacket.getInt();
			lon					= rcvPacket.getInt();
			alt					= rcvPacket.getInt();
			eph					= rcvPacket.getIntS();
			epv					= rcvPacket.getIntS();
			vel					= rcvPacket.getIntS();
			cog					= rcvPacket.getIntS();
			fix_type				= rcvPacket.getShortB();
			satellites_visible	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_usec
					 ,lat
					 ,lon
					 ,alt
					 ,eph
					 ,epv
					 ,vel
					 ,cog
					 ,fix_type
					 ,satellites_visible
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,int v_lat
						,int v_lon
						,int v_alt
						,int v_eph
						,int v_epv
						,int v_vel
						,int v_cog
						,short v_fix_type
						,short v_satellites_visible
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 2, 2, 2, 2, 1, 1};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
		sndPacket.putInt(v_lat);	// Add "lat" parameter
		sndPacket.putInt(v_lon);	// Add "lon" parameter
		sndPacket.putInt(v_alt);	// Add "alt" parameter
		sndPacket.putShortI(v_eph);	// Add "eph" parameter
		sndPacket.putShortI(v_epv);	// Add "epv" parameter
		sndPacket.putShortI(v_vel);	// Add "vel" parameter
		sndPacket.putShortI(v_cog);	// Add "cog" parameter
		sndPacket.putByteS(v_fix_type);	// Add "fix_type" parameter
		sndPacket.putByteS(v_satellites_visible);	// Add "satellites_visible" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GPS_RAW_INT_time_usec"
 				+ ", GPS_RAW_INT_lat"
 				+ ", GPS_RAW_INT_lon"
 				+ ", GPS_RAW_INT_alt"
 				+ ", GPS_RAW_INT_eph"
 				+ ", GPS_RAW_INT_epv"
 				+ ", GPS_RAW_INT_vel"
 				+ ", GPS_RAW_INT_cog"
 				+ ", GPS_RAW_INT_fix_type"
 				+ ", GPS_RAW_INT_satellites_visible"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
 				+ ", " + lat
 				+ ", " + lon
 				+ ", " + alt
 				+ ", " + eph
 				+ ", " + epv
 				+ ", " + vel
 				+ ", " + cog
 				+ ", " + fix_type
 				+ ", " + satellites_visible
				);
		return param;
	}
}
