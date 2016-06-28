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
Message ID: PARAM_VALUE(22)
--------------------------------------
%%~ Emit the value of a onboard parameter. The inclusion of param_count and param_index 
%%~ in the message allows the recipient to keep track of received parameters and 
%%~ allows him to re-request missing parameters after a loss or timeout.
--------------------------------------
*/
public class PARAM_VALUE_class //implements Loggable
{
	public static final int msgID = 22;
	public float	 param_value;	 	// Onboard parameter value
	public int		 param_count;	 	// Total number of onboard parameters
	public int		 param_index;	 	// Index of this onboard parameter
	public byte[]	 param_id = new byte[16];		// Onboard parameter id, terminated by NULL if the length is less than 16 human-readable chars and WITHOUT null termination (NULL) byte if the length is exactly 16 chars - applications have to provide 16+1 bytes storage if the ID is stored as string
	public short	 param_type;	 	// Onboard parameter type: see the MAV_PARAM_TYPE enum for supported data types.

	private packet rcvPacket;
	private packet sndPacket;

	public PARAM_VALUE_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public PARAM_VALUE_class(PARAM_VALUE_class o)
	{
		param_value = o.param_value;
		param_count = o.param_count;
		param_index = o.param_index;
		param_id = o.param_id;
		param_type = o.param_type;
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

			// int[] mavLen = {4, 2, 2, 16, 1};
			// int[] javLen = {4, 4, 4, 16, 2};

			param_value	= rcvPacket.getFloat();
			param_count	= rcvPacket.getIntS();
			param_index	= rcvPacket.getIntS();
			rcvPacket.getByte(param_id, 0, 16);
			param_type	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  param_value
					 ,param_count
					 ,param_index
					 ,param_id
					 ,param_type
					 );
	}

	public byte[] encode(
						 float v_param_value
						,int v_param_count
						,int v_param_index
						,byte[] v_param_id
						,short v_param_type
						)
	{
		// int[] mavLen = {4, 2, 2, 16, 1};
		// int[] javLen = {4, 4, 4, 16, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_param_value);	// Add "param_value" parameter
		sndPacket.putShortI(v_param_count);	// Add "param_count" parameter
		sndPacket.putShortI(v_param_index);	// Add "param_index" parameter
		sndPacket.putByte(v_param_id,0,16);	// Add "param_id" parameter
		sndPacket.putByteS(v_param_type);	// Add "param_type" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", PARAM_VALUE_param_value"
 				+ ", PARAM_VALUE_param_count"
 				+ ", PARAM_VALUE_param_index"
 				+ ", PARAM_VALUE_param_id"
 				+ ", PARAM_VALUE_param_type"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + param_value
 				+ ", " + param_count
 				+ ", " + param_index
 				+ ", " + param_id
 				+ ", " + param_type
				);
		return param;
	}
}
