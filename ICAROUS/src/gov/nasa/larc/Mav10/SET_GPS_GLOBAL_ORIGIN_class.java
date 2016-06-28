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
Message ID: SET_GPS_GLOBAL_ORIGIN(48)
--------------------------------------
%%~ As local waypoints exist, the global MISSION reference allows to transform between 
%%~ the local coordinate frame and the global (GPS) coordinate frame. This can be 
%%~ necessary when e.g. in- and outdoor settings are connected and the MAV should move 
%%~ from in- to outdoor.
--------------------------------------
*/
public class SET_GPS_GLOBAL_ORIGIN_class //implements Loggable
{
	public static final int msgID = 48;
	public int		 latitude;		 	// Latitude (WGS84), in degrees * 1E7
	public int		 longitude;		 	// Longitude (WGS84, in degrees * 1E7
	public int		 altitude;		 	// Altitude (AMSL), in meters * 1000 (positive for up)
	public short	 target_system;	 	// System ID

	private packet rcvPacket;
	private packet sndPacket;

	public SET_GPS_GLOBAL_ORIGIN_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
	}

	public SET_GPS_GLOBAL_ORIGIN_class(SET_GPS_GLOBAL_ORIGIN_class o)
	{
		latitude = o.latitude;
		longitude = o.longitude;
		altitude = o.altitude;
		target_system = o.target_system;
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

			// int[] mavLen = {4, 4, 4, 1};
			// int[] javLen = {4, 4, 4, 2};

			latitude			= rcvPacket.getInt();
			longitude		= rcvPacket.getInt();
			altitude			= rcvPacket.getInt();
			target_system	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  latitude
					 ,longitude
					 ,altitude
					 ,(short)1
					 );
	}

	public byte[] encode(
						 int v_latitude
						,int v_longitude
						,int v_altitude
						)
	{
		return encode(
					  v_latitude
					 ,v_longitude
					 ,v_altitude
					 ,  (short)1
					 );
	}

	public byte[] encode(
						 int v_latitude
						,int v_longitude
						,int v_altitude
						,short v_target_system
						)
	{
		// int[] mavLen = {4, 4, 4, 1};
		// int[] javLen = {4, 4, 4, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putInt(v_latitude);	// Add "latitude" parameter
		sndPacket.putInt(v_longitude);	// Add "longitude" parameter
		sndPacket.putInt(v_altitude);	// Add "altitude" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SET_GPS_GLOBAL_ORIGIN_latitude"
 				+ ", SET_GPS_GLOBAL_ORIGIN_longitude"
 				+ ", SET_GPS_GLOBAL_ORIGIN_altitude"
 				+ ", SET_GPS_GLOBAL_ORIGIN_target_system"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + latitude
 				+ ", " + longitude
 				+ ", " + altitude
 				+ ", " + target_system
				);
		return param;
	}
}
