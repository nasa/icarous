/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: LOCAL_POSITION(31)
--------------------------------------
%%~ The filtered local position (e.g. fused computer vision and accelerometers). Coordinate 
%%~ frame is right-handed, Z-axis down (aeronautical frame)
--------------------------------------
*/
public class LOCAL_POSITION_class implements Loggable
{
	public static final int msgID = 31;
	public long	 usec; 	// Timestamp (microseconds since UNIX epoch or microseconds since system boot)
	public float	 x;	 	// X Position
	public float	 y;	 	// Y Position
	public float	 z;	 	// Z Position
	public float	 vx;	 	// X Speed
	public float	 vy;	 	// Y Speed
	public float	 vz;	 	// Z Speed

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public LOCAL_POSITION_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public LOCAL_POSITION_class(LOCAL_POSITION_class o)
	{
		usec = o.usec;
		x = o.x;
		y = o.y;
		z = o.z;
		vx = o.vx;
		vy = o.vy;
		vz = o.vz;
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

			usec	= rcvPacket.getLong();
			x	= rcvPacket.getFloat();
			y	= rcvPacket.getFloat();
			z	= rcvPacket.getFloat();
			vx	= rcvPacket.getFloat();
			vy	= rcvPacket.getFloat();
			vz	= rcvPacket.getFloat();
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
					 ,vx
					 ,vy
					 ,vz
					 );
	}

	public byte[] encode(
						 long v_usec
						,float v_x
						,float v_y
						,float v_z
						,float v_vx
						,float v_vy
						,float v_vz
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
		sndPacket.putFloat(v_vx);	// Add "vx" parameter
		sndPacket.putFloat(v_vy);	// Add "vy" parameter
		sndPacket.putFloat(v_vz);	// Add "vz" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", LOCAL_POSITION_usec"
 				+ ", LOCAL_POSITION_x"
 				+ ", LOCAL_POSITION_y"
 				+ ", LOCAL_POSITION_z"
 				+ ", LOCAL_POSITION_vx"
 				+ ", LOCAL_POSITION_vy"
 				+ ", LOCAL_POSITION_vz"
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
 				+ ", " + vx
 				+ ", " + vy
 				+ ", " + vz
				);
		return param;
	}
}
