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
Message ID: LOG_REQUEST_DATA(119)
--------------------------------------
%%~ Request a chunk of a log
--------------------------------------
*/
public class LOG_REQUEST_DATA_class //implements Loggable
{
	public static final int msgID = 119;
	public long	 ofs;				 	// Offset into the log
	public long	 count;			 	// Number of bytes
	public int		 id;				 	// Log id (from LOG_ENTRY reply)
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID

	private packet rcvPacket;
	private packet sndPacket;

	public LOG_REQUEST_DATA_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public LOG_REQUEST_DATA_class(LOG_REQUEST_DATA_class o)
	{
		ofs = o.ofs;
		count = o.count;
		id = o.id;
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

			// int[] mavLen = {4, 4, 2, 1, 1};
			// int[] javLen = {8, 8, 4, 2, 2};

			ofs				= rcvPacket.getLongI();
			count			= rcvPacket.getLongI();
			id				= rcvPacket.getIntS();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  ofs
					 ,count
					 ,id
					 ,(short)1
					 ,(short)1
					 );
	}

	public byte[] encode(
						 long v_ofs
						,long v_count
						,int v_id
						)
	{
		return encode(
					  v_ofs
					 ,v_count
					 ,v_id
					 ,  (short)1
					 ,  (short)1
					 );
	}

	public byte[] encode(
						 long v_ofs
						,long v_count
						,int v_id
						,short v_target_system
						,short v_target_component
						)
	{
		// int[] mavLen = {4, 4, 2, 1, 1};
		// int[] javLen = {8, 8, 4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_ofs);	// Add "ofs" parameter
		sndPacket.putIntL(v_count);	// Add "count" parameter
		sndPacket.putShortI(v_id);	// Add "id" parameter
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
 				+ ", LOG_REQUEST_DATA_ofs"
 				+ ", LOG_REQUEST_DATA_count"
 				+ ", LOG_REQUEST_DATA_id"
 				+ ", LOG_REQUEST_DATA_target_system"
 				+ ", LOG_REQUEST_DATA_target_component"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + ofs
 				+ ", " + count
 				+ ", " + id
 				+ ", " + target_system
 				+ ", " + target_component
				);
		return param;
	}
}
