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
Message ID: ATTITUDE(30)
--------------------------------------
%%~ The attitude in the aeronautical frame (right-handed, Z-down, X-front, Y-right).
--------------------------------------
*/
public class ATTITUDE_class //implements Loggable
{
	public static final int msgID = 30;
	public long	 time_boot_ms; 	// Timestamp (milliseconds since system boot)
	public float	 roll;		 	// Roll angle (rad, -pi..+pi)
	public float	 pitch;		 	// Pitch angle (rad, -pi..+pi)
	public float	 yaw;			 	// Yaw angle (rad, -pi..+pi)
	public float	 rollspeed;	 	// Roll angular speed (rad/s)
	public float	 pitchspeed;	 	// Pitch angular speed (rad/s)
	public float	 yawspeed;	 	// Yaw angular speed (rad/s)

	private packet rcvPacket;
	private packet sndPacket;

	public ATTITUDE_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public ATTITUDE_class(ATTITUDE_class o)
	{
		time_boot_ms = o.time_boot_ms;
		roll = o.roll;
		pitch = o.pitch;
		yaw = o.yaw;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4};

			time_boot_ms	= rcvPacket.getLongI();
			roll			= rcvPacket.getFloat();
			pitch		= rcvPacket.getFloat();
			yaw			= rcvPacket.getFloat();
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
					 ,roll
					 ,pitch
					 ,yaw
					 ,rollspeed
					 ,pitchspeed
					 ,yawspeed
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,float v_roll
						,float v_pitch
						,float v_yaw
						,float v_rollspeed
						,float v_pitchspeed
						,float v_yawspeed
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_yaw);	// Add "yaw" parameter
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
 				+ ", ATTITUDE_time_boot_ms"
 				+ ", ATTITUDE_roll"
 				+ ", ATTITUDE_pitch"
 				+ ", ATTITUDE_yaw"
 				+ ", ATTITUDE_rollspeed"
 				+ ", ATTITUDE_pitchspeed"
 				+ ", ATTITUDE_yawspeed"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + roll
 				+ ", " + pitch
 				+ ", " + yaw
 				+ ", " + rollspeed
 				+ ", " + pitchspeed
 				+ ", " + yawspeed
				);
		return param;
	}
}
