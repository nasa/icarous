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
Message ID: VFR_HUD(74)
--------------------------------------
%%~ Metrics typically displayed on a HUD for fixed wing aircraft
--------------------------------------
*/
public class VFR_HUD_class //implements Loggable
{
	public static final int msgID = 74;
	public float	 airspeed;	 	// Current airspeed in m/s
	public float	 groundspeed;	 	// Current ground speed in m/s
	public float	 alt;			 	// Current altitude (MSL), in meters
	public float	 climb;		 	// Current climb rate in meters/second
	public short	 heading;		 	// Current heading in degrees, in compass units (0..360, 0=north)
	public int		 throttle;	 	// Current throttle setting in integer percent, 0 to 100

	private packet rcvPacket;
	private packet sndPacket;

	public VFR_HUD_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public VFR_HUD_class(VFR_HUD_class o)
	{
		airspeed = o.airspeed;
		groundspeed = o.groundspeed;
		alt = o.alt;
		climb = o.climb;
		heading = o.heading;
		throttle = o.throttle;
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

			// int[] mavLen = {4, 4, 4, 4, 2, 2};
			// int[] javLen = {4, 4, 4, 4, 2, 4};

			airspeed		= rcvPacket.getFloat();
			groundspeed	= rcvPacket.getFloat();
			alt			= rcvPacket.getFloat();
			climb		= rcvPacket.getFloat();
			heading		= rcvPacket.getShort();
			throttle		= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  airspeed
					 ,groundspeed
					 ,alt
					 ,climb
					 ,heading
					 ,throttle
					 );
	}

	public byte[] encode(
						 float v_airspeed
						,float v_groundspeed
						,float v_alt
						,float v_climb
						,short v_heading
						,int v_throttle
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 2, 2};
		// int[] javLen = {4, 4, 4, 4, 2, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_airspeed);	// Add "airspeed" parameter
		sndPacket.putFloat(v_groundspeed);	// Add "groundspeed" parameter
		sndPacket.putFloat(v_alt);	// Add "alt" parameter
		sndPacket.putFloat(v_climb);	// Add "climb" parameter
		sndPacket.putShort(v_heading);	// Add "heading" parameter
		sndPacket.putShortI(v_throttle);	// Add "throttle" parameter

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
 				+ ", VFR_HUD_alt"
 				+ ", VFR_HUD_climb"
 				+ ", VFR_HUD_heading"
 				+ ", VFR_HUD_throttle"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + airspeed
 				+ ", " + groundspeed
 				+ ", " + alt
 				+ ", " + climb
 				+ ", " + heading
 				+ ", " + throttle
				);
		return param;
	}
}
