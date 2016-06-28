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
Message ID: GIMBAL_HOME_OFFSET_CALIBRATION_RESULT(205)
--------------------------------------
%%~              Sent by the gimbal after it receives a SET_HOME_OFFSETS message to 
%%~ indicate the result of the home offset calibration         
--------------------------------------
*/
public class GIMBAL_HOME_OFFSET_CALIBRATION_RESULT_class //implements Loggable
{
	public static final int msgID = 205;
	public short	 calibration_result;	 	// The result of the home offset calibration

	private packet rcvPacket;
	private packet sndPacket;

	public GIMBAL_HOME_OFFSET_CALIBRATION_RESULT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public GIMBAL_HOME_OFFSET_CALIBRATION_RESULT_class(GIMBAL_HOME_OFFSET_CALIBRATION_RESULT_class o)
	{
		calibration_result = o.calibration_result;
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

			// int[] mavLen = {1};
			// int[] javLen = {2};

			calibration_result	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  calibration_result
					 );
	}

	public byte[] encode(
						 short v_calibration_result
						)
	{
		// int[] mavLen = {1};
		// int[] javLen = {2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_calibration_result);	// Add "calibration_result" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GIMBAL_HOME_OFFSET_CALIBRATION_RESULT_calibration_result"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + calibration_result
				);
		return param;
	}
}
