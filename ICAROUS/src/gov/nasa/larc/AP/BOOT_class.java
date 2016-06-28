/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: BOOT(1)
--------------------------------------
%%~ The boot message indicates that a system is starting. The onboard software version 
%%~ allows to keep track of onboard soft/firmware revisions.
--------------------------------------
*/
public class BOOT_class implements Loggable
{
	public static final int msgID = 1;
	public long version;	 	// The onboard software version

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public BOOT_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public BOOT_class(BOOT_class o)
	{
		version = o.version;
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

			// int[] mavLen = {4};
			// int[] javLen = {8};

			version	= rcvPacket.getLongI();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  version
					 );
	}

	public byte[] encode(
						 long v_version
						)
	{
		// int[] mavLen = {4};
		// int[] javLen = {8};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_version);	// Add "version" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", BOOT_version"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + version
				);
		return param;
	}
}
