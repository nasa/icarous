/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: PING(3)
--------------------------------------
%%~ A ping message either requesting or responding to a ping. This allows to measure 
%%~ the system latencies, including serial port, radio modem and UDP connections.
--------------------------------------
*/
public class PING_class implements Loggable
{
	public static final int msgID = 3;
	public long	 seq;				 	// PING sequence
	public short	 target_system;	 	// 0: request ping from all receiving systems, if greater than 0: message is a ping response and number is the system id of the requesting system
	public short	 target_component; 	// 0: request ping from all receiving components, if greater than 0: message is a ping response and number is the system id of the requesting system
	public long	 time;			 	// Unix timestamp in microseconds

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public PING_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public PING_class(PING_class o)
	{
		seq = o.seq;
		target_system = o.target_system;
		target_component = o.target_component;
		time = o.time;
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

			// int[] mavLen = {4, 1, 1, 8};
			// int[] javLen = {8, 2, 2, 8};

			seq				= rcvPacket.getLongI();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			time				= rcvPacket.getLong();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  seq
					 ,(short)1
					 ,(short)1
					 ,time
					 );
	}

	public byte[] encode(
						 long v_seq
						,long v_time
						)
	{
		return encode(
					  v_seq
					 ,  (short)1
					 ,  (short)1
					 ,v_time
					 );
	}

	public byte[] encode(
						 long v_seq
						,short v_target_system
						,short v_target_component
						,long v_time
						)
	{
		// int[] mavLen = {4, 1, 1, 8};
		// int[] javLen = {8, 2, 2, 8};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_seq);	// Add "seq" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putLong(v_time);	// Add "time" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", PING_seq"
 				+ ", PING_target_system"
 				+ ", PING_target_component"
 				+ ", PING_time"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + seq
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + time
				);
		return param;
	}
}
