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
Message ID: COMPASSMOT_STATUS(177)
--------------------------------------
%%~ Status of compassmot calibration
--------------------------------------
*/
public class COMPASSMOT_STATUS_class //implements Loggable
{
	public static final int msgID = 177;
	public float	 current;			 	// current (amps)
	public float	 CompensationX;	 	// Motor Compensation X
	public float	 CompensationY;	 	// Motor Compensation Y
	public float	 CompensationZ;	 	// Motor Compensation Z
	public int		 throttle;		 	// throttle (percent*10)
	public int		 interference;	 	// interference (percent)

	private packet rcvPacket;
	private packet sndPacket;

	public COMPASSMOT_STATUS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public COMPASSMOT_STATUS_class(COMPASSMOT_STATUS_class o)
	{
		current = o.current;
		CompensationX = o.CompensationX;
		CompensationY = o.CompensationY;
		CompensationZ = o.CompensationZ;
		throttle = o.throttle;
		interference = o.interference;
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

			// int[] mavLen = {4, 4, 4, 4, 2, 2};
			// int[] javLen = {4, 4, 4, 4, 4, 4};

			current			= rcvPacket.getFloat();
			CompensationX	= rcvPacket.getFloat();
			CompensationY	= rcvPacket.getFloat();
			CompensationZ	= rcvPacket.getFloat();
			throttle			= rcvPacket.getIntS();
			interference		= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  current
					 ,CompensationX
					 ,CompensationY
					 ,CompensationZ
					 ,throttle
					 ,interference
					 );
	}

	public byte[] encode(
						 float v_current
						,float v_CompensationX
						,float v_CompensationY
						,float v_CompensationZ
						,int v_throttle
						,int v_interference
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 2, 2};
		// int[] javLen = {4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_current);	// Add "current" parameter
		sndPacket.putFloat(v_CompensationX);	// Add "CompensationX" parameter
		sndPacket.putFloat(v_CompensationY);	// Add "CompensationY" parameter
		sndPacket.putFloat(v_CompensationZ);	// Add "CompensationZ" parameter
		sndPacket.putShortI(v_throttle);	// Add "throttle" parameter
		sndPacket.putShortI(v_interference);	// Add "interference" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", COMPASSMOT_STATUS_current"
 				+ ", COMPASSMOT_STATUS_CompensationX"
 				+ ", COMPASSMOT_STATUS_CompensationY"
 				+ ", COMPASSMOT_STATUS_CompensationZ"
 				+ ", COMPASSMOT_STATUS_throttle"
 				+ ", COMPASSMOT_STATUS_interference"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + current
 				+ ", " + CompensationX
 				+ ", " + CompensationY
 				+ ", " + CompensationZ
 				+ ", " + throttle
 				+ ", " + interference
				);
		return param;
	}
}
