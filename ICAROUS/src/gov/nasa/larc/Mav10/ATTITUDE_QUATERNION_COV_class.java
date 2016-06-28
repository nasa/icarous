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
Message ID: ATTITUDE_QUATERNION_COV(61)
--------------------------------------
%%~ The attitude in the aeronautical frame (right-handed, Z-down, X-front, Y-right), 
%%~ expressed as quaternion. Quaternion order is w, x, y, z and a zero rotation would 
%%~ be expressed as (1 0 0 0).
--------------------------------------
*/
public class ATTITUDE_QUATERNION_COV_class //implements Loggable
{
	public static final int msgID = 61;
	public long	 time_boot_ms; 	// Timestamp (milliseconds since system boot)
	public float[]	 q = new float[4];				// Quaternion components, w, x, y, z (1 0 0 0 is the null-rotation)
	public float	 rollspeed;	 	// Roll angular speed (rad/s)
	public float	 pitchspeed;	 	// Pitch angular speed (rad/s)
	public float	 yawspeed;	 	// Yaw angular speed (rad/s)
	public float[]	 covariance = new float[9];		// Attitude covariance

	private packet rcvPacket;
	private packet sndPacket;

	public ATTITUDE_QUATERNION_COV_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public ATTITUDE_QUATERNION_COV_class(ATTITUDE_QUATERNION_COV_class o)
	{
		time_boot_ms = o.time_boot_ms;
		q = o.q;
		rollspeed = o.rollspeed;
		pitchspeed = o.pitchspeed;
		yawspeed = o.yawspeed;
		covariance = o.covariance;
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

			// int[] mavLen = {4, 16, 4, 4, 4, 36};
			// int[] javLen = {8, 16, 4, 4, 4, 36};

			time_boot_ms	= rcvPacket.getLongI();
			rcvPacket.getByte(q, 0, 4);
			rollspeed	= rcvPacket.getFloat();
			pitchspeed	= rcvPacket.getFloat();
			yawspeed		= rcvPacket.getFloat();
			rcvPacket.getByte(covariance, 0, 9);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
					 ,q
					 ,rollspeed
					 ,pitchspeed
					 ,yawspeed
					 ,covariance
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,float[] v_q
						,float v_rollspeed
						,float v_pitchspeed
						,float v_yawspeed
						,float[] v_covariance
						)
	{
		// int[] mavLen = {4, 16, 4, 4, 4, 36};
		// int[] javLen = {8, 16, 4, 4, 4, 36};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putByte(v_q,0,4);	// Add "q" parameter
		sndPacket.putFloat(v_rollspeed);	// Add "rollspeed" parameter
		sndPacket.putFloat(v_pitchspeed);	// Add "pitchspeed" parameter
		sndPacket.putFloat(v_yawspeed);	// Add "yawspeed" parameter
		sndPacket.putByte(v_covariance,0,9);	// Add "covariance" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", ATTITUDE_QUATERNION_COV_time_boot_ms"
 				+ ", ATTITUDE_QUATERNION_COV_q"
 				+ ", ATTITUDE_QUATERNION_COV_rollspeed"
 				+ ", ATTITUDE_QUATERNION_COV_pitchspeed"
 				+ ", ATTITUDE_QUATERNION_COV_yawspeed"
 				+ ", ATTITUDE_QUATERNION_COV_covariance"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + q
 				+ ", " + rollspeed
 				+ ", " + pitchspeed
 				+ ", " + yawspeed
 				+ ", " + covariance
				);
		return param;
	}
}
