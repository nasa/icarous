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
Message ID: LOG_DATA(120)
--------------------------------------
%%~ Reply to LOG_REQUEST_DATA
--------------------------------------
*/
public class LOG_DATA_class //implements Loggable
{
	public static final int msgID = 120;
	public long	 ofs;		 	// Offset into the log
	public int		 id;		 	// Log id (from LOG_ENTRY reply)
	public short	 count;	 	// Number of bytes (zero for end of log)
	public short[]	 data = new short[90];		// log data

	private packet rcvPacket;
	private packet sndPacket;

	public LOG_DATA_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public LOG_DATA_class(LOG_DATA_class o)
	{
		ofs = o.ofs;
		id = o.id;
		count = o.count;
		data = o.data;
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

			// int[] mavLen = {4, 2, 1, 90};
			// int[] javLen = {8, 4, 2, 180};

			ofs		= rcvPacket.getLongI();
			id		= rcvPacket.getIntS();
			count	= rcvPacket.getShortB();
			rcvPacket.getByte(data, 0, 90);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  ofs
					 ,id
					 ,count
					 ,data
					 );
	}

	public byte[] encode(
						 long v_ofs
						,int v_id
						,short v_count
						,short[] v_data
						)
	{
		// int[] mavLen = {4, 2, 1, 90};
		// int[] javLen = {8, 4, 2, 180};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_ofs);	// Add "ofs" parameter
		sndPacket.putShortI(v_id);	// Add "id" parameter
		sndPacket.putByteS(v_count);	// Add "count" parameter
		sndPacket.putByte(v_data,0,90);	// Add "data" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", LOG_DATA_ofs"
 				+ ", LOG_DATA_id"
 				+ ", LOG_DATA_count"
 				+ ", LOG_DATA_data"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + ofs
 				+ ", " + id
 				+ ", " + count
 				+ ", " + data
				);
		return param;
	}
}
