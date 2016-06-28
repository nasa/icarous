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
Message ID: RAW_IMU(27)
--------------------------------------
%%~ The RAW IMU readings for the usual 9DOF sensor setup. This message should always 
%%~ contain the true raw values without any scaling to allow data capture and system 
%%~ debugging.
--------------------------------------
*/
public class RAW_IMU_class //implements Loggable
{
	public static final int msgID = 27;
	public long	 time_usec;	 	// Timestamp (microseconds since UNIX epoch or microseconds since system boot)
	public short	 xacc;		 	// X acceleration (raw)
	public short	 yacc;		 	// Y acceleration (raw)
	public short	 zacc;		 	// Z acceleration (raw)
	public short	 xgyro;		 	// Angular speed around X axis (raw)
	public short	 ygyro;		 	// Angular speed around Y axis (raw)
	public short	 zgyro;		 	// Angular speed around Z axis (raw)
	public short	 xmag;		 	// X Magnetic field (raw)
	public short	 ymag;		 	// Y Magnetic field (raw)
	public short	 zmag;		 	// Z Magnetic field (raw)

	private packet rcvPacket;
	private packet sndPacket;

	public RAW_IMU_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public RAW_IMU_class(RAW_IMU_class o)
	{
		time_usec = o.time_usec;
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

			time_usec	= rcvPacket.getLong();
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
					  time_usec
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
						 long v_time_usec
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
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
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
 				+ ", RAW_IMU_time_usec"
 				+ ", RAW_IMU_xacc"
 				+ ", RAW_IMU_yacc"
 				+ ", RAW_IMU_zacc"
 				+ ", RAW_IMU_xgyro"
 				+ ", RAW_IMU_ygyro"
 				+ ", RAW_IMU_zgyro"
 				+ ", RAW_IMU_xmag"
 				+ ", RAW_IMU_ymag"
 				+ ", RAW_IMU_zmag"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
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
