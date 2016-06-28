/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: SCALED_IMU(26)
--------------------------------------
%%~ The RAW IMU readings for the usual 9DOF sensor setup. This message should contain 
%%~ the scaled values to the described units
--------------------------------------
*/
public class SCALED_IMU_class implements Loggable
{
	public static final int msgID = 26;
	public long	 usec;	 	// Timestamp (microseconds since UNIX epoch or microseconds since system boot)
	public short	 xacc;	 	// X acceleration (mg)
	public short	 yacc;	 	// Y acceleration (mg)
	public short	 zacc;	 	// Z acceleration (mg)
	public short	 xgyro;	 	// Angular speed around X axis (millirad /sec)
	public short	 ygyro;	 	// Angular speed around Y axis (millirad /sec)
	public short	 zgyro;	 	// Angular speed around Z axis (millirad /sec)
	public short	 xmag;	 	// X Magnetic field (milli tesla)
	public short	 ymag;	 	// Y Magnetic field (milli tesla)
	public short	 zmag;	 	// Z Magnetic field (milli tesla)

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public SCALED_IMU_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public SCALED_IMU_class(SCALED_IMU_class o)
	{
		usec = o.usec;
		xacc = o.xacc;
		yacc = o.yacc;
		zacc = o.zacc;
		xgyro = o.xgyro;
		ygyro = o.ygyro;
		zgyro = o.zgyro;
		xmag = o.xmag;
		ymag = o.ymag;
		zmag = o.zmag;
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

			// int[] mavLen = {8, 2, 2, 2, 2, 2, 2, 2, 2, 2};
			// int[] javLen = {8, 2, 2, 2, 2, 2, 2, 2, 2, 2};

			usec		= rcvPacket.getLong();
			xacc		= rcvPacket.getShort();
			yacc		= rcvPacket.getShort();
			zacc		= rcvPacket.getShort();
			xgyro	= rcvPacket.getShort();
			ygyro	= rcvPacket.getShort();
			zgyro	= rcvPacket.getShort();
			xmag		= rcvPacket.getShort();
			ymag		= rcvPacket.getShort();
			zmag		= rcvPacket.getShort();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  usec
					 ,xacc
					 ,yacc
					 ,zacc
					 ,xgyro
					 ,ygyro
					 ,zgyro
					 ,xmag
					 ,ymag
					 ,zmag
					 );
	}

	public byte[] encode(
						 long v_usec
						,short v_xacc
						,short v_yacc
						,short v_zacc
						,short v_xgyro
						,short v_ygyro
						,short v_zgyro
						,short v_xmag
						,short v_ymag
						,short v_zmag
						)
	{
		// int[] mavLen = {8, 2, 2, 2, 2, 2, 2, 2, 2, 2};
		// int[] javLen = {8, 2, 2, 2, 2, 2, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_usec);	// Add "usec" parameter
		sndPacket.putShort(v_xacc);	// Add "xacc" parameter
		sndPacket.putShort(v_yacc);	// Add "yacc" parameter
		sndPacket.putShort(v_zacc);	// Add "zacc" parameter
		sndPacket.putShort(v_xgyro);	// Add "xgyro" parameter
		sndPacket.putShort(v_ygyro);	// Add "ygyro" parameter
		sndPacket.putShort(v_zgyro);	// Add "zgyro" parameter
		sndPacket.putShort(v_xmag);	// Add "xmag" parameter
		sndPacket.putShort(v_ymag);	// Add "ymag" parameter
		sndPacket.putShort(v_zmag);	// Add "zmag" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SCALED_IMU_usec"
 				+ ", SCALED_IMU_xacc"
 				+ ", SCALED_IMU_yacc"
 				+ ", SCALED_IMU_zacc"
 				+ ", SCALED_IMU_xgyro"
 				+ ", SCALED_IMU_ygyro"
 				+ ", SCALED_IMU_zgyro"
 				+ ", SCALED_IMU_xmag"
 				+ ", SCALED_IMU_ymag"
 				+ ", SCALED_IMU_zmag"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + usec
 				+ ", " + xacc
 				+ ", " + yacc
 				+ ", " + zacc
 				+ ", " + xgyro
 				+ ", " + ygyro
 				+ ", " + zgyro
 				+ ", " + xmag
 				+ ", " + ymag
 				+ ", " + zmag
				);
		return param;
	}
}
