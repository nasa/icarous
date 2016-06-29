/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: CONTROL_STATUS(52)
--------------------------------------
--------------------------------------
*/
public class CONTROL_STATUS_class implements Loggable
{
	public static final int msgID = 52;
	public short	 position_fix;	 	// Position fix: 0: lost, 2: 2D position fix, 3: 3D position fix 
	public short	 vision_fix;		 	// Vision position fix: 0: lost, 1: 2D local position hold, 2: 2D global position fix, 3: 3D global position fix 
	public short	 gps_fix;			 	// GPS position fix: 0: no reception, 1: Minimum 1 satellite, but no position fix, 2: 2D position fix, 3: 3D position fix 
	public short	 ahrs_health;		 	// Attitude estimation health: 0: poor, 255: excellent
	public short	 control_att;		 	// 0: Attitude control disabled, 1: enabled
	public short	 control_pos_xy;	 	// 0: X, Y position control disabled, 1: enabled
	public short	 control_pos_z;	 	// 0: Z position control disabled, 1: enabled
	public short	 control_pos_yaw;	 	// 0: Yaw angle control disabled, 1: enabled

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public CONTROL_STATUS_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public CONTROL_STATUS_class(CONTROL_STATUS_class o)
	{
		position_fix = o.position_fix;
		vision_fix = o.vision_fix;
		gps_fix = o.gps_fix;
		ahrs_health = o.ahrs_health;
		control_att = o.control_att;
		control_pos_xy = o.control_pos_xy;
		control_pos_z = o.control_pos_z;
		control_pos_yaw = o.control_pos_yaw;
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

			// int[] mavLen = {1, 1, 1, 1, 1, 1, 1, 1};
			// int[] javLen = {2, 2, 2, 2, 2, 2, 2, 2};

			position_fix		= rcvPacket.getShortB();
			vision_fix		= rcvPacket.getShortB();
			gps_fix			= rcvPacket.getShortB();
			ahrs_health		= rcvPacket.getShortB();
			control_att		= rcvPacket.getShortB();
			control_pos_xy	= rcvPacket.getShortB();
			control_pos_z	= rcvPacket.getShortB();
			control_pos_yaw	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  position_fix
					 ,vision_fix
					 ,gps_fix
					 ,ahrs_health
					 ,control_att
					 ,control_pos_xy
					 ,control_pos_z
					 ,control_pos_yaw
					 );
	}

	public byte[] encode(
						 short v_position_fix
						,short v_vision_fix
						,short v_gps_fix
						,short v_ahrs_health
						,short v_control_att
						,short v_control_pos_xy
						,short v_control_pos_z
						,short v_control_pos_yaw
						)
	{
		// int[] mavLen = {1, 1, 1, 1, 1, 1, 1, 1};
		// int[] javLen = {2, 2, 2, 2, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_position_fix);	// Add "position_fix" parameter
		sndPacket.putByteS(v_vision_fix);	// Add "vision_fix" parameter
		sndPacket.putByteS(v_gps_fix);	// Add "gps_fix" parameter
		sndPacket.putByteS(v_ahrs_health);	// Add "ahrs_health" parameter
		sndPacket.putByteS(v_control_att);	// Add "control_att" parameter
		sndPacket.putByteS(v_control_pos_xy);	// Add "control_pos_xy" parameter
		sndPacket.putByteS(v_control_pos_z);	// Add "control_pos_z" parameter
		sndPacket.putByteS(v_control_pos_yaw);	// Add "control_pos_yaw" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", CONTROL_STATUS_position_fix"
 				+ ", CONTROL_STATUS_vision_fix"
 				+ ", CONTROL_STATUS_gps_fix"
 				+ ", CONTROL_STATUS_ahrs_health"
 				+ ", CONTROL_STATUS_control_att"
 				+ ", CONTROL_STATUS_control_pos_xy"
 				+ ", CONTROL_STATUS_control_pos_z"
 				+ ", CONTROL_STATUS_control_pos_yaw"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + position_fix
 				+ ", " + vision_fix
 				+ ", " + gps_fix
 				+ ", " + ahrs_health
 				+ ", " + control_att
 				+ ", " + control_pos_xy
 				+ ", " + control_pos_z
 				+ ", " + control_pos_yaw
				);
		return param;
	}
}
