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
Message ID: HWSTATUS(165)
--------------------------------------
%%~ Status of key hardware
--------------------------------------
*/
public class HWSTATUS_class //implements Loggable
{
	public static final int msgID = 165;
	public int		 Vcc;		 	// board voltage (mV)
	public short	 I2Cerr;	 	// I2C error count

	private packet rcvPacket;
	private packet sndPacket;

	public HWSTATUS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public HWSTATUS_class(HWSTATUS_class o)
	{
		Vcc = o.Vcc;
		I2Cerr = o.I2Cerr;
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

			// int[] mavLen = {2, 1};
			// int[] javLen = {4, 2};

			Vcc		= rcvPacket.getIntS();
			I2Cerr	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  Vcc
					 ,I2Cerr
					 );
	}

	public byte[] encode(
						 int v_Vcc
						,short v_I2Cerr
						)
	{
		// int[] mavLen = {2, 1};
		// int[] javLen = {4, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_Vcc);	// Add "Vcc" parameter
		sndPacket.putByteS(v_I2Cerr);	// Add "I2Cerr" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", HWSTATUS_Vcc"
 				+ ", HWSTATUS_I2Cerr"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + Vcc
 				+ ", " + I2Cerr
				);
		return param;
	}
}
