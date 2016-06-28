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
Message ID: RC_CHANNELS_RAW(35)
--------------------------------------
%%~ The RAW values of the RC channels received. The standard PPM modulation is as follows: 
%%~ 1000 microseconds: 0%, 2000 microseconds: 100%. Individual receivers/transmitters 
%%~ might violate this specification.
--------------------------------------
*/
public class RC_CHANNELS_RAW_class //implements Loggable
{
	public static final int msgID = 35;
	public long	 time_boot_ms; 	// Timestamp (milliseconds since system boot)
	public int		 chan1_raw;	 	// RC channel 1 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan2_raw;	 	// RC channel 2 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan3_raw;	 	// RC channel 3 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan4_raw;	 	// RC channel 4 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan5_raw;	 	// RC channel 5 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan6_raw;	 	// RC channel 6 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan7_raw;	 	// RC channel 7 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public int		 chan8_raw;	 	// RC channel 8 value, in microseconds. A value of UINT16_MAX implies the channel is unused.
	public short	 port;		 	// Servo output port (set of 8 outputs = 1 port). Most MAVs will just use one, but this allows for more than 8 servos.
	public short	 rssi;		 	// Receive signal strength indicator, 0: 0%, 100: 100%, 255: invalid/unknown.

	private packet rcvPacket;
	private packet sndPacket;

	public RC_CHANNELS_RAW_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public RC_CHANNELS_RAW_class(RC_CHANNELS_RAW_class o)
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
		port = o.port;
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

			// int[] mavLen = {4, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2};

			time_boot_ms	= rcvPacket.getLongI();
			chan1_raw	= rcvPacket.getIntS();
			chan2_raw	= rcvPacket.getIntS();
			chan3_raw	= rcvPacket.getIntS();
			chan4_raw	= rcvPacket.getIntS();
			chan5_raw	= rcvPacket.getIntS();
			chan6_raw	= rcvPacket.getIntS();
			chan7_raw	= rcvPacket.getIntS();
			chan8_raw	= rcvPacket.getIntS();
			port			= rcvPacket.getShortB();
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
					 ,port
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
						,short v_port
						,short v_rssi
						)
	{
		// int[] mavLen = {4, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2};

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
		sndPacket.putByteS(v_port);	// Add "port" parameter
		sndPacket.putByteS(v_rssi);	// Add "rssi" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", RC_CHANNELS_RAW_time_boot_ms"
 				+ ", RC_CHANNELS_RAW_chan1_raw"
 				+ ", RC_CHANNELS_RAW_chan2_raw"
 				+ ", RC_CHANNELS_RAW_chan3_raw"
 				+ ", RC_CHANNELS_RAW_chan4_raw"
 				+ ", RC_CHANNELS_RAW_chan5_raw"
 				+ ", RC_CHANNELS_RAW_chan6_raw"
 				+ ", RC_CHANNELS_RAW_chan7_raw"
 				+ ", RC_CHANNELS_RAW_chan8_raw"
 				+ ", RC_CHANNELS_RAW_port"
 				+ ", RC_CHANNELS_RAW_rssi"
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
 				+ ", " + port
 				+ ", " + rssi
				);
		return param;
	}
}
