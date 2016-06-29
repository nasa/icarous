/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: MOUNT_STATUS(158)
--------------------------------------
%%~ Message with some status from APM to GCS about camera or antenna mount
--------------------------------------
*/
public class MOUNT_STATUS_class implements Loggable
{
	public static final int msgID = 158;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public int		 pointing_a;		 	// pitch(deg*100) or lat, depending on mount mode
	public int		 pointing_b;		 	// roll(deg*100) or lon depending on mount mode
	public int		 pointing_c;		 	// yaw(deg*100) or alt (in cm) depending on mount mode

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public MOUNT_STATUS_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public MOUNT_STATUS_class(MOUNT_STATUS_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		pointing_a = o.pointing_a;
		pointing_b = o.pointing_b;
		pointing_c = o.pointing_c;
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

			// int[] mavLen = {1, 1, 4, 4, 4};
			// int[] javLen = {2, 2, 4, 4, 4};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			pointing_a		= rcvPacket.getInt();
			pointing_b		= rcvPacket.getInt();
			pointing_c		= rcvPacket.getInt();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,pointing_a
					 ,pointing_b
					 ,pointing_c
					 );
	}

	public byte[] encode(
						 int v_pointing_a
						,int v_pointing_b
						,int v_pointing_c
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_pointing_a
					 ,v_pointing_b
					 ,v_pointing_c
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,int v_pointing_a
						,int v_pointing_b
						,int v_pointing_c
						)
	{
		// int[] mavLen = {1, 1, 4, 4, 4};
		// int[] javLen = {2, 2, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putInt(v_pointing_a);	// Add "pointing_a" parameter
		sndPacket.putInt(v_pointing_b);	// Add "pointing_b" parameter
		sndPacket.putInt(v_pointing_c);	// Add "pointing_c" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", MOUNT_STATUS_target_system"
 				+ ", MOUNT_STATUS_target_component"
 				+ ", MOUNT_STATUS_pointing_a"
 				+ ", MOUNT_STATUS_pointing_b"
 				+ ", MOUNT_STATUS_pointing_c"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + pointing_a
 				+ ", " + pointing_b
 				+ ", " + pointing_c
				);
		return param;
	}
}
