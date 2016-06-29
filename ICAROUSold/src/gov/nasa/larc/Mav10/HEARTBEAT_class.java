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
Message ID: HEARTBEAT(0)
--------------------------------------
%%~ The heartbeat message shows that a system is present and responding. The type of 
%%~ the MAV and Autopilot hardware allow the receiving system to treat further messages 
%%~ from this system appropriate (e.g. by laying out the user interface based on 
%%~ the autopilot).
--------------------------------------
*/
public class HEARTBEAT_class //implements Loggable
{
	public static final int msgID = 0;
	public long	 custom_mode;		 	// A bitfield for use for autopilot-specific flags.
	public short	 type;			 	// Type of the MAV (quadrotor, helicopter, etc., up to 15 types, defined in MAV_TYPE ENUM)
	public short	 autopilot;		 	// Autopilot type / class. defined in MAV_AUTOPILOT ENUM
	public short	 base_mode;		 	// System mode bitfield, see MAV_MODE_FLAG ENUM in mavlink/include/mavlink_types.h
	public short	 system_status;	 	// System status flag, see MAV_STATE ENUM
	public short	 mavlink_version;	 	// MAVLink version, not writable by user, gets added by protocol because of magic data type: uint8_t_mavlink_version

	private packet rcvPacket;
	private packet sndPacket;

	public HEARTBEAT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public HEARTBEAT_class(HEARTBEAT_class o)
	{
		custom_mode = o.custom_mode;
		type = o.type;
		autopilot = o.autopilot;
		base_mode = o.base_mode;
		system_status = o.system_status;
		mavlink_version = o.mavlink_version;
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

			// int[] mavLen = {4, 1, 1, 1, 1, 1};
			// int[] javLen = {8, 2, 2, 2, 2, 2};

			custom_mode		= rcvPacket.getLongI();
			type				= rcvPacket.getShortB();
			autopilot		= rcvPacket.getShortB();
			base_mode		= rcvPacket.getShortB();
			system_status	= rcvPacket.getShortB();
			mavlink_version	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  custom_mode
					 ,type
					 ,autopilot
					 ,base_mode
					 ,system_status
					 ,mavlink_version
					 );
	}

	public byte[] encode(
						 long v_custom_mode
						,short v_type
						,short v_autopilot
						,short v_base_mode
						,short v_system_status
						,short v_mavlink_version
						)
	{
		// int[] mavLen = {4, 1, 1, 1, 1, 1};
		// int[] javLen = {8, 2, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_custom_mode);	// Add "custom_mode" parameter
		sndPacket.putByteS(v_type);	// Add "type" parameter
		sndPacket.putByteS(v_autopilot);	// Add "autopilot" parameter
		sndPacket.putByteS(v_base_mode);	// Add "base_mode" parameter
		sndPacket.putByteS(v_system_status);	// Add "system_status" parameter
		sndPacket.putByteS(v_mavlink_version);	// Add "mavlink_version" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", HEARTBEAT_custom_mode"
 				+ ", HEARTBEAT_type"
 				+ ", HEARTBEAT_autopilot"
 				+ ", HEARTBEAT_base_mode"
 				+ ", HEARTBEAT_system_status"
 				+ ", HEARTBEAT_mavlink_version"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + custom_mode
 				+ ", " + type
 				+ ", " + autopilot
 				+ ", " + base_mode
 				+ ", " + system_status
 				+ ", " + mavlink_version
				);
		return param;
	}
}
