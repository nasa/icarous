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
Message ID: PID_TUNING(194)
--------------------------------------
%%~ PID tuning information
--------------------------------------
*/
public class PID_TUNING_class //implements Loggable
{
	public static final int msgID = 194;
	public float	 desired;	 	// desired rate (degrees/s)
	public float	 achieved; 	// achieved rate (degrees/s)
	public float	 FF;		 	// FF component
	public float	 P;		 	// P component
	public float	 I;		 	// I component
	public float	 D;		 	// D component
	public short	 axis;	 	// axis

	private packet rcvPacket;
	private packet sndPacket;

	public PID_TUNING_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public PID_TUNING_class(PID_TUNING_class o)
	{
		desired = o.desired;
		achieved = o.achieved;
		FF = o.FF;
		P = o.P;
		I = o.I;
		D = o.D;
		axis = o.axis;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 1};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 2};

			desired	= rcvPacket.getFloat();
			achieved	= rcvPacket.getFloat();
			FF		= rcvPacket.getFloat();
			P		= rcvPacket.getFloat();
			I		= rcvPacket.getFloat();
			D		= rcvPacket.getFloat();
			axis		= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  desired
					 ,achieved
					 ,FF
					 ,P
					 ,I
					 ,D
					 ,axis
					 );
	}

	public byte[] encode(
						 float v_desired
						,float v_achieved
						,float v_FF
						,float v_P
						,float v_I
						,float v_D
						,short v_axis
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 1};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_desired);	// Add "desired" parameter
		sndPacket.putFloat(v_achieved);	// Add "achieved" parameter
		sndPacket.putFloat(v_FF);	// Add "FF" parameter
		sndPacket.putFloat(v_P);	// Add "P" parameter
		sndPacket.putFloat(v_I);	// Add "I" parameter
		sndPacket.putFloat(v_D);	// Add "D" parameter
		sndPacket.putByteS(v_axis);	// Add "axis" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", PID_TUNING_desired"
 				+ ", PID_TUNING_achieved"
 				+ ", PID_TUNING_FF"
 				+ ", PID_TUNING_P"
 				+ ", PID_TUNING_I"
 				+ ", PID_TUNING_D"
 				+ ", PID_TUNING_axis"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + desired
 				+ ", " + achieved
 				+ ", " + FF
 				+ ", " + P
 				+ ", " + I
 				+ ", " + D
 				+ ", " + axis
				);
		return param;
	}
}
