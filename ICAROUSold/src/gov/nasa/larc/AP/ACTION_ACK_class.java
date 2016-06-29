/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: ACTION_ACK(9)
--------------------------------------
%%~ This message acknowledges an action. IMPORTANT: The acknowledgement can be also 
%%~ negative, e.g. the MAV rejects a reset message because it is in-flight. The action 
%%~ ids are defined in ENUM MAV_ACTION in mavlink/include/mavlink_types.h
--------------------------------------
*/
public class ACTION_ACK_class implements Loggable
{
	public static final int msgID = 9;
	public short	 action;	 	// The action id
	public short	 result;	 	// 0: Action DENIED, 1: Action executed

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public ACTION_ACK_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public ACTION_ACK_class(ACTION_ACK_class o)
	{
		action = o.action;
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

			action	= rcvPacket.getShortB();
			result	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  action
					 ,result
					 );
	}

	public byte[] encode(
						 short v_action
						,short v_result
						)
	{
		// int[] mavLen = {1, 1};
		// int[] javLen = {2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_action);	// Add "action" parameter
		sndPacket.putByteS(v_result);	// Add "result" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", ACTION_ACK_action"
 				+ ", ACTION_ACK_result"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + action
 				+ ", " + result
				);
		return param;
	}
}
