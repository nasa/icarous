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
Message ID: GOPRO_GET_RESPONSE(217)
--------------------------------------
%%~ Response from a GOPRO_COMMAND get request
--------------------------------------
*/
public class GOPRO_GET_RESPONSE_class //implements Loggable
{
	public static final int msgID = 217;
	public short	 cmd_id;	 	// Command ID
	public short	 value;	 	// Value

	private packet rcvPacket;
	private packet sndPacket;

	public GOPRO_GET_RESPONSE_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public GOPRO_GET_RESPONSE_class(GOPRO_GET_RESPONSE_class o)
	{
		cmd_id = o.cmd_id;
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

			// int[] mavLen = {1, 1};
			// int[] javLen = {2, 2};

			cmd_id	= rcvPacket.getShortB();
			value	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  cmd_id
					 ,value
					 );
	}

	public byte[] encode(
						 short v_cmd_id
						,short v_value
						)
	{
		// int[] mavLen = {1, 1};
		// int[] javLen = {2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_cmd_id);	// Add "cmd_id" parameter
		sndPacket.putByteS(v_value);	// Add "value" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GOPRO_GET_RESPONSE_cmd_id"
 				+ ", GOPRO_GET_RESPONSE_value"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + cmd_id
 				+ ", " + value
				);
		return param;
	}
}
