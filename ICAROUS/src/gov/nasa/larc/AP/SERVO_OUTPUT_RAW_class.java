/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: SERVO_OUTPUT_RAW(37)
--------------------------------------
%%~ The RAW values of the servo outputs (for RC input from the remote, use the RC_CHANNELS 
%%~ messages). The standard PPM modulation is as follows: 1000 microseconds: 
%%~ 0%, 2000 microseconds: 100%.
--------------------------------------
*/
public class SERVO_OUTPUT_RAW_class implements Loggable
{
	public static final int msgID = 37;
	public int	 servo1_raw;	 	// Servo output 1 value, in microseconds
	public int	 servo2_raw;	 	// Servo output 2 value, in microseconds
	public int	 servo3_raw;	 	// Servo output 3 value, in microseconds
	public int	 servo4_raw;	 	// Servo output 4 value, in microseconds
	public int	 servo5_raw;	 	// Servo output 5 value, in microseconds
	public int	 servo6_raw;	 	// Servo output 6 value, in microseconds
	public int	 servo7_raw;	 	// Servo output 7 value, in microseconds
	public int	 servo8_raw;	 	// Servo output 8 value, in microseconds

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public SERVO_OUTPUT_RAW_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public SERVO_OUTPUT_RAW_class(SERVO_OUTPUT_RAW_class o)
	{
		servo1_raw = o.servo1_raw;
		servo2_raw = o.servo2_raw;
		servo3_raw = o.servo3_raw;
		servo4_raw = o.servo4_raw;
		servo5_raw = o.servo5_raw;
		servo6_raw = o.servo6_raw;
		servo7_raw = o.servo7_raw;
		servo8_raw = o.servo8_raw;
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

			// int[] mavLen = {2, 2, 2, 2, 2, 2, 2, 2};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4};

			servo1_raw	= rcvPacket.getIntS();
			servo2_raw	= rcvPacket.getIntS();
			servo3_raw	= rcvPacket.getIntS();
			servo4_raw	= rcvPacket.getIntS();
			servo5_raw	= rcvPacket.getIntS();
			servo6_raw	= rcvPacket.getIntS();
			servo7_raw	= rcvPacket.getIntS();
			servo8_raw	= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  servo1_raw
					 ,servo2_raw
					 ,servo3_raw
					 ,servo4_raw
					 ,servo5_raw
					 ,servo6_raw
					 ,servo7_raw
					 ,servo8_raw
					 );
	}

	public byte[] encode(
						 int v_servo1_raw
						,int v_servo2_raw
						,int v_servo3_raw
						,int v_servo4_raw
						,int v_servo5_raw
						,int v_servo6_raw
						,int v_servo7_raw
						,int v_servo8_raw
						)
	{
		// int[] mavLen = {2, 2, 2, 2, 2, 2, 2, 2};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_servo1_raw);	// Add "servo1_raw" parameter
		sndPacket.putShortI(v_servo2_raw);	// Add "servo2_raw" parameter
		sndPacket.putShortI(v_servo3_raw);	// Add "servo3_raw" parameter
		sndPacket.putShortI(v_servo4_raw);	// Add "servo4_raw" parameter
		sndPacket.putShortI(v_servo5_raw);	// Add "servo5_raw" parameter
		sndPacket.putShortI(v_servo6_raw);	// Add "servo6_raw" parameter
		sndPacket.putShortI(v_servo7_raw);	// Add "servo7_raw" parameter
		sndPacket.putShortI(v_servo8_raw);	// Add "servo8_raw" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SERVO_OUTPUT_RAW_servo1_raw"
 				+ ", SERVO_OUTPUT_RAW_servo2_raw"
 				+ ", SERVO_OUTPUT_RAW_servo3_raw"
 				+ ", SERVO_OUTPUT_RAW_servo4_raw"
 				+ ", SERVO_OUTPUT_RAW_servo5_raw"
 				+ ", SERVO_OUTPUT_RAW_servo6_raw"
 				+ ", SERVO_OUTPUT_RAW_servo7_raw"
 				+ ", SERVO_OUTPUT_RAW_servo8_raw"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + servo1_raw
 				+ ", " + servo2_raw
 				+ ", " + servo3_raw
 				+ ", " + servo4_raw
 				+ ", " + servo5_raw
 				+ ", " + servo6_raw
 				+ ", " + servo7_raw
 				+ ", " + servo8_raw
				);
		return param;
	}
}
