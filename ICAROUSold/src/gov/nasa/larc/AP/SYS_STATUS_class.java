/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: SYS_STATUS(34)
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
public class SYS_STATUS_class implements Loggable
{
	public static final int msgID = 34;
	public short	 mode;				 	// System mode, see MAV_MODE ENUM in mavlink/include/mavlink_types.h
	public short	 nav_mode;			 	// Navigation mode, see MAV_NAV_MODE ENUM
	public short	 status;				 	// System status flag, see MAV_STATUS ENUM
	public int		 load;				 	// Maximum usage in percent of the mainloop time, (0%: 0, 100%: 1000) should be always below 1000
	public int		 vbat;				 	// Battery voltage, in millivolts (1 = 1 millivolt)
	public int		 battery_remaining;	 	// Remaining battery energy: (0%: 0, 100%: 1000)
	public int		 packet_drop;			 	// Dropped packets (packets that were corrupted on reception on the MAV)

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public SYS_STATUS_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public SYS_STATUS_class(SYS_STATUS_class o)
	{
		mode = o.mode;
		nav_mode = o.nav_mode;
		status = o.status;
		load = o.load;
		vbat = o.vbat;
		battery_remaining = o.battery_remaining;
		packet_drop = o.packet_drop;
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

			// int[] mavLen = {1, 1, 1, 2, 2, 2, 2};
			// int[] javLen = {2, 2, 2, 4, 4, 4, 4};

			mode					= rcvPacket.getShortB();
			nav_mode				= rcvPacket.getShortB();
			status				= rcvPacket.getShortB();
			load					= rcvPacket.getIntS();
			vbat					= rcvPacket.getIntS();
			battery_remaining	= rcvPacket.getIntS();
			packet_drop			= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  mode
					 ,nav_mode
					 ,status
					 ,load
					 ,vbat
					 ,battery_remaining
					 ,packet_drop
					 );
	}

	public byte[] encode(
						 short v_mode
						,short v_nav_mode
						,short v_status
						,int v_load
						,int v_vbat
						,int v_battery_remaining
						,int v_packet_drop
						)
	{
		// int[] mavLen = {1, 1, 1, 2, 2, 2, 2};
		// int[] javLen = {2, 2, 2, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_mode);	// Add "mode" parameter
		sndPacket.putByteS(v_nav_mode);	// Add "nav_mode" parameter
		sndPacket.putByteS(v_status);	// Add "status" parameter
		sndPacket.putShortI(v_load);	// Add "load" parameter
		sndPacket.putShortI(v_vbat);	// Add "vbat" parameter
		sndPacket.putShortI(v_battery_remaining);	// Add "battery_remaining" parameter
		sndPacket.putShortI(v_packet_drop);	// Add "packet_drop" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SYS_STATUS_mode"
 				+ ", SYS_STATUS_nav_mode"
 				+ ", SYS_STATUS_status"
 				+ ", SYS_STATUS_load"
 				+ ", SYS_STATUS_vbat"
 				+ ", SYS_STATUS_battery_remaining"
 				+ ", SYS_STATUS_packet_drop"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + mode
 				+ ", " + nav_mode
 				+ ", " + status
 				+ ", " + load
 				+ ", " + vbat
 				+ ", " + battery_remaining
 				+ ", " + packet_drop
				);
		return param;
	}
}
