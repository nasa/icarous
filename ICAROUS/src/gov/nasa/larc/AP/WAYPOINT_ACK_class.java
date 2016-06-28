/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: WAYPOINT_ACK(47)
--------------------------------------
%%~ Ack message during waypoint handling. The type field states if this message is a 
%%~ positive ack (type=0) or if an error happened (type=non-zero).
--------------------------------------
*/
public class WAYPOINT_ACK_class implements Loggable
{
	public static final int msgID = 47;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public short	 type;			 	// 0: OK, 1: Error

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public WAYPOINT_ACK_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public WAYPOINT_ACK_class(WAYPOINT_ACK_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		type = o.type;
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

			// int[] mavLen = {1, 1, 1};
			// int[] javLen = {2, 2, 2};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			type				= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,type
					 );
	}

	public byte[] encode(
						 short v_type
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_type
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,short v_type
						)
	{
		// int[] mavLen = {1, 1, 1};
		// int[] javLen = {2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_type);	// Add "type" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", WAYPOINT_ACK_target_system"
 				+ ", WAYPOINT_ACK_target_component"
 				+ ", WAYPOINT_ACK_type"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + type
				);
		return param;
	}
}
