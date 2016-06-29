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
Message ID: RC_CHANNELS(65)
--------------------------------------
%%~ The PPM values of the RC channels received. The standard PPM modulation is as follows: 
%%~ 1000 microseconds: 0%, 2000 microseconds: 100%. Individual receivers/transmitters 
%%~ might violate this specification.
--------------------------------------
*/
public class RC_CHANNELS_class //implements Loggable
{
	public static final int msgID = 65;
	public long	 time_boot_ms; 	// Timestamp (milliseconds since system boot)
	public int		 chan1_raw;	 	// RC channel 1 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan2_raw;	 	// RC channel 2 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan3_raw;	 	// RC channel 3 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan4_raw;	 	// RC channel 4 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan5_raw;	 	// RC channel 5 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan6_raw;	 	// RC channel 6 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan7_raw;	 	// RC channel 7 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan8_raw;	 	// RC channel 8 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan9_raw;	 	// RC channel 9 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan10_raw;	 	// RC channel 10 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan11_raw;	 	// RC channel 11 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan12_raw;	 	// RC channel 12 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan13_raw;	 	// RC channel 13 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan14_raw;	 	// RC channel 14 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan15_raw;	 	// RC channel 15 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan16_raw;	 	// RC channel 16 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan17_raw;	 	// RC channel 17 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan18_raw;	 	// RC channel 18 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public short	 chancount;	 	// Total number of RC channels being received. This can be larger than 18, indicating that more channels are available but not given in this message. This value should be 0 when no RC channels are available.
	public short	 rssi;		 	// Receive signal strength indicator, 0: 0%, 100: 100%, 255: invalid/unknown.

	private packet rcvPacket;
	private packet sndPacket;

