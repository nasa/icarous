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
Message ID: PING(4)
--------------------------------------
%%~ A ping message either requesting or responding to a ping. This allows to measure 
%%~ the system latencies, including serial port, radio modem and UDP connections.
--------------------------------------
*/
public class PING_class //implements Loggable
{
	public static final int msgID = 4;
	public long	 time_usec;		 	// Unix timestamp in microseconds or since system boot if smaller than MAVLink epoch (1.1.2009)
	public long	 seq;				 	// PING sequence
	public short	 target_system;	 	// 0: request ping from all receiving systems, if greater than 0: message is a ping response and number is the system id of the requesting system
	public short	 target_component; 	// 0: request ping from all receiving components, if greater than 0: message is a ping response and number is the system id of the requesting system

	private packet rcvPacket;
	private packet sndPacket;

	public PING_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public PING_class(PING_class o)
	{
		time_usec = o.time_usec;
		seq = o.seq;
		target_system = o.target_system;
		target_component = o.target_component;
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

			// int[] mavLen = {8, 4, 1, 1};
			// int[] javLen = {8, 8, 2, 2};

			time_usec		= rcvPacket.getLong();
			seq				= rcvPacket.getLongI();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_usec
					 ,seq
					 ,(short)1
					 ,(short)1
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,long v_seq
						)
	{
		return encode(
					  v_time_usec
					 ,v_seq
					 ,  (short)1
					 ,  (short)1
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,long v_seq
						,short v_target_system
						,short v_target_component
						)
	{
		// int[] mavLen = {8, 4, 1, 1};
		// int[] javLen = {8, 8, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
		sndPacket.putIntL(v_seq);	// Add "seq" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", PING_time_usec"
 				+ ", PING_seq"
 				+ ", PING_target_system"
 				+ ", PING_target_component"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
 				+ ", " + seq
 				+ ", " + target_system
 				+ ", " + target_component
				);
		return param;
	}
}
