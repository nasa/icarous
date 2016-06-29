/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: GPS_STATUS(27)
--------------------------------------
%%~ The positioning status, as reported by GPS. This message is intended to display 
%%~ status information about each satellite visible to the receiver. See message GLOBAL_POSITION 
%%~ for the global position estimate. This message can contain information 
%%~ for up to 20 satellites.
--------------------------------------
*/
public class GPS_STATUS_class implements Loggable
{
	public static final int msgID = 27;
	public short	 satellites_visible;	 	// Number of satellites visible
	public byte[]	 satellite_prn = new byte[20];			// Global satellite ID
	public byte[]	 satellite_used = new byte[20];			// 0: Satellite not used, 1: used for localization
	public byte[]	 satellite_elevation = new byte[20];		// Elevation (0: right on top of receiver, 90: on the horizon) of satellite
	public byte[]	 satellite_azimuth = new byte[20];		// Direction of satellite, 0: 0 deg, 255: 360 deg.
	public byte[]	 satellite_snr = new byte[20];			// Signal to noise ratio of satellite

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public GPS_STATUS_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public GPS_STATUS_class(GPS_STATUS_class o)
	{
		satellites_visible = o.satellites_visible;
		satellite_prn = o.satellite_prn;
		satellite_used = o.satellite_used;
		satellite_elevation = o.satellite_elevation;
		satellite_azimuth = o.satellite_azimuth;
		satellite_snr = o.satellite_snr;
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

			// int[] mavLen = {1, 20, 20, 20, 20, 20};
			// int[] javLen = {2, 20, 20, 20, 20, 20};

			satellites_visible	= rcvPacket.getShortB();
			rcvPacket.getByte(satellite_prn, 0, 20);
			rcvPacket.getByte(satellite_used, 0, 20);
			rcvPacket.getByte(satellite_elevation, 0, 20);
			rcvPacket.getByte(satellite_azimuth, 0, 20);
			rcvPacket.getByte(satellite_snr, 0, 20);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  satellites_visible
					 ,satellite_prn
					 ,satellite_used
					 ,satellite_elevation
					 ,satellite_azimuth
					 ,satellite_snr
					 );
	}

	public byte[] encode(
						 short v_satellites_visible
						,byte[] v_satellite_prn
						,byte[] v_satellite_used
						,byte[] v_satellite_elevation
						,byte[] v_satellite_azimuth
						,byte[] v_satellite_snr
						)
	{
		// int[] mavLen = {1, 20, 20, 20, 20, 20};
		// int[] javLen = {2, 20, 20, 20, 20, 20};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_satellites_visible);	// Add "satellites_visible" parameter
		sndPacket.putByte(v_satellite_prn,0,20);	// Add "satellite_prn" parameter
		sndPacket.putByte(v_satellite_used,0,20);	// Add "satellite_used" parameter
		sndPacket.putByte(v_satellite_elevation,0,20);	// Add "satellite_elevation" parameter
		sndPacket.putByte(v_satellite_azimuth,0,20);	// Add "satellite_azimuth" parameter
		sndPacket.putByte(v_satellite_snr,0,20);	// Add "satellite_snr" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GPS_STATUS_satellites_visible"
 				+ ", GPS_STATUS_satellite_prn"
 				+ ", GPS_STATUS_satellite_used"
 				+ ", GPS_STATUS_satellite_elevation"
 				+ ", GPS_STATUS_satellite_azimuth"
 				+ ", GPS_STATUS_satellite_snr"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + satellites_visible
 				+ ", " + satellite_prn
 				+ ", " + satellite_used
 				+ ", " + satellite_elevation
 				+ ", " + satellite_azimuth
 				+ ", " + satellite_snr
				);
		return param;
	}
}
