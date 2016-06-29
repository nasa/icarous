/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: LOCAL_POSITION_SETPOINT_SET(50)
--------------------------------------
%%~ Set the setpoint for a local position controller. This is the position in local 
%%~ coordinates the MAV should fly to. This message is sent by the path/waypoint planner 
%%~ to the onboard position controller. As some MAVs have a degree of freedom in 
%%~ yaw (e.g. all helicopters/quadrotors), the desired yaw angle is part of the message.
--------------------------------------
*/
public class LOCAL_POSITION_SETPOINT_SET_class implements Loggable
{
	public static final int msgID = 50;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public float	 x;				 	// x position
	public float	 y;				 	// y position
	public float	 z;				 	// z position
	public float	 yaw;				 	// Desired yaw angle

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public LOCAL_POSITION_SETPOINT_SET_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public LOCAL_POSITION_SETPOINT_SET_class(LOCAL_POSITION_SETPOINT_SET_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		x = o.x;
		y = o.y;
		z = o.z;
		yaw = o.yaw;
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
			x				= rcvPacket.getFloat();
			y				= rcvPacket.getFloat();
			z				= rcvPacket.getFloat();
			yaw				= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,x
					 ,y
					 ,z
					 ,yaw
					 );
	}

	public byte[] encode(
						 float v_x
						,float v_y
						,float v_z
						,float v_yaw
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_x
					 ,v_y
					 ,v_z
					 ,v_yaw
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,float v_x
						,float v_y
						,float v_z
						,float v_yaw
						)
	{
		// int[] mavLen = {1, 1, 4, 4, 4, 4};
		// int[] javLen = {2, 2, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putFloat(v_x);	// Add "x" parameter
		sndPacket.putFloat(v_y);	// Add "y" parameter
		sndPacket.putFloat(v_z);	// Add "z" parameter
		sndPacket.putFloat(v_yaw);	// Add "yaw" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", LOCAL_POSITION_SETPOINT_SET_target_system"
 				+ ", LOCAL_POSITION_SETPOINT_SET_target_component"
 				+ ", LOCAL_POSITION_SETPOINT_SET_x"
 				+ ", LOCAL_POSITION_SETPOINT_SET_y"
 				+ ", LOCAL_POSITION_SETPOINT_SET_z"
 				+ ", LOCAL_POSITION_SETPOINT_SET_yaw"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + x
 				+ ", " + y
 				+ ", " + z
 				+ ", " + yaw
				);
		return param;
	}
}
