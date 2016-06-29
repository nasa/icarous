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
Message ID: RAW_PRESSURE(28)
--------------------------------------
%%~ The RAW pressure readings for the typical setup of one absolute pressure and one 
%%~ differential pressure sensor. The sensor values should be the raw, UNSCALED ADC 
%%~ values.
--------------------------------------
*/
public class RAW_PRESSURE_class //implements Loggable
{
	public static final int msgID = 28;
	public long	 time_usec;	 	// Timestamp (microseconds since UNIX epoch or microseconds since system boot)
	public short	 press_abs;	 	// Absolute pressure (raw)
	public short	 press_diff1;	 	// Differential pressure 1 (raw)
	public short	 press_diff2;	 	// Differential pressure 2 (raw)
	public short	 temperature;	 	// Raw Temperature measurement (raw)

	private packet rcvPacket;
	private packet sndPacket;

	public RAW_PRESSURE_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public RAW_PRESSURE_class(RAW_PRESSURE_class o)
	{
		time_usec = o.time_usec;
		press_abs = o.press_abs;
		press_diff1 = o.press_diff1;
		press_diff2 = o.press_diff2;
		temperature = o.temperature;
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

			// int[] mavLen = {8, 2, 2, 2, 2};
			// int[] javLen = {8, 2, 2, 2, 2};

			time_usec	= rcvPacket.getLong();
			press_abs	= rcvPacket.getShort();
			press_diff1	= rcvPacket.getShort();
			press_diff2	= rcvPacket.getShort();
			temperature	= rcvPacket.getShort();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_usec
					 ,press_abs
					 ,press_diff1
					 ,press_diff2
					 ,temperature
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,short v_press_abs
						,short v_press_diff1
						,short v_press_diff2
						,short v_temperature
						)
	{
		// int[] mavLen = {8, 2, 2, 2, 2};
		// int[] javLen = {8, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
		sndPacket.putShort(v_press_abs);	// Add "press_abs" parameter
		sndPacket.putShort(v_press_diff1);	// Add "press_diff1" parameter
		sndPacket.putShort(v_press_diff2);	// Add "press_diff2" parameter
		sndPacket.putShort(v_temperature);	// Add "temperature" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", RAW_PRESSURE_time_usec"
 				+ ", RAW_PRESSURE_press_abs"
 				+ ", RAW_PRESSURE_press_diff1"
 				+ ", RAW_PRESSURE_press_diff2"
 				+ ", RAW_PRESSURE_temperature"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
 				+ ", " + press_abs
 				+ ", " + press_diff1
 				+ ", " + press_diff2
 				+ ", " + temperature
				);
		return param;
	}
}
