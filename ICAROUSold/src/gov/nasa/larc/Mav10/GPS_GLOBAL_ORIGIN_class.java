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
Message ID: GPS_GLOBAL_ORIGIN(49)
--------------------------------------
%%~ Once the MAV sets a new GPS-Local correspondence, this message announces the origin 
%%~ (0,0,0) position
--------------------------------------
*/
public class GPS_GLOBAL_ORIGIN_class //implements Loggable
{
	public static final int msgID = 49;
	public int	 latitude;	 	// Latitude (WGS84), in degrees * 1E7
	public int	 longitude;	 	// Longitude (WGS84), in degrees * 1E7
	public int	 altitude;	 	// Altitude (AMSL), in meters * 1000 (positive for up)

	private packet rcvPacket;
	private packet sndPacket;

	public GPS_GLOBAL_ORIGIN_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public GPS_GLOBAL_ORIGIN_class(GPS_GLOBAL_ORIGIN_class o)
	{
		latitude = o.latitude;
		longitude = o.longitude;
		altitude = o.altitude;
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

			// int[] mavLen = {4, 4, 4};
			// int[] javLen = {4, 4, 4};

			latitude		= rcvPacket.getInt();
			longitude	= rcvPacket.getInt();
			altitude		= rcvPacket.getInt();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  latitude
					 ,longitude
					 ,altitude
					 );
	}

	public byte[] encode(
						 int v_latitude
						,int v_longitude
						,int v_altitude
						)
	{
		// int[] mavLen = {4, 4, 4};
		// int[] javLen = {4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putInt(v_latitude);	// Add "latitude" parameter
		sndPacket.putInt(v_longitude);	// Add "longitude" parameter
		sndPacket.putInt(v_altitude);	// Add "altitude" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GPS_GLOBAL_ORIGIN_latitude"
 				+ ", GPS_GLOBAL_ORIGIN_longitude"
 				+ ", GPS_GLOBAL_ORIGIN_altitude"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + latitude
 				+ ", " + longitude
 				+ ", " + altitude
				);
		return param;
	}
}
