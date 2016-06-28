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
Message ID: FENCE_STATUS(162)
--------------------------------------
%%~ Status of geo-fencing. Sent in extended      status stream when fencing enabled
--------------------------------------
*/
public class FENCE_STATUS_class //implements Loggable
{
	public static final int msgID = 162;
	public long	 breach_time;		 	// time of last breach in milliseconds since boot
	public int		 breach_count;	 	// number of fence breaches
	public short	 breach_status;	 	// 0 if currently inside fence, 1 if outside
	public short	 breach_type;		 	// last breach type (see FENCE_BREACH_* enum)

	private packet rcvPacket;
	private packet sndPacket;

	public FENCE_STATUS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public FENCE_STATUS_class(FENCE_STATUS_class o)
	{
		breach_time = o.breach_time;
		breach_count = o.breach_count;
		breach_status = o.breach_status;
		breach_type = o.breach_type;
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

			// int[] mavLen = {4, 2, 1, 1};
			// int[] javLen = {8, 4, 2, 2};

			breach_time		= rcvPacket.getLongI();
			breach_count		= rcvPacket.getIntS();
			breach_status	= rcvPacket.getShortB();
			breach_type		= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  breach_time
					 ,breach_count
					 ,breach_status
					 ,breach_type
					 );
	}

	public byte[] encode(
						 long v_breach_time
						,int v_breach_count
						,short v_breach_status
						,short v_breach_type
						)
	{
		// int[] mavLen = {4, 2, 1, 1};
		// int[] javLen = {8, 4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_breach_time);	// Add "breach_time" parameter
		sndPacket.putShortI(v_breach_count);	// Add "breach_count" parameter
		sndPacket.putByteS(v_breach_status);	// Add "breach_status" parameter
		sndPacket.putByteS(v_breach_type);	// Add "breach_type" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", FENCE_STATUS_breach_time"
 				+ ", FENCE_STATUS_breach_count"
 				+ ", FENCE_STATUS_breach_status"
 				+ ", FENCE_STATUS_breach_type"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + breach_time
 				+ ", " + breach_count
 				+ ", " + breach_status
 				+ ", " + breach_type
				);
		return param;
	}
}
