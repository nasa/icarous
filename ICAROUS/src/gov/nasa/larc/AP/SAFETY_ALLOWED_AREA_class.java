/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: SAFETY_ALLOWED_AREA(54)
--------------------------------------
%%~ Read out the safety zone the MAV currently assumes.
--------------------------------------
*/
public class SAFETY_ALLOWED_AREA_class implements Loggable
{
	public static final int msgID = 54;
	public short	 frame;	 	// Coordinate frame, as defined by MAV_FRAME enum in mavlink_types.h. Can be either global, GPS, right-handed with Z axis up or local, right handed, Z axis down.
	public float	 p1x;		 	// x position 1 / Latitude 1
	public float	 p1y;		 	// y position 1 / Longitude 1
	public float	 p1z;		 	// z position 1 / Altitude 1
	public float	 p2x;		 	// x position 2 / Latitude 2
	public float	 p2y;		 	// y position 2 / Longitude 2
	public float	 p2z;		 	// z position 2 / Altitude 2

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public SAFETY_ALLOWED_AREA_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public SAFETY_ALLOWED_AREA_class(SAFETY_ALLOWED_AREA_class o)
	{
		frame = o.frame;
		p1x = o.p1x;
		p1y = o.p1y;
		p1z = o.p1z;
		p2x = o.p2x;
		p2y = o.p2y;
		p2z = o.p2z;
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
			// int[] javLen = {2, 4, 4, 4, 4, 4, 4};

			frame	= rcvPacket.getShortB();
			p1x		= rcvPacket.getFloat();
			p1y		= rcvPacket.getFloat();
			p1z		= rcvPacket.getFloat();
			p2x		= rcvPacket.getFloat();
			p2y		= rcvPacket.getFloat();
			p2z		= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  frame
					 ,p1x
					 ,p1y
					 ,p1z
					 ,p2x
					 ,p2y
					 ,p2z
					 );
	}

	public byte[] encode(
						 short v_frame
						,float v_p1x
						,float v_p1y
						,float v_p1z
						,float v_p2x
						,float v_p2y
						,float v_p2z
						)
	{
		// int[] mavLen = {1, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {2, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_frame);	// Add "frame" parameter
		sndPacket.putFloat(v_p1x);	// Add "p1x" parameter
		sndPacket.putFloat(v_p1y);	// Add "p1y" parameter
		sndPacket.putFloat(v_p1z);	// Add "p1z" parameter
		sndPacket.putFloat(v_p2x);	// Add "p2x" parameter
		sndPacket.putFloat(v_p2y);	// Add "p2y" parameter
		sndPacket.putFloat(v_p2z);	// Add "p2z" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SAFETY_ALLOWED_AREA_frame"
 				+ ", SAFETY_ALLOWED_AREA_p1x"
 				+ ", SAFETY_ALLOWED_AREA_p1y"
 				+ ", SAFETY_ALLOWED_AREA_p1z"
 				+ ", SAFETY_ALLOWED_AREA_p2x"
 				+ ", SAFETY_ALLOWED_AREA_p2y"
 				+ ", SAFETY_ALLOWED_AREA_p2z"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + frame
 				+ ", " + p1x
 				+ ", " + p1y
 				+ ", " + p1z
 				+ ", " + p2x
 				+ ", " + p2y
 				+ ", " + p2z
				);
		return param;
	}
}
