/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: NAMED_VALUE_FLOAT(252)
--------------------------------------
%%~ Send a key-value pair as float. The use of this message is discouraged for normal 
%%~ packets, but a quite efficient way for testing new messages and getting experimental 
%%~ debug output.
--------------------------------------
*/
public class NAMED_VALUE_FLOAT_class implements Loggable
{
	public static final int msgID = 252;
	public byte[]	 name = new byte[10];		// Name of the debug variable
	public float	 value;	 	// Floating point value

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public NAMED_VALUE_FLOAT_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public NAMED_VALUE_FLOAT_class(NAMED_VALUE_FLOAT_class o)
	{
		name = o.name;
		value = o.value;
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

			// int[] mavLen = {10, 4};
			// int[] javLen = {10, 4};

			rcvPacket.getByte(name, 0, 10);
			value	= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  name
					 ,value
					 );
	}

	public byte[] encode(
						 byte[] v_name
						,float v_value
						)
	{
		// int[] mavLen = {10, 4};
		// int[] javLen = {10, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByte(v_name,0,10);	// Add "name" parameter
		sndPacket.putFloat(v_value);	// Add "value" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", NAMED_VALUE_FLOAT_name"
 				+ ", NAMED_VALUE_FLOAT_value"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + name
 				+ ", " + value
				);
		return param;
	}
}
