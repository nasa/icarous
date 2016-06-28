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
Message ID: SCALED_IMU3(129)
--------------------------------------
%%~ The RAW IMU readings for 3rd 9DOF sensor setup. This message should contain the 
%%~ scaled values to the described units
--------------------------------------
*/
public class SCALED_IMU3_class //implements Loggable
{
	public static final int msgID = 129;
	public long	 time_boot_ms; 	// Timestamp (milliseconds since system boot)
	public short	 xacc;		 	// X acceleration (mg)
	public short	 yacc;		 	// Y acceleration (mg)
	public short	 zacc;		 	// Z acceleration (mg)
	public short	 xgyro;		 	// Angular speed around X axis (millirad /sec)
	public short	 ygyro;		 	// Angular speed around Y axis (millirad /sec)
	public short	 zgyro;		 	// Angular speed around Z axis (millirad /sec)
	public short	 xmag;		 	// X Magnetic field (milli tesla)
	public short	 ymag;		 	// Y Magnetic field (milli tesla)
	public short	 zmag;		 	// Z Magnetic field (milli tesla)

	private packet rcvPacket;
	private packet sndPacket;

	public SCALED_IMU3_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public SCALED_IMU3_class(SCALED_IMU3_class o)
	{
		time_boot_ms = o.time_boot_ms;
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

			// int[] mavLen = {4, 2, 2, 2, 2, 2, 2, 2, 2, 2};
			// int[] javLen = {8, 2, 2, 2, 2, 2, 2, 2, 2, 2};

			time_boot_ms	= rcvPacket.getLongI();
			xacc			= rcvPacket.getShort();
			yacc			= rcvPacket.getShort();
			zacc			= rcvPacket.getShort();
			xgyro		= rcvPacket.getShort();
			ygyro		= rcvPacket.getShort();
			zgyro		= rcvPacket.getShort();
			xmag			= rcvPacket.getShort();
			ymag			= rcvPacket.getShort();
			zmag			= rcvPacket.getShort();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
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
						 long v_time_boot_ms
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
		// int[] mavLen = {4, 2, 2, 2, 2, 2, 2, 2, 2, 2};
		// int[] javLen = {8, 2, 2, 2, 2, 2, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
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
 				+ ", SCALED_IMU3_time_boot_ms"
 				+ ", SCALED_IMU3_xacc"
 				+ ", SCALED_IMU3_yacc"
 				+ ", SCALED_IMU3_zacc"
 				+ ", SCALED_IMU3_xgyro"
 				+ ", SCALED_IMU3_ygyro"
 				+ ", SCALED_IMU3_zgyro"
 				+ ", SCALED_IMU3_xmag"
 				+ ", SCALED_IMU3_ymag"
 				+ ", SCALED_IMU3_zmag"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
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
