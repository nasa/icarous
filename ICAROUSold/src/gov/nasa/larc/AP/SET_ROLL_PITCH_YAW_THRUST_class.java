/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: SET_ROLL_PITCH_YAW_THRUST(55)
--------------------------------------
%%~ Set roll, pitch and yaw.
--------------------------------------
*/
public class SET_ROLL_PITCH_YAW_THRUST_class implements Loggable
{
	public static final int msgID = 55;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public float	 roll;			 	// Desired roll angle in radians
	public float	 pitch;			 	// Desired pitch angle in radians
	public float	 yaw;				 	// Desired yaw angle in radians
	public float	 thrust;			 	// Collective thrust, normalized to 0 .. 1

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public SET_ROLL_PITCH_YAW_THRUST_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public SET_ROLL_PITCH_YAW_THRUST_class(SET_ROLL_PITCH_YAW_THRUST_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
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

			// int[] mavLen = {1, 1, 4, 4, 4, 4};
			// int[] javLen = {2, 2, 4, 4, 4, 4};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			roll				= rcvPacket.getFloat();
			pitch			= rcvPacket.getFloat();
			yaw				= rcvPacket.getFloat();
			thrust			= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,roll
					 ,pitch
					 ,yaw
					 ,thrust
					 );
	}

	public byte[] encode(
						 float v_roll
						,float v_pitch
						,float v_yaw
						,float v_thrust
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_roll
					 ,v_pitch
					 ,v_yaw
					 ,v_thrust
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,float v_roll
						,float v_pitch
						,float v_yaw
						,float v_thrust
						)
	{
		// int[] mavLen = {1, 1, 4, 4, 4, 4};
		// int[] javLen = {2, 2, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
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
 				+ ", SET_ROLL_PITCH_YAW_THRUST_target_system"
 				+ ", SET_ROLL_PITCH_YAW_THRUST_target_component"
 				+ ", SET_ROLL_PITCH_YAW_THRUST_roll"
 				+ ", SET_ROLL_PITCH_YAW_THRUST_pitch"
 				+ ", SET_ROLL_PITCH_YAW_THRUST_yaw"
 				+ ", SET_ROLL_PITCH_YAW_THRUST_thrust"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + roll
 				+ ", " + pitch
 				+ ", " + yaw
 				+ ", " + thrust
				);
		return param;
	}
}
