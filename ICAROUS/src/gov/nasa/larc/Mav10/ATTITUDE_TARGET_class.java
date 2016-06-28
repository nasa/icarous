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
Message ID: ATTITUDE_TARGET(83)
--------------------------------------
%%~ Set the vehicle attitude and body angular rates.
--------------------------------------
*/
public class ATTITUDE_TARGET_class //implements Loggable
{
	public static final int msgID = 83;
	public long	 time_boot_ms;	 	// Timestamp in milliseconds since system boot
	public float[]	 q = new float[4];					// Attitude quaternion (w, x, y, z order, zero-rotation is 1, 0, 0, 0)
	public float	 body_roll_rate;	 	// Body roll rate in radians per second
	public float	 body_pitch_rate;	 	// Body roll rate in radians per second
	public float	 body_yaw_rate;	 	// Body roll rate in radians per second
	public float	 thrust;			 	// Collective thrust, normalized to 0 .. 1 (-1 .. 1 for vehicles capable of reverse trust)
	public short	 type_mask;		 	// Mappings: If any of these bits are set, the corresponding input should be ignored: bit 1: body roll rate, bit 2: body pitch rate, bit 3: body yaw rate. bit 4-bit 7: reserved, bit 8: attitude

	private packet rcvPacket;
	private packet sndPacket;

	public ATTITUDE_TARGET_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public ATTITUDE_TARGET_class(ATTITUDE_TARGET_class o)
	{
		time_boot_ms = o.time_boot_ms;
		q = o.q;
		body_roll_rate = o.body_roll_rate;
		body_pitch_rate = o.body_pitch_rate;
		body_yaw_rate = o.body_yaw_rate;
		thrust = o.thrust;
		type_mask = o.type_mask;
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

			// int[] mavLen = {4, 16, 4, 4, 4, 4, 1};
			// int[] javLen = {8, 16, 4, 4, 4, 4, 2};

			time_boot_ms		= rcvPacket.getLongI();
			rcvPacket.getByte(q, 0, 4);
			body_roll_rate	= rcvPacket.getFloat();
			body_pitch_rate	= rcvPacket.getFloat();
			body_yaw_rate	= rcvPacket.getFloat();
			thrust			= rcvPacket.getFloat();
			type_mask		= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
					 ,q
					 ,body_roll_rate
					 ,body_pitch_rate
					 ,body_yaw_rate
					 ,thrust
					 ,type_mask
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,float[] v_q
						,float v_body_roll_rate
						,float v_body_pitch_rate
						,float v_body_yaw_rate
						,float v_thrust
						,short v_type_mask
						)
	{
		// int[] mavLen = {4, 16, 4, 4, 4, 4, 1};
		// int[] javLen = {8, 16, 4, 4, 4, 4, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putByte(v_q,0,4);	// Add "q" parameter
		sndPacket.putFloat(v_body_roll_rate);	// Add "body_roll_rate" parameter
		sndPacket.putFloat(v_body_pitch_rate);	// Add "body_pitch_rate" parameter
		sndPacket.putFloat(v_body_yaw_rate);	// Add "body_yaw_rate" parameter
		sndPacket.putFloat(v_thrust);	// Add "thrust" parameter
		sndPacket.putByteS(v_type_mask);	// Add "type_mask" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", ATTITUDE_TARGET_time_boot_ms"
 				+ ", ATTITUDE_TARGET_q"
 				+ ", ATTITUDE_TARGET_body_roll_rate"
 				+ ", ATTITUDE_TARGET_body_pitch_rate"
 				+ ", ATTITUDE_TARGET_body_yaw_rate"
 				+ ", ATTITUDE_TARGET_thrust"
 				+ ", ATTITUDE_TARGET_type_mask"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + q
 				+ ", " + body_roll_rate
 				+ ", " + body_pitch_rate
 				+ ", " + body_yaw_rate
 				+ ", " + thrust
 				+ ", " + type_mask
				);
		return param;
	}
}
