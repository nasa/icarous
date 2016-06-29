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
Message ID: EKF_STATUS_REPORT(193)
--------------------------------------
%%~ EKF Status message including flags and variances
--------------------------------------
*/
public class EKF_STATUS_REPORT_class //implements Loggable
{
	public static final int msgID = 193;
	public float	 velocity_variance;	 	// Velocity variance
	public float	 pos_horiz_variance;	 	// Horizontal Position variance
	public float	 pos_vert_variance;	 	// Vertical Position variance
	public float	 compass_variance;	 	// Compass variance
	public float	 terrain_alt_variance; 	// Terrain Altitude variance
	public int		 flags;				 	// Flags

	private packet rcvPacket;
	private packet sndPacket;

	public EKF_STATUS_REPORT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public EKF_STATUS_REPORT_class(EKF_STATUS_REPORT_class o)
	{
		velocity_variance = o.velocity_variance;
		pos_horiz_variance = o.pos_horiz_variance;
		pos_vert_variance = o.pos_vert_variance;
		compass_variance = o.compass_variance;
		terrain_alt_variance = o.terrain_alt_variance;
		flags = o.flags;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 2};
			// int[] javLen = {4, 4, 4, 4, 4, 4};

			velocity_variance	= rcvPacket.getFloat();
			pos_horiz_variance	= rcvPacket.getFloat();
			pos_vert_variance	= rcvPacket.getFloat();
			compass_variance		= rcvPacket.getFloat();
			terrain_alt_variance	= rcvPacket.getFloat();
			flags				= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  velocity_variance
					 ,pos_horiz_variance
					 ,pos_vert_variance
					 ,compass_variance
					 ,terrain_alt_variance
					 ,flags
					 );
	}

	public byte[] encode(
						 float v_velocity_variance
						,float v_pos_horiz_variance
						,float v_pos_vert_variance
						,float v_compass_variance
						,float v_terrain_alt_variance
						,int v_flags
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 2};
		// int[] javLen = {4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_velocity_variance);	// Add "velocity_variance" parameter
		sndPacket.putFloat(v_pos_horiz_variance);	// Add "pos_horiz_variance" parameter
		sndPacket.putFloat(v_pos_vert_variance);	// Add "pos_vert_variance" parameter
		sndPacket.putFloat(v_compass_variance);	// Add "compass_variance" parameter
		sndPacket.putFloat(v_terrain_alt_variance);	// Add "terrain_alt_variance" parameter
		sndPacket.putShortI(v_flags);	// Add "flags" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", EKF_STATUS_REPORT_velocity_variance"
 				+ ", EKF_STATUS_REPORT_pos_horiz_variance"
 				+ ", EKF_STATUS_REPORT_pos_vert_variance"
 				+ ", EKF_STATUS_REPORT_compass_variance"
 				+ ", EKF_STATUS_REPORT_terrain_alt_variance"
 				+ ", EKF_STATUS_REPORT_flags"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + velocity_variance
 				+ ", " + pos_horiz_variance
 				+ ", " + pos_vert_variance
 				+ ", " + compass_variance
 				+ ", " + terrain_alt_variance
 				+ ", " + flags
				);
		return param;
	}
}
