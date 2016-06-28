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
Message ID: DATA32(170)
--------------------------------------
%%~ Data packet, size 32
--------------------------------------
*/
public class DATA32_class //implements Loggable
{
	public static final int msgID = 170;
	public short	 type; 	// data type
	public short	 len;	 	// data length
	public short[]	 data = new short[32];	// raw data

	private packet rcvPacket;
	private packet sndPacket;

	public DATA32_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public DATA32_class(DATA32_class o)
	{
		type = o.type;
		len = o.len;
		data = o.data;
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

			// int[] mavLen = {1, 1, 32};
			// int[] javLen = {2, 2, 64};

			type	= rcvPacket.getShortB();
			len	= rcvPacket.getShortB();
			rcvPacket.getByte(data, 0, 32);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  type
					 ,len
					 ,data
					 );
	}

	public byte[] encode(
						 short v_type
						,short v_len
						,short[] v_data
						)
	{
		// int[] mavLen = {1, 1, 32};
		// int[] javLen = {2, 2, 64};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_type);	// Add "type" parameter
		sndPacket.putByteS(v_len);	// Add "len" parameter
		sndPacket.putByte(v_data,0,32);	// Add "data" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", DATA32_type"
 				+ ", DATA32_len"
 				+ ", DATA32_data"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + type
 				+ ", " + len
 				+ ", " + data
				);
		return param;
	}
}
