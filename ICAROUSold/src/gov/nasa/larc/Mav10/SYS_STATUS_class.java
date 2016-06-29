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
Message ID: SYS_STATUS(1)
--------------------------------------
%%~ The general system state. If the system is following the MAVLink standard, the system 
%%~ state is mainly defined by three orthogonal states/modes: The system mode, 
%%~ which is either LOCKED (motors shut down and locked), MANUAL (system under RC control), 
%%~ GUIDED (system with autonomous position control, position setpoint controlled 
%%~ manually) or AUTO (system guided by path/waypoint planner). The NAV_MODE 
%%~ defined the current flight state: LIFTOFF (often an open-loop maneuver), LANDING, 
%%~ WAYPOINTS or VECTOR. This represents the internal navigation state machine. The 
%%~ system status shows wether the system is currently active or not and if an emergency 
%%~ occured. During the CRITICAL and EMERGENCY states the MAV is still considered 
%%~ to be active, but should start emergency procedures autonomously. After a failure 
%%~ occured it should first move from active to critical to allow manual intervention 
%%~ and then move to emergency after a certain timeout.
--------------------------------------
*/
public class SYS_STATUS_class //implements Loggable
{
	public static final int msgID = 1;
	public long	 onboard_control_sensors_present;	 	// Bitmask showing which onboard controllers and sensors are present. Value of 0: not present. Value of 1: present. Indices defined by ENUM MAV_SYS_STATUS_SENSOR
	public long	 onboard_control_sensors_enabled;	 	// Bitmask showing which onboard controllers and sensors are enabled:  Value of 0: not enabled. Value of 1: enabled. Indices defined by ENUM MAV_SYS_STATUS_SENSOR
	public long	 onboard_control_sensors_health;	 	// Bitmask showing which onboard controllers and sensors are operational or have an error:  Value of 0: not enabled. Value of 1: enabled. Indices defined by ENUM MAV_SYS_STATUS_SENSOR
	public int		 load;							 	// Maximum usage in percent of the mainloop time, (0%: 0, 100%: 1000) should be always below 1000
	public int		 voltage_battery;					 	// Battery voltage, in millivolts (1 = 1 millivolt)
	public short	 current_battery;					 	// Battery current, in 10*milliamperes (1 = 10 milliampere), -1: autopilot does not measure the current
	public int		 drop_rate_comm;					 	// Communication drops in percent, (0%: 0, 100%: 10'000), (UART, I2C, SPI, CAN), dropped packets on all links (packets that were corrupted on reception on the MAV)
	public int		 errors_comm;						 	// Communication errors (UART, I2C, SPI, CAN), dropped packets on all links (packets that were corrupted on reception on the MAV)
	public int		 errors_count1;					 	// Autopilot-specific errors
	public int		 errors_count2;					 	// Autopilot-specific errors
	public int		 errors_count3;					 	// Autopilot-specific errors
	public int		 errors_count4;					 	// Autopilot-specific errors
	public byte	 battery_remaining;				 	// Remaining battery energy: (0%: 0, 100%: 100), -1: autopilot estimate the remaining battery

	private packet rcvPacket;
	private packet sndPacket;

