/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: SYSTEM_TIME(2)
--------------------------------------
%%~ The system time is the time of the master clock, typically the computer clock of 
%%~ the main onboard computer.
--------------------------------------
*/
public class SYSTEM_TIME_class implements Loggable
{
	public static final int msgID = 2;
	public long time_usec;	 	// Timestamp of the master clock in microseconds since UNIX epoch.

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public SYSTEM_TIME_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public SYSTEM_TIME_class(SYSTEM_TIME_class o)
	{
		time_usec = o.time_usec;
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

			// int[] mavLen = {8};
			// int[] javLen = {8};

			time_usec	= rcvPacket.getLong();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_usec
					 );
	}

	public byte[] encode(
						 long v_time_usec
						)
	{
		// int[] mavLen = {8};
		// int[] javLen = {8};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SYSTEM_TIME_time_usec"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
				);
		return param;
	}
}
