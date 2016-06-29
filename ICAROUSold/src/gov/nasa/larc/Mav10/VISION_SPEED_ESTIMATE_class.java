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
Message ID: VISION_SPEED_ESTIMATE(103)
--------------------------------------
--------------------------------------
*/
public class VISION_SPEED_ESTIMATE_class //implements Loggable
{
	public static final int msgID = 103;
	public long	 usec; 	// Timestamp (microseconds, synced to UNIX time or since system boot)
	public float	 x;	 	// Global X speed
	public float	 y;	 	// Global Y speed
	public float	 z;	 	// Global Z speed

	private packet rcvPacket;
	private packet sndPacket;

	public VISION_SPEED_ESTIMATE_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public VISION_SPEED_ESTIMATE_class(VISION_SPEED_ESTIMATE_class o)
	{
		usec = o.usec;
		x = o.x;
		y = o.y;
		z = o.z;
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

			// int[] mavLen = {8, 4, 4, 4};
			// int[] javLen = {8, 4, 4, 4};

			usec	= rcvPacket.getLong();
			x	= rcvPacket.getFloat();
			y	= rcvPacket.getFloat();
			z	= rcvPacket.getFloat();
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
					 );
	}

	public byte[] encode(
						 long v_usec
						,float v_x
						,float v_y
						,float v_z
						)
	{
		// int[] mavLen = {8, 4, 4, 4};
		// int[] javLen = {8, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_usec);	// Add "usec" parameter
		sndPacket.putFloat(v_x);	// Add "x" parameter
		sndPacket.putFloat(v_y);	// Add "y" parameter
		sndPacket.putFloat(v_z);	// Add "z" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", VISION_SPEED_ESTIMATE_usec"
 				+ ", VISION_SPEED_ESTIMATE_x"
 				+ ", VISION_SPEED_ESTIMATE_y"
 				+ ", VISION_SPEED_ESTIMATE_z"
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
				);
		return param;
	}
}
