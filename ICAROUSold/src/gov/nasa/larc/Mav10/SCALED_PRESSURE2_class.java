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
Message ID: SCALED_PRESSURE2(137)
--------------------------------------
%%~ Barometer readings for 2nd barometer
--------------------------------------
*/
public class SCALED_PRESSURE2_class //implements Loggable
{
	public static final int msgID = 137;
	public long	 time_boot_ms; 	// Timestamp (milliseconds since system boot)
	public float	 press_abs;	 	// Absolute pressure (hectopascal)
	public float	 press_diff;	 	// Differential pressure 1 (hectopascal)
	public short	 temperature;	 	// Temperature measurement (0.01 degrees celsius)

	private packet rcvPacket;
	private packet sndPacket;

	public SCALED_PRESSURE2_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public SCALED_PRESSURE2_class(SCALED_PRESSURE2_class o)
	{
		time_boot_ms = o.time_boot_ms;
		press_abs = o.press_abs;
		press_diff = o.press_diff;
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

			// int[] mavLen = {4, 4, 4, 2};
			// int[] javLen = {8, 4, 4, 2};

			time_boot_ms	= rcvPacket.getLongI();
			press_abs	= rcvPacket.getFloat();
			press_diff	= rcvPacket.getFloat();
			temperature	= rcvPacket.getShort();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
					 ,press_abs
					 ,press_diff
					 ,temperature
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,float v_press_abs
						,float v_press_diff
						,short v_temperature
						)
	{
		// int[] mavLen = {4, 4, 4, 2};
		// int[] javLen = {8, 4, 4, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putFloat(v_press_abs);	// Add "press_abs" parameter
		sndPacket.putFloat(v_press_diff);	// Add "press_diff" parameter
		sndPacket.putShort(v_temperature);	// Add "temperature" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SCALED_PRESSURE2_time_boot_ms"
 				+ ", SCALED_PRESSURE2_press_abs"
 				+ ", SCALED_PRESSURE2_press_diff"
 				+ ", SCALED_PRESSURE2_temperature"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + press_abs
 				+ ", " + press_diff
 				+ ", " + temperature
				);
		return param;
	}
}