	public RC_CHANNELS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public RC_CHANNELS_class(RC_CHANNELS_class o)
	{
		time_boot_ms = o.time_boot_ms;
		chan1_raw = o.chan1_raw;
		chan2_raw = o.chan2_raw;
		chan3_raw = o.chan3_raw;
		chan4_raw = o.chan4_raw;
		chan5_raw = o.chan5_raw;
		chan6_raw = o.chan6_raw;
		chan7_raw = o.chan7_raw;
		chan8_raw = o.chan8_raw;
		chan9_raw = o.chan9_raw;
		chan10_raw = o.chan10_raw;
		chan11_raw = o.chan11_raw;
		chan12_raw = o.chan12_raw;
		chan13_raw = o.chan13_raw;
		chan14_raw = o.chan14_raw;
		chan15_raw = o.chan15_raw;
		chan16_raw = o.chan16_raw;
		chan17_raw = o.chan17_raw;
		chan18_raw = o.chan18_raw;
		chancount = o.chancount;
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

			// int[] mavLen = {4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2};

			time_boot_ms	= rcvPacket.getLongI();
			chan1_raw	= rcvPacket.getIntS();
			chan2_raw	= rcvPacket.getIntS();
			chan3_raw	= rcvPacket.getIntS();
			chan4_raw	= rcvPacket.getIntS();
			chan5_raw	= rcvPacket.getIntS();
			chan6_raw	= rcvPacket.getIntS();
			chan7_raw	= rcvPacket.getIntS();
			chan8_raw	= rcvPacket.getIntS();
			chan9_raw	= rcvPacket.getIntS();
			chan10_raw	= rcvPacket.getIntS();
			chan11_raw	= rcvPacket.getIntS();
			chan12_raw	= rcvPacket.getIntS();
			chan13_raw	= rcvPacket.getIntS();
			chan14_raw	= rcvPacket.getIntS();
			chan15_raw	= rcvPacket.getIntS();
			chan16_raw	= rcvPacket.getIntS();
			chan17_raw	= rcvPacket.getIntS();
			chan18_raw	= rcvPacket.getIntS();
			chancount	= rcvPacket.getShortB();
			rssi			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
					 ,chan1_raw
					 ,chan2_raw
					 ,chan3_raw
					 ,chan4_raw
					 ,chan5_raw
					 ,chan6_raw
					 ,chan7_raw
					 ,chan8_raw
					 ,chan9_raw
					 ,chan10_raw
					 ,chan11_raw
					 ,chan12_raw
					 ,chan13_raw
					 ,chan14_raw
					 ,chan15_raw
					 ,chan16_raw
					 ,chan17_raw
					 ,chan18_raw
					 ,chancount
					 ,rssi
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,int v_chan1_raw
						,int v_chan2_raw
						,int v_chan3_raw
						,int v_chan4_raw
						,int v_chan5_raw
						,int v_chan6_raw
						,int v_chan7_raw
						,int v_chan8_raw
						,int v_chan9_raw
						,int v_chan10_raw
						,int v_chan11_raw
						,int v_chan12_raw
						,int v_chan13_raw
						,int v_chan14_raw
						,int v_chan15_raw
						,int v_chan16_raw
						,int v_chan17_raw
						,int v_chan18_raw
						,short v_chancount
						,short v_rssi
						)
	{
		// int[] mavLen = {4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putShortI(v_chan1_raw);	// Add "chan1_raw" parameter
		sndPacket.putShortI(v_chan2_raw);	// Add "chan2_raw" parameter
		sndPacket.putShortI(v_chan3_raw);	// Add "chan3_raw" parameter
		sndPacket.putShortI(v_chan4_raw);	// Add "chan4_raw" parameter
		sndPacket.putShortI(v_chan5_raw);	// Add "chan5_raw" parameter
		sndPacket.putShortI(v_chan6_raw);	// Add "chan6_raw" parameter
		sndPacket.putShortI(v_chan7_raw);	// Add "chan7_raw" parameter
		sndPacket.putShortI(v_chan8_raw);	// Add "chan8_raw" parameter
		sndPacket.putShortI(v_chan9_raw);	// Add "chan9_raw" parameter
		sndPacket.putShortI(v_chan10_raw);	// Add "chan10_raw" parameter
		sndPacket.putShortI(v_chan11_raw);	// Add "chan11_raw" parameter
		sndPacket.putShortI(v_chan12_raw);	// Add "chan12_raw" parameter
		sndPacket.putShortI(v_chan13_raw);	// Add "chan13_raw" parameter
		sndPacket.putShortI(v_chan14_raw);	// Add "chan14_raw" parameter
		sndPacket.putShortI(v_chan15_raw);	// Add "chan15_raw" parameter
		sndPacket.putShortI(v_chan16_raw);	// Add "chan16_raw" parameter
		sndPacket.putShortI(v_chan17_raw);	// Add "chan17_raw" parameter
		sndPacket.putShortI(v_chan18_raw);	// Add "chan18_raw" parameter
		sndPacket.putByteS(v_chancount);	// Add "chancount" parameter
		sndPacket.putByteS(v_rssi);	// Add "rssi" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", RC_CHANNELS_time_boot_ms"
 				+ ", RC_CHANNELS_chan1_raw"
 				+ ", RC_CHANNELS_chan2_raw"
 				+ ", RC_CHANNELS_chan3_raw"
 				+ ", RC_CHANNELS_chan4_raw"
 				+ ", RC_CHANNELS_chan5_raw"
 				+ ", RC_CHANNELS_chan6_raw"
 				+ ", RC_CHANNELS_chan7_raw"
 				+ ", RC_CHANNELS_chan8_raw"
 				+ ", RC_CHANNELS_chan9_raw"
 				+ ", RC_CHANNELS_chan10_raw"
 				+ ", RC_CHANNELS_chan11_raw"
 				+ ", RC_CHANNELS_chan12_raw"
 				+ ", RC_CHANNELS_chan13_raw"
 				+ ", RC_CHANNELS_chan14_raw"
 				+ ", RC_CHANNELS_chan15_raw"
 				+ ", RC_CHANNELS_chan16_raw"
 				+ ", RC_CHANNELS_chan17_raw"
 				+ ", RC_CHANNELS_chan18_raw"
 				+ ", RC_CHANNELS_chancount"
 				+ ", RC_CHANNELS_rssi"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + chan1_raw
 				+ ", " + chan2_raw
 				+ ", " + chan3_raw
 				+ ", " + chan4_raw
 				+ ", " + chan5_raw
 				+ ", " + chan6_raw
 				+ ", " + chan7_raw
 				+ ", " + chan8_raw
 				+ ", " + chan9_raw
 				+ ", " + chan10_raw
 				+ ", " + chan11_raw
 				+ ", " + chan12_raw
 				+ ", " + chan13_raw
 				+ ", " + chan14_raw
 				+ ", " + chan15_raw
 				+ ", " + chan16_raw
 				+ ", " + chan17_raw
 				+ ", " + chan18_raw
 				+ ", " + chancount
 				+ ", " + rssi
				);
		return param;
	}
}
