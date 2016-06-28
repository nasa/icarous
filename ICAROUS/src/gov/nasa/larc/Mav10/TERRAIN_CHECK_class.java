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
Message ID: TERRAIN_CHECK(135)
--------------------------------------
%%~ Request that the vehicle report terrain height at the given location. Used by GCS 
%%~ to check if vehicle has all terrain data needed for a mission.
--------------------------------------
*/
public class TERRAIN_CHECK_class //implements Loggable
{
	public static final int msgID = 135;
	public int	 lat;	 	// Latitude (degrees *10^7)
	public int	 lon;	 	// Longitude (degrees *10^7)

	private packet rcvPacket;
	private packet sndPacket;

	public TERRAIN_CHECK_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public TERRAIN_CHECK_class(TERRAIN_CHECK_class o)
	{
		lat = o.lat;
		lon = o.lon;
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

			// int[] mavLen = {4, 4};
			// int[] javLen = {4, 4};

			lat	= rcvPacket.getInt();
			lon	= rcvPacket.getInt();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  lat
					 ,lon
					 );
	}

	public byte[] encode(
						 int v_lat
						,int v_lon
						)
	{
		// int[] mavLen = {4, 4};
		// int[] javLen = {4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putInt(v_lat);	// Add "lat" parameter
		sndPacket.putInt(v_lon);	// Add "lon" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", TERRAIN_CHECK_lat"
 				+ ", TERRAIN_CHECK_lon"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + lat
 				+ ", " + lon
				);
		return param;
	}
}
