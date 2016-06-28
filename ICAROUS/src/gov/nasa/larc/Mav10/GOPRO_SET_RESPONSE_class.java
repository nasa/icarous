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
Message ID: GOPRO_SET_RESPONSE(219)
--------------------------------------
%%~ Response from a GOPRO_COMMAND set request
--------------------------------------
*/
public class GOPRO_SET_RESPONSE_class //implements Loggable
{
	public static final int msgID = 219;
	public short	 cmd_id;	 	// Command ID
	public short	 result;	 	// Result

	private packet rcvPacket;
	private packet sndPacket;

	public GOPRO_SET_RESPONSE_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public GOPRO_SET_RESPONSE_class(GOPRO_SET_RESPONSE_class o)
	{
		cmd_id = o.cmd_id;
		result = o.result;
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
			result	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  cmd_id
					 ,result
					 );
	}

	public byte[] encode(
						 short v_cmd_id
						,short v_result
						)
	{
		// int[] mavLen = {1, 1};
		// int[] javLen = {2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_cmd_id);	// Add "cmd_id" parameter
		sndPacket.putByteS(v_result);	// Add "result" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GOPRO_SET_RESPONSE_cmd_id"
 				+ ", GOPRO_SET_RESPONSE_result"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + cmd_id
 				+ ", " + result
				);
		return param;
	}
}
