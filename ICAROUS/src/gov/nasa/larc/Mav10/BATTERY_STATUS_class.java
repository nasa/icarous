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
Message ID: BATTERY_STATUS(147)
--------------------------------------
%%~ Battery information
--------------------------------------
*/
public class BATTERY_STATUS_class //implements Loggable
{
	public static final int msgID = 147;
	public int		 current_consumed;	 	// Consumed charge, in milliampere hours (1 = 1 mAh), -1: autopilot does not provide mAh consumption estimate
	public int		 energy_consumed;		 	// Consumed energy, in 100*Joules (intergrated U*I*dt)  (1 = 100 Joule), -1: autopilot does not provide energy consumption estimate
	public short	 temperature;			 	// Temperature of the battery in centi-degrees celsius. INT16_MAX for unknown temperature.
	public int[]		 voltages = new int[10];				// Battery voltage of cells, in millivolts (1 = 1 millivolt)
	public short	 current_battery;		 	// Battery current, in 10*milliamperes (1 = 10 milliampere), -1: autopilot does not measure the current
	public short	 id;					 	// Battery ID
	public short	 battery_function;	 	// Function of the battery
	public short	 type;				 	// Type (chemistry) of the battery
	public byte	 battery_remaining;	 	// Remaining battery energy: (0%: 0, 100%: 100), -1: autopilot does not estimate the remaining battery

	private packet rcvPacket;
	private packet sndPacket;

	public BATTERY_STATUS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public BATTERY_STATUS_class(BATTERY_STATUS_class o)
	{
		current_consumed = o.current_consumed;
		energy_consumed = o.energy_consumed;
		temperature = o.temperature;
		voltages = o.voltages;
		current_battery = o.current_battery;
		id = o.id;
		battery_function = o.battery_function;
		type = o.type;
		battery_remaining = o.battery_remaining;
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

			// int[] mavLen = {4, 4, 2, 20, 2, 1, 1, 1, 1};
			// int[] javLen = {4, 4, 2, 40, 2, 2, 2, 2, 1};

			current_consumed		= rcvPacket.getInt();
			energy_consumed		= rcvPacket.getInt();
			temperature			= rcvPacket.getShort();
			rcvPacket.getByte(voltages, 0, 10);
			current_battery		= rcvPacket.getShort();
			id					= rcvPacket.getShortB();
			battery_function		= rcvPacket.getShortB();
			type					= rcvPacket.getShortB();
			battery_remaining	= rcvPacket.getByte();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  current_consumed
					 ,energy_consumed
					 ,temperature
					 ,voltages
					 ,current_battery
					 ,id
					 ,battery_function
					 ,type
					 ,battery_remaining
					 );
	}

	public byte[] encode(
						 int v_current_consumed
						,int v_energy_consumed
						,short v_temperature
						,int[] v_voltages
						,short v_current_battery
						,short v_id
						,short v_battery_function
						,short v_type
						,byte v_battery_remaining
						)
	{
		// int[] mavLen = {4, 4, 2, 20, 2, 1, 1, 1, 1};
		// int[] javLen = {4, 4, 2, 40, 2, 2, 2, 2, 1};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putInt(v_current_consumed);	// Add "current_consumed" parameter
		sndPacket.putInt(v_energy_consumed);	// Add "energy_consumed" parameter
		sndPacket.putShort(v_temperature);	// Add "temperature" parameter
		sndPacket.putByte(v_voltages,0,10);	// Add "voltages" parameter
		sndPacket.putShort(v_current_battery);	// Add "current_battery" parameter
		sndPacket.putByteS(v_id);	// Add "id" parameter
		sndPacket.putByteS(v_battery_function);	// Add "battery_function" parameter
		sndPacket.putByteS(v_type);	// Add "type" parameter
		sndPacket.putByte(v_battery_remaining);	// Add "battery_remaining" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", BATTERY_STATUS_current_consumed"
 				+ ", BATTERY_STATUS_energy_consumed"
 				+ ", BATTERY_STATUS_temperature"
 				+ ", BATTERY_STATUS_voltages"
 				+ ", BATTERY_STATUS_current_battery"
 				+ ", BATTERY_STATUS_id"
 				+ ", BATTERY_STATUS_battery_function"
 				+ ", BATTERY_STATUS_type"
 				+ ", BATTERY_STATUS_battery_remaining"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + current_consumed
 				+ ", " + energy_consumed
 				+ ", " + temperature
 				+ ", " + voltages
 				+ ", " + current_battery
 				+ ", " + id
 				+ ", " + battery_function
 				+ ", " + type
 				+ ", " + battery_remaining
				);
		return param;
	}
}
