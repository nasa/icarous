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
Message ID: LOG_ENTRY(118)
--------------------------------------
%%~ Reply to LOG_REQUEST_LIST
--------------------------------------
*/
public class LOG_ENTRY_class //implements Loggable
{
	public static final int msgID = 118;
	public long time_utc;	 	// UTC timestamp of log in seconds since 1970, or 0 if not available
	public long size;		 	// Size of the log (may be approximate) in bytes
	public int	 id;			 	// Log id
	public int	 num_logs;	 	// Total number of logs
	public int	 last_log_num; 	// High log number

	private packet rcvPacket;
	private packet sndPacket;

	public LOG_ENTRY_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public LOG_ENTRY_class(LOG_ENTRY_class o)
	{
		time_utc = o.time_utc;
		size = o.size;
		id = o.id;
		num_logs = o.num_logs;
		last_log_num = o.last_log_num;
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

			// int[] mavLen = {4, 4, 2, 2, 2};
			// int[] javLen = {8, 8, 4, 4, 4};

			time_utc		= rcvPacket.getLongI();
			size			= rcvPacket.getLongI();
			id			= rcvPacket.getIntS();
			num_logs		= rcvPacket.getIntS();
			last_log_num	= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_utc
					 ,size
					 ,id
					 ,num_logs
					 ,last_log_num
					 );
	}

	public byte[] encode(
						 long v_time_utc
						,long v_size
						,int v_id
						,int v_num_logs
						,int v_last_log_num
						)
	{
		// int[] mavLen = {4, 4, 2, 2, 2};
		// int[] javLen = {8, 8, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_utc);	// Add "time_utc" parameter
		sndPacket.putIntL(v_size);	// Add "size" parameter
		sndPacket.putShortI(v_id);	// Add "id" parameter
		sndPacket.putShortI(v_num_logs);	// Add "num_logs" parameter
		sndPacket.putShortI(v_last_log_num);	// Add "last_log_num" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", LOG_ENTRY_time_utc"
 				+ ", LOG_ENTRY_size"
 				+ ", LOG_ENTRY_id"
 				+ ", LOG_ENTRY_num_logs"
 				+ ", LOG_ENTRY_last_log_num"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_utc
 				+ ", " + size
 				+ ", " + id
 				+ ", " + num_logs
 				+ ", " + last_log_num
				);
		return param;
	}
}
