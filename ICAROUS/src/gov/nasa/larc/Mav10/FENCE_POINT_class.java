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
Message ID: FENCE_POINT(160)
--------------------------------------
%%~ A fence point. Used to set a point when from        GCS -> MAV. Also used to return 
%%~ a point from MAV -> GCS
--------------------------------------
*/
public class FENCE_POINT_class //implements Loggable
{
	public static final int msgID = 160;
	public float	 lat;				 	// Latitude of point
	public float	 lng;				 	// Longitude of point
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public short	 idx;				 	// point index (first point is 1, 0 is for return point)
	public short	 count;			 	// total number of points (for sanity checking)

	private packet rcvPacket;
	private packet sndPacket;

	public FENCE_POINT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public FENCE_POINT_class(FENCE_POINT_class o)
	{
		lat = o.lat;
		lng = o.lng;
		target_system = o.target_system;
		target_component = o.target_component;
		idx = o.idx;
		count = o.count;
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

			// int[] mavLen = {4, 4, 1, 1, 1, 1};
			// int[] javLen = {4, 4, 2, 2, 2, 2};

			lat				= rcvPacket.getFloat();
			lng				= rcvPacket.getFloat();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			idx				= rcvPacket.getShortB();
			count			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  lat
					 ,lng
					 ,(short)1
					 ,(short)1
					 ,idx
					 ,count
					 );
	}

	public byte[] encode(
						 float v_lat
						,float v_lng
						,short v_idx
						,short v_count
						)
	{
		return encode(
					  v_lat
					 ,v_lng
					 ,  (short)1
					 ,  (short)1
					 ,v_idx
					 ,v_count
					 );
	}

	public byte[] encode(
						 float v_lat
						,float v_lng
						,short v_target_system
						,short v_target_component
						,short v_idx
						,short v_count
						)
	{
		// int[] mavLen = {4, 4, 1, 1, 1, 1};
		// int[] javLen = {4, 4, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_lat);	// Add "lat" parameter
		sndPacket.putFloat(v_lng);	// Add "lng" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_idx);	// Add "idx" parameter
		sndPacket.putByteS(v_count);	// Add "count" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", FENCE_POINT_lat"
 				+ ", FENCE_POINT_lng"
 				+ ", FENCE_POINT_target_system"
 				+ ", FENCE_POINT_target_component"
 				+ ", FENCE_POINT_idx"
 				+ ", FENCE_POINT_count"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + lat
 				+ ", " + lng
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + idx
 				+ ", " + count
				);
		return param;
	}
}
