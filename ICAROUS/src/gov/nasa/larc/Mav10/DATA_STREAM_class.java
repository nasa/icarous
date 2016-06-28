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
Message ID: DATA_STREAM(67)
--------------------------------------
--------------------------------------
*/
public class DATA_STREAM_class //implements Loggable
{
	public static final int msgID = 67;
	public int		 message_rate; 	// The requested interval between two messages of this type
	public short	 stream_id;	 	// The ID of the requested data stream
	public short	 on_off;		 	// 1 stream is enabled, 0 stream is stopped.

	private packet rcvPacket;
	private packet sndPacket;

	public DATA_STREAM_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public DATA_STREAM_class(DATA_STREAM_class o)
	{
		message_rate = o.message_rate;
		stream_id = o.stream_id;
		on_off = o.on_off;
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

			// int[] mavLen = {2, 1, 1};
			// int[] javLen = {4, 2, 2};

			message_rate	= rcvPacket.getIntS();
			stream_id	= rcvPacket.getShortB();
			on_off		= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  message_rate
					 ,stream_id
					 ,on_off
					 );
	}

	public byte[] encode(
						 int v_message_rate
						,short v_stream_id
						,short v_on_off
						)
	{
		// int[] mavLen = {2, 1, 1};
		// int[] javLen = {4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_message_rate);	// Add "message_rate" parameter
		sndPacket.putByteS(v_stream_id);	// Add "stream_id" parameter
		sndPacket.putByteS(v_on_off);	// Add "on_off" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", DATA_STREAM_message_rate"
 				+ ", DATA_STREAM_stream_id"
 				+ ", DATA_STREAM_on_off"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + message_rate
 				+ ", " + stream_id
 				+ ", " + on_off
				);
		return param;
	}
}
