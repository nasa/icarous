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
Message ID: GIMBAL_REPORT(200)
--------------------------------------
%%~ 3 axis gimbal mesuraments
--------------------------------------
*/
public class GIMBAL_REPORT_class //implements Loggable
{
	public static final int msgID = 200;
	public float	 delta_time;		 	// Time since last update (seconds)
	public float	 delta_angle_x;	 	// Delta angle X (radians)
	public float	 delta_angle_y;	 	// Delta angle Y (radians)
	public float	 delta_angle_z;	 	// Delta angle X (radians)
	public float	 delta_velocity_x; 	// Delta velocity X (m/s)
	public float	 delta_velocity_y; 	// Delta velocity Y (m/s)
	public float	 delta_velocity_z; 	// Delta velocity Z (m/s)
	public float	 joint_roll;		 	//  Joint ROLL (radians)
	public float	 joint_el;		 	//  Joint EL (radians)
	public float	 joint_az;		 	//  Joint AZ (radians)
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID

	private packet rcvPacket;
	private packet sndPacket;

	public GIMBAL_REPORT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public GIMBAL_REPORT_class(GIMBAL_REPORT_class o)
	{
		delta_time = o.delta_time;
		delta_angle_x = o.delta_angle_x;
		delta_angle_y = o.delta_angle_y;
		delta_angle_z = o.delta_angle_z;
		delta_velocity_x = o.delta_velocity_x;
		delta_velocity_y = o.delta_velocity_y;
		delta_velocity_z = o.delta_velocity_z;
		joint_roll = o.joint_roll;
		joint_el = o.joint_el;
		joint_az = o.joint_az;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2};

			delta_time		= rcvPacket.getFloat();
			delta_angle_x	= rcvPacket.getFloat();
			delta_angle_y	= rcvPacket.getFloat();
			delta_angle_z	= rcvPacket.getFloat();
			delta_velocity_x	= rcvPacket.getFloat();
			delta_velocity_y	= rcvPacket.getFloat();
			delta_velocity_z	= rcvPacket.getFloat();
			joint_roll		= rcvPacket.getFloat();
			joint_el			= rcvPacket.getFloat();
			joint_az			= rcvPacket.getFloat();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  delta_time
					 ,delta_angle_x
					 ,delta_angle_y
					 ,delta_angle_z
					 ,delta_velocity_x
					 ,delta_velocity_y
					 ,delta_velocity_z
					 ,joint_roll
					 ,joint_el
					 ,joint_az
					 ,(short)1
					 ,(short)1
					 );
	}

	public byte[] encode(
						 float v_delta_time
						,float v_delta_angle_x
						,float v_delta_angle_y
						,float v_delta_angle_z
						,float v_delta_velocity_x
						,float v_delta_velocity_y
						,float v_delta_velocity_z
						,float v_joint_roll
						,float v_joint_el
						,float v_joint_az
						)
	{
		return encode(
					  v_delta_time
					 ,v_delta_angle_x
					 ,v_delta_angle_y
					 ,v_delta_angle_z
					 ,v_delta_velocity_x
					 ,v_delta_velocity_y
					 ,v_delta_velocity_z
					 ,v_joint_roll
					 ,v_joint_el
					 ,v_joint_az
					 ,  (short)1
					 ,  (short)1
					 );
	}

	public byte[] encode(
						 float v_delta_time
						,float v_delta_angle_x
						,float v_delta_angle_y
						,float v_delta_angle_z
						,float v_delta_velocity_x
						,float v_delta_velocity_y
						,float v_delta_velocity_z
						,float v_joint_roll
						,float v_joint_el
						,float v_joint_az
						,short v_target_system
						,short v_target_component
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_delta_time);	// Add "delta_time" parameter
		sndPacket.putFloat(v_delta_angle_x);	// Add "delta_angle_x" parameter
		sndPacket.putFloat(v_delta_angle_y);	// Add "delta_angle_y" parameter
		sndPacket.putFloat(v_delta_angle_z);	// Add "delta_angle_z" parameter
		sndPacket.putFloat(v_delta_velocity_x);	// Add "delta_velocity_x" parameter
		sndPacket.putFloat(v_delta_velocity_y);	// Add "delta_velocity_y" parameter
		sndPacket.putFloat(v_delta_velocity_z);	// Add "delta_velocity_z" parameter
		sndPacket.putFloat(v_joint_roll);	// Add "joint_roll" parameter
		sndPacket.putFloat(v_joint_el);	// Add "joint_el" parameter
		sndPacket.putFloat(v_joint_az);	// Add "joint_az" parameter
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
 				+ ", GIMBAL_REPORT_delta_time"
 				+ ", GIMBAL_REPORT_delta_angle_x"
 				+ ", GIMBAL_REPORT_delta_angle_y"
 				+ ", GIMBAL_REPORT_delta_angle_z"
 				+ ", GIMBAL_REPORT_delta_velocity_x"
 				+ ", GIMBAL_REPORT_delta_velocity_y"
 				+ ", GIMBAL_REPORT_delta_velocity_z"
 				+ ", GIMBAL_REPORT_joint_roll"
 				+ ", GIMBAL_REPORT_joint_el"
 				+ ", GIMBAL_REPORT_joint_az"
 				+ ", GIMBAL_REPORT_target_system"
 				+ ", GIMBAL_REPORT_target_component"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + delta_time
 				+ ", " + delta_angle_x
 				+ ", " + delta_angle_y
 				+ ", " + delta_angle_z
 				+ ", " + delta_velocity_x
 				+ ", " + delta_velocity_y
 				+ ", " + delta_velocity_z
 				+ ", " + joint_roll
 				+ ", " + joint_el
 				+ ", " + joint_az
 				+ ", " + target_system
 				+ ", " + target_component
				);
		return param;
	}
}
