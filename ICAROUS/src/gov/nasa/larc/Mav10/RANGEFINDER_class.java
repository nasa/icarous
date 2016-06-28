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
Message ID: RANGEFINDER(173)
--------------------------------------
%%~ Rangefinder reporting
--------------------------------------
*/
public class RANGEFINDER_class //implements Loggable
{
	public static final int msgID = 173;
	public float	 distance; 	// distance in meters
	public float	 voltage;	 	// raw voltage if available, zero otherwise

	private packet rcvPacket;
	private packet sndPacket;

	public RANGEFINDER_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public RANGEFINDER_class(RANGEFINDER_class o)
	{
		distance = o.distance;
		voltage = o.voltage;
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

			// int[] mavLen = {4, 4};
			// int[] javLen = {4, 4};

			distance	= rcvPacket.getFloat();
			voltage	= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  distance
					 ,voltage
					 );
	}

	public byte[] encode(
						 float v_distance
						,float v_voltage
						)
	{
		// int[] mavLen = {4, 4};
		// int[] javLen = {4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_distance);	// Add "distance" parameter
		sndPacket.putFloat(v_voltage);	// Add "voltage" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", RANGEFINDER_distance"
 				+ ", RANGEFINDER_voltage"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + distance
 				+ ", " + voltage
				);
		return param;
	}
}
