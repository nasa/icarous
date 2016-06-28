/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: ANALOG_EU(112)
--------------------------------------
%%~ analog channels EU (Raw counts converted to engineering units, e.g. feet, meters, 
%%~ Amps, degrees, etc)
--------------------------------------
*/
public class ANALOG_EU_class implements Loggable
{
	public static final int msgID = 112;
	public float	 chan01;	 	// adc channel_01
	public float	 chan02;	 	// adc channel_02
	public float	 chan03;	 	// adc channel_03
	public float	 chan04;	 	// adc channel_04
	public float	 chan05;	 	// adc channel_05
	public float	 chan06;	 	// adc channel_06
	public float	 chan07;	 	// adc channel_07
	public float	 chan08;	 	// adc channel_08
	public float	 chan09;	 	// adc channel_09
	public float	 chan10;	 	// adc channel_10
	public float	 chan11;	 	// adc channel_11
	public float	 chan12;	 	// adc channel_12
	public float	 chan13;	 	// adc channel_13
	public float	 chan14;	 	// adc channel_14
	public float	 chan15;	 	// adc channel_15
	public float	 chan16;	 	// adc channel_16
	public long	 usec;	 	// time

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public ANALOG_EU_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public ANALOG_EU_class(ANALOG_EU_class o)
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8};

			chan01	= rcvPacket.getFloat();
			chan02	= rcvPacket.getFloat();
			chan03	= rcvPacket.getFloat();
			chan04	= rcvPacket.getFloat();
			chan05	= rcvPacket.getFloat();
			chan06	= rcvPacket.getFloat();
			chan07	= rcvPacket.getFloat();
			chan08	= rcvPacket.getFloat();
			chan09	= rcvPacket.getFloat();
			chan10	= rcvPacket.getFloat();
			chan11	= rcvPacket.getFloat();
			chan12	= rcvPacket.getFloat();
			chan13	= rcvPacket.getFloat();
			chan14	= rcvPacket.getFloat();
			chan15	= rcvPacket.getFloat();
			chan16	= rcvPacket.getFloat();
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
						 float v_chan01
						,float v_chan02
						,float v_chan03
						,float v_chan04
						,float v_chan05
						,float v_chan06
						,float v_chan07
						,float v_chan08
						,float v_chan09
						,float v_chan10
						,float v_chan11
						,float v_chan12
						,float v_chan13
						,float v_chan14
						,float v_chan15
						,float v_chan16
						,long v_usec
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_chan01);	// Add "chan01" parameter
		sndPacket.putFloat(v_chan02);	// Add "chan02" parameter
		sndPacket.putFloat(v_chan03);	// Add "chan03" parameter
		sndPacket.putFloat(v_chan04);	// Add "chan04" parameter
		sndPacket.putFloat(v_chan05);	// Add "chan05" parameter
		sndPacket.putFloat(v_chan06);	// Add "chan06" parameter
		sndPacket.putFloat(v_chan07);	// Add "chan07" parameter
		sndPacket.putFloat(v_chan08);	// Add "chan08" parameter
		sndPacket.putFloat(v_chan09);	// Add "chan09" parameter
		sndPacket.putFloat(v_chan10);	// Add "chan10" parameter
		sndPacket.putFloat(v_chan11);	// Add "chan11" parameter
		sndPacket.putFloat(v_chan12);	// Add "chan12" parameter
		sndPacket.putFloat(v_chan13);	// Add "chan13" parameter
		sndPacket.putFloat(v_chan14);	// Add "chan14" parameter
		sndPacket.putFloat(v_chan15);	// Add "chan15" parameter
		sndPacket.putFloat(v_chan16);	// Add "chan16" parameter
		sndPacket.putLong(v_usec);	// Add "usec" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", ANALOG_EU_chan01"
 				+ ", ANALOG_EU_chan02"
 				+ ", ANALOG_EU_chan03"
 				+ ", ANALOG_EU_chan04"
 				+ ", ANALOG_EU_chan05"
 				+ ", ANALOG_EU_chan06"
 				+ ", ANALOG_EU_chan07"
 				+ ", ANALOG_EU_chan08"
 				+ ", ANALOG_EU_chan09"
 				+ ", ANALOG_EU_chan10"
 				+ ", ANALOG_EU_chan11"
 				+ ", ANALOG_EU_chan12"
 				+ ", ANALOG_EU_chan13"
 				+ ", ANALOG_EU_chan14"
 				+ ", ANALOG_EU_chan15"
 				+ ", ANALOG_EU_chan16"
 				+ ", ANALOG_EU_usec"
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
