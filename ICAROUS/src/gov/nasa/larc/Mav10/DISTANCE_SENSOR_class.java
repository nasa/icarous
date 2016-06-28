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
Message ID: DISTANCE_SENSOR(132)
--------------------------------------
--------------------------------------
*/
public class DISTANCE_SENSOR_class //implements Loggable
{
	public static final int msgID = 132;
	public long	 time_boot_ms;	 	// Time since system boot
	public int		 min_distance;	 	// Minimum distance the sensor can measure in centimeters
	public int		 max_distance;	 	// Maximum distance the sensor can measure in centimeters
	public int		 current_distance; 	// Current distance reading
	public short	 type;			 	// Type from MAV_DISTANCE_SENSOR enum.
	public short	 id;				 	// Onboard ID of the sensor
	public short	 orientation;		 	// Direction the sensor faces from FIXME enum.
	public short	 covariance;		 	// Measurement covariance in centimeters, 0 for unknown / invalid readings

	private packet rcvPacket;
	private packet sndPacket;

	public DISTANCE_SENSOR_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public DISTANCE_SENSOR_class(DISTANCE_SENSOR_class o)
	{
		time_boot_ms = o.time_boot_ms;
		min_distance = o.min_distance;
		max_distance = o.max_distance;
		current_distance = o.current_distance;
		type = o.type;
		id = o.id;
		orientation = o.orientation;
		covariance = o.covariance;
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

			// int[] mavLen = {4, 2, 2, 2, 1, 1, 1, 1};
			// int[] javLen = {8, 4, 4, 4, 2, 2, 2, 2};

			time_boot_ms		= rcvPacket.getLongI();
			min_distance		= rcvPacket.getIntS();
			max_distance		= rcvPacket.getIntS();
			current_distance	= rcvPacket.getIntS();
			type				= rcvPacket.getShortB();
			id				= rcvPacket.getShortB();
			orientation		= rcvPacket.getShortB();
			covariance		= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
					 ,min_distance
					 ,max_distance
					 ,current_distance
					 ,type
					 ,id
					 ,orientation
					 ,covariance
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,int v_min_distance
						,int v_max_distance
						,int v_current_distance
						,short v_type
						,short v_id
						,short v_orientation
						,short v_covariance
						)
	{
		// int[] mavLen = {4, 2, 2, 2, 1, 1, 1, 1};
		// int[] javLen = {8, 4, 4, 4, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putShortI(v_min_distance);	// Add "min_distance" parameter
		sndPacket.putShortI(v_max_distance);	// Add "max_distance" parameter
		sndPacket.putShortI(v_current_distance);	// Add "current_distance" parameter
		sndPacket.putByteS(v_type);	// Add "type" parameter
		sndPacket.putByteS(v_id);	// Add "id" parameter
		sndPacket.putByteS(v_orientation);	// Add "orientation" parameter
		sndPacket.putByteS(v_covariance);	// Add "covariance" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", DISTANCE_SENSOR_time_boot_ms"
 				+ ", DISTANCE_SENSOR_min_distance"
 				+ ", DISTANCE_SENSOR_max_distance"
 				+ ", DISTANCE_SENSOR_current_distance"
 				+ ", DISTANCE_SENSOR_type"
 				+ ", DISTANCE_SENSOR_id"
 				+ ", DISTANCE_SENSOR_orientation"
 				+ ", DISTANCE_SENSOR_covariance"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + min_distance
 				+ ", " + max_distance
 				+ ", " + current_distance
 				+ ", " + type
 				+ ", " + id
 				+ ", " + orientation
 				+ ", " + covariance
				);
		return param;
	}
}
