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
Message ID: CAMERA_FEEDBACK(180)
--------------------------------------
%%~ Camera Capture Feedback
--------------------------------------
*/
public class CAMERA_FEEDBACK_class //implements Loggable
{
	public static final int msgID = 180;
	public long	 time_usec;		 	// Image timestamp (microseconds since UNIX epoch), as passed in by CAMERA_STATUS message (or autopilot if no CCB)
	public int		 lat;				 	// Latitude in (deg * 1E7)
	public int		 lng;				 	// Longitude in (deg * 1E7)
	public float	 alt_msl;			 	// Altitude Absolute (meters AMSL)
	public float	 alt_rel;			 	// Altitude Relative (meters above HOME location)
	public float	 roll;			 	// Camera Roll angle (earth frame, degrees, +-180)
	public float	 pitch;			 	// Camera Pitch angle (earth frame, degrees, +-180)
	public float	 yaw;				 	// Camera Yaw (earth frame, degrees, 0-360, true)
	public float	 foc_len;			 	// Focal Length (mm)
	public int		 img_idx;			 	// Image index
	public short	 target_system;	 	// System ID
	public short	 cam_idx;			 	// Camera ID
	public short	 flags;			 	// See CAMERA_FEEDBACK_FLAGS enum for definition of the bitmask

	private packet rcvPacket;
	private packet sndPacket;

	public CAMERA_FEEDBACK_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
	}

	public CAMERA_FEEDBACK_class(CAMERA_FEEDBACK_class o)
	{
		time_usec = o.time_usec;
		lat = o.lat;
		lng = o.lng;
		alt_msl = o.alt_msl;
		alt_rel = o.alt_rel;
		roll = o.roll;
		pitch = o.pitch;
		yaw = o.yaw;
		foc_len = o.foc_len;
		img_idx = o.img_idx;
		target_system = o.target_system;
		cam_idx = o.cam_idx;
		flags = o.flags;
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

			// int[] mavLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 2, 1, 1, 1};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2};

			time_usec		= rcvPacket.getLong();
			lat				= rcvPacket.getInt();
			lng				= rcvPacket.getInt();
			alt_msl			= rcvPacket.getFloat();
			alt_rel			= rcvPacket.getFloat();
			roll				= rcvPacket.getFloat();
			pitch			= rcvPacket.getFloat();
			yaw				= rcvPacket.getFloat();
			foc_len			= rcvPacket.getFloat();
			img_idx			= rcvPacket.getIntS();
			target_system	= rcvPacket.getShortB();
			cam_idx			= rcvPacket.getShortB();
			flags			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_usec
					 ,lat
					 ,lng
					 ,alt_msl
					 ,alt_rel
					 ,roll
					 ,pitch
					 ,yaw
					 ,foc_len
					 ,img_idx
					 ,(short)1
					 ,cam_idx
					 ,flags
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,int v_lat
						,int v_lng
						,float v_alt_msl
						,float v_alt_rel
						,float v_roll
						,float v_pitch
						,float v_yaw
						,float v_foc_len
						,int v_img_idx
						,short v_cam_idx
						,short v_flags
						)
	{
		return encode(
					  v_time_usec
					 ,v_lat
					 ,v_lng
					 ,v_alt_msl
					 ,v_alt_rel
					 ,v_roll
					 ,v_pitch
					 ,v_yaw
					 ,v_foc_len
					 ,v_img_idx
					 ,  (short)1
					 ,v_cam_idx
					 ,v_flags
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,int v_lat
						,int v_lng
						,float v_alt_msl
						,float v_alt_rel
						,float v_roll
						,float v_pitch
						,float v_yaw
						,float v_foc_len
						,int v_img_idx
						,short v_target_system
						,short v_cam_idx
						,short v_flags
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 2, 1, 1, 1};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
		sndPacket.putInt(v_lat);	// Add "lat" parameter
		sndPacket.putInt(v_lng);	// Add "lng" parameter
		sndPacket.putFloat(v_alt_msl);	// Add "alt_msl" parameter
		sndPacket.putFloat(v_alt_rel);	// Add "alt_rel" parameter
		sndPacket.putFloat(v_roll);	// Add "roll" parameter
		sndPacket.putFloat(v_pitch);	// Add "pitch" parameter
		sndPacket.putFloat(v_yaw);	// Add "yaw" parameter
		sndPacket.putFloat(v_foc_len);	// Add "foc_len" parameter
		sndPacket.putShortI(v_img_idx);	// Add "img_idx" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_cam_idx);	// Add "cam_idx" parameter
		sndPacket.putByteS(v_flags);	// Add "flags" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", CAMERA_FEEDBACK_time_usec"
 				+ ", CAMERA_FEEDBACK_lat"
 				+ ", CAMERA_FEEDBACK_lng"
 				+ ", CAMERA_FEEDBACK_alt_msl"
 				+ ", CAMERA_FEEDBACK_alt_rel"
 				+ ", CAMERA_FEEDBACK_roll"
 				+ ", CAMERA_FEEDBACK_pitch"
 				+ ", CAMERA_FEEDBACK_yaw"
 				+ ", CAMERA_FEEDBACK_foc_len"
 				+ ", CAMERA_FEEDBACK_img_idx"
 				+ ", CAMERA_FEEDBACK_target_system"
 				+ ", CAMERA_FEEDBACK_cam_idx"
 				+ ", CAMERA_FEEDBACK_flags"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
 				+ ", " + lat
 				+ ", " + lng
 				+ ", " + alt_msl
 				+ ", " + alt_rel
 				+ ", " + roll
 				+ ", " + pitch
 				+ ", " + yaw
 				+ ", " + foc_len
 				+ ", " + img_idx
 				+ ", " + target_system
 				+ ", " + cam_idx
 				+ ", " + flags
				);
		return param;
	}
}
