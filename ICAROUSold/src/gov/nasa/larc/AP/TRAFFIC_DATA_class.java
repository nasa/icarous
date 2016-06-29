/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: TRAFFIC_DATA(114)
--------------------------------------
%%~ Message that provides 6 dof position data about other traffic. This includes: Vehicle 
%%~ number, latitud (deg), longitud (deg), altitud(meters), roll (deg), pitch 
%%~ (deg), heading(deg)
--------------------------------------
*/
public class TRAFFIC_DATA_class implements Loggable
{
	public static final int msgID = 114;
	public byte	 num;		 	// vehicle number
	public float	 lat;		 	// latitude (deg)
	public float	 lon;		 	// longitude (deg)
	public float	 alt;		 	// altitude (meters)
	public float	 roll;	 	// roll (deg)
	public float	 pitch;	 	// pitch (deg)
	public float	 heading;	 	// heading (deg)

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public TRAFFIC_DATA_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public TRAFFIC_DATA_class(TRAFFIC_DATA_class o)
	{
		num = o.num;
		lat = o.lat;
		lon = o.lon;
		alt = o.alt;
		roll = o.roll;
		pitch = o.pitch;
		heading = o.heading;
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

			// int[] mavLen = {1, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {1, 4, 4, 4, 4, 4, 4};

			num		= rcvPacket.getByte();
			lat		= rcvPacket.getFloat();
			lon		= rcvPacket.getFloat();
			alt		= rcvPacket.getFloat();
			roll		= rcvPacket.getFloat();
			pitch	= rcvPacket.getFloat();
			heading	= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  num
					 ,lat
					 ,lon
					 ,alt
					 ,roll
					 ,pitch
					 ,heading
					 );
	}

	public byte[] encode(
						 byte v_num
						,float v_lat
						,float v_lon
						,float v_alt
						,float v_roll
						,float v_pitch
						,float v_heading
						)
	{
		// int[] mavLen = {1, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {1, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByte(v_num);	// Add "num" parameter
		sndPacket.putFloat(v_lat);	// Add "lat" parameter
		sndPacket.putFloat(v_lon);	// Add "lon" parameter
		sndPacket.putFloat(v_alt);	// Add "alt" parameter
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_heading);	// Add "heading" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", TRAFFIC_DATA_num"
 				+ ", TRAFFIC_DATA_lat"
 				+ ", TRAFFIC_DATA_lon"
 				+ ", TRAFFIC_DATA_alt"
 				+ ", TRAFFIC_DATA_roll"
 				+ ", TRAFFIC_DATA_pitch"
 				+ ", TRAFFIC_DATA_heading"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + num
 				+ ", " + lat
 				+ ", " + lon
 				+ ", " + alt
 				+ ", " + roll
 				+ ", " + pitch
 				+ ", " + heading
				);
		return param;
	}
}
