/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: DEBUG(255)
--------------------------------------
%%~ Send a debug value. The index is used to discriminate between values. These values 
%%~ show up in the plot of QGroundControl as DEBUG N.
--------------------------------------
*/
public class DEBUG_class implements Loggable
{
	public static final int msgID = 255;
	public short	 ind;		 	// index of debug variable
	public float	 value;	 	// DEBUG value

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public DEBUG_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public DEBUG_class(DEBUG_class o)
	{
		ind = o.ind;
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

			// int[] mavLen = {1, 4};
			// int[] javLen = {2, 4};

			ind		= rcvPacket.getShortB();
			value	= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  ind
					 ,value
					 );
	}

	public byte[] encode(
						 short v_ind
						,float v_value
						)
	{
		// int[] mavLen = {1, 4};
		// int[] javLen = {2, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_ind);	// Add "ind" parameter
		sndPacket.putFloat(v_value);	// Add "value" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", DEBUG_ind"
 				+ ", DEBUG_value"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + ind
 				+ ", " + value
				);
		return param;
	}
}
