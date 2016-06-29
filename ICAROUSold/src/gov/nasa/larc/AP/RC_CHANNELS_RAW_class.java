/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: RC_CHANNELS_RAW(35)
--------------------------------------
%%~ The RAW values of the RC channels received. The standard PPM modulation is as follows: 
%%~ 1000 microseconds: 0%, 2000 microseconds: 100%. Individual receivers/transmitters 
%%~ might violate this specification.
--------------------------------------
*/
public class RC_CHANNELS_RAW_class implements Loggable
{
	public static final int msgID = 35;
	public int		 chan1_raw;	 	// RC channel 1 value, in microseconds
	public int		 chan2_raw;	 	// RC channel 2 value, in microseconds
	public int		 chan3_raw;	 	// RC channel 3 value, in microseconds
	public int		 chan4_raw;	 	// RC channel 4 value, in microseconds
	public int		 chan5_raw;	 	// RC channel 5 value, in microseconds
	public int		 chan6_raw;	 	// RC channel 6 value, in microseconds
	public int		 chan7_raw;	 	// RC channel 7 value, in microseconds
	public int		 chan8_raw;	 	// RC channel 8 value, in microseconds
	public short	 rssi;		 	// Receive signal strength indicator, 0: 0%, 255: 100%

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public RC_CHANNELS_RAW_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public RC_CHANNELS_RAW_class(RC_CHANNELS_RAW_class o)
	{
		chan1_raw = o.chan1_raw;
		chan2_raw = o.chan2_raw;
		chan3_raw = o.chan3_raw;
		chan4_raw = o.chan4_raw;
		chan5_raw = o.chan5_raw;
		chan6_raw = o.chan6_raw;
		chan7_raw = o.chan7_raw;
		chan8_raw = o.chan8_raw;
		rssi = o.rssi;
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

			// int[] mavLen = {2, 2, 2, 2, 2, 2, 2, 2, 1};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 2};

			chan1_raw	= rcvPacket.getIntS();
			chan2_raw	= rcvPacket.getIntS();
			chan3_raw	= rcvPacket.getIntS();
			chan4_raw	= rcvPacket.getIntS();
			chan5_raw	= rcvPacket.getIntS();
			chan6_raw	= rcvPacket.getIntS();
			chan7_raw	= rcvPacket.getIntS();
			chan8_raw	= rcvPacket.getIntS();
			rssi			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  chan1_raw
					 ,chan2_raw
					 ,chan3_raw
					 ,chan4_raw
					 ,chan5_raw
					 ,chan6_raw
					 ,chan7_raw
					 ,chan8_raw
					 ,rssi
					 );
	}

	public byte[] encode(
						 int v_chan1_raw
						,int v_chan2_raw
						,int v_chan3_raw
						,int v_chan4_raw
						,int v_chan5_raw
						,int v_chan6_raw
						,int v_chan7_raw
						,int v_chan8_raw
						,short v_rssi
						)
	{
		// int[] mavLen = {2, 2, 2, 2, 2, 2, 2, 2, 1};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_chan1_raw);	// Add "chan1_raw" parameter
		sndPacket.putShortI(v_chan2_raw);	// Add "chan2_raw" parameter
		sndPacket.putShortI(v_chan3_raw);	// Add "chan3_raw" parameter
		sndPacket.putShortI(v_chan4_raw);	// Add "chan4_raw" parameter
		sndPacket.putShortI(v_chan5_raw);	// Add "chan5_raw" parameter
		sndPacket.putShortI(v_chan6_raw);	// Add "chan6_raw" parameter
		sndPacket.putShortI(v_chan7_raw);	// Add "chan7_raw" parameter
		sndPacket.putShortI(v_chan8_raw);	// Add "chan8_raw" parameter
		sndPacket.putByteS(v_rssi);	// Add "rssi" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", RC_CHANNELS_RAW_chan1_raw"
 				+ ", RC_CHANNELS_RAW_chan2_raw"
 				+ ", RC_CHANNELS_RAW_chan3_raw"
 				+ ", RC_CHANNELS_RAW_chan4_raw"
 				+ ", RC_CHANNELS_RAW_chan5_raw"
 				+ ", RC_CHANNELS_RAW_chan6_raw"
 				+ ", RC_CHANNELS_RAW_chan7_raw"
 				+ ", RC_CHANNELS_RAW_chan8_raw"
 				+ ", RC_CHANNELS_RAW_rssi"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + chan1_raw
 				+ ", " + chan2_raw
 				+ ", " + chan3_raw
 				+ ", " + chan4_raw
 				+ ", " + chan5_raw
 				+ ", " + chan6_raw
 				+ ", " + chan7_raw
 				+ ", " + chan8_raw
 				+ ", " + rssi
				);
		return param;
	}
}
