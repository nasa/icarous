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
Message ID: LOCAL_POSITION_NED(32)
--------------------------------------
%%~ The filtered local position (e.g. fused computer vision and accelerometers). Coordinate 
%%~ frame is right-handed, Z-axis down (aeronautical frame, NED / north-east-down 
%%~ convention)
--------------------------------------
*/
public class LOCAL_POSITION_NED_class //implements Loggable
{
	public static final int msgID = 32;
	public long	 time_boot_ms; 	// Timestamp (milliseconds since system boot)
	public float	 x;			 	// X Position
	public float	 y;			 	// Y Position
	public float	 z;			 	// Z Position
	public float	 vx;			 	// X Speed
	public float	 vy;			 	// Y Speed
	public float	 vz;			 	// Z Speed

	private packet rcvPacket;
	private packet sndPacket;

	public LOCAL_POSITION_NED_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public LOCAL_POSITION_NED_class(LOCAL_POSITION_NED_class o)
	{
		time_boot_ms = o.time_boot_ms;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4};

			time_boot_ms	= rcvPacket.getLongI();
			x			= rcvPacket.getFloat();
			y			= rcvPacket.getFloat();
			z			= rcvPacket.getFloat();
			vx			= rcvPacket.getFloat();
			vy			= rcvPacket.getFloat();
			vz			= rcvPacket.getFloat();
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
					 ,vx
					 ,vy
					 ,vz
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,float v_x
						,float v_y
						,float v_z
						,float v_vx
						,float v_vy
						,float v_vz
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
 				+ ", LOCAL_POSITION_NED_time_boot_ms"
 				+ ", LOCAL_POSITION_NED_x"
 				+ ", LOCAL_POSITION_NED_y"
 				+ ", LOCAL_POSITION_NED_z"
 				+ ", LOCAL_POSITION_NED_vx"
 				+ ", LOCAL_POSITION_NED_vy"
 				+ ", LOCAL_POSITION_NED_vz"
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
 				+ ", " + vx
 				+ ", " + vy
 				+ ", " + vz
				);
		return param;
	}
}
