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
Message ID: GPS_INJECT_DATA(123)
--------------------------------------
%%~ data for injecting into the onboard GPS (used for DGPS)
--------------------------------------
*/
public class GPS_INJECT_DATA_class //implements Loggable
{
	public static final int msgID = 123;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public short	 len;				 	// data length
	public short[]	 data = new short[110];				// raw data (110 is enough for 12 satellites of RTCMv2)

	private packet rcvPacket;
	private packet sndPacket;

	public GPS_INJECT_DATA_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public GPS_INJECT_DATA_class(GPS_INJECT_DATA_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
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

			// int[] mavLen = {1, 1, 1, 110};
			// int[] javLen = {2, 2, 2, 220};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			len				= rcvPacket.getShortB();
			rcvPacket.getByte(data, 0, 110);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,len
					 ,data
					 );
	}

	public byte[] encode(
						 short v_len
						,short[] v_data
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_len
					 ,v_data
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,short v_len
						,short[] v_data
						)
	{
		// int[] mavLen = {1, 1, 1, 110};
		// int[] javLen = {2, 2, 2, 220};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_len);	// Add "len" parameter
		sndPacket.putByte(v_data,0,110);	// Add "data" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GPS_INJECT_DATA_target_system"
 				+ ", GPS_INJECT_DATA_target_component"
 				+ ", GPS_INJECT_DATA_len"
 				+ ", GPS_INJECT_DATA_data"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + len
 				+ ", " + data
				);
		return param;
	}
}
