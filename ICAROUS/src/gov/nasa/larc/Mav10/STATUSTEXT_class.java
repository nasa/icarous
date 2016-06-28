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
Message ID: STATUSTEXT(253)
--------------------------------------
%%~ Status text message. These messages are printed in yellow in the COMM console of 
%%~ QGroundControl. WARNING: They consume quite some bandwidth, so use only for important 
%%~ status and error messages. If implemented wisely, these messages are buffered 
%%~ on the MCU and sent only at a limited rate (e.g. 10 Hz).
--------------------------------------
*/
public class STATUSTEXT_class //implements Loggable
{
	public static final int msgID = 253;
	public short	 severity; 	// Severity of status. Relies on the definitions within RFC-5424. See enum MAV_SEVERITY.
	public byte[]	 text = new byte[50];		// Status text message, without null termination character

	private packet rcvPacket;
	private packet sndPacket;

	public STATUSTEXT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public STATUSTEXT_class(STATUSTEXT_class o)
	{
		severity = o.severity;
		text = o.text;
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

			// int[] mavLen = {1, 50};
			// int[] javLen = {2, 50};

			severity	= rcvPacket.getShortB();
			rcvPacket.getByte(text, 0, 50);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  severity
					 ,text
					 );
	}

	public byte[] encode(
						 short v_severity
						,byte[] v_text
						)
	{
		// int[] mavLen = {1, 50};
		// int[] javLen = {2, 50};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_severity);	// Add "severity" parameter
		sndPacket.putByte(v_text,0,50);	// Add "text" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", STATUSTEXT_severity"
 				+ ", STATUSTEXT_text"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + severity
 				+ ", " + text
				);
		return param;
	}
}
