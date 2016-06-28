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
Message ID: GIMBAL_AXIS_CALIBRATION_PROGRESS(203)
--------------------------------------
%%~              Reports progress and success or failure of gimbal axis calibration 
%%~ procedure         
--------------------------------------
*/
public class GIMBAL_AXIS_CALIBRATION_PROGRESS_class //implements Loggable
{
	public static final int msgID = 203;
	public short	 calibration_axis;	 	// Which gimbal axis we're reporting calibration progress for
	public short	 calibration_progress; 	// The current calibration progress for this axis, 0x64=100%
	public short	 calibration_status;	 	// The status of the running calibration

	private packet rcvPacket;
	private packet sndPacket;

	public GIMBAL_AXIS_CALIBRATION_PROGRESS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public GIMBAL_AXIS_CALIBRATION_PROGRESS_class(GIMBAL_AXIS_CALIBRATION_PROGRESS_class o)
	{
		calibration_axis = o.calibration_axis;
		calibration_progress = o.calibration_progress;
		calibration_status = o.calibration_status;
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

			calibration_axis		= rcvPacket.getShortB();
			calibration_progress	= rcvPacket.getShortB();
			calibration_status	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  calibration_axis
					 ,calibration_progress
					 ,calibration_status
					 );
	}

	public byte[] encode(
						 short v_calibration_axis
						,short v_calibration_progress
						,short v_calibration_status
						)
	{
		// int[] mavLen = {1, 1, 1};
		// int[] javLen = {2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_calibration_axis);	// Add "calibration_axis" parameter
		sndPacket.putByteS(v_calibration_progress);	// Add "calibration_progress" parameter
		sndPacket.putByteS(v_calibration_status);	// Add "calibration_status" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GIMBAL_AXIS_CALIBRATION_PROGRESS_calibration_axis"
 				+ ", GIMBAL_AXIS_CALIBRATION_PROGRESS_calibration_progress"
 				+ ", GIMBAL_AXIS_CALIBRATION_PROGRESS_calibration_status"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + calibration_axis
 				+ ", " + calibration_progress
 				+ ", " + calibration_status
				);
		return param;
	}
}
