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
Message ID: DEBUG(254)
--------------------------------------
%%~ Send a debug value. The index is used to discriminate between values. These values 
%%~ show up in the plot of QGroundControl as DEBUG N.
--------------------------------------
*/
public class DEBUG_class //implements Loggable
{
	public static final int msgID = 254;
	public long	 time_boot_ms; 	// Timestamp (milliseconds since system boot)
	public float	 value;		 	// DEBUG value
	public short	 ind;			 	// index of debug variable

	private packet rcvPacket;
	private packet sndPacket;

	public DEBUG_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public DEBUG_class(DEBUG_class o)
	{
		time_boot_ms = o.time_boot_ms;
		value = o.value;
		ind = o.ind;
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

			// int[] mavLen = {4, 4, 1};
			// int[] javLen = {8, 4, 2};

			time_boot_ms	= rcvPacket.getLongI();
			value		= rcvPacket.getFloat();
			ind			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
					 ,value
					 ,ind
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,float v_value
						,short v_ind
						)
	{
		// int[] mavLen = {4, 4, 1};
		// int[] javLen = {8, 4, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putFloat(v_value);	// Add "value" parameter
		sndPacket.putByteS(v_ind);	// Add "ind" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", DEBUG_time_boot_ms"
 				+ ", DEBUG_value"
 				+ ", DEBUG_ind"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + value
 				+ ", " + ind
				);
		return param;
	}
}