	public SYS_STATUS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public SYS_STATUS_class(SYS_STATUS_class o)
	{
		onboard_control_sensors_present = o.onboard_control_sensors_present;
		onboard_control_sensors_enabled = o.onboard_control_sensors_enabled;
		onboard_control_sensors_health = o.onboard_control_sensors_health;
		load = o.load;
		voltage_battery = o.voltage_battery;
		current_battery = o.current_battery;
		drop_rate_comm = o.drop_rate_comm;
		errors_comm = o.errors_comm;
		errors_count1 = o.errors_count1;
		errors_count2 = o.errors_count2;
		errors_count3 = o.errors_count3;
		errors_count4 = o.errors_count4;
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

			// int[] mavLen = {4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1};
			// int[] javLen = {8, 8, 8, 4, 4, 2, 4, 4, 4, 4, 4, 4, 1};

			onboard_control_sensors_present	= rcvPacket.getLongI();
			onboard_control_sensors_enabled	= rcvPacket.getLongI();
			onboard_control_sensors_health	= rcvPacket.getLongI();
			load								= rcvPacket.getIntS();
			voltage_battery					= rcvPacket.getIntS();
			current_battery					= rcvPacket.getShort();
			drop_rate_comm					= rcvPacket.getIntS();
			errors_comm						= rcvPacket.getIntS();
			errors_count1					= rcvPacket.getIntS();
			errors_count2					= rcvPacket.getIntS();
			errors_count3					= rcvPacket.getIntS();
			errors_count4					= rcvPacket.getIntS();
			battery_remaining				= rcvPacket.getByte();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  onboard_control_sensors_present
					 ,onboard_control_sensors_enabled
					 ,onboard_control_sensors_health
					 ,load
					 ,voltage_battery
					 ,current_battery
					 ,drop_rate_comm
					 ,errors_comm
					 ,errors_count1
					 ,errors_count2
					 ,errors_count3
					 ,errors_count4
					 ,battery_remaining
					 );
	}

	public byte[] encode(
						 long v_onboard_control_sensors_present
						,long v_onboard_control_sensors_enabled
						,long v_onboard_control_sensors_health
						,int v_load
						,int v_voltage_battery
						,short v_current_battery
						,int v_drop_rate_comm
						,int v_errors_comm
						,int v_errors_count1
						,int v_errors_count2
						,int v_errors_count3
						,int v_errors_count4
						,byte v_battery_remaining
						)
	{
		// int[] mavLen = {4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1};
		// int[] javLen = {8, 8, 8, 4, 4, 2, 4, 4, 4, 4, 4, 4, 1};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_onboard_control_sensors_present);	// Add "onboard_control_sensors_present" parameter
		sndPacket.putIntL(v_onboard_control_sensors_enabled);	// Add "onboard_control_sensors_enabled" parameter
		sndPacket.putIntL(v_onboard_control_sensors_health);	// Add "onboard_control_sensors_health" parameter
		sndPacket.putShortI(v_load);	// Add "load" parameter
		sndPacket.putShortI(v_voltage_battery);	// Add "voltage_battery" parameter
		sndPacket.putShort(v_current_battery);	// Add "current_battery" parameter
		sndPacket.putShortI(v_drop_rate_comm);	// Add "drop_rate_comm" parameter
		sndPacket.putShortI(v_errors_comm);	// Add "errors_comm" parameter
		sndPacket.putShortI(v_errors_count1);	// Add "errors_count1" parameter
		sndPacket.putShortI(v_errors_count2);	// Add "errors_count2" parameter
		sndPacket.putShortI(v_errors_count3);	// Add "errors_count3" parameter
		sndPacket.putShortI(v_errors_count4);	// Add "errors_count4" parameter
		sndPacket.putByte(v_battery_remaining);	// Add "battery_remaining" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SYS_STATUS_onboard_control_sensors_present"
 				+ ", SYS_STATUS_onboard_control_sensors_enabled"
 				+ ", SYS_STATUS_onboard_control_sensors_health"
 				+ ", SYS_STATUS_load"
 				+ ", SYS_STATUS_voltage_battery"
 				+ ", SYS_STATUS_current_battery"
 				+ ", SYS_STATUS_drop_rate_comm"
 				+ ", SYS_STATUS_errors_comm"
 				+ ", SYS_STATUS_errors_count1"
 				+ ", SYS_STATUS_errors_count2"
 				+ ", SYS_STATUS_errors_count3"
 				+ ", SYS_STATUS_errors_count4"
 				+ ", SYS_STATUS_battery_remaining"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + onboard_control_sensors_present
 				+ ", " + onboard_control_sensors_enabled
 				+ ", " + onboard_control_sensors_health
 				+ ", " + load
 				+ ", " + voltage_battery
 				+ ", " + current_battery
 				+ ", " + drop_rate_comm
 				+ ", " + errors_comm
 				+ ", " + errors_count1
 				+ ", " + errors_count2
 				+ ", " + errors_count3
 				+ ", " + errors_count4
 				+ ", " + battery_remaining
				);
		return param;
	}
}
