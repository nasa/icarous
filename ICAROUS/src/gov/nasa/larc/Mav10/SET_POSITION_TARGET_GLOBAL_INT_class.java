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
Message ID: SET_POSITION_TARGET_GLOBAL_INT(86)
--------------------------------------
%%~ Set vehicle position, velocity and acceleration setpoint in the WGS84 coordinate 
%%~ system.
--------------------------------------
*/
public class SET_POSITION_TARGET_GLOBAL_INT_class //implements Loggable
{
	public static final int msgID = 86;
	public long	 time_boot_ms;	 	// Timestamp in milliseconds since system boot. The rationale for the timestamp in the setpoint is to allow the system to compensate for the transport delay of the setpoint. This allows the system to compensate processing latency.
	public int		 lat_int;			 	// X Position in WGS84 frame in 1e7 * meters
	public int		 lon_int;			 	// Y Position in WGS84 frame in 1e7 * meters
	public float	 alt;				 	// Altitude in meters in AMSL altitude, not WGS84 if absolute or relative, above terrain if GLOBAL_TERRAIN_ALT_INT
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
	public short	 coordinate_frame; 	// Valid options are: MAV_FRAME_GLOBAL_INT = 5, MAV_FRAME_GLOBAL_RELATIVE_ALT_INT = 6, MAV_FRAME_GLOBAL_TERRAIN_ALT_INT = 11

	private packet rcvPacket;
	private packet sndPacket;

	public SET_POSITION_TARGET_GLOBAL_INT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public SET_POSITION_TARGET_GLOBAL_INT_class(SET_POSITION_TARGET_GLOBAL_INT_class o)
	{
		time_boot_ms = o.time_boot_ms;
		lat_int = o.lat_int;
		lon_int = o.lon_int;
		alt = o.alt;
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
			lat_int			= rcvPacket.getInt();
			lon_int			= rcvPacket.getInt();
			alt				= rcvPacket.getFloat();
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
					 ,lat_int
					 ,lon_int
					 ,alt
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
						,int v_lat_int
						,int v_lon_int
						,float v_alt
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
					 ,v_lat_int
					 ,v_lon_int
					 ,v_alt
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
						,int v_lat_int
						,int v_lon_int
						,float v_alt
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
		sndPacket.putInt(v_lat_int);	// Add "lat_int" parameter
		sndPacket.putInt(v_lon_int);	// Add "lon_int" parameter
		sndPacket.putFloat(v_alt);	// Add "alt" parameter
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
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_time_boot_ms"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_lat_int"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_lon_int"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_alt"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_vx"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_vy"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_vz"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_afx"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_afy"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_afz"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_yaw"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_yaw_rate"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_type_mask"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_target_system"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_target_component"
 				+ ", SET_POSITION_TARGET_GLOBAL_INT_coordinate_frame"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + lat_int
 				+ ", " + lon_int
 				+ ", " + alt
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
