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
Message ID: LED_CONTROL(186)
--------------------------------------
%%~ Control vehicle LEDs
--------------------------------------
*/
public class LED_CONTROL_class //implements Loggable
{
	public static final int msgID = 186;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public short	 instance;		 	// Instance (LED instance to control or 255 for all LEDs)
	public short	 pattern;			 	// Pattern (see LED_PATTERN_ENUM)
	public short	 custom_len;		 	// Custom Byte Length
	public short[]	 custom_bytes = new short[24];		// Custom Bytes

	private packet rcvPacket;
	private packet sndPacket;

	public LED_CONTROL_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public LED_CONTROL_class(LED_CONTROL_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		instance = o.instance;
		pattern = o.pattern;
		custom_len = o.custom_len;
		custom_bytes = o.custom_bytes;
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

			// int[] mavLen = {1, 1, 1, 1, 1, 24};
			// int[] javLen = {2, 2, 2, 2, 2, 48};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			instance			= rcvPacket.getShortB();
			pattern			= rcvPacket.getShortB();
			custom_len		= rcvPacket.getShortB();
			rcvPacket.getByte(custom_bytes, 0, 24);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,instance
					 ,pattern
					 ,custom_len
					 ,custom_bytes
					 );
	}

	public byte[] encode(
						 short v_instance
						,short v_pattern
						,short v_custom_len
						,short[] v_custom_bytes
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_instance
					 ,v_pattern
					 ,v_custom_len
					 ,v_custom_bytes
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,short v_instance
						,short v_pattern
						,short v_custom_len
						,short[] v_custom_bytes
						)
	{
		// int[] mavLen = {1, 1, 1, 1, 1, 24};
		// int[] javLen = {2, 2, 2, 2, 2, 48};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_instance);	// Add "instance" parameter
		sndPacket.putByteS(v_pattern);	// Add "pattern" parameter
		sndPacket.putByteS(v_custom_len);	// Add "custom_len" parameter
		sndPacket.putByte(v_custom_bytes,0,24);	// Add "custom_bytes" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", LED_CONTROL_target_system"
 				+ ", LED_CONTROL_target_component"
 				+ ", LED_CONTROL_instance"
 				+ ", LED_CONTROL_pattern"
 				+ ", LED_CONTROL_custom_len"
 				+ ", LED_CONTROL_custom_bytes"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + instance
 				+ ", " + pattern
 				+ ", " + custom_len
 				+ ", " + custom_bytes
				);
		return param;
	}
}
