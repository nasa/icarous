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
Message ID: GIMBAL_CONTROL(201)
--------------------------------------
%%~ Control message for rate gimbal
--------------------------------------
*/
public class GIMBAL_CONTROL_class //implements Loggable
{
	public static final int msgID = 201;
	public float	 demanded_rate_x;	 	// Demanded angular rate X (rad/s)
	public float	 demanded_rate_y;	 	// Demanded angular rate Y (rad/s)
	public float	 demanded_rate_z;	 	// Demanded angular rate Z (rad/s)
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID

	private packet rcvPacket;
	private packet sndPacket;

	public GIMBAL_CONTROL_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public GIMBAL_CONTROL_class(GIMBAL_CONTROL_class o)
	{
		demanded_rate_x = o.demanded_rate_x;
		demanded_rate_y = o.demanded_rate_y;
		demanded_rate_z = o.demanded_rate_z;
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

			demanded_rate_x	= rcvPacket.getFloat();
			demanded_rate_y	= rcvPacket.getFloat();
			demanded_rate_z	= rcvPacket.getFloat();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  demanded_rate_x
					 ,demanded_rate_y
					 ,demanded_rate_z
					 ,(short)1
					 ,(short)1
					 );
	}

	public byte[] encode(
						 float v_demanded_rate_x
						,float v_demanded_rate_y
						,float v_demanded_rate_z
						)
	{
		return encode(
					  v_demanded_rate_x
					 ,v_demanded_rate_y
					 ,v_demanded_rate_z
					 ,  (short)1
					 ,  (short)1
					 );
	}

	public byte[] encode(
						 float v_demanded_rate_x
						,float v_demanded_rate_y
						,float v_demanded_rate_z
						,short v_target_system
						,short v_target_component
						)
	{
		// int[] mavLen = {4, 4, 4, 1, 1};
		// int[] javLen = {4, 4, 4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_demanded_rate_x);	// Add "demanded_rate_x" parameter
		sndPacket.putFloat(v_demanded_rate_y);	// Add "demanded_rate_y" parameter
		sndPacket.putFloat(v_demanded_rate_z);	// Add "demanded_rate_z" parameter
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
 				+ ", GIMBAL_CONTROL_demanded_rate_x"
 				+ ", GIMBAL_CONTROL_demanded_rate_y"
 				+ ", GIMBAL_CONTROL_demanded_rate_z"
 				+ ", GIMBAL_CONTROL_target_system"
 				+ ", GIMBAL_CONTROL_target_component"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + demanded_rate_x
 				+ ", " + demanded_rate_y
 				+ ", " + demanded_rate_z
 				+ ", " + target_system
 				+ ", " + target_component
				);
		return param;
	}
}
