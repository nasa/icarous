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
Message ID: GLOBAL_VISION_POSITION_ESTIMATE(101)
--------------------------------------
--------------------------------------
*/
public class GLOBAL_VISION_POSITION_ESTIMATE_class //implements Loggable
{
	public static final int msgID = 101;
	public long	 usec;	 	// Timestamp (microseconds, synced to UNIX time or since system boot)
	public float	 x;		 	// Global X position
	public float	 y;		 	// Global Y position
	public float	 z;		 	// Global Z position
	public float	 roll;	 	// Roll angle in rad
	public float	 pitch;	 	// Pitch angle in rad
	public float	 yaw;		 	// Yaw angle in rad

	private packet rcvPacket;
	private packet sndPacket;

	public GLOBAL_VISION_POSITION_ESTIMATE_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public GLOBAL_VISION_POSITION_ESTIMATE_class(GLOBAL_VISION_POSITION_ESTIMATE_class o)
	{
		usec = o.usec;
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

			// int[] mavLen = {8, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4};

			usec		= rcvPacket.getLong();
			x		= rcvPacket.getFloat();
			y		= rcvPacket.getFloat();
			z		= rcvPacket.getFloat();
			roll		= rcvPacket.getFloat();
			pitch	= rcvPacket.getFloat();
			yaw		= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  usec
					 ,x
					 ,y
					 ,z
					 ,roll
					 ,pitch
					 ,yaw
					 );
	}

	public byte[] encode(
						 long v_usec
						,float v_x
						,float v_y
						,float v_z
						,float v_roll
						,float v_pitch
						,float v_yaw
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_usec);	// Add "usec" parameter
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
 				+ ", GLOBAL_VISION_POSITION_ESTIMATE_usec"
 				+ ", GLOBAL_VISION_POSITION_ESTIMATE_x"
 				+ ", GLOBAL_VISION_POSITION_ESTIMATE_y"
 				+ ", GLOBAL_VISION_POSITION_ESTIMATE_z"
 				+ ", GLOBAL_VISION_POSITION_ESTIMATE_roll"
 				+ ", GLOBAL_VISION_POSITION_ESTIMATE_pitch"
 				+ ", GLOBAL_VISION_POSITION_ESTIMATE_yaw"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + usec
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
