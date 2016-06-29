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
Message ID: MISSION_CURRENT(42)
--------------------------------------
%%~ Message that announces the sequence number of the current active mission item. The 
%%~ MAV will fly towards this mission item.
--------------------------------------
*/
public class MISSION_CURRENT_class //implements Loggable
{
	public static final int msgID = 42;
	public int	 seq;	 	// Sequence

	private packet rcvPacket;
	private packet sndPacket;

	public MISSION_CURRENT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public MISSION_CURRENT_class(MISSION_CURRENT_class o)
	{
		seq = o.seq;
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

			// int[] mavLen = {2};
			// int[] javLen = {4};

			seq	= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  seq
					 );
	}

	public byte[] encode(
						 int v_seq
						)
	{
		// int[] mavLen = {2};
		// int[] javLen = {4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_seq);	// Add "seq" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", MISSION_CURRENT_seq"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + seq
				);
		return param;
	}
}
