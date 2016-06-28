/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: SYSTEM_TIME_UTC(4)
--------------------------------------
%%~ UTC date and time from GPS module
--------------------------------------
*/
public class SYSTEM_TIME_UTC_class implements Loggable
{
	public static final int msgID = 4;
	public long utc_date; 	// GPS UTC date ddmmyy
	public long utc_time; 	// GPS UTC time hhmmss

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public SYSTEM_TIME_UTC_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public SYSTEM_TIME_UTC_class(SYSTEM_TIME_UTC_class o)
	{
		utc_date = o.utc_date;
		utc_time = o.utc_time;
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

			// int[] mavLen = {4, 4};
			// int[] javLen = {8, 8};

			utc_date	= rcvPacket.getLongI();
			utc_time	= rcvPacket.getLongI();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  utc_date
					 ,utc_time
					 );
	}

	public byte[] encode(
						 long v_utc_date
						,long v_utc_time
						)
	{
		// int[] mavLen = {4, 4};
		// int[] javLen = {8, 8};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_utc_date);	// Add "utc_date" parameter
		sndPacket.putIntL(v_utc_time);	// Add "utc_time" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SYSTEM_TIME_UTC_utc_date"
 				+ ", SYSTEM_TIME_UTC_utc_time"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + utc_date
 				+ ", " + utc_time
				);
		return param;
	}
}
