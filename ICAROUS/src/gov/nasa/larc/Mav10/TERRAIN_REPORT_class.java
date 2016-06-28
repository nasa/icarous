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
Message ID: TERRAIN_REPORT(136)
--------------------------------------
%%~ Response from a TERRAIN_CHECK request
--------------------------------------
*/
public class TERRAIN_REPORT_class //implements Loggable
{
	public static final int msgID = 136;
	public int		 lat;				 	// Latitude (degrees *10^7)
	public int		 lon;				 	// Longitude (degrees *10^7)
	public float	 terrain_height;	 	// Terrain height in meters AMSL
	public float	 current_height;	 	// Current vehicle height above lat/lon terrain height (meters)
	public int		 spacing;			 	// grid spacing (zero if terrain at this location unavailable)
	public int		 pending;			 	// Number of 4x4 terrain blocks waiting to be received or read from disk
	public int		 loaded;			 	// Number of 4x4 terrain blocks in memory

	private packet rcvPacket;
	private packet sndPacket;

	public TERRAIN_REPORT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public TERRAIN_REPORT_class(TERRAIN_REPORT_class o)
	{
		lat = o.lat;
		lon = o.lon;
		terrain_height = o.terrain_height;
		current_height = o.current_height;
		spacing = o.spacing;
		pending = o.pending;
		loaded = o.loaded;
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

			// int[] mavLen = {4, 4, 4, 4, 2, 2, 2};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4};

			lat				= rcvPacket.getInt();
			lon				= rcvPacket.getInt();
			terrain_height	= rcvPacket.getFloat();
			current_height	= rcvPacket.getFloat();
			spacing			= rcvPacket.getIntS();
			pending			= rcvPacket.getIntS();
			loaded			= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  lat
					 ,lon
					 ,terrain_height
					 ,current_height
					 ,spacing
					 ,pending
					 ,loaded
					 );
	}

	public byte[] encode(
						 int v_lat
						,int v_lon
						,float v_terrain_height
						,float v_current_height
						,int v_spacing
						,int v_pending
						,int v_loaded
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 2, 2, 2};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putInt(v_lat);	// Add "lat" parameter
		sndPacket.putInt(v_lon);	// Add "lon" parameter
		sndPacket.putFloat(v_terrain_height);	// Add "terrain_height" parameter
		sndPacket.putFloat(v_current_height);	// Add "current_height" parameter
		sndPacket.putShortI(v_spacing);	// Add "spacing" parameter
		sndPacket.putShortI(v_pending);	// Add "pending" parameter
		sndPacket.putShortI(v_loaded);	// Add "loaded" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", TERRAIN_REPORT_lat"
 				+ ", TERRAIN_REPORT_lon"
 				+ ", TERRAIN_REPORT_terrain_height"
 				+ ", TERRAIN_REPORT_current_height"
 				+ ", TERRAIN_REPORT_spacing"
 				+ ", TERRAIN_REPORT_pending"
 				+ ", TERRAIN_REPORT_loaded"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + lat
 				+ ", " + lon
 				+ ", " + terrain_height
 				+ ", " + current_height
 				+ ", " + spacing
 				+ ", " + pending
 				+ ", " + loaded
				);
		return param;
	}
}
