/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: MOUNT_CONFIGURE(156)
--------------------------------------
%%~ Message to configure a camera mount, directional antenna, etc.
--------------------------------------
*/
public class MOUNT_CONFIGURE_class implements Loggable
{
	public static final int msgID = 156;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public short	 mount_mode;		 	// mount operating mode (see MAV_MOUNT_MODE enum)
	public short	 stab_roll;		 	// (1 = yes, 0 = no)
	public short	 stab_pitch;		 	// (1 = yes, 0 = no)
	public short	 stab_yaw;		 	// (1 = yes, 0 = no)

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public MOUNT_CONFIGURE_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public MOUNT_CONFIGURE_class(MOUNT_CONFIGURE_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		mount_mode = o.mount_mode;
		stab_roll = o.stab_roll;
		stab_pitch = o.stab_pitch;
		stab_yaw = o.stab_yaw;
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

			// int[] mavLen = {1, 1, 1, 1, 1, 1};
			// int[] javLen = {2, 2, 2, 2, 2, 2};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			mount_mode		= rcvPacket.getShortB();
			stab_roll		= rcvPacket.getShortB();
			stab_pitch		= rcvPacket.getShortB();
			stab_yaw			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,mount_mode
					 ,stab_roll
					 ,stab_pitch
					 ,stab_yaw
					 );
	}

	public byte[] encode(
						 short v_mount_mode
						,short v_stab_roll
						,short v_stab_pitch
						,short v_stab_yaw
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_mount_mode
					 ,v_stab_roll
					 ,v_stab_pitch
					 ,v_stab_yaw
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,short v_mount_mode
						,short v_stab_roll
						,short v_stab_pitch
						,short v_stab_yaw
						)
	{
		// int[] mavLen = {1, 1, 1, 1, 1, 1};
		// int[] javLen = {2, 2, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_mount_mode);	// Add "mount_mode" parameter
		sndPacket.putByteS(v_stab_roll);	// Add "stab_roll" parameter
		sndPacket.putByteS(v_stab_pitch);	// Add "stab_pitch" parameter
		sndPacket.putByteS(v_stab_yaw);	// Add "stab_yaw" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", MOUNT_CONFIGURE_target_system"
 				+ ", MOUNT_CONFIGURE_target_component"
 				+ ", MOUNT_CONFIGURE_mount_mode"
 				+ ", MOUNT_CONFIGURE_stab_roll"
 				+ ", MOUNT_CONFIGURE_stab_pitch"
 				+ ", MOUNT_CONFIGURE_stab_yaw"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + mount_mode
 				+ ", " + stab_roll
 				+ ", " + stab_pitch
 				+ ", " + stab_yaw
				);
		return param;
	}
}
