/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: MEMINFO(152)
--------------------------------------
%%~ state of APM memory
--------------------------------------
*/
public class MEMINFO_class implements Loggable
{
	public static final int msgID = 152;
	public int	 brkval;	 	// heap top
	public int	 freemem;	 	// free memory

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public MEMINFO_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public MEMINFO_class(MEMINFO_class o)
	{
		brkval = o.brkval;
		freemem = o.freemem;
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

			// int[] mavLen = {2, 2};
			// int[] javLen = {4, 4};

			brkval	= rcvPacket.getIntS();
			freemem	= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  brkval
					 ,freemem
					 );
	}

	public byte[] encode(
						 int v_brkval
						,int v_freemem
						)
	{
		// int[] mavLen = {2, 2};
		// int[] javLen = {4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_brkval);	// Add "brkval" parameter
		sndPacket.putShortI(v_freemem);	// Add "freemem" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", MEMINFO_brkval"
 				+ ", MEMINFO_freemem"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + brkval
 				+ ", " + freemem
				);
		return param;
	}
}
