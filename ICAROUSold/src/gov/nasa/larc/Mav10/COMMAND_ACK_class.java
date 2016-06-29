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
Message ID: COMMAND_ACK(77)
--------------------------------------
%%~ Report status of a command. Includes feedback wether the command was executed.
--------------------------------------
*/
public class COMMAND_ACK_class //implements Loggable
{
	public static final int msgID = 77;
	public int		 command;	 	// Command ID, as defined by MAV_CMD enum.
	public short	 result;	 	// See MAV_RESULT enum

	private packet rcvPacket;
	private packet sndPacket;

	public COMMAND_ACK_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public COMMAND_ACK_class(COMMAND_ACK_class o)
	{
		command = o.command;
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

			// int[] mavLen = {2, 1};
			// int[] javLen = {4, 2};

			command	= rcvPacket.getIntS();
			result	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  command
					 ,result
					 );
	}

	public byte[] encode(
						 int v_command
						,short v_result
						)
	{
		// int[] mavLen = {2, 1};
		// int[] javLen = {4, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_command);	// Add "command" parameter
		sndPacket.putByteS(v_result);	// Add "result" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", COMMAND_ACK_command"
 				+ ", COMMAND_ACK_result"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + command
 				+ ", " + result
				);
		return param;
	}
}
