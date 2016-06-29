/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: OPTICAL_FLOW(100)
--------------------------------------
%%~ Optical flow from a flow sensor (e.g. optical mouse sensor)
--------------------------------------
*/
public class OPTICAL_FLOW_class implements Loggable
{
	public static final int msgID = 100;
	public long	 time;			 	// Timestamp (UNIX)
	public short	 sensor_id;		 	// Sensor ID
	public short	 flow_x;			 	// Flow in pixels in x-sensor direction
	public short	 flow_y;			 	// Flow in pixels in y-sensor direction
	public short	 quality;			 	// Optical flow quality / confidence. 0: bad, 255: maximum quality
	public float	 ground_distance;	 	// Ground distance in meters

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public OPTICAL_FLOW_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public OPTICAL_FLOW_class(OPTICAL_FLOW_class o)
	{
		time = o.time;
		sensor_id = o.sensor_id;
		flow_x = o.flow_x;
		flow_y = o.flow_y;
		quality = o.quality;
		ground_distance = o.ground_distance;
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

			// int[] mavLen = {8, 1, 2, 2, 1, 4};
			// int[] javLen = {8, 2, 2, 2, 2, 4};

			time				= rcvPacket.getLong();
			sensor_id		= rcvPacket.getShortB();
			flow_x			= rcvPacket.getShort();
			flow_y			= rcvPacket.getShort();
			quality			= rcvPacket.getShortB();
			ground_distance	= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time
					 ,sensor_id
					 ,flow_x
					 ,flow_y
					 ,quality
					 ,ground_distance
					 );
	}

	public byte[] encode(
						 long v_time
						,short v_sensor_id
						,short v_flow_x
						,short v_flow_y
						,short v_quality
						,float v_ground_distance
						)
	{
		// int[] mavLen = {8, 1, 2, 2, 1, 4};
		// int[] javLen = {8, 2, 2, 2, 2, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time);	// Add "time" parameter
		sndPacket.putByteS(v_sensor_id);	// Add "sensor_id" parameter
		sndPacket.putShort(v_flow_x);	// Add "flow_x" parameter
		sndPacket.putShort(v_flow_y);	// Add "flow_y" parameter
		sndPacket.putByteS(v_quality);	// Add "quality" parameter
		sndPacket.putFloat(v_ground_distance);	// Add "ground_distance" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", OPTICAL_FLOW_time"
 				+ ", OPTICAL_FLOW_sensor_id"
 				+ ", OPTICAL_FLOW_flow_x"
 				+ ", OPTICAL_FLOW_flow_y"
 				+ ", OPTICAL_FLOW_quality"
 				+ ", OPTICAL_FLOW_ground_distance"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time
 				+ ", " + sensor_id
 				+ ", " + flow_x
 				+ ", " + flow_y
 				+ ", " + quality
 				+ ", " + ground_distance
				);
		return param;
	}
}
