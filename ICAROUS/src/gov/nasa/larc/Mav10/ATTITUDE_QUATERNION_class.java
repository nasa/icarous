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
Message ID: ATTITUDE_QUATERNION(31)
--------------------------------------
%%~ The attitude in the aeronautical frame (right-handed, Z-down, X-front, Y-right), 
%%~ expressed as quaternion. Quaternion order is w, x, y, z and a zero rotation would 
%%~ be expressed as (1 0 0 0).
--------------------------------------
*/
public class ATTITUDE_QUATERNION_class //implements Loggable
{
	public static final int msgID = 31;
	public long	 time_boot_ms; 	// Timestamp (milliseconds since system boot)
	public float	 q1;			 	// Quaternion component 1, w (1 in null-rotation)
	public float	 q2;			 	// Quaternion component 2, x (0 in null-rotation)
	public float	 q3;			 	// Quaternion component 3, y (0 in null-rotation)
	public float	 q4;			 	// Quaternion component 4, z (0 in null-rotation)
	public float	 rollspeed;	 	// Roll angular speed (rad/s)
	public float	 pitchspeed;	 	// Pitch angular speed (rad/s)
	public float	 yawspeed;	 	// Yaw angular speed (rad/s)

	private packet rcvPacket;
	private packet sndPacket;

	public ATTITUDE_QUATERNION_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public ATTITUDE_QUATERNION_class(ATTITUDE_QUATERNION_class o)
	{
		time_boot_ms = o.time_boot_ms;
		q1 = o.q1;
		q2 = o.q2;
		q3 = o.q3;
		q4 = o.q4;
		rollspeed = o.rollspeed;
		pitchspeed = o.pitchspeed;
		yawspeed = o.yawspeed;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4};

			time_boot_ms	= rcvPacket.getLongI();
			q1			= rcvPacket.getFloat();
			q2			= rcvPacket.getFloat();
			q3			= rcvPacket.getFloat();
			q4			= rcvPacket.getFloat();
			rollspeed	= rcvPacket.getFloat();
			pitchspeed	= rcvPacket.getFloat();
			yawspeed		= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
					 ,q1
					 ,q2
					 ,q3
					 ,q4
					 ,rollspeed
					 ,pitchspeed
					 ,yawspeed
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,float v_q1
						,float v_q2
						,float v_q3
						,float v_q4
						,float v_rollspeed
						,float v_pitchspeed
						,float v_yawspeed
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putFloat(v_q1);	// Add "q1" parameter
		sndPacket.putFloat(v_q2);	// Add "q2" parameter
		sndPacket.putFloat(v_q3);	// Add "q3" parameter
		sndPacket.putFloat(v_q4);	// Add "q4" parameter
		sndPacket.putFloat(v_rollspeed);	// Add "rollspeed" parameter
		sndPacket.putFloat(v_pitchspeed);	// Add "pitchspeed" parameter
		sndPacket.putFloat(v_yawspeed);	// Add "yawspeed" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", ATTITUDE_QUATERNION_time_boot_ms"
 				+ ", ATTITUDE_QUATERNION_q1"
 				+ ", ATTITUDE_QUATERNION_q2"
 				+ ", ATTITUDE_QUATERNION_q3"
 				+ ", ATTITUDE_QUATERNION_q4"
 				+ ", ATTITUDE_QUATERNION_rollspeed"
 				+ ", ATTITUDE_QUATERNION_pitchspeed"
 				+ ", ATTITUDE_QUATERNION_yawspeed"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + q1
 				+ ", " + q2
 				+ ", " + q3
 				+ ", " + q4
 				+ ", " + rollspeed
 				+ ", " + pitchspeed
 				+ ", " + yawspeed
				);
		return param;
	}
}
