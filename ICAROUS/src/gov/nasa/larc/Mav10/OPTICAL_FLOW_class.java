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
Message ID: OPTICAL_FLOW(100)
--------------------------------------
%%~ Optical flow from a flow sensor (e.g. optical mouse sensor)
--------------------------------------
*/
public class OPTICAL_FLOW_class //implements Loggable
{
	public static final int msgID = 100;
	public long	 time_usec;		 	// Timestamp (UNIX)
	public float	 flow_comp_m_x;	 	// Flow in meters in x-sensor direction, angular-speed compensated
	public float	 flow_comp_m_y;	 	// Flow in meters in y-sensor direction, angular-speed compensated
	public float	 ground_distance;	 	// Ground distance in meters. Positive value: distance known. Negative value: Unknown distance
	public short	 flow_x;			 	// Flow in pixels * 10 in x-sensor direction (dezi-pixels)
	public short	 flow_y;			 	// Flow in pixels * 10 in y-sensor direction (dezi-pixels)
	public short	 sensor_id;		 	// Sensor ID
	public short	 quality;			 	// Optical flow quality / confidence. 0: bad, 255: maximum quality

	private packet rcvPacket;
	private packet sndPacket;

	public OPTICAL_FLOW_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public OPTICAL_FLOW_class(OPTICAL_FLOW_class o)
	{
		time_usec = o.time_usec;
		flow_comp_m_x = o.flow_comp_m_x;
		flow_comp_m_y = o.flow_comp_m_y;
		ground_distance = o.ground_distance;
		flow_x = o.flow_x;
		flow_y = o.flow_y;
		sensor_id = o.sensor_id;
		quality = o.quality;
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

			// int[] mavLen = {8, 4, 4, 4, 2, 2, 1, 1};
			// int[] javLen = {8, 4, 4, 4, 2, 2, 2, 2};

			time_usec		= rcvPacket.getLong();
			flow_comp_m_x	= rcvPacket.getFloat();
			flow_comp_m_y	= rcvPacket.getFloat();
			ground_distance	= rcvPacket.getFloat();
			flow_x			= rcvPacket.getShort();
			flow_y			= rcvPacket.getShort();
			sensor_id		= rcvPacket.getShortB();
			quality			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_usec
					 ,flow_comp_m_x
					 ,flow_comp_m_y
					 ,ground_distance
					 ,flow_x
					 ,flow_y
					 ,sensor_id
					 ,quality
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,float v_flow_comp_m_x
						,float v_flow_comp_m_y
						,float v_ground_distance
						,short v_flow_x
						,short v_flow_y
						,short v_sensor_id
						,short v_quality
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 2, 2, 1, 1};
		// int[] javLen = {8, 4, 4, 4, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
		sndPacket.putFloat(v_flow_comp_m_x);	// Add "flow_comp_m_x" parameter
		sndPacket.putFloat(v_flow_comp_m_y);	// Add "flow_comp_m_y" parameter
		sndPacket.putFloat(v_ground_distance);	// Add "ground_distance" parameter
		sndPacket.putShort(v_flow_x);	// Add "flow_x" parameter
		sndPacket.putShort(v_flow_y);	// Add "flow_y" parameter
		sndPacket.putByteS(v_sensor_id);	// Add "sensor_id" parameter
		sndPacket.putByteS(v_quality);	// Add "quality" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", OPTICAL_FLOW_time_usec"
 				+ ", OPTICAL_FLOW_flow_comp_m_x"
 				+ ", OPTICAL_FLOW_flow_comp_m_y"
 				+ ", OPTICAL_FLOW_ground_distance"
 				+ ", OPTICAL_FLOW_flow_x"
 				+ ", OPTICAL_FLOW_flow_y"
 				+ ", OPTICAL_FLOW_sensor_id"
 				+ ", OPTICAL_FLOW_quality"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
 				+ ", " + flow_comp_m_x
 				+ ", " + flow_comp_m_y
 				+ ", " + ground_distance
 				+ ", " + flow_x
 				+ ", " + flow_y
 				+ ", " + sensor_id
 				+ ", " + quality
				);
		return param;
	}
}
