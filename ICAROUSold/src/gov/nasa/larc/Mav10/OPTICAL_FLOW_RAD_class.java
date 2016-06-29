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
Message ID: OPTICAL_FLOW_RAD(106)
--------------------------------------
%%~ Optical flow from an angular rate flow sensor (e.g. PX4FLOW or mouse sensor)
--------------------------------------
*/
public class OPTICAL_FLOW_RAD_class //implements Loggable
{
	public static final int msgID = 106;
	public long	 time_usec;				 	// Timestamp (microseconds, synced to UNIX time or since system boot)
	public long	 integration_time_us;		 	// Integration time in microseconds. Divide integrated_x and integrated_y by the integration time to obtain average flow. The integration time also indicates the.
	public float	 integrated_x;			 	// Flow in radians around X axis (Sensor RH rotation about the X axis induces a positive flow. Sensor linear motion along the positive Y axis induces a negative flow.)
	public float	 integrated_y;			 	// Flow in radians around Y axis (Sensor RH rotation about the Y axis induces a positive flow. Sensor linear motion along the positive X axis induces a positive flow.)
	public float	 integrated_xgyro;		 	// RH rotation around X axis (rad)
	public float	 integrated_ygyro;		 	// RH rotation around Y axis (rad)
	public float	 integrated_zgyro;		 	// RH rotation around Z axis (rad)
	public long	 time_delta_distance_us;	 	// Time in microseconds since the distance was sampled.
	public float	 distance;				 	// Distance to the center of the flow field in meters. Positive value (including zero): distance known. Negative value: Unknown distance.
	public short	 temperature;				 	// Temperature * 100 in centi-degrees Celsius
	public short	 sensor_id;				 	// Sensor ID
	public short	 quality;					 	// Optical flow quality / confidence. 0: no valid flow, 255: maximum quality

	private packet rcvPacket;
	private packet sndPacket;

	public OPTICAL_FLOW_RAD_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public OPTICAL_FLOW_RAD_class(OPTICAL_FLOW_RAD_class o)
	{
		time_usec = o.time_usec;
		integration_time_us = o.integration_time_us;
		integrated_x = o.integrated_x;
		integrated_y = o.integrated_y;
		integrated_xgyro = o.integrated_xgyro;
		integrated_ygyro = o.integrated_ygyro;
		integrated_zgyro = o.integrated_zgyro;
		time_delta_distance_us = o.time_delta_distance_us;
		distance = o.distance;
		temperature = o.temperature;
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

			// int[] mavLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 2, 1, 1};
			// int[] javLen = {8, 8, 4, 4, 4, 4, 4, 8, 4, 2, 2, 2};

			time_usec				= rcvPacket.getLong();
			integration_time_us		= rcvPacket.getLongI();
			integrated_x				= rcvPacket.getFloat();
			integrated_y				= rcvPacket.getFloat();
			integrated_xgyro			= rcvPacket.getFloat();
			integrated_ygyro			= rcvPacket.getFloat();
			integrated_zgyro			= rcvPacket.getFloat();
			time_delta_distance_us	= rcvPacket.getLongI();
			distance					= rcvPacket.getFloat();
			temperature				= rcvPacket.getShort();
			sensor_id				= rcvPacket.getShortB();
			quality					= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_usec
					 ,integration_time_us
					 ,integrated_x
					 ,integrated_y
					 ,integrated_xgyro
					 ,integrated_ygyro
					 ,integrated_zgyro
					 ,time_delta_distance_us
					 ,distance
					 ,temperature
					 ,sensor_id
					 ,quality
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,long v_integration_time_us
						,float v_integrated_x
						,float v_integrated_y
						,float v_integrated_xgyro
						,float v_integrated_ygyro
						,float v_integrated_zgyro
						,long v_time_delta_distance_us
						,float v_distance
						,short v_temperature
						,short v_sensor_id
						,short v_quality
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 2, 1, 1};
		// int[] javLen = {8, 8, 4, 4, 4, 4, 4, 8, 4, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
		sndPacket.putIntL(v_integration_time_us);	// Add "integration_time_us" parameter
		sndPacket.putFloat(v_integrated_x);	// Add "integrated_x" parameter
		sndPacket.putFloat(v_integrated_y);	// Add "integrated_y" parameter
		sndPacket.putFloat(v_integrated_xgyro);	// Add "integrated_xgyro" parameter
		sndPacket.putFloat(v_integrated_ygyro);	// Add "integrated_ygyro" parameter
		sndPacket.putFloat(v_integrated_zgyro);	// Add "integrated_zgyro" parameter
		sndPacket.putIntL(v_time_delta_distance_us);	// Add "time_delta_distance_us" parameter
		sndPacket.putFloat(v_distance);	// Add "distance" parameter
		sndPacket.putShort(v_temperature);	// Add "temperature" parameter
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
 				+ ", OPTICAL_FLOW_RAD_time_usec"
 				+ ", OPTICAL_FLOW_RAD_integration_time_us"
 				+ ", OPTICAL_FLOW_RAD_integrated_x"
 				+ ", OPTICAL_FLOW_RAD_integrated_y"
 				+ ", OPTICAL_FLOW_RAD_integrated_xgyro"
 				+ ", OPTICAL_FLOW_RAD_integrated_ygyro"
 				+ ", OPTICAL_FLOW_RAD_integrated_zgyro"
 				+ ", OPTICAL_FLOW_RAD_time_delta_distance_us"
 				+ ", OPTICAL_FLOW_RAD_distance"
 				+ ", OPTICAL_FLOW_RAD_temperature"
 				+ ", OPTICAL_FLOW_RAD_sensor_id"
 				+ ", OPTICAL_FLOW_RAD_quality"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
 				+ ", " + integration_time_us
 				+ ", " + integrated_x
 				+ ", " + integrated_y
 				+ ", " + integrated_xgyro
 				+ ", " + integrated_ygyro
 				+ ", " + integrated_zgyro
 				+ ", " + time_delta_distance_us
 				+ ", " + distance
 				+ ", " + temperature
 				+ ", " + sensor_id
 				+ ", " + quality
				);
		return param;
	}
}
