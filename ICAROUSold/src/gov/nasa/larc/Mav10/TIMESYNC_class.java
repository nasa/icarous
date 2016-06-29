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
Message ID: TIMESYNC(111)
--------------------------------------
%%~ Time synchronization message.
--------------------------------------
*/
public class TIMESYNC_class //implements Loggable
{
	public static final int msgID = 111;
	public long tc1;	 	// Time sync timestamp 1
	public long ts1;	 	// Time sync timestamp 2

	private packet rcvPacket;
	private packet sndPacket;

	public TIMESYNC_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public TIMESYNC_class(TIMESYNC_class o)
	{
		tc1 = o.tc1;
		ts1 = o.ts1;
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

			// int[] mavLen = {8, 8};
			// int[] javLen = {8, 8};

			tc1	= rcvPacket.getLong();
			ts1	= rcvPacket.getLong();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  tc1
					 ,ts1
					 );
	}

	public byte[] encode(
						 long v_tc1
						,long v_ts1
						)
	{
		// int[] mavLen = {8, 8};
		// int[] javLen = {8, 8};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_tc1);	// Add "tc1" parameter
		sndPacket.putLong(v_ts1);	// Add "ts1" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", TIMESYNC_tc1"
 				+ ", TIMESYNC_ts1"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + tc1
 				+ ", " + ts1
				);
		return param;
	}
}
