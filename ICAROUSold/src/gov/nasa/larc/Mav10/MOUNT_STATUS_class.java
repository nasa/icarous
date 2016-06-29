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
Message ID: MOUNT_STATUS(158)
--------------------------------------
%%~ Message with some status from APM to GCS about camera or antenna mount
--------------------------------------
*/
public class MOUNT_STATUS_class //implements Loggable
{
	public static final int msgID = 158;
	public int		 pointing_a;		 	// pitch(deg*100)
	public int		 pointing_b;		 	// roll(deg*100)
	public int		 pointing_c;		 	// yaw(deg*100)
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID

	private packet rcvPacket;
	private packet sndPacket;

	public MOUNT_STATUS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public MOUNT_STATUS_class(MOUNT_STATUS_class o)
	{
		pointing_a = o.pointing_a;
		pointing_b = o.pointing_b;
		pointing_c = o.pointing_c;
		target_system = o.target_system;
		target_component = o.target_component;
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

			// int[] mavLen = {4, 4, 4, 1, 1};
			// int[] javLen = {4, 4, 4, 2, 2};

			pointing_a		= rcvPacket.getInt();
			pointing_b		= rcvPacket.getInt();
			pointing_c		= rcvPacket.getInt();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  pointing_a
					 ,pointing_b
					 ,pointing_c
					 ,(short)1
					 ,(short)1
					 );
	}

	public byte[] encode(
						 int v_pointing_a
						,int v_pointing_b
						,int v_pointing_c
						)
	{
		return encode(
					  v_pointing_a
					 ,v_pointing_b
					 ,v_pointing_c
					 ,  (short)1
					 ,  (short)1
					 );
	}

	public byte[] encode(
						 int v_pointing_a
						,int v_pointing_b
						,int v_pointing_c
						,short v_target_system
						,short v_target_component
						)
	{
		// int[] mavLen = {4, 4, 4, 1, 1};
		// int[] javLen = {4, 4, 4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putInt(v_pointing_a);	// Add "pointing_a" parameter
		sndPacket.putInt(v_pointing_b);	// Add "pointing_b" parameter
		sndPacket.putInt(v_pointing_c);	// Add "pointing_c" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", MOUNT_STATUS_pointing_a"
 				+ ", MOUNT_STATUS_pointing_b"
 				+ ", MOUNT_STATUS_pointing_c"
 				+ ", MOUNT_STATUS_target_system"
 				+ ", MOUNT_STATUS_target_component"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + pointing_a
 				+ ", " + pointing_b
 				+ ", " + pointing_c
 				+ ", " + target_system
 				+ ", " + target_component
				);
		return param;
	}
}
