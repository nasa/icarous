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
Message ID: TERRAIN_DATA(134)
--------------------------------------
%%~ Terrain data sent from GCS. The lat/lon and grid_spacing must be the same as a lat/lon 
%%~ from a TERRAIN_REQUEST
--------------------------------------
*/
public class TERRAIN_DATA_class //implements Loggable
{
	public static final int msgID = 134;
	public int		 lat;			 	// Latitude of SW corner of first grid (degrees *10^7)
	public int		 lon;			 	// Longitude of SW corner of first grid (in degrees *10^7)
	public int		 grid_spacing; 	// Grid spacing in meters
	public short[]	 data = new short[16];			// Terrain data in meters AMSL
	public short	 gridbit;		 	// bit within the terrain request mask

	private packet rcvPacket;
	private packet sndPacket;

	public TERRAIN_DATA_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public TERRAIN_DATA_class(TERRAIN_DATA_class o)
	{
		lat = o.lat;
		lon = o.lon;
		grid_spacing = o.grid_spacing;
		data = o.data;
		gridbit = o.gridbit;
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

			// int[] mavLen = {4, 4, 2, 32, 1};
			// int[] javLen = {4, 4, 4, 32, 2};

			lat			= rcvPacket.getInt();
			lon			= rcvPacket.getInt();
			grid_spacing	= rcvPacket.getIntS();
			rcvPacket.getByte(data, 0, 16);
			gridbit		= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  lat
					 ,lon
					 ,grid_spacing
					 ,data
					 ,gridbit
					 );
	}

	public byte[] encode(
						 int v_lat
						,int v_lon
						,int v_grid_spacing
						,short[] v_data
						,short v_gridbit
						)
	{
		// int[] mavLen = {4, 4, 2, 32, 1};
		// int[] javLen = {4, 4, 4, 32, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putInt(v_lat);	// Add "lat" parameter
		sndPacket.putInt(v_lon);	// Add "lon" parameter
		sndPacket.putShortI(v_grid_spacing);	// Add "grid_spacing" parameter
		sndPacket.putByte(v_data,0,16);	// Add "data" parameter
		sndPacket.putByteS(v_gridbit);	// Add "gridbit" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", TERRAIN_DATA_lat"
 				+ ", TERRAIN_DATA_lon"
 				+ ", TERRAIN_DATA_grid_spacing"
 				+ ", TERRAIN_DATA_data"
 				+ ", TERRAIN_DATA_gridbit"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + lat
 				+ ", " + lon
 				+ ", " + grid_spacing
 				+ ", " + data
 				+ ", " + gridbit
				);
		return param;
	}
}
