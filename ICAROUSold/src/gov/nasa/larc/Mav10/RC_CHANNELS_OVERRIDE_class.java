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
Message ID: RC_CHANNELS_OVERRIDE(70)
--------------------------------------
%%~ The RAW values of the RC channels sent to the MAV to override info received from 
%%~ the RC radio. A value of UINT16_MAX means no change to that channel. A value of 
%%~ 0 means control of that channel should be released back to the RC radio. The standard 
%%~ PPM modulation is as follows: 1000 microseconds: 0%, 2000 microseconds: 
%%~ 100%. Individual receivers/transmitters might violate this specification.
--------------------------------------
*/
public class RC_CHANNELS_OVERRIDE_class //implements Loggable
{
	public static final int msgID = 70;
	public int		 chan1_raw;		 	// RC channel 1 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	public int		 chan2_raw;		 	// RC channel 2 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	public int		 chan3_raw;		 	// RC channel 3 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	public int		 chan4_raw;		 	// RC channel 4 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	public int		 chan5_raw;		 	// RC channel 5 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	public int		 chan6_raw;		 	// RC channel 6 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	public int		 chan7_raw;		 	// RC channel 7 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	public int		 chan8_raw;		 	// RC channel 8 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID

	private packet rcvPacket;
	private packet sndPacket;

	public RC_CHANNELS_OVERRIDE_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public RC_CHANNELS_OVERRIDE_class(RC_CHANNELS_OVERRIDE_class o)
	{
		chan1_raw = o.chan1_raw;
		chan2_raw = o.chan2_raw;
		chan3_raw = o.chan3_raw;
		chan4_raw = o.chan4_raw;
		chan5_raw = o.chan5_raw;
		chan6_raw = o.chan6_raw;
		chan7_raw = o.chan7_raw;
		chan8_raw = o.chan8_raw;
		target_system = o.target_system;
		target_component = o.target_component;
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

			// int[] mavLen = {2, 2, 2, 2, 2, 2, 2, 2, 1, 1};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 2, 2};

			chan1_raw		= rcvPacket.getIntS();
			chan2_raw		= rcvPacket.getIntS();
			chan3_raw		= rcvPacket.getIntS();
			chan4_raw		= rcvPacket.getIntS();
			chan5_raw		= rcvPacket.getIntS();
			chan6_raw		= rcvPacket.getIntS();
			chan7_raw		= rcvPacket.getIntS();
			chan8_raw		= rcvPacket.getIntS();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
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
					 ,(short)1
					 ,(short)1
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
						)
	{
		return encode(
					  v_chan1_raw
					 ,v_chan2_raw
					 ,v_chan3_raw
					 ,v_chan4_raw
					 ,v_chan5_raw
					 ,v_chan6_raw
					 ,v_chan7_raw
					 ,v_chan8_raw
					 ,  (short)1
					 ,  (short)1
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
						,short v_target_system
						,short v_target_component
						)
	{
		// int[] mavLen = {2, 2, 2, 2, 2, 2, 2, 2, 1, 1};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 2, 2};

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
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", RC_CHANNELS_OVERRIDE_chan1_raw"
 				+ ", RC_CHANNELS_OVERRIDE_chan2_raw"
 				+ ", RC_CHANNELS_OVERRIDE_chan3_raw"
 				+ ", RC_CHANNELS_OVERRIDE_chan4_raw"
 				+ ", RC_CHANNELS_OVERRIDE_chan5_raw"
 				+ ", RC_CHANNELS_OVERRIDE_chan6_raw"
 				+ ", RC_CHANNELS_OVERRIDE_chan7_raw"
 				+ ", RC_CHANNELS_OVERRIDE_chan8_raw"
 				+ ", RC_CHANNELS_OVERRIDE_target_system"
 				+ ", RC_CHANNELS_OVERRIDE_target_component"
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
 				+ ", " + target_system
 				+ ", " + target_component
				);
		return param;
	}
}
