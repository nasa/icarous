/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: SET_NAV_MODE(12)
--------------------------------------
%%~ Set the system navigation mode, as defined by enum MAV_NAV_MODE in mavlink/include/mavlink_types.h. 
%%~ The navigation mode applies to the whole aircraft and thus all 
%%~ components.
--------------------------------------
*/
public class SET_NAV_MODE_class implements Loggable
{
	public static final int msgID = 12;
	public short	 target;	 	// The system setting the mode
	public short	 nav_mode; 	// The new navigation mode

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public SET_NAV_MODE_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public SET_NAV_MODE_class(SET_NAV_MODE_class o)
	{
		target = o.target;
		nav_mode = o.nav_mode;
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

			// int[] mavLen = {1, 1};
			// int[] javLen = {2, 2};

			target	= rcvPacket.getShortB();
			nav_mode	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  target
					 ,nav_mode
					 );
	}

	public byte[] encode(
						 short v_target
						,short v_nav_mode
						)
	{
		// int[] mavLen = {1, 1};
		// int[] javLen = {2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target);	// Add "target" parameter
		sndPacket.putByteS(v_nav_mode);	// Add "nav_mode" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SET_NAV_MODE_target"
 				+ ", SET_NAV_MODE_nav_mode"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target
 				+ ", " + nav_mode
				);
		return param;
	}
}
