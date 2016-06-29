/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: STATE_DATA(115)
--------------------------------------
%%~ Message that information about the state of the aircraft
--------------------------------------
*/
public class STATE_DATA_class implements Loggable
{
	public static final int msgID = 115;
	public short	 num;		 	// Vehicle number, e.g. 1 for R1, 2 for R2, 3 for R3
	public short	 atloiter; 	// if 0, vehicle is not currently loitering, if 1, vehicle is loitering
	public short	 mode;	 	// mode: MANUAL=0, CIRCLE=1, STABILIZE=2, FLY_BY_WIRE_A=5, FLY_BY_WIRE_B=6, FLY_BY_WIRE_C=7, AUTO=10, RTL=11, LOITER=12, GUIDED=15, INITIALISING=16
	public long	 usec;	 	// time

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public STATE_DATA_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public STATE_DATA_class(STATE_DATA_class o)
	{
		num = o.num;
		atloiter = o.atloiter;
		mode = o.mode;
		usec = o.usec;
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

			// int[] mavLen = {1, 1, 1, 8};
			// int[] javLen = {2, 2, 2, 8};

			num		= rcvPacket.getShortB();
			atloiter	= rcvPacket.getShortB();
			mode		= rcvPacket.getShortB();
			usec		= rcvPacket.getLong();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  num
					 ,atloiter
					 ,mode
					 ,usec
					 );
	}

	public byte[] encode(
						 short v_num
						,short v_atloiter
						,short v_mode
						,long v_usec
						)
	{
		// int[] mavLen = {1, 1, 1, 8};
		// int[] javLen = {2, 2, 2, 8};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_num);	// Add "num" parameter
		sndPacket.putByteS(v_atloiter);	// Add "atloiter" parameter
		sndPacket.putByteS(v_mode);	// Add "mode" parameter
		sndPacket.putLong(v_usec);	// Add "usec" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", STATE_DATA_num"
 				+ ", STATE_DATA_atloiter"
 				+ ", STATE_DATA_mode"
 				+ ", STATE_DATA_usec"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + num
 				+ ", " + atloiter
 				+ ", " + mode
 				+ ", " + usec
				);
		return param;
	}
}
