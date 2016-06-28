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
Message ID: LATLON_HEADING(239)
--------------------------------------
%%~ Message that provides 6 dof position data about other traffic. This includes: Vehicle 
%%~ number, latitud (deg), longitud (deg), altitud(meters), roll (deg), pitch 
%%~ (deg), heading(deg), and more
--------------------------------------
*/
public class LATLON_HEADING_class //implements Loggable
{
	public static final int msgID = 239;
	public float	 lat;		 	// latitude (deg)
	public float	 lon;		 	// longitude (deg)
	public float	 alt;		 	// altitude (meters)
	public float	 vspd;	 	// vertical speed (m/s)
	public float	 spd;		 	// groundspeed (m/s)
	public float	 roll;	 	// roll (deg)
	public float	 pitch;	 	// pitch (deg)
	public float	 heading;	 	// heading (deg)
	public long	 time;	 	// timestamp (ms)
	public byte[]	 ID = new byte[8];			// vehicle ID

	private packet rcvPacket;
	private packet sndPacket;

	public LATLON_HEADING_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public LATLON_HEADING_class(LATLON_HEADING_class o)
	{
		lat = o.lat;
		lon = o.lon;
		alt = o.alt;
		vspd = o.vspd;
		spd = o.spd;
		roll = o.roll;
		pitch = o.pitch;
		heading = o.heading;
		time = o.time;
		ID = o.ID;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 8};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 8, 8};

			lat		= rcvPacket.getFloat();
			lon		= rcvPacket.getFloat();
			alt		= rcvPacket.getFloat();
			vspd		= rcvPacket.getFloat();
			spd		= rcvPacket.getFloat();
			roll		= rcvPacket.getFloat();
			pitch	= rcvPacket.getFloat();
			heading	= rcvPacket.getFloat();
			time		= rcvPacket.getLongI();
			rcvPacket.getByte(ID, 0, 8);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  lat
					 ,lon
					 ,alt
					 ,vspd
					 ,spd
					 ,roll
					 ,pitch
					 ,heading
					 ,time
					 ,ID
					 );
	}

	public byte[] encode(
						 float v_lat
						,float v_lon
						,float v_alt
						,float v_vspd
						,float v_spd
						,float v_roll
						,float v_pitch
						,float v_heading
						,long v_time
						,byte[] v_ID
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 8};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 8, 8};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_lat);	// Add "lat" parameter
		sndPacket.putFloat(v_lon);	// Add "lon" parameter
		sndPacket.putFloat(v_alt);	// Add "alt" parameter
		sndPacket.putFloat(v_vspd);	// Add "vspd" parameter
		sndPacket.putFloat(v_spd);	// Add "spd" parameter
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_heading);	// Add "heading" parameter
		sndPacket.putIntL(v_time);	// Add "time" parameter
		sndPacket.putByte(v_ID,0,8);	// Add "ID" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", LATLON_HEADING_lat"
 				+ ", LATLON_HEADING_lon"
 				+ ", LATLON_HEADING_alt"
 				+ ", LATLON_HEADING_vspd"
 				+ ", LATLON_HEADING_spd"
 				+ ", LATLON_HEADING_roll"
 				+ ", LATLON_HEADING_pitch"
 				+ ", LATLON_HEADING_heading"
 				+ ", LATLON_HEADING_time"
 				+ ", LATLON_HEADING_ID"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + lat
 				+ ", " + lon
 				+ ", " + alt
 				+ ", " + vspd
 				+ ", " + spd
 				+ ", " + roll
 				+ ", " + pitch
 				+ ", " + heading
 				+ ", " + time
 				+ ", " + ID
				);
		return param;
	}
}
