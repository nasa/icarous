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
Message ID: GIMBAL_REPORT_AXIS_CALIBRATION_STATUS(212)
--------------------------------------
%%~        Reports the calibration status for each gimbal axis (whether the axis requires 
%%~ calibration or not)      
--------------------------------------
*/
public class GIMBAL_REPORT_AXIS_CALIBRATION_STATUS_class //implements Loggable
{
	public static final int msgID = 212;
	public short	 yaw_requires_calibration;	 	// Whether or not the yaw axis requires calibration, see GIMBAL_AXIS_CALIBRATION_REQUIRED enumeration
	public short	 pitch_requires_calibration;	 	// Whether or not the pitch axis requires calibration, see GIMBAL_AXIS_CALIBRATION_REQUIRED enumeration
	public short	 roll_requires_calibration;	 	// Whether or not the roll axis requires calibration, see GIMBAL_AXIS_CALIBRATION_REQUIRED enumeration

	private packet rcvPacket;
	private packet sndPacket;

	public GIMBAL_REPORT_AXIS_CALIBRATION_STATUS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public GIMBAL_REPORT_AXIS_CALIBRATION_STATUS_class(GIMBAL_REPORT_AXIS_CALIBRATION_STATUS_class o)
	{
		yaw_requires_calibration = o.yaw_requires_calibration;
		pitch_requires_calibration = o.pitch_requires_calibration;
		roll_requires_calibration = o.roll_requires_calibration;
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

			// int[] mavLen = {1, 1, 1};
			// int[] javLen = {2, 2, 2};

			yaw_requires_calibration		= rcvPacket.getShortB();
			pitch_requires_calibration	= rcvPacket.getShortB();
			roll_requires_calibration	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  yaw_requires_calibration
					 ,pitch_requires_calibration
					 ,roll_requires_calibration
					 );
	}

	public byte[] encode(
						 short v_yaw_requires_calibration
						,short v_pitch_requires_calibration
						,short v_roll_requires_calibration
						)
	{
		// int[] mavLen = {1, 1, 1};
		// int[] javLen = {2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_yaw_requires_calibration);	// Add "yaw_requires_calibration" parameter
		sndPacket.putByteS(v_pitch_requires_calibration);	// Add "pitch_requires_calibration" parameter
		sndPacket.putByteS(v_roll_requires_calibration);	// Add "roll_requires_calibration" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GIMBAL_REPORT_AXIS_CALIBRATION_STATUS_yaw_requires_calibration"
 				+ ", GIMBAL_REPORT_AXIS_CALIBRATION_STATUS_pitch_requires_calibration"
 				+ ", GIMBAL_REPORT_AXIS_CALIBRATION_STATUS_roll_requires_calibration"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + yaw_requires_calibration
 				+ ", " + pitch_requires_calibration
 				+ ", " + roll_requires_calibration
				);
		return param;
	}
}
