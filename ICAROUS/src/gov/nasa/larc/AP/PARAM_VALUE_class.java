/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: PARAM_VALUE(22)
--------------------------------------
%%~ Emit the value of a onboard parameter. The inclusion of param_count and param_index 
%%~ in the message allows the recipient to keep track of received parameters and 
%%~ allows him to re-request missing parameters after a loss or timeout.
--------------------------------------
*/
public class PARAM_VALUE_class implements Loggable
{
	public static final int msgID = 22;
	public byte[]	 param_id = new byte[15];		// Onboard parameter id
	public float	 param_value;	 	// Onboard parameter value
	public int		 param_count;	 	// Total number of onboard parameters
	public int		 param_index;	 	// Index of this onboard parameter

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public PARAM_VALUE_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public PARAM_VALUE_class(PARAM_VALUE_class o)
	{
		param_id = o.param_id;
		param_value = o.param_value;
		param_count = o.param_count;
		param_index = o.param_index;
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

			// int[] mavLen = {15, 4, 2, 2};
			// int[] javLen = {15, 4, 4, 4};

			rcvPacket.getByte(param_id, 0, 15);
			param_value	= rcvPacket.getFloat();
			param_count	= rcvPacket.getIntS();
			param_index	= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  param_id
					 ,param_value
					 ,param_count
					 ,param_index
					 );
	}

	public byte[] encode(
						 byte[] v_param_id
						,float v_param_value
						,int v_param_count
						,int v_param_index
						)
	{
		// int[] mavLen = {15, 4, 2, 2};
		// int[] javLen = {15, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByte(v_param_id,0,15);	// Add "param_id" parameter
		sndPacket.putFloat(v_param_value);	// Add "param_value" parameter
		sndPacket.putShortI(v_param_count);	// Add "param_count" parameter
		sndPacket.putShortI(v_param_index);	// Add "param_index" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", PARAM_VALUE_param_id"
 				+ ", PARAM_VALUE_param_value"
 				+ ", PARAM_VALUE_param_count"
 				+ ", PARAM_VALUE_param_index"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + param_id
 				+ ", " + param_value
 				+ ", " + param_count
 				+ ", " + param_index
				);
		return param;
	}
}
