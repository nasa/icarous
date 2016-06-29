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
Message ID: LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET(89)
--------------------------------------
%%~ The offset in X, Y, Z and yaw between the LOCAL_POSITION_NED messages of MAV X and 
%%~ the global coordinate frame in NED coordinates. Coordinate frame is right-handed, 
%%~ Z-axis down (aeronautical frame, NED / north-east-down convention)
--------------------------------------
*/
public class LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET_class //implements Loggable
{
	public static final int msgID = 89;
	public long	 time_boot_ms; 	// Timestamp (milliseconds since system boot)
	public float	 x;			 	// X Position
	public float	 y;			 	// Y Position
	public float	 z;			 	// Z Position
	public float	 roll;		 	// Roll
	public float	 pitch;		 	// Pitch
	public float	 yaw;			 	// Yaw

	private packet rcvPacket;
	private packet sndPacket;

	public LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET_class(LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET_class o)
	{
		time_boot_ms = o.time_boot_ms;
		x = o.x;
		y = o.y;
		z = o.z;
		roll = o.roll;
		pitch = o.pitch;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4};

			time_boot_ms	= rcvPacket.getLongI();
			x			= rcvPacket.getFloat();
			y			= rcvPacket.getFloat();
			z			= rcvPacket.getFloat();
			roll			= rcvPacket.getFloat();
			pitch		= rcvPacket.getFloat();
			yaw			= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
					 ,x
					 ,y
					 ,z
					 ,roll
					 ,pitch
					 ,yaw
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,float v_x
						,float v_y
						,float v_z
						,float v_roll
						,float v_pitch
						,float v_yaw
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putFloat(v_x);	// Add "x" parameter
		sndPacket.putFloat(v_y);	// Add "y" parameter
		sndPacket.putFloat(v_z);	// Add "z" parameter
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_yaw);	// Add "yaw" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET_time_boot_ms"
 				+ ", LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET_x"
 				+ ", LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET_y"
 				+ ", LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET_z"
 				+ ", LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET_roll"
 				+ ", LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET_pitch"
 				+ ", LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET_yaw"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + x
 				+ ", " + y
 				+ ", " + z
 				+ ", " + roll
 				+ ", " + pitch
 				+ ", " + yaw
				);
		return param;
	}
}
