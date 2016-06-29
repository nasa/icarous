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
Message ID: NAMED_VALUE_FLOAT(251)
--------------------------------------
%%~ Send a key-value pair as float. The use of this message is discouraged for normal 
%%~ packets, but a quite efficient way for testing new messages and getting experimental 
%%~ debug output.
--------------------------------------
*/
public class NAMED_VALUE_FLOAT_class //implements Loggable
{
	public static final int msgID = 251;
	public long	 time_boot_ms; 	// Timestamp (milliseconds since system boot)
	public float	 value;		 	// Floating point value
	public byte[]	 name = new byte[10];			// Name of the debug variable

	private packet rcvPacket;
	private packet sndPacket;

	public NAMED_VALUE_FLOAT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public NAMED_VALUE_FLOAT_class(NAMED_VALUE_FLOAT_class o)
	{
		time_boot_ms = o.time_boot_ms;
		value = o.value;
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

			// int[] mavLen = {4, 4, 10};
			// int[] javLen = {8, 4, 10};

			time_boot_ms	= rcvPacket.getLongI();
			value		= rcvPacket.getFloat();
			rcvPacket.getByte(name, 0, 10);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
					 ,value
					 ,name
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,float v_value
						,byte[] v_name
						)
	{
		// int[] mavLen = {4, 4, 10};
		// int[] javLen = {8, 4, 10};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putFloat(v_value);	// Add "value" parameter
		sndPacket.putByte(v_name,0,10);	// Add "name" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", NAMED_VALUE_FLOAT_time_boot_ms"
 				+ ", NAMED_VALUE_FLOAT_value"
 				+ ", NAMED_VALUE_FLOAT_name"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + value
 				+ ", " + name
				);
		return param;
	}
}
