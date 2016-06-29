/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: FENCE_POINT(160)
--------------------------------------
%%~ A fence point. Used to set a point when from        GCS -> MAV. Also used to return 
%%~ a point from MAV -> GCS
--------------------------------------
*/
public class FENCE_POINT_class implements Loggable
{
	public static final int msgID = 160;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public short	 idx;				 	// point index (first point is 1, 0 is for return point)
	public short	 count;			 	// total number of points (for sanity checking)
	public float	 lat;				 	// Latitude of point
	public float	 lng;				 	// Longitude of point

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public FENCE_POINT_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public FENCE_POINT_class(FENCE_POINT_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		idx = o.idx;
		count = o.count;
		lat = o.lat;
		lng = o.lng;
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

			// int[] mavLen = {1, 1, 1, 1, 4, 4};
			// int[] javLen = {2, 2, 2, 2, 4, 4};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			idx				= rcvPacket.getShortB();
			count			= rcvPacket.getShortB();
			lat				= rcvPacket.getFloat();
			lng				= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,idx
					 ,count
					 ,lat
					 ,lng
					 );
	}

	public byte[] encode(
						 short v_idx
						,short v_count
						,float v_lat
						,float v_lng
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_idx
					 ,v_count
					 ,v_lat
					 ,v_lng
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,short v_idx
						,short v_count
						,float v_lat
						,float v_lng
						)
	{
		// int[] mavLen = {1, 1, 1, 1, 4, 4};
		// int[] javLen = {2, 2, 2, 2, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_idx);	// Add "idx" parameter
		sndPacket.putByteS(v_count);	// Add "count" parameter
		sndPacket.putFloat(v_lat);	// Add "lat" parameter
		sndPacket.putFloat(v_lng);	// Add "lng" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", FENCE_POINT_target_system"
 				+ ", FENCE_POINT_target_component"
 				+ ", FENCE_POINT_idx"
 				+ ", FENCE_POINT_count"
 				+ ", FENCE_POINT_lat"
 				+ ", FENCE_POINT_lng"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + idx
 				+ ", " + count
 				+ ", " + lat
 				+ ", " + lng
				);
		return param;
	}
}
