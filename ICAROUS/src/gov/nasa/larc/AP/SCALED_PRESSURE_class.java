/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: SCALED_PRESSURE(38)
--------------------------------------
%%~ The pressure readings for the typical setup of one absolute and differential pressure 
%%~ sensor. The units are as specified in each field.
--------------------------------------
*/
public class SCALED_PRESSURE_class implements Loggable
{
	public static final int msgID = 38;
	public long	 usec;		 	// Timestamp (microseconds since UNIX epoch or microseconds since system boot)
	public float	 press_abs;	 	// Absolute pressure (hectopascal)
	public float	 press_diff;	 	// Differential pressure 1 (hectopascal)
	public short	 temperature;	 	// Temperature measurement (0.01 degrees celsius)

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public SCALED_PRESSURE_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public SCALED_PRESSURE_class(SCALED_PRESSURE_class o)
	{
		usec = o.usec;
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

			// int[] mavLen = {8, 4, 4, 2};
			// int[] javLen = {8, 4, 4, 2};

			usec			= rcvPacket.getLong();
			press_abs	= rcvPacket.getFloat();
			press_diff	= rcvPacket.getFloat();
			temperature	= rcvPacket.getShort();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  usec
					 ,press_abs
					 ,press_diff
					 ,temperature
					 );
	}

	public byte[] encode(
						 long v_usec
						,float v_press_abs
						,float v_press_diff
						,short v_temperature
						)
	{
		// int[] mavLen = {8, 4, 4, 2};
		// int[] javLen = {8, 4, 4, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_usec);	// Add "usec" parameter
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
 				+ ", SCALED_PRESSURE_usec"
 				+ ", SCALED_PRESSURE_press_abs"
 				+ ", SCALED_PRESSURE_press_diff"
 				+ ", SCALED_PRESSURE_temperature"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + usec
 				+ ", " + press_abs
 				+ ", " + press_diff
 				+ ", " + temperature
				);
		return param;
	}
}
