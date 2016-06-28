/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: POSITION_TARGET(63)
--------------------------------------
%%~ The goal position of the system. This position is the input to any navigation or 
%%~ path planning algorithm and does NOT represent the current controller setpoint.
--------------------------------------
*/
public class POSITION_TARGET_class implements Loggable
{
	public static final int msgID = 63;
	public float	 x;	 	// x position
	public float	 y;	 	// y position
	public float	 z;	 	// z position
	public float	 yaw;	 	// yaw orientation in radians, 0 = NORTH

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public POSITION_TARGET_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public POSITION_TARGET_class(POSITION_TARGET_class o)
	{
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

			// int[] mavLen = {4, 4, 4, 4};
			// int[] javLen = {4, 4, 4, 4};

			x	= rcvPacket.getFloat();
			y	= rcvPacket.getFloat();
			z	= rcvPacket.getFloat();
			yaw	= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  x
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
		// int[] mavLen = {4, 4, 4, 4};
		// int[] javLen = {4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
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
 				+ ", POSITION_TARGET_x"
 				+ ", POSITION_TARGET_y"
 				+ ", POSITION_TARGET_z"
 				+ ", POSITION_TARGET_yaw"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + x
 				+ ", " + y
 				+ ", " + z
 				+ ", " + yaw
				);
		return param;
	}
}
