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
Message ID: HIL_GPS(113)
--------------------------------------
%%~ The global position, as returned by the Global Positioning System (GPS). This is 
%%~                  NOT the global position estimate of the sytem, but rather a RAW 
%%~ sensor value. See message GLOBAL_POSITION for the global position estimate. Coordinate 
%%~ frame is right-handed, Z-axis up (GPS frame).
--------------------------------------
*/
public class HIL_GPS_class //implements Loggable
{
	public static final int msgID = 113;
	public long	 time_usec;			 	// Timestamp (microseconds since UNIX epoch or microseconds since system boot)
	public int		 lat;					 	// Latitude (WGS84), in degrees * 1E7
	public int		 lon;					 	// Longitude (WGS84), in degrees * 1E7
	public int		 alt;					 	// Altitude (AMSL, not WGS84), in meters * 1000 (positive for up)
	public int		 eph;					 	// GPS HDOP horizontal dilution of position in cm (m*100). If unknown, set to: 65535
	public int		 epv;					 	// GPS VDOP vertical dilution of position in cm (m*100). If unknown, set to: 65535
	public int		 vel;					 	// GPS ground speed (m/s * 100). If unknown, set to: 65535
	public short	 vn;					 	// GPS velocity in cm/s in NORTH direction in earth-fixed NED frame
	public short	 ve;					 	// GPS velocity in cm/s in EAST direction in earth-fixed NED frame
	public short	 vd;					 	// GPS velocity in cm/s in DOWN direction in earth-fixed NED frame
	public int		 cog;					 	// Course over ground (NOT heading, but direction of movement) in degrees * 100, 0.0..359.99 degrees. If unknown, set to: 65535
	public short	 fix_type;			 	// 0-1: no fix, 2: 2D fix, 3: 3D fix. Some applications will not use the value of this field unless it is at least two, so always correctly fill in the fix.
	public short	 satellites_visible;	 	// Number of satellites visible. If unknown, set to 255

	private packet rcvPacket;
	private packet sndPacket;

	public HIL_GPS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public HIL_GPS_class(HIL_GPS_class o)
	{
		time_usec = o.time_usec;
		lat = o.lat;
		lon = o.lon;
		alt = o.alt;
		eph = o.eph;
		epv = o.epv;
		vel = o.vel;
		vn = o.vn;
		ve = o.ve;
		vd = o.vd;
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

			// int[] mavLen = {8, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 1, 1};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 2, 2, 2, 4, 2, 2};

			time_usec			= rcvPacket.getLong();
			lat					= rcvPacket.getInt();
			lon					= rcvPacket.getInt();
			alt					= rcvPacket.getInt();
			eph					= rcvPacket.getIntS();
			epv					= rcvPacket.getIntS();
			vel					= rcvPacket.getIntS();
			vn					= rcvPacket.getShort();
			ve					= rcvPacket.getShort();
			vd					= rcvPacket.getShort();
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
					 ,vn
					 ,ve
					 ,vd
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
						,short v_vn
						,short v_ve
						,short v_vd
						,int v_cog
						,short v_fix_type
						,short v_satellites_visible
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 1, 1};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 2, 2, 2, 4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
		sndPacket.putInt(v_lat);	// Add "lat" parameter
		sndPacket.putInt(v_lon);	// Add "lon" parameter
		sndPacket.putInt(v_alt);	// Add "alt" parameter
		sndPacket.putShortI(v_eph);	// Add "eph" parameter
		sndPacket.putShortI(v_epv);	// Add "epv" parameter
		sndPacket.putShortI(v_vel);	// Add "vel" parameter
		sndPacket.putShort(v_vn);	// Add "vn" parameter
		sndPacket.putShort(v_ve);	// Add "ve" parameter
		sndPacket.putShort(v_vd);	// Add "vd" parameter
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
 				+ ", HIL_GPS_time_usec"
 				+ ", HIL_GPS_lat"
 				+ ", HIL_GPS_lon"
 				+ ", HIL_GPS_alt"
 				+ ", HIL_GPS_eph"
 				+ ", HIL_GPS_epv"
 				+ ", HIL_GPS_vel"
 				+ ", HIL_GPS_vn"
 				+ ", HIL_GPS_ve"
 				+ ", HIL_GPS_vd"
 				+ ", HIL_GPS_cog"
 				+ ", HIL_GPS_fix_type"
 				+ ", HIL_GPS_satellites_visible"
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
 				+ ", " + vn
 				+ ", " + ve
 				+ ", " + vd
 				+ ", " + cog
 				+ ", " + fix_type
 				+ ", " + satellites_visible
				);
		return param;
	}
}
