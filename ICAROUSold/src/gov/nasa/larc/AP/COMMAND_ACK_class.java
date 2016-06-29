/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: COMMAND_ACK(76)
--------------------------------------
%%~ Report status of a command. Includes feedback wether the command was executed
--------------------------------------
*/
public class COMMAND_ACK_class implements Loggable
{
	public static final int msgID = 76;
	public float	 command;	 	// Current airspeed in m/s
	public float	 result;	 	// 1: Action ACCEPTED and EXECUTED, 1: Action TEMPORARY REJECTED/DENIED, 2: Action PERMANENTLY DENIED, 3: Action UNKNOWN/UNSUPPORTED, 4: Requesting CONFIRMATION

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public COMMAND_ACK_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
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

			// int[] mavLen = {4, 4};
			// int[] javLen = {4, 4};

			command	= rcvPacket.getFloat();
			result	= rcvPacket.getFloat();
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
						 float v_command
						,float v_result
						)
	{
		// int[] mavLen = {4, 4};
		// int[] javLen = {4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_command);	// Add "command" parameter
		sndPacket.putFloat(v_result);	// Add "result" parameter

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
