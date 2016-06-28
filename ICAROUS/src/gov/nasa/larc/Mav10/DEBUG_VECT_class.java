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
Message ID: DEBUG_VECT(250)
--------------------------------------
--------------------------------------
*/
public class DEBUG_VECT_class //implements Loggable
{
	public static final int msgID = 250;
	public long	 time_usec;	 	// Timestamp
	public float	 x;			 	// x
	public float	 y;			 	// y
	public float	 z;			 	// z
	public byte[]	 name = new byte[10];			// Name

	private packet rcvPacket;
	private packet sndPacket;

	public DEBUG_VECT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public DEBUG_VECT_class(DEBUG_VECT_class o)
	{
		time_usec = o.time_usec;
		x = o.x;
		y = o.y;
		z = o.z;
		name = o.name;
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

			// int[] mavLen = {8, 4, 4, 4, 10};
			// int[] javLen = {8, 4, 4, 4, 10};

			time_usec	= rcvPacket.getLong();
			x			= rcvPacket.getFloat();
			y			= rcvPacket.getFloat();
			z			= rcvPacket.getFloat();
			rcvPacket.getByte(name, 0, 10);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_usec
					 ,x
					 ,y
					 ,z
					 ,name
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,float v_x
						,float v_y
						,float v_z
						,byte[] v_name
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 10};
		// int[] javLen = {8, 4, 4, 4, 10};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
		sndPacket.putFloat(v_x);	// Add "x" parameter
		sndPacket.putFloat(v_y);	// Add "y" parameter
		sndPacket.putFloat(v_z);	// Add "z" parameter
		sndPacket.putByte(v_name,0,10);	// Add "name" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", DEBUG_VECT_time_usec"
 				+ ", DEBUG_VECT_x"
 				+ ", DEBUG_VECT_y"
 				+ ", DEBUG_VECT_z"
 				+ ", DEBUG_VECT_name"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
 				+ ", " + x
 				+ ", " + y
 				+ ", " + z
 				+ ", " + name
				);
		return param;
	}
}
