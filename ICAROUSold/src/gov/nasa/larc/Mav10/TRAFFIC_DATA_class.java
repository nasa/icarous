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
Message ID: TRAFFIC_DATA(233)
--------------------------------------
%%~ Message that provides 6 dof position data about other traffic. This includes: Vehicle 
%%~ number, latitud (deg), longitud (deg), altitud(meters), roll (deg), pitch 
%%~ (deg), heading(deg)
--------------------------------------
*/
public class TRAFFIC_DATA_class //implements Loggable
{
	public static final int msgID = 233;
	public float	 lat;			 	// latitude (deg)
	public float	 lon;			 	// longitude (deg)
	public float	 alt;			 	// altitude (meters)
	public float	 roll;		 	// roll (deg)
	public float	 pitch;		 	// pitch (deg)
	public float	 heading;		 	// heading (deg)
	public byte[]	 ID = new byte[8];				// vehicle ID
	public short	 ownship_flag; 	// identifies whether the aircraft is ownship (1) or not (0)

	private packet rcvPacket;
	private packet sndPacket;

	public TRAFFIC_DATA_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public TRAFFIC_DATA_class(TRAFFIC_DATA_class o)
	{
		lat = o.lat;
		lon = o.lon;
		alt = o.alt;
		roll = o.roll;
		pitch = o.pitch;
		heading = o.heading;
		ID = o.ID;
		ownship_flag = o.ownship_flag;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 8, 1};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 8, 2};

			lat			= rcvPacket.getFloat();
			lon			= rcvPacket.getFloat();
			alt			= rcvPacket.getFloat();
			roll			= rcvPacket.getFloat();
			pitch		= rcvPacket.getFloat();
			heading		= rcvPacket.getFloat();
			rcvPacket.getByte(ID, 0, 8);
			ownship_flag	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  lat
					 ,lon
					 ,alt
					 ,roll
					 ,pitch
					 ,heading
					 ,ID
					 ,ownship_flag
					 );
	}

	public byte[] encode(
						 float v_lat
						,float v_lon
						,float v_alt
						,float v_roll
						,float v_pitch
						,float v_heading
						,byte[] v_ID
						,short v_ownship_flag
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 8, 1};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 8, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_lat);	// Add "lat" parameter
		sndPacket.putFloat(v_lon);	// Add "lon" parameter
		sndPacket.putFloat(v_alt);	// Add "alt" parameter
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_heading);	// Add "heading" parameter
		sndPacket.putByte(v_ID,0,8);	// Add "ID" parameter
		sndPacket.putByteS(v_ownship_flag);	// Add "ownship_flag" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", TRAFFIC_DATA_lat"
 				+ ", TRAFFIC_DATA_lon"
 				+ ", TRAFFIC_DATA_alt"
 				+ ", TRAFFIC_DATA_roll"
 				+ ", TRAFFIC_DATA_pitch"
 				+ ", TRAFFIC_DATA_heading"
 				+ ", TRAFFIC_DATA_ID"
 				+ ", TRAFFIC_DATA_ownship_flag"
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
 				+ ", " + roll
 				+ ", " + pitch
 				+ ", " + heading
 				+ ", " + ID
 				+ ", " + ownship_flag
				);
		return param;
	}
}
