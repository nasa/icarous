/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: HIL_CONTROLS(68)
--------------------------------------
%%~ Hardware in the loop control outputs
--------------------------------------
*/
public class HIL_CONTROLS_class implements Loggable
{
	public static final int msgID = 68;
	public long	 time_us;			 	// Timestamp (microseconds since UNIX epoch or microseconds since system boot)
	public float	 roll_ailerons;	 	// Control output -3 .. 1
	public float	 pitch_elevator;	 	// Control output -1 .. 1
	public float	 yaw_rudder;		 	// Control output -1 .. 1
	public float	 throttle;		 	// Throttle 0 .. 1
	public short	 mode;			 	// System mode (MAV_MODE)
	public short	 nav_mode;		 	// Navigation mode (MAV_NAV_MODE)

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public HIL_CONTROLS_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public HIL_CONTROLS_class(HIL_CONTROLS_class o)
	{
		time_us = o.time_us;
		roll_ailerons = o.roll_ailerons;
		pitch_elevator = o.pitch_elevator;
		yaw_rudder = o.yaw_rudder;
		throttle = o.throttle;
		mode = o.mode;
		nav_mode = o.nav_mode;
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

			// int[] mavLen = {8, 4, 4, 4, 4, 1, 1};
			// int[] javLen = {8, 4, 4, 4, 4, 2, 2};

			time_us			= rcvPacket.getLong();
			roll_ailerons	= rcvPacket.getFloat();
			pitch_elevator	= rcvPacket.getFloat();
			yaw_rudder		= rcvPacket.getFloat();
			throttle			= rcvPacket.getFloat();
			mode				= rcvPacket.getShortB();
			nav_mode			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_us
					 ,roll_ailerons
					 ,pitch_elevator
					 ,yaw_rudder
					 ,throttle
					 ,mode
					 ,nav_mode
					 );
	}

	public byte[] encode(
						 long v_time_us
						,float v_roll_ailerons
						,float v_pitch_elevator
						,float v_yaw_rudder
						,float v_throttle
						,short v_mode
						,short v_nav_mode
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 4, 1, 1};
		// int[] javLen = {8, 4, 4, 4, 4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_us);	// Add "time_us" parameter
		sndPacket.putFloat(v_roll_ailerons);	// Add "roll_ailerons" parameter
		sndPacket.putFloat(v_pitch_elevator);	// Add "pitch_elevator" parameter
		sndPacket.putFloat(v_yaw_rudder);	// Add "yaw_rudder" parameter
		sndPacket.putFloat(v_throttle);	// Add "throttle" parameter
		sndPacket.putByteS(v_mode);	// Add "mode" parameter
		sndPacket.putByteS(v_nav_mode);	// Add "nav_mode" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", HIL_CONTROLS_time_us"
 				+ ", HIL_CONTROLS_roll_ailerons"
 				+ ", HIL_CONTROLS_pitch_elevator"
 				+ ", HIL_CONTROLS_yaw_rudder"
 				+ ", HIL_CONTROLS_throttle"
 				+ ", HIL_CONTROLS_mode"
 				+ ", HIL_CONTROLS_nav_mode"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_us
 				+ ", " + roll_ailerons
 				+ ", " + pitch_elevator
 				+ ", " + yaw_rudder
 				+ ", " + throttle
 				+ ", " + mode
 				+ ", " + nav_mode
				);
		return param;
	}
}
