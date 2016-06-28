/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: ROLL_PITCH_YAW_THRUST_SETPOINT(57)
--------------------------------------
%%~ Setpoint in roll, pitch, yaw currently active on the system.
--------------------------------------
*/
public class ROLL_PITCH_YAW_THRUST_SETPOINT_class implements Loggable
{
	public static final int msgID = 57;
	public long	 time_us;	 	// Timestamp in micro seconds since unix epoch
	public float	 roll;	 	// Desired roll angle in radians
	public float	 pitch;	 	// Desired pitch angle in radians
	public float	 yaw;		 	// Desired yaw angle in radians
	public float	 thrust;	 	// Collective thrust, normalized to 0 .. 1

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public ROLL_PITCH_YAW_THRUST_SETPOINT_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public ROLL_PITCH_YAW_THRUST_SETPOINT_class(ROLL_PITCH_YAW_THRUST_SETPOINT_class o)
	{
		time_us = o.time_us;
		roll = o.roll;
		pitch = o.pitch;
		yaw = o.yaw;
		thrust = o.thrust;
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

			// int[] mavLen = {8, 4, 4, 4, 4};
			// int[] javLen = {8, 4, 4, 4, 4};

			time_us	= rcvPacket.getLong();
			roll		= rcvPacket.getFloat();
			pitch	= rcvPacket.getFloat();
			yaw		= rcvPacket.getFloat();
			thrust	= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_us
					 ,roll
					 ,pitch
					 ,yaw
					 ,thrust
					 );
	}

	public byte[] encode(
						 long v_time_us
						,float v_roll
						,float v_pitch
						,float v_yaw
						,float v_thrust
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 4};
		// int[] javLen = {8, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_us);	// Add "time_us" parameter
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_yaw);	// Add "yaw" parameter
		sndPacket.putFloat(v_thrust);	// Add "thrust" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", ROLL_PITCH_YAW_THRUST_SETPOINT_time_us"
 				+ ", ROLL_PITCH_YAW_THRUST_SETPOINT_roll"
 				+ ", ROLL_PITCH_YAW_THRUST_SETPOINT_pitch"
 				+ ", ROLL_PITCH_YAW_THRUST_SETPOINT_yaw"
 				+ ", ROLL_PITCH_YAW_THRUST_SETPOINT_thrust"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_us
 				+ ", " + roll
 				+ ", " + pitch
 				+ ", " + yaw
 				+ ", " + thrust
				);
		return param;
	}
}
