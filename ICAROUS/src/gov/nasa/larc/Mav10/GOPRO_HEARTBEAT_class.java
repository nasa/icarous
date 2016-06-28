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
Message ID: GOPRO_HEARTBEAT(215)
--------------------------------------
%%~ Heartbeat from a HeroBus attached GoPro
--------------------------------------
*/
public class GOPRO_HEARTBEAT_class //implements Loggable
{
	public static final int msgID = 215;
	public short	 status;	 	// Status

	private packet rcvPacket;
	private packet sndPacket;

	public GOPRO_HEARTBEAT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public GOPRO_HEARTBEAT_class(GOPRO_HEARTBEAT_class o)
	{
		status = o.status;
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

			// int[] mavLen = {1};
			// int[] javLen = {2};

			status	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  status
					 );
	}

	public byte[] encode(
						 short v_status
						)
	{
		// int[] mavLen = {1};
		// int[] javLen = {2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_status);	// Add "status" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GOPRO_HEARTBEAT_status"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + status
				);
		return param;
	}
}
