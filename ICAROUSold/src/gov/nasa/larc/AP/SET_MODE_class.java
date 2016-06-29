/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: SET_MODE(11)
--------------------------------------
%%~ Set the system mode, as defined by enum MAV_MODE in mavlink/include/mavlink_types.h. 
%%~ There is no target component id as the mode is by definition for the overall 
%%~ aircraft, not only for one component.
--------------------------------------
*/
public class SET_MODE_class implements Loggable
{
	public static final int msgID = 11;
	public short	 target;	 	// The system setting the mode
	public short	 mode;	 	// The new mode

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public SET_MODE_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public SET_MODE_class(SET_MODE_class o)
	{
		target = o.target;
		mode = o.mode;
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
			mode		= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  target
					 ,mode
					 );
	}

	public byte[] encode(
						 short v_target
						,short v_mode
						)
	{
		// int[] mavLen = {1, 1};
		// int[] javLen = {2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target);	// Add "target" parameter
		sndPacket.putByteS(v_mode);	// Add "mode" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SET_MODE_target"
 				+ ", SET_MODE_mode"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target
 				+ ", " + mode
				);
		return param;
	}
}
