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
Message ID: RC_CHANNELS_SCALED(34)
--------------------------------------
%%~ The scaled values of the RC channels received. (-100%) -10000, (0%) 0, (100%) 
%%~ 10000. Channels that are inactive should be set to UINT16_MAX.
--------------------------------------
*/
public class RC_CHANNELS_SCALED_class //implements Loggable
{
	public static final int msgID = 34;
	public long	 time_boot_ms; 	// Timestamp (milliseconds since system boot)
	public short	 chan1_scaled; 	// RC channel 1 value scaled, (-100%) -10000, (0%) 0, (100%) 10000, (invalid) INT16_MAX.
	public short	 chan2_scaled; 	// RC channel 2 value scaled, (-100%) -10000, (0%) 0, (100%) 10000, (invalid) INT16_MAX.
	public short	 chan3_scaled; 	// RC channel 3 value scaled, (-100%) -10000, (0%) 0, (100%) 10000, (invalid) INT16_MAX.
	public short	 chan4_scaled; 	// RC channel 4 value scaled, (-100%) -10000, (0%) 0, (100%) 10000, (invalid) INT16_MAX.
	public short	 chan5_scaled; 	// RC channel 5 value scaled, (-100%) -10000, (0%) 0, (100%) 10000, (invalid) INT16_MAX.
	public short	 chan6_scaled; 	// RC channel 6 value scaled, (-100%) -10000, (0%) 0, (100%) 10000, (invalid) INT16_MAX.
	public short	 chan7_scaled; 	// RC channel 7 value scaled, (-100%) -10000, (0%) 0, (100%) 10000, (invalid) INT16_MAX.
	public short	 chan8_scaled; 	// RC channel 8 value scaled, (-100%) -10000, (0%) 0, (100%) 10000, (invalid) INT16_MAX.
	public short	 port;		 	// Servo output port (set of 8 outputs = 1 port). Most MAVs will just use one, but this allows for more than 8 servos.
	public short	 rssi;		 	// Receive signal strength indicator, 0: 0%, 100: 100%, 255: invalid/unknown.

	private packet rcvPacket;
	private packet sndPacket;

	public RC_CHANNELS_SCALED_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public RC_CHANNELS_SCALED_class(RC_CHANNELS_SCALED_class o)
	{
		time_boot_ms = o.time_boot_ms;
		chan1_scaled = o.chan1_scaled;
		chan2_scaled = o.chan2_scaled;
		chan3_scaled = o.chan3_scaled;
		chan4_scaled = o.chan4_scaled;
		chan5_scaled = o.chan5_scaled;
		chan6_scaled = o.chan6_scaled;
		chan7_scaled = o.chan7_scaled;
		chan8_scaled = o.chan8_scaled;
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
			// int[] javLen = {8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

			time_boot_ms	= rcvPacket.getLongI();
			chan1_scaled	= rcvPacket.getShort();
			chan2_scaled	= rcvPacket.getShort();
			chan3_scaled	= rcvPacket.getShort();
			chan4_scaled	= rcvPacket.getShort();
			chan5_scaled	= rcvPacket.getShort();
			chan6_scaled	= rcvPacket.getShort();
			chan7_scaled	= rcvPacket.getShort();
			chan8_scaled	= rcvPacket.getShort();
			port			= rcvPacket.getShortB();
			rssi			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_boot_ms
					 ,chan1_scaled
					 ,chan2_scaled
					 ,chan3_scaled
					 ,chan4_scaled
					 ,chan5_scaled
					 ,chan6_scaled
					 ,chan7_scaled
					 ,chan8_scaled
					 ,port
					 ,rssi
					 );
	}

	public byte[] encode(
						 long v_time_boot_ms
						,short v_chan1_scaled
						,short v_chan2_scaled
						,short v_chan3_scaled
						,short v_chan4_scaled
						,short v_chan5_scaled
						,short v_chan6_scaled
						,short v_chan7_scaled
						,short v_chan8_scaled
						,short v_port
						,short v_rssi
						)
	{
		// int[] mavLen = {4, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1};
		// int[] javLen = {8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_time_boot_ms);	// Add "time_boot_ms" parameter
		sndPacket.putShort(v_chan1_scaled);	// Add "chan1_scaled" parameter
		sndPacket.putShort(v_chan2_scaled);	// Add "chan2_scaled" parameter
		sndPacket.putShort(v_chan3_scaled);	// Add "chan3_scaled" parameter
		sndPacket.putShort(v_chan4_scaled);	// Add "chan4_scaled" parameter
		sndPacket.putShort(v_chan5_scaled);	// Add "chan5_scaled" parameter
		sndPacket.putShort(v_chan6_scaled);	// Add "chan6_scaled" parameter
		sndPacket.putShort(v_chan7_scaled);	// Add "chan7_scaled" parameter
		sndPacket.putShort(v_chan8_scaled);	// Add "chan8_scaled" parameter
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
 				+ ", RC_CHANNELS_SCALED_time_boot_ms"
 				+ ", RC_CHANNELS_SCALED_chan1_scaled"
 				+ ", RC_CHANNELS_SCALED_chan2_scaled"
 				+ ", RC_CHANNELS_SCALED_chan3_scaled"
 				+ ", RC_CHANNELS_SCALED_chan4_scaled"
 				+ ", RC_CHANNELS_SCALED_chan5_scaled"
 				+ ", RC_CHANNELS_SCALED_chan6_scaled"
 				+ ", RC_CHANNELS_SCALED_chan7_scaled"
 				+ ", RC_CHANNELS_SCALED_chan8_scaled"
 				+ ", RC_CHANNELS_SCALED_port"
 				+ ", RC_CHANNELS_SCALED_rssi"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_boot_ms
 				+ ", " + chan1_scaled
 				+ ", " + chan2_scaled
 				+ ", " + chan3_scaled
 				+ ", " + chan4_scaled
 				+ ", " + chan5_scaled
 				+ ", " + chan6_scaled
 				+ ", " + chan7_scaled
 				+ ", " + chan8_scaled
 				+ ", " + port
 				+ ", " + rssi
				);
		return param;
	}
}
