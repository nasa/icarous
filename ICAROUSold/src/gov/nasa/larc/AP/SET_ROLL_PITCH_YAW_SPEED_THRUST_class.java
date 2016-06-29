/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: SET_ROLL_PITCH_YAW_SPEED_THRUST(56)
--------------------------------------
%%~ Set roll, pitch and yaw.
--------------------------------------
*/
public class SET_ROLL_PITCH_YAW_SPEED_THRUST_class implements Loggable
{
	public static final int msgID = 56;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public float	 roll_speed;		 	// Desired roll angular speed in rad/s
	public float	 pitch_speed;		 	// Desired pitch angular speed in rad/s
	public float	 yaw_speed;		 	// Desired yaw angular speed in rad/s
	public float	 thrust;			 	// Collective thrust, normalized to 0 .. 1

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public SET_ROLL_PITCH_YAW_SPEED_THRUST_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public SET_ROLL_PITCH_YAW_SPEED_THRUST_class(SET_ROLL_PITCH_YAW_SPEED_THRUST_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		roll_speed = o.roll_speed;
		pitch_speed = o.pitch_speed;
		yaw_speed = o.yaw_speed;
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
			roll_speed		= rcvPacket.getFloat();
			pitch_speed		= rcvPacket.getFloat();
			yaw_speed		= rcvPacket.getFloat();
			thrust			= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,roll_speed
					 ,pitch_speed
					 ,yaw_speed
					 ,thrust
					 );
	}

	public byte[] encode(
						 float v_roll_speed
						,float v_pitch_speed
						,float v_yaw_speed
						,float v_thrust
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_roll_speed
					 ,v_pitch_speed
					 ,v_yaw_speed
					 ,v_thrust
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,float v_roll_speed
						,float v_pitch_speed
						,float v_yaw_speed
						,float v_thrust
						)
	{
		// int[] mavLen = {1, 1, 4, 4, 4, 4};
		// int[] javLen = {2, 2, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putFloat(v_roll_speed);	// Add "roll_speed" parameter
		sndPacket.putFloat(v_pitch_speed);	// Add "pitch_speed" parameter
		sndPacket.putFloat(v_yaw_speed);	// Add "yaw_speed" parameter
		sndPacket.putFloat(v_thrust);	// Add "thrust" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SET_ROLL_PITCH_YAW_SPEED_THRUST_target_system"
 				+ ", SET_ROLL_PITCH_YAW_SPEED_THRUST_target_component"
 				+ ", SET_ROLL_PITCH_YAW_SPEED_THRUST_roll_speed"
 				+ ", SET_ROLL_PITCH_YAW_SPEED_THRUST_pitch_speed"
 				+ ", SET_ROLL_PITCH_YAW_SPEED_THRUST_yaw_speed"
 				+ ", SET_ROLL_PITCH_YAW_SPEED_THRUST_thrust"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + roll_speed
 				+ ", " + pitch_speed
 				+ ", " + yaw_speed
 				+ ", " + thrust
				);
		return param;
	}
}
