/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: VFR_HUD(74)
--------------------------------------
%%~ Metrics typically displayed on a HUD for fixed wing aircraft
--------------------------------------
*/
public class VFR_HUD_class implements Loggable
{
	public static final int msgID = 74;
	public float	 airspeed;	 	// Current airspeed in m/s
	public float	 groundspeed;	 	// Current ground speed in m/s
	public short	 heading;		 	// Current heading in degrees, in compass units (0..360, 0=north)
	public int		 throttle;	 	// Current throttle setting in integer percent, 0 to 100
	public float	 alt;			 	// Current altitude (MSL), in meters
	public float	 climb;		 	// Current climb rate in meters/second

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public VFR_HUD_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public VFR_HUD_class(VFR_HUD_class o)
	{
		airspeed = o.airspeed;
		groundspeed = o.groundspeed;
		heading = o.heading;
		throttle = o.throttle;
		alt = o.alt;
		climb = o.climb;
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

			// int[] mavLen = {4, 4, 2, 2, 4, 4};
			// int[] javLen = {4, 4, 2, 4, 4, 4};

			airspeed		= rcvPacket.getFloat();
			groundspeed	= rcvPacket.getFloat();
			heading		= rcvPacket.getShort();
			throttle		= rcvPacket.getIntS();
			alt			= rcvPacket.getFloat();
			climb		= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  airspeed
					 ,groundspeed
					 ,heading
					 ,throttle
					 ,alt
					 ,climb
					 );
	}

	public byte[] encode(
						 float v_airspeed
						,float v_groundspeed
						,short v_heading
						,int v_throttle
						,float v_alt
						,float v_climb
						)
	{
		// int[] mavLen = {4, 4, 2, 2, 4, 4};
		// int[] javLen = {4, 4, 2, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_airspeed);	// Add "airspeed" parameter
		sndPacket.putFloat(v_groundspeed);	// Add "groundspeed" parameter
		sndPacket.putShort(v_heading);	// Add "heading" parameter
		sndPacket.putShortI(v_throttle);	// Add "throttle" parameter
		sndPacket.putFloat(v_alt);	// Add "alt" parameter
		sndPacket.putFloat(v_climb);	// Add "climb" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", VFR_HUD_airspeed"
 				+ ", VFR_HUD_groundspeed"
 				+ ", VFR_HUD_heading"
 				+ ", VFR_HUD_throttle"
 				+ ", VFR_HUD_alt"
 				+ ", VFR_HUD_climb"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + airspeed
 				+ ", " + groundspeed
 				+ ", " + heading
 				+ ", " + throttle
 				+ ", " + alt
 				+ ", " + climb
				);
		return param;
	}
}
