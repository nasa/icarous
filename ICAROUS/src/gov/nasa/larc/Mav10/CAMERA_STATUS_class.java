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
Message ID: CAMERA_STATUS(179)
--------------------------------------
%%~ Camera Event
--------------------------------------
*/
public class CAMERA_STATUS_class //implements Loggable
{
	public static final int msgID = 179;
	public long	 time_usec;		 	// Image timestamp (microseconds since UNIX epoch, according to camera clock)
	public float	 p1;				 	// Parameter 1 (meaning depends on event, see CAMERA_STATUS_TYPES enum)
	public float	 p2;				 	// Parameter 2 (meaning depends on event, see CAMERA_STATUS_TYPES enum)
	public float	 p3;				 	// Parameter 3 (meaning depends on event, see CAMERA_STATUS_TYPES enum)
	public float	 p4;				 	// Parameter 4 (meaning depends on event, see CAMERA_STATUS_TYPES enum)
	public int		 img_idx;			 	// Image index
	public short	 target_system;	 	// System ID
	public short	 cam_idx;			 	// Camera ID
	public short	 event_id;		 	// See CAMERA_STATUS_TYPES enum for definition of the bitmask

	private packet rcvPacket;
	private packet sndPacket;

	public CAMERA_STATUS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
	}

	public CAMERA_STATUS_class(CAMERA_STATUS_class o)
	{
		time_usec = o.time_usec;
		p1 = o.p1;
		p2 = o.p2;
		p3 = o.p3;
		p4 = o.p4;
		img_idx = o.img_idx;
		target_system = o.target_system;
		cam_idx = o.cam_idx;
		event_id = o.event_id;
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

			// int[] mavLen = {8, 4, 4, 4, 4, 2, 1, 1, 1};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 2, 2, 2};

			time_usec		= rcvPacket.getLong();
			p1				= rcvPacket.getFloat();
			p2				= rcvPacket.getFloat();
			p3				= rcvPacket.getFloat();
			p4				= rcvPacket.getFloat();
			img_idx			= rcvPacket.getIntS();
			target_system	= rcvPacket.getShortB();
			cam_idx			= rcvPacket.getShortB();
			event_id			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_usec
					 ,p1
					 ,p2
					 ,p3
					 ,p4
					 ,img_idx
					 ,(short)1
					 ,cam_idx
					 ,event_id
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,float v_p1
						,float v_p2
						,float v_p3
						,float v_p4
						,int v_img_idx
						,short v_cam_idx
						,short v_event_id
						)
	{
		return encode(
					  v_time_usec
					 ,v_p1
					 ,v_p2
					 ,v_p3
					 ,v_p4
					 ,v_img_idx
					 ,  (short)1
					 ,v_cam_idx
					 ,v_event_id
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,float v_p1
						,float v_p2
						,float v_p3
						,float v_p4
						,int v_img_idx
						,short v_target_system
						,short v_cam_idx
						,short v_event_id
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 4, 2, 1, 1, 1};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
		sndPacket.putFloat(v_p1);	// Add "p1" parameter
		sndPacket.putFloat(v_p2);	// Add "p2" parameter
		sndPacket.putFloat(v_p3);	// Add "p3" parameter
		sndPacket.putFloat(v_p4);	// Add "p4" parameter
		sndPacket.putShortI(v_img_idx);	// Add "img_idx" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_cam_idx);	// Add "cam_idx" parameter
		sndPacket.putByteS(v_event_id);	// Add "event_id" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", CAMERA_STATUS_time_usec"
 				+ ", CAMERA_STATUS_p1"
 				+ ", CAMERA_STATUS_p2"
 				+ ", CAMERA_STATUS_p3"
 				+ ", CAMERA_STATUS_p4"
 				+ ", CAMERA_STATUS_img_idx"
 				+ ", CAMERA_STATUS_target_system"
 				+ ", CAMERA_STATUS_cam_idx"
 				+ ", CAMERA_STATUS_event_id"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
 				+ ", " + p1
 				+ ", " + p2
 				+ ", " + p3
 				+ ", " + p4
 				+ ", " + img_idx
 				+ ", " + target_system
 				+ ", " + cam_idx
 				+ ", " + event_id
				);
		return param;
	}
}
