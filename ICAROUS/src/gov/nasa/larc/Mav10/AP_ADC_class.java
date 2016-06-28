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
Message ID: AP_ADC(153)
--------------------------------------
%%~ raw ADC output
--------------------------------------
*/
public class AP_ADC_class //implements Loggable
{
	public static final int msgID = 153;
	public int	 adc1; 	// ADC output 1
	public int	 adc2; 	// ADC output 2
	public int	 adc3; 	// ADC output 3
	public int	 adc4; 	// ADC output 4
	public int	 adc5; 	// ADC output 5
	public int	 adc6; 	// ADC output 6

	private packet rcvPacket;
	private packet sndPacket;

	public AP_ADC_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public AP_ADC_class(AP_ADC_class o)
	{
		adc1 = o.adc1;
		adc2 = o.adc2;
		adc3 = o.adc3;
		adc4 = o.adc4;
		adc5 = o.adc5;
		adc6 = o.adc6;
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

			// int[] mavLen = {2, 2, 2, 2, 2, 2};
			// int[] javLen = {4, 4, 4, 4, 4, 4};

			adc1	= rcvPacket.getIntS();
			adc2	= rcvPacket.getIntS();
			adc3	= rcvPacket.getIntS();
			adc4	= rcvPacket.getIntS();
			adc5	= rcvPacket.getIntS();
			adc6	= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  adc1
					 ,adc2
					 ,adc3
					 ,adc4
					 ,adc5
					 ,adc6
					 );
	}

	public byte[] encode(
						 int v_adc1
						,int v_adc2
						,int v_adc3
						,int v_adc4
						,int v_adc5
						,int v_adc6
						)
	{
		// int[] mavLen = {2, 2, 2, 2, 2, 2};
		// int[] javLen = {4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_adc1);	// Add "adc1" parameter
		sndPacket.putShortI(v_adc2);	// Add "adc2" parameter
		sndPacket.putShortI(v_adc3);	// Add "adc3" parameter
		sndPacket.putShortI(v_adc4);	// Add "adc4" parameter
		sndPacket.putShortI(v_adc5);	// Add "adc5" parameter
		sndPacket.putShortI(v_adc6);	// Add "adc6" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", AP_ADC_adc1"
 				+ ", AP_ADC_adc2"
 				+ ", AP_ADC_adc3"
 				+ ", AP_ADC_adc4"
 				+ ", AP_ADC_adc5"
 				+ ", AP_ADC_adc6"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + adc1
 				+ ", " + adc2
 				+ ", " + adc3
 				+ ", " + adc4
 				+ ", " + adc5
 				+ ", " + adc6
				);
		return param;
	}
}
