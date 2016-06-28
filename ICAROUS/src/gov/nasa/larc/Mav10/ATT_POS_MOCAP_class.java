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
Message ID: ATT_POS_MOCAP(138)
--------------------------------------
%%~ Motion capture attitude and position
--------------------------------------
*/
public class ATT_POS_MOCAP_class //implements Loggable
{
	public static final int msgID = 138;
	public long	 time_usec;	 	// Timestamp (micros since boot or Unix epoch)
	public float[]	 q = new float[4];				// Attitude quaternion (w, x, y, z order, zero-rotation is 1, 0, 0, 0)
	public float	 x;			 	// X position in meters (NED)
	public float	 y;			 	// Y position in meters (NED)
	public float	 z;			 	// Z position in meters (NED)

	private packet rcvPacket;
	private packet sndPacket;

	public ATT_POS_MOCAP_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public ATT_POS_MOCAP_class(ATT_POS_MOCAP_class o)
	{
		time_usec = o.time_usec;
		q = o.q;
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

			// int[] mavLen = {8, 16, 4, 4, 4};
			// int[] javLen = {8, 16, 4, 4, 4};

			time_usec	= rcvPacket.getLong();
			rcvPacket.getByte(q, 0, 4);
			x			= rcvPacket.getFloat();
			y			= rcvPacket.getFloat();
			z			= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_usec
					 ,q
					 ,x
					 ,y
					 ,z
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,float[] v_q
						,float v_x
						,float v_y
						,float v_z
						)
	{
		// int[] mavLen = {8, 16, 4, 4, 4};
		// int[] javLen = {8, 16, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
		sndPacket.putByte(v_q,0,4);	// Add "q" parameter
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
 				+ ", ATT_POS_MOCAP_time_usec"
 				+ ", ATT_POS_MOCAP_q"
 				+ ", ATT_POS_MOCAP_x"
 				+ ", ATT_POS_MOCAP_y"
 				+ ", ATT_POS_MOCAP_z"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
 				+ ", " + q
 				+ ", " + x
 				+ ", " + y
 				+ ", " + z
				);
		return param;
	}
}
