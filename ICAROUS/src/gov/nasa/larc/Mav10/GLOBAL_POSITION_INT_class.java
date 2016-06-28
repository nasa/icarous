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
Message ID: GLOBAL_POSITION_INT(33)
--------------------------------------
%%~ The filtered global position (e.g. fused GPS and accelerometers). The position is 
%%~ in GPS-frame (right-handed, Z-up). It                is designed as scaled integer 
%%~ message since the resolution of float is not sufficient.
--------------------------------------
*/
public class GLOBAL_POSITION_INT_class //implements Loggable
{
	public static final int msgID = 33;
	public long	 time_boot_ms; 	// Timestamp (milliseconds since system boot)
	public int		 lat;			 	// Latitude, expressed as * 1E7
	public int		 lon;			 	// Longitude, expressed as * 1E7
	public int		 alt;			 	// Altitude in meters, expressed as * 1000 (millimeters), AMSL (not WGS84 - note that virtually all GPS modules provide the AMSL as well)
	public int		 relative_alt; 	// Altitude above ground in meters, expressed as * 1000 (millimeters)
	public short	 vx;			 	// Ground X Speed (Latitude), expressed as m/s * 100
	public short	 vy;			 	// Ground Y Speed (Longitude), expressed as m/s * 100
	public short	 vz;			 	// Ground Z Speed (Altitude), expressed as m/s * 100
	public int		 hdg;			 	// Compass heading in degrees * 100, 0.0..359.99 degrees. If unknown, set to: UINT16_MAX

	private packet rcvPacket;
	private packet sndPacket;

	public GLOBAL_POSITION_INT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public GLOBAL_POSITION_INT_class(GLOBAL_POSITION_INT_class o)
	{
		time_boot_ms = o.time_boot_ms;
		lat = o.lat;
		lon = o.lon;
		alt = o.alt;
		relative_alt = o.relative_alt;
		vx = o.vx;
		vy = o.vy;
		vz = o.vz;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 2, 2, 2, 2};
			// int[] javLen = {8, 4, 4, 4, 4, 2, 2, 2, 4};

			time_boot_ms	= rcvPacket.getLongI();
			lat			= rcvPacket.getInt();
			lon			= rcvPacket.getInt();
			alt			= rcvPacket.getInt();
			relative_alt	= rcvPacket.getInt();
			vx			= rcvPacket.getShort();
			vy			= rcvPacket.getShort();
			vz			= rcvPacket.getShort();
			hdg			= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
					 ,lat
					 ,lon
					 ,alt
					 ,relative_alt
					 ,vx
					 ,vy
					 ,vz
					 ,hdg
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,int v_lat
						,int v_lon
						,int v_alt
						,int v_relative_alt
						,short v_vx
						,short v_vy
						,short v_vz
						,int v_hdg
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 2, 2, 2, 2};
		// int[] javLen = {8, 4, 4, 4, 4, 2, 2, 2, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putInt(v_lat);	// Add "lat" parameter
		sndPacket.putInt(v_lon);	// Add "lon" parameter
		sndPacket.putInt(v_alt);	// Add "alt" parameter
		sndPacket.putInt(v_relative_alt);	// Add "relative_alt" parameter
		sndPacket.putShort(v_vx);	// Add "vx" parameter
		sndPacket.putShort(v_vy);	// Add "vy" parameter
		sndPacket.putShort(v_vz);	// Add "vz" parameter
		sndPacket.putShortI(v_hdg);	// Add "hdg" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GLOBAL_POSITION_INT_time_boot_ms"
 				+ ", GLOBAL_POSITION_INT_lat"
 				+ ", GLOBAL_POSITION_INT_lon"
 				+ ", GLOBAL_POSITION_INT_alt"
 				+ ", GLOBAL_POSITION_INT_relative_alt"
 				+ ", GLOBAL_POSITION_INT_vx"
 				+ ", GLOBAL_POSITION_INT_vy"
 				+ ", GLOBAL_POSITION_INT_vz"
 				+ ", GLOBAL_POSITION_INT_hdg"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + lat
 				+ ", " + lon
 				+ ", " + alt
 				+ ", " + relative_alt
 				+ ", " + vx
 				+ ", " + vy
 				+ ", " + vz
 				+ ", " + hdg
				);
		return param;
	}
}
