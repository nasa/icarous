/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: OBJECT_DETECTION_EVENT(140)
--------------------------------------
%%~ Object has been detected
--------------------------------------
*/
public class OBJECT_DETECTION_EVENT_class implements Loggable
{
	public static final int msgID = 140;
	public long	 time;		 	// Timestamp in milliseconds since system boot
	public int		 object_id;	 	// Object ID
	public short	 type;		 	// Object type: 0: image, 1: letter, 2: ground vehicle, 3: air vehicle, 4: surface vehicle, 5: sub-surface vehicle, 6: human, 7: animal
	public byte[]	 name = new byte[20];			// Name of the object as defined by the detector
	public short	 quality;		 	// Detection quality / confidence. 0: bad, 255: maximum confidence
	public float	 bearing;		 	// Angle of the object with respect to the body frame in NED coordinates in radians. 0: front
	public float	 distance;	 	// Ground distance in meters

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public OBJECT_DETECTION_EVENT_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public OBJECT_DETECTION_EVENT_class(OBJECT_DETECTION_EVENT_class o)
	{
		time = o.time;
		object_id = o.object_id;
		type = o.type;
		name = o.name;
		quality = o.quality;
		bearing = o.bearing;
		distance = o.distance;
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

			// int[] mavLen = {4, 2, 1, 20, 1, 4, 4};
			// int[] javLen = {8, 4, 2, 20, 2, 4, 4};

			time			= rcvPacket.getLongI();
			object_id	= rcvPacket.getIntS();
			type			= rcvPacket.getShortB();
			rcvPacket.getByte(name, 0, 20);
			quality		= rcvPacket.getShortB();
			bearing		= rcvPacket.getFloat();
			distance		= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time
					 ,object_id
					 ,type
					 ,name
					 ,quality
					 ,bearing
					 ,distance
					 );
	}

	public byte[] encode(
						 long v_time
						,int v_object_id
						,short v_type
						,byte[] v_name
						,short v_quality
						,float v_bearing
						,float v_distance
						)
	{
		// int[] mavLen = {4, 2, 1, 20, 1, 4, 4};
		// int[] javLen = {8, 4, 2, 20, 2, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time);	// Add "time" parameter
		sndPacket.putShortI(v_object_id);	// Add "object_id" parameter
		sndPacket.putByteS(v_type);	// Add "type" parameter
		sndPacket.putByte(v_name,0,20);	// Add "name" parameter
		sndPacket.putByteS(v_quality);	// Add "quality" parameter
		sndPacket.putFloat(v_bearing);	// Add "bearing" parameter
		sndPacket.putFloat(v_distance);	// Add "distance" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", OBJECT_DETECTION_EVENT_time"
 				+ ", OBJECT_DETECTION_EVENT_object_id"
 				+ ", OBJECT_DETECTION_EVENT_type"
 				+ ", OBJECT_DETECTION_EVENT_name"
 				+ ", OBJECT_DETECTION_EVENT_quality"
 				+ ", OBJECT_DETECTION_EVENT_bearing"
 				+ ", OBJECT_DETECTION_EVENT_distance"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time
 				+ ", " + object_id
 				+ ", " + type
 				+ ", " + name
 				+ ", " + quality
 				+ ", " + bearing
 				+ ", " + distance
				);
		return param;
	}
}
