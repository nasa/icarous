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
Message ID: BATTERY2(181)
--------------------------------------
%%~ 2nd Battery status
--------------------------------------
*/
public class BATTERY2_class //implements Loggable
{
	public static final int msgID = 181;
	public int		 voltage;			 	// voltage in millivolts
	public short	 current_battery;	 	// Battery current, in 10*milliamperes (1 = 10 milliampere), -1: autopilot does not measure the current

	private packet rcvPacket;
	private packet sndPacket;

	public BATTERY2_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public BATTERY2_class(BATTERY2_class o)
	{
		voltage = o.voltage;
		current_battery = o.current_battery;
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

			// int[] mavLen = {2, 2};
			// int[] javLen = {4, 2};

			voltage			= rcvPacket.getIntS();
			current_battery	= rcvPacket.getShort();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  voltage
					 ,current_battery
					 );
	}

	public byte[] encode(
						 int v_voltage
						,short v_current_battery
						)
	{
		// int[] mavLen = {2, 2};
		// int[] javLen = {4, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_voltage);	// Add "voltage" parameter
		sndPacket.putShort(v_current_battery);	// Add "current_battery" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", BATTERY2_voltage"
 				+ ", BATTERY2_current_battery"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + voltage
 				+ ", " + current_battery
				);
		return param;
	}
}
