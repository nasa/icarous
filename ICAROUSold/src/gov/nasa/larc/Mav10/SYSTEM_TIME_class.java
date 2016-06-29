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
Message ID: SYSTEM_TIME(2)
--------------------------------------
%%~ The system time is the time of the master clock, typically the computer clock of 
%%~ the main onboard computer.
--------------------------------------
*/
public class SYSTEM_TIME_class //implements Loggable
{
	public static final int msgID = 2;
	public long time_unix_usec;	 	// Timestamp of the master clock in microseconds since UNIX epoch.
	public long time_boot_ms;	 	// Timestamp of the component clock since boot time in milliseconds.

	private packet rcvPacket;
	private packet sndPacket;

	public SYSTEM_TIME_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public SYSTEM_TIME_class(SYSTEM_TIME_class o)
	{
		time_unix_usec = o.time_unix_usec;
		time_boot_ms = o.time_boot_ms;
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

			// int[] mavLen = {8, 4};
			// int[] javLen = {8, 8};

			time_unix_usec	= rcvPacket.getLong();
			time_boot_ms		= rcvPacket.getLongI();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_unix_usec
					 ,time_boot_ms
					 );
	}

	public byte[] encode(
						 long v_time_unix_usec
						,long v_time_boot_ms
						)
	{
		// int[] mavLen = {8, 4};
		// int[] javLen = {8, 8};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_unix_usec);	// Add "time_unix_usec" parameter
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SYSTEM_TIME_time_unix_usec"
 				+ ", SYSTEM_TIME_time_boot_ms"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_unix_usec
 				+ ", " + time_boot_ms
				);
		return param;
	}
}
