/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: GLOBAL_POSITION_INT(73)
--------------------------------------
%%~ The filtered global position (e.g. fused GPS and accelerometers). The position is 
%%~ in GPS-frame (right-handed, Z-up)
--------------------------------------
*/
public class GLOBAL_POSITION_INT_class implements Loggable
{
	public static final int msgID = 73;
	public int		 lat;	 	// Latitude, expressed as * 1E7
	public int		 lon;	 	// Longitude, expressed as * 1E7
	public int		 alt;	 	// Altitude in meters, expressed as * 1000 (millimeters)
	public short	 vx;	 	// Ground X Speed (Latitude), expressed as m/s * 100
	public short	 vy;	 	// Ground Y Speed (Longitude), expressed as m/s * 100
	public short	 vz;	 	// Ground Z Speed (Altitude), expressed as m/s * 100

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public GLOBAL_POSITION_INT_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public GLOBAL_POSITION_INT_class(GLOBAL_POSITION_INT_class o)
	{
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

			// int[] mavLen = {4, 4, 4, 2, 2, 2};
			// int[] javLen = {4, 4, 4, 2, 2, 2};

			lat	= rcvPacket.getInt();
			lon	= rcvPacket.getInt();
			alt	= rcvPacket.getInt();
			vx	= rcvPacket.getShort();
			vy	= rcvPacket.getShort();
			vz	= rcvPacket.getShort();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  lat
					 ,lon
					 ,alt
					 ,vx
					 ,vy
					 ,vz
					 );
	}

	public byte[] encode(
						 int v_lat
						,int v_lon
						,int v_alt
						,short v_vx
						,short v_vy
						,short v_vz
						)
	{
		// int[] mavLen = {4, 4, 4, 2, 2, 2};
		// int[] javLen = {4, 4, 4, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putInt(v_lat);	// Add "lat" parameter
		sndPacket.putInt(v_lon);	// Add "lon" parameter
		sndPacket.putInt(v_alt);	// Add "alt" parameter
		sndPacket.putShort(v_vx);	// Add "vx" parameter
		sndPacket.putShort(v_vy);	// Add "vy" parameter
		sndPacket.putShort(v_vz);	// Add "vz" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GLOBAL_POSITION_INT_lat"
 				+ ", GLOBAL_POSITION_INT_lon"
 				+ ", GLOBAL_POSITION_INT_alt"
 				+ ", GLOBAL_POSITION_INT_vx"
 				+ ", GLOBAL_POSITION_INT_vy"
 				+ ", GLOBAL_POSITION_INT_vz"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
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
