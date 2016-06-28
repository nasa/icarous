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
Message ID: MANUAL_SETPOINT(81)
--------------------------------------
%%~ Setpoint in roll, pitch, yaw and thrust from the operator
--------------------------------------
*/
public class MANUAL_SETPOINT_class //implements Loggable
{
	public static final int msgID = 81;
	public long	 time_boot_ms;			 	// Timestamp in milliseconds since system boot
	public float	 roll;					 	// Desired roll rate in radians per second
	public float	 pitch;					 	// Desired pitch rate in radians per second
	public float	 yaw;						 	// Desired yaw rate in radians per second
	public float	 thrust;					 	// Collective thrust, normalized to 0 .. 1
	public short	 mode_switch;				 	// Flight mode switch position, 0.. 255
	public short	 manual_override_switch;	 	// Override mode switch position, 0.. 255

	private packet rcvPacket;
	private packet sndPacket;

	public MANUAL_SETPOINT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public MANUAL_SETPOINT_class(MANUAL_SETPOINT_class o)
	{
		time_boot_ms = o.time_boot_ms;
		roll = o.roll;
		pitch = o.pitch;
		yaw = o.yaw;
		thrust = o.thrust;
		mode_switch = o.mode_switch;
		manual_override_switch = o.manual_override_switch;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 1, 1};
			// int[] javLen = {8, 4, 4, 4, 4, 2, 2};

			time_boot_ms				= rcvPacket.getLongI();
			roll						= rcvPacket.getFloat();
			pitch					= rcvPacket.getFloat();
			yaw						= rcvPacket.getFloat();
			thrust					= rcvPacket.getFloat();
			mode_switch				= rcvPacket.getShortB();
			manual_override_switch	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
					 ,roll
					 ,pitch
					 ,yaw
					 ,thrust
					 ,mode_switch
					 ,manual_override_switch
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,float v_roll
						,float v_pitch
						,float v_yaw
						,float v_thrust
						,short v_mode_switch
						,short v_manual_override_switch
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 1, 1};
		// int[] javLen = {8, 4, 4, 4, 4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_yaw);	// Add "yaw" parameter
		sndPacket.putFloat(v_thrust);	// Add "thrust" parameter
		sndPacket.putByteS(v_mode_switch);	// Add "mode_switch" parameter
		sndPacket.putByteS(v_manual_override_switch);	// Add "manual_override_switch" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", MANUAL_SETPOINT_time_boot_ms"
 				+ ", MANUAL_SETPOINT_roll"
 				+ ", MANUAL_SETPOINT_pitch"
 				+ ", MANUAL_SETPOINT_yaw"
 				+ ", MANUAL_SETPOINT_thrust"
 				+ ", MANUAL_SETPOINT_mode_switch"
 				+ ", MANUAL_SETPOINT_manual_override_switch"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + roll
 				+ ", " + pitch
 				+ ", " + yaw
 				+ ", " + thrust
 				+ ", " + mode_switch
 				+ ", " + manual_override_switch
				);
		return param;
	}
}
