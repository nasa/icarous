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
Message ID: WIND(168)
--------------------------------------
%%~ Wind estimation
--------------------------------------
*/
public class WIND_class //implements Loggable
{
	public static final int msgID = 168;
	public float	 direction;	 	// wind direction that wind is coming from (degrees)
	public float	 speed;		 	// wind speed in ground plane (m/s)
	public float	 speed_z;		 	// vertical wind speed (m/s)

	private packet rcvPacket;
	private packet sndPacket;

	public WIND_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public WIND_class(WIND_class o)
	{
		direction = o.direction;
		speed = o.speed;
		speed_z = o.speed_z;
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

			// int[] mavLen = {4, 4, 4};
			// int[] javLen = {4, 4, 4};

			direction	= rcvPacket.getFloat();
			speed		= rcvPacket.getFloat();
			speed_z		= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  direction
					 ,speed
					 ,speed_z
					 );
	}

	public byte[] encode(
						 float v_direction
						,float v_speed
						,float v_speed_z
						)
	{
		// int[] mavLen = {4, 4, 4};
		// int[] javLen = {4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_direction);	// Add "direction" parameter
		sndPacket.putFloat(v_speed);	// Add "speed" parameter
		sndPacket.putFloat(v_speed_z);	// Add "speed_z" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", WIND_direction"
 				+ ", WIND_speed"
 				+ ", WIND_speed_z"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + direction
 				+ ", " + speed
 				+ ", " + speed_z
				);
		return param;
	}
}
