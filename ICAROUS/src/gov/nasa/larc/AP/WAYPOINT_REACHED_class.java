/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: WAYPOINT_REACHED(46)
--------------------------------------
%%~ A certain waypoint has been reached. The system will either hold this position (or 
%%~ circle on the orbit) or (if the autocontinue on the WP was set) continue to the 
%%~ next waypoint.
--------------------------------------
*/
public class WAYPOINT_REACHED_class implements Loggable
{
	public static final int msgID = 46;
	public int	 seq;	 	// Sequence

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public WAYPOINT_REACHED_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public WAYPOINT_REACHED_class(WAYPOINT_REACHED_class o)
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
 				+ ", WAYPOINT_REACHED_seq"
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
