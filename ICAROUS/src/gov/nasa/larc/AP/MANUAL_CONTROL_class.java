/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: MANUAL_CONTROL(69)
--------------------------------------
--------------------------------------
*/
public class MANUAL_CONTROL_class implements Loggable
{
	public static final int msgID = 69;
	public short	 target;			 	// The system to be controlled
	public float	 roll;			 	// roll
	public float	 pitch;			 	// pitch
	public float	 yaw;				 	// yaw
	public float	 thrust;			 	// thrust
	public short	 roll_manual;		 	// roll control enabled auto:0, manual:1
	public short	 pitch_manual;	 	// pitch auto:0, manual:1
	public short	 yaw_manual;		 	// yaw auto:0, manual:1
	public short	 thrust_manual;	 	// thrust auto:0, manual:1

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public MANUAL_CONTROL_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public MANUAL_CONTROL_class(MANUAL_CONTROL_class o)
	{
		target = o.target;
		roll = o.roll;
		pitch = o.pitch;
		yaw = o.yaw;
		thrust = o.thrust;
		roll_manual = o.roll_manual;
		pitch_manual = o.pitch_manual;
		yaw_manual = o.yaw_manual;
		thrust_manual = o.thrust_manual;
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

			// int[] mavLen = {1, 4, 4, 4, 4, 1, 1, 1, 1};
			// int[] javLen = {2, 4, 4, 4, 4, 2, 2, 2, 2};

			target			= rcvPacket.getShortB();
			roll				= rcvPacket.getFloat();
			pitch			= rcvPacket.getFloat();
			yaw				= rcvPacket.getFloat();
			thrust			= rcvPacket.getFloat();
			roll_manual		= rcvPacket.getShortB();
			pitch_manual		= rcvPacket.getShortB();
			yaw_manual		= rcvPacket.getShortB();
			thrust_manual	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  target
					 ,roll
					 ,pitch
					 ,yaw
					 ,thrust
					 ,roll_manual
					 ,pitch_manual
					 ,yaw_manual
					 ,thrust_manual
					 );
	}

	public byte[] encode(
						 short v_target
						,float v_roll
						,float v_pitch
						,float v_yaw
						,float v_thrust
						,short v_roll_manual
						,short v_pitch_manual
						,short v_yaw_manual
						,short v_thrust_manual
						)
	{
		// int[] mavLen = {1, 4, 4, 4, 4, 1, 1, 1, 1};
		// int[] javLen = {2, 4, 4, 4, 4, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target);	// Add "target" parameter
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_yaw);	// Add "yaw" parameter
		sndPacket.putFloat(v_thrust);	// Add "thrust" parameter
		sndPacket.putByteS(v_roll_manual);	// Add "roll_manual" parameter
		sndPacket.putByteS(v_pitch_manual);	// Add "pitch_manual" parameter
		sndPacket.putByteS(v_yaw_manual);	// Add "yaw_manual" parameter
		sndPacket.putByteS(v_thrust_manual);	// Add "thrust_manual" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", MANUAL_CONTROL_target"
 				+ ", MANUAL_CONTROL_roll"
 				+ ", MANUAL_CONTROL_pitch"
 				+ ", MANUAL_CONTROL_yaw"
 				+ ", MANUAL_CONTROL_thrust"
 				+ ", MANUAL_CONTROL_roll_manual"
 				+ ", MANUAL_CONTROL_pitch_manual"
 				+ ", MANUAL_CONTROL_yaw_manual"
 				+ ", MANUAL_CONTROL_thrust_manual"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target
 				+ ", " + roll
 				+ ", " + pitch
 				+ ", " + yaw
 				+ ", " + thrust
 				+ ", " + roll_manual
 				+ ", " + pitch_manual
 				+ ", " + yaw_manual
 				+ ", " + thrust_manual
				);
		return param;
	}
}
