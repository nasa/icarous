/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//MavLink 0.9

package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: HEARTBEAT(0)
--------------------------------------
%%~ The heartbeat message shows that a system is present and responding. The type of 
%%~ the MAV and Autopilot hardware allow the receiving system to treat further messages 
%%~ from this system appropriate (e.g. by laying out the user interface based on 
%%~ the autopilot).
--------------------------------------
*/
public class HEARTBEAT_class implements Loggable
{
	public static final int msgID = 0;
	public short	 type;			 	// Type of the MAV (quadrotor, helicopter, etc., up to 15 types, defined in MAV_TYPE ENUM)
	public short	 autopilot;		 	// Type of the Autopilot: 0: Generic, 1: PIXHAWK, 2: SLUGS, 3: Ardupilot (up to 15 types), defined in MAV_AUTOPILOT_TYPE ENUM
	public short	 mavlink_version;	 	// MAVLink version
	public short	 vehicleID;		 	// Vehicle ID: 1: R1, 2: R2, 3: R3 (up to xx types), defined in "mydefines.h"
	public short	 numWpts;			 	// Number of Waypoints in Flight Plan

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public HEARTBEAT_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public HEARTBEAT_class(HEARTBEAT_class o)
	{
		type = o.type;
		autopilot = o.autopilot;
		mavlink_version = o.mavlink_version;
		vehicleID = o.vehicleID;
		numWpts = o.numWpts;
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

			// int[] mavLen = {1, 1, 1, 1, 1};
			// int[] javLen = {2, 2, 2, 2, 2};

			type				= rcvPacket.getShortB();
			autopilot		= rcvPacket.getShortB();
			mavlink_version	= rcvPacket.getShortB();
			vehicleID		= rcvPacket.getShortB();
			numWpts			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  type
					 ,autopilot
					 ,mavlink_version
					 ,vehicleID
					 ,numWpts
					 );
	}

	public byte[] encode(
						 short v_type
						,short v_autopilot
						,short v_mavlink_version
						,short v_vehicleID
						,short v_numWpts
						)
	{
		// int[] mavLen = {1, 1, 1, 1, 1};
		// int[] javLen = {2, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_type);	// Add "type" parameter
		sndPacket.putByteS(v_autopilot);	// Add "autopilot" parameter
		sndPacket.putByteS(v_mavlink_version);	// Add "mavlink_version" parameter
		sndPacket.putByteS(v_vehicleID);	// Add "vehicleID" parameter
		sndPacket.putByteS(v_numWpts);	// Add "numWpts" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", HEARTBEAT_type"
 				+ ", HEARTBEAT_autopilot"
 				+ ", HEARTBEAT_mavlink_version"
 				+ ", HEARTBEAT_vehicleID"
 				+ ", HEARTBEAT_numWpts"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + type
 				+ ", " + autopilot
 				+ ", " + mavlink_version
 				+ ", " + vehicleID
 				+ ", " + numWpts
				);
		return param;
	}
}
