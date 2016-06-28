/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: SAFETY_SET_ALLOWED_AREA(53)
--------------------------------------
%%~ Set a safety zone (volume), which is defined by two corners of a cube. This message 
%%~ can be used to tell the MAV which setpoints/waypoints to accept and which to 
%%~ reject. Safety areas are often enforced by national or competition regulations.
--------------------------------------
*/
public class SAFETY_SET_ALLOWED_AREA_class implements Loggable
{
	public static final int msgID = 53;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public short	 frame;			 	// Coordinate frame, as defined by MAV_FRAME enum in mavlink_types.h. Can be either global, GPS, right-handed with Z axis up or local, right handed, Z axis down.
	public float	 p1x;				 	// x position 1 / Latitude 1
	public float	 p1y;				 	// y position 1 / Longitude 1
	public float	 p1z;				 	// z position 1 / Altitude 1
	public float	 p2x;				 	// x position 2 / Latitude 2
	public float	 p2y;				 	// y position 2 / Longitude 2
	public float	 p2z;				 	// z position 2 / Altitude 2

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public SAFETY_SET_ALLOWED_AREA_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public SAFETY_SET_ALLOWED_AREA_class(SAFETY_SET_ALLOWED_AREA_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
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

			// int[] mavLen = {1, 1, 1, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {2, 2, 2, 4, 4, 4, 4, 4, 4};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			frame			= rcvPacket.getShortB();
			p1x				= rcvPacket.getFloat();
			p1y				= rcvPacket.getFloat();
			p1z				= rcvPacket.getFloat();
			p2x				= rcvPacket.getFloat();
			p2y				= rcvPacket.getFloat();
			p2z				= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,frame
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
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_frame
					 ,v_p1x
					 ,v_p1y
					 ,v_p1z
					 ,v_p2x
					 ,v_p2y
					 ,v_p2z
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,short v_frame
						,float v_p1x
						,float v_p1y
						,float v_p1z
						,float v_p2x
						,float v_p2y
						,float v_p2z
						)
	{
		// int[] mavLen = {1, 1, 1, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {2, 2, 2, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
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
 				+ ", SAFETY_SET_ALLOWED_AREA_target_system"
 				+ ", SAFETY_SET_ALLOWED_AREA_target_component"
 				+ ", SAFETY_SET_ALLOWED_AREA_frame"
 				+ ", SAFETY_SET_ALLOWED_AREA_p1x"
 				+ ", SAFETY_SET_ALLOWED_AREA_p1y"
 				+ ", SAFETY_SET_ALLOWED_AREA_p1z"
 				+ ", SAFETY_SET_ALLOWED_AREA_p2x"
 				+ ", SAFETY_SET_ALLOWED_AREA_p2y"
 				+ ", SAFETY_SET_ALLOWED_AREA_p2z"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
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
