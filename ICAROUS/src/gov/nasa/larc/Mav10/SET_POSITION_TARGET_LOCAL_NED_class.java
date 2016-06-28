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
Message ID: SET_POSITION_TARGET_LOCAL_NED(84)
--------------------------------------
%%~ Set vehicle position, velocity and acceleration setpoint in local frame.
--------------------------------------
*/
public class SET_POSITION_TARGET_LOCAL_NED_class //implements Loggable
{
	public static final int msgID = 84;
	public long	 time_boot_ms;	 	// Timestamp in milliseconds since system boot
	public float	 x;				 	// X Position in NED frame in meters
	public float	 y;				 	// Y Position in NED frame in meters
	public float	 z;				 	// Z Position in NED frame in meters (note, altitude is negative in NED)
	public float	 vx;				 	// X velocity in NED frame in meter / s
	public float	 vy;				 	// Y velocity in NED frame in meter / s
	public float	 vz;				 	// Z velocity in NED frame in meter / s
	public float	 afx;				 	// X acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N
	public float	 afy;				 	// Y acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N
	public float	 afz;				 	// Z acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N
	public float	 yaw;				 	// yaw setpoint in rad
	public float	 yaw_rate;		 	// yaw rate setpoint in rad/s
	public int		 type_mask;		 	// Bitmask to indicate which dimensions should be ignored by the vehicle: a value of 0b0000000000000000 or 0b0000001000000000 indicates that none of the setpoint dimensions should be ignored. If bit 10 is set the floats afx afy afz should be interpreted as force instead of acceleration. Mapping: bit 1: x, bit 2: y, bit 3: z, bit 4: vx, bit 5: vy, bit 6: vz, bit 7: ax, bit 8: ay, bit 9: az, bit 10: is force setpoint, bit 11: yaw, bit 12: yaw rate
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public short	 coordinate_frame; 	// Valid options are: MAV_FRAME_LOCAL_NED = 1, MAV_FRAME_LOCAL_OFFSET_NED = 7, MAV_FRAME_BODY_NED = 8, MAV_FRAME_BODY_OFFSET_NED = 9

	private packet rcvPacket;
	private packet sndPacket;

	public SET_POSITION_TARGET_LOCAL_NED_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public SET_POSITION_TARGET_LOCAL_NED_class(SET_POSITION_TARGET_LOCAL_NED_class o)
	{
		time_boot_ms = o.time_boot_ms;
		x = o.x;
		y = o.y;
		z = o.z;
		vx = o.vx;
		vy = o.vy;
		vz = o.vz;
		afx = o.afx;
		afy = o.afy;
		afz = o.afz;
		yaw = o.yaw;
		yaw_rate = o.yaw_rate;
		type_mask = o.type_mask;
		target_system = o.target_system;
		target_component = o.target_component;
		coordinate_frame = o.coordinate_frame;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 1, 1, 1};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2};

			time_boot_ms		= rcvPacket.getLongI();
			x				= rcvPacket.getFloat();
			y				= rcvPacket.getFloat();
			z				= rcvPacket.getFloat();
			vx				= rcvPacket.getFloat();
			vy				= rcvPacket.getFloat();
			vz				= rcvPacket.getFloat();
			afx				= rcvPacket.getFloat();
			afy				= rcvPacket.getFloat();
			afz				= rcvPacket.getFloat();
			yaw				= rcvPacket.getFloat();
			yaw_rate			= rcvPacket.getFloat();
			type_mask		= rcvPacket.getIntS();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			coordinate_frame	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
					 ,x
					 ,y
					 ,z
					 ,vx
					 ,vy
					 ,vz
					 ,afx
					 ,afy
					 ,afz
					 ,yaw
					 ,yaw_rate
					 ,type_mask
					 ,(short)1
					 ,(short)1
					 ,coordinate_frame
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,float v_x
						,float v_y
						,float v_z
						,float v_vx
						,float v_vy
						,float v_vz
						,float v_afx
						,float v_afy
						,float v_afz
						,float v_yaw
						,float v_yaw_rate
						,int v_type_mask
						,short v_coordinate_frame
						)
	{
		return encode(
					  v_time_boot_ms
					 ,v_x
					 ,v_y
					 ,v_z
					 ,v_vx
					 ,v_vy
					 ,v_vz
					 ,v_afx
					 ,v_afy
					 ,v_afz
					 ,v_yaw
					 ,v_yaw_rate
					 ,v_type_mask
					 ,  (short)1
					 ,  (short)1
					 ,v_coordinate_frame
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,float v_x
						,float v_y
						,float v_z
						,float v_vx
						,float v_vy
						,float v_vz
						,float v_afx
						,float v_afy
						,float v_afz
						,float v_yaw
						,float v_yaw_rate
						,int v_type_mask
						,short v_target_system
						,short v_target_component
						,short v_coordinate_frame
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 1, 1, 1};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putFloat(v_x);	// Add "x" parameter
		sndPacket.putFloat(v_y);	// Add "y" parameter
		sndPacket.putFloat(v_z);	// Add "z" parameter
		sndPacket.putFloat(v_vx);	// Add "vx" parameter
		sndPacket.putFloat(v_vy);	// Add "vy" parameter
		sndPacket.putFloat(v_vz);	// Add "vz" parameter
		sndPacket.putFloat(v_afx);	// Add "afx" parameter
		sndPacket.putFloat(v_afy);	// Add "afy" parameter
		sndPacket.putFloat(v_afz);	// Add "afz" parameter
		sndPacket.putFloat(v_yaw);	// Add "yaw" parameter
		sndPacket.putFloat(v_yaw_rate);	// Add "yaw_rate" parameter
		sndPacket.putShortI(v_type_mask);	// Add "type_mask" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_coordinate_frame);	// Add "coordinate_frame" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_time_boot_ms"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_x"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_y"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_z"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_vx"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_vy"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_vz"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_afx"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_afy"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_afz"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_yaw"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_yaw_rate"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_type_mask"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_target_system"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_target_component"
 				+ ", SET_POSITION_TARGET_LOCAL_NED_coordinate_frame"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + x
 				+ ", " + y
 				+ ", " + z
 				+ ", " + vx
 				+ ", " + vy
 				+ ", " + vz
 				+ ", " + afx
 				+ ", " + afy
 				+ ", " + afz
 				+ ", " + yaw
 				+ ", " + yaw_rate
 				+ ", " + type_mask
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + coordinate_frame
				);
		return param;
	}
}
