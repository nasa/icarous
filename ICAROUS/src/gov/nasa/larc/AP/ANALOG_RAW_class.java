/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: ANALOG_RAW(111)
--------------------------------------
%%~ analog channels raw (counts from ADC 0-1024)
--------------------------------------
*/
public class ANALOG_RAW_class implements Loggable
{
	public static final int msgID = 111;
	public int	 chan01;	 	// adc channel_01
	public int	 chan02;	 	// adc channel_02
	public int	 chan03;	 	// adc channel_03
	public int	 chan04;	 	// adc channel_04
	public int	 chan05;	 	// adc channel_05
	public int	 chan06;	 	// adc channel_06
	public int	 chan07;	 	// adc channel_07
	public int	 chan08;	 	// adc channel_08
	public int	 chan09;	 	// adc channel_09
	public int	 chan10;	 	// adc channel_10
	public int	 chan11;	 	// adc channel_11
	public int	 chan12;	 	// adc channel_12
	public int	 chan13;	 	// adc channel_13
	public int	 chan14;	 	// adc channel_14
	public int	 chan15;	 	// adc channel_15
	public int	 chan16;	 	// adc channel_16
	public long usec;	 	// time

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public ANALOG_RAW_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public ANALOG_RAW_class(ANALOG_RAW_class o)
	{
		chan01 = o.chan01;
		chan02 = o.chan02;
		chan03 = o.chan03;
		chan04 = o.chan04;
		chan05 = o.chan05;
		chan06 = o.chan06;
		chan07 = o.chan07;
		chan08 = o.chan08;
		chan09 = o.chan09;
		chan10 = o.chan10;
		chan11 = o.chan11;
		chan12 = o.chan12;
		chan13 = o.chan13;
		chan14 = o.chan14;
		chan15 = o.chan15;
		chan16 = o.chan16;
		usec = o.usec;
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

			// int[] mavLen = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8};

			chan01	= rcvPacket.getIntS();
			chan02	= rcvPacket.getIntS();
			chan03	= rcvPacket.getIntS();
			chan04	= rcvPacket.getIntS();
			chan05	= rcvPacket.getIntS();
			chan06	= rcvPacket.getIntS();
			chan07	= rcvPacket.getIntS();
			chan08	= rcvPacket.getIntS();
			chan09	= rcvPacket.getIntS();
			chan10	= rcvPacket.getIntS();
			chan11	= rcvPacket.getIntS();
			chan12	= rcvPacket.getIntS();
			chan13	= rcvPacket.getIntS();
			chan14	= rcvPacket.getIntS();
			chan15	= rcvPacket.getIntS();
			chan16	= rcvPacket.getIntS();
			usec		= rcvPacket.getLong();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  chan01
					 ,chan02
					 ,chan03
					 ,chan04
					 ,chan05
					 ,chan06
					 ,chan07
					 ,chan08
					 ,chan09
					 ,chan10
					 ,chan11
					 ,chan12
					 ,chan13
					 ,chan14
					 ,chan15
					 ,chan16
					 ,usec
					 );
	}

	public byte[] encode(
						 int v_chan01
						,int v_chan02
						,int v_chan03
						,int v_chan04
						,int v_chan05
						,int v_chan06
						,int v_chan07
						,int v_chan08
						,int v_chan09
						,int v_chan10
						,int v_chan11
						,int v_chan12
						,int v_chan13
						,int v_chan14
						,int v_chan15
						,int v_chan16
						,long v_usec
						)
	{
		// int[] mavLen = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_chan01);	// Add "chan01" parameter
		sndPacket.putShortI(v_chan02);	// Add "chan02" parameter
		sndPacket.putShortI(v_chan03);	// Add "chan03" parameter
		sndPacket.putShortI(v_chan04);	// Add "chan04" parameter
		sndPacket.putShortI(v_chan05);	// Add "chan05" parameter
		sndPacket.putShortI(v_chan06);	// Add "chan06" parameter
		sndPacket.putShortI(v_chan07);	// Add "chan07" parameter
		sndPacket.putShortI(v_chan08);	// Add "chan08" parameter
		sndPacket.putShortI(v_chan09);	// Add "chan09" parameter
		sndPacket.putShortI(v_chan10);	// Add "chan10" parameter
		sndPacket.putShortI(v_chan11);	// Add "chan11" parameter
		sndPacket.putShortI(v_chan12);	// Add "chan12" parameter
		sndPacket.putShortI(v_chan13);	// Add "chan13" parameter
		sndPacket.putShortI(v_chan14);	// Add "chan14" parameter
		sndPacket.putShortI(v_chan15);	// Add "chan15" parameter
		sndPacket.putShortI(v_chan16);	// Add "chan16" parameter
		sndPacket.putLong(v_usec);	// Add "usec" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", ANALOG_RAW_chan01"
 				+ ", ANALOG_RAW_chan02"
 				+ ", ANALOG_RAW_chan03"
 				+ ", ANALOG_RAW_chan04"
 				+ ", ANALOG_RAW_chan05"
 				+ ", ANALOG_RAW_chan06"
 				+ ", ANALOG_RAW_chan07"
 				+ ", ANALOG_RAW_chan08"
 				+ ", ANALOG_RAW_chan09"
 				+ ", ANALOG_RAW_chan10"
 				+ ", ANALOG_RAW_chan11"
 				+ ", ANALOG_RAW_chan12"
 				+ ", ANALOG_RAW_chan13"
 				+ ", ANALOG_RAW_chan14"
 				+ ", ANALOG_RAW_chan15"
 				+ ", ANALOG_RAW_chan16"
 				+ ", ANALOG_RAW_usec"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + chan01
 				+ ", " + chan02
 				+ ", " + chan03
 				+ ", " + chan04
 				+ ", " + chan05
 				+ ", " + chan06
 				+ ", " + chan07
 				+ ", " + chan08
 				+ ", " + chan09
 				+ ", " + chan10
 				+ ", " + chan11
 				+ ", " + chan12
 				+ ", " + chan13
 				+ ", " + chan14
 				+ ", " + chan15
 				+ ", " + chan16
 				+ ", " + usec
				);
		return param;
	}
}
