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
Message ID: NAV_CONTROLLER_OUTPUT(62)
--------------------------------------
%%~ Outputs of the APM navigation controller. The primary use of this message is to 
%%~ check the response and signs of the controller before actual flight and to assist 
%%~ with tuning controller parameters.
--------------------------------------
*/
public class NAV_CONTROLLER_OUTPUT_class //implements Loggable
{
	public static final int msgID = 62;
	public float	 nav_roll;		 	// Current desired roll in degrees
	public float	 nav_pitch;		 	// Current desired pitch in degrees
	public float	 alt_error;		 	// Current altitude error in meters
	public float	 aspd_error;		 	// Current airspeed error in meters/second
	public float	 xtrack_error;	 	// Current crosstrack error on x-y plane in meters
	public short	 nav_bearing;		 	// Current desired heading in degrees
	public short	 target_bearing;	 	// Bearing to current MISSION/target in degrees
	public int		 wp_dist;			 	// Distance to active MISSION in meters

	private packet rcvPacket;
	private packet sndPacket;

	public NAV_CONTROLLER_OUTPUT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public NAV_CONTROLLER_OUTPUT_class(NAV_CONTROLLER_OUTPUT_class o)
	{
		nav_roll = o.nav_roll;
		nav_pitch = o.nav_pitch;
		alt_error = o.alt_error;
		aspd_error = o.aspd_error;
		xtrack_error = o.xtrack_error;
		nav_bearing = o.nav_bearing;
		target_bearing = o.target_bearing;
		wp_dist = o.wp_dist;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 2, 2, 2};
			// int[] javLen = {4, 4, 4, 4, 4, 2, 2, 4};

			nav_roll			= rcvPacket.getFloat();
			nav_pitch		= rcvPacket.getFloat();
			alt_error		= rcvPacket.getFloat();
			aspd_error		= rcvPacket.getFloat();
			xtrack_error		= rcvPacket.getFloat();
			nav_bearing		= rcvPacket.getShort();
			target_bearing	= rcvPacket.getShort();
			wp_dist			= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  nav_roll
					 ,nav_pitch
					 ,alt_error
					 ,aspd_error
					 ,xtrack_error
					 ,nav_bearing
					 ,target_bearing
					 ,wp_dist
					 );
	}

	public byte[] encode(
						 float v_nav_roll
						,float v_nav_pitch
						,float v_alt_error
						,float v_aspd_error
						,float v_xtrack_error
						,short v_nav_bearing
						,short v_target_bearing
						,int v_wp_dist
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 2, 2, 2};
		// int[] javLen = {4, 4, 4, 4, 4, 2, 2, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_nav_roll);	// Add "nav_roll" parameter
		sndPacket.putFloat(v_nav_pitch);	// Add "nav_pitch" parameter
		sndPacket.putFloat(v_alt_error);	// Add "alt_error" parameter
		sndPacket.putFloat(v_aspd_error);	// Add "aspd_error" parameter
		sndPacket.putFloat(v_xtrack_error);	// Add "xtrack_error" parameter
		sndPacket.putShort(v_nav_bearing);	// Add "nav_bearing" parameter
		sndPacket.putShort(v_target_bearing);	// Add "target_bearing" parameter
		sndPacket.putShortI(v_wp_dist);	// Add "wp_dist" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", NAV_CONTROLLER_OUTPUT_nav_roll"
 				+ ", NAV_CONTROLLER_OUTPUT_nav_pitch"
 				+ ", NAV_CONTROLLER_OUTPUT_alt_error"
 				+ ", NAV_CONTROLLER_OUTPUT_aspd_error"
 				+ ", NAV_CONTROLLER_OUTPUT_xtrack_error"
 				+ ", NAV_CONTROLLER_OUTPUT_nav_bearing"
 				+ ", NAV_CONTROLLER_OUTPUT_target_bearing"
 				+ ", NAV_CONTROLLER_OUTPUT_wp_dist"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + nav_roll
 				+ ", " + nav_pitch
 				+ ", " + alt_error
 				+ ", " + aspd_error
 				+ ", " + xtrack_error
 				+ ", " + nav_bearing
 				+ ", " + target_bearing
 				+ ", " + wp_dist
				);
		return param;
	}
}
