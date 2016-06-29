/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: GPS_SET_GLOBAL_ORIGIN(48)
--------------------------------------
%%~ As local waypoints exist, the global waypoint reference allows to transform between 
%%~ the local coordinate frame and the global (GPS) coordinate frame. This can be 
%%~ necessary when e.g. in- and outdoor settings are connected and the MAV should 
%%~ move from in- to outdoor.
--------------------------------------
*/
public class GPS_SET_GLOBAL_ORIGIN_class implements Loggable
{
	public static final int msgID = 48;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public int		 latitude;		 	// global position * 1E7
	public int		 longitude;		 	// global position * 1E7
	public int		 altitude;		 	// global position * 1000

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public GPS_SET_GLOBAL_ORIGIN_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public GPS_SET_GLOBAL_ORIGIN_class(GPS_SET_GLOBAL_ORIGIN_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		latitude = o.latitude;
		longitude = o.longitude;
		altitude = o.altitude;
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

			// int[] mavLen = {1, 1, 4, 4, 4};
			// int[] javLen = {2, 2, 4, 4, 4};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			latitude			= rcvPacket.getInt();
			longitude		= rcvPacket.getInt();
			altitude			= rcvPacket.getInt();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,latitude
					 ,longitude
					 ,altitude
					 );
	}

	public byte[] encode(
						 int v_latitude
						,int v_longitude
						,int v_altitude
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_latitude
					 ,v_longitude
					 ,v_altitude
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,int v_latitude
						,int v_longitude
						,int v_altitude
						)
	{
		// int[] mavLen = {1, 1, 4, 4, 4};
		// int[] javLen = {2, 2, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putInt(v_latitude);	// Add "latitude" parameter
		sndPacket.putInt(v_longitude);	// Add "longitude" parameter
		sndPacket.putInt(v_altitude);	// Add "altitude" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GPS_SET_GLOBAL_ORIGIN_target_system"
 				+ ", GPS_SET_GLOBAL_ORIGIN_target_component"
 				+ ", GPS_SET_GLOBAL_ORIGIN_latitude"
 				+ ", GPS_SET_GLOBAL_ORIGIN_longitude"
 				+ ", GPS_SET_GLOBAL_ORIGIN_altitude"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + latitude
 				+ ", " + longitude
 				+ ", " + altitude
				);
		return param;
	}
}
