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
Message ID: MOUNT_CONTROL(157)
--------------------------------------
%%~ Message to control a camera mount, directional antenna, etc.
--------------------------------------
*/
public class MOUNT_CONTROL_class //implements Loggable
{
	public static final int msgID = 157;
	public int		 input_a;			 	// pitch(deg*100) or lat, depending on mount mode
	public int		 input_b;			 	// roll(deg*100) or lon depending on mount mode
	public int		 input_c;			 	// yaw(deg*100) or alt (in cm) depending on mount mode
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public short	 save_position;	 	// if "1" it will save current trimmed position on EEPROM (just valid for NEUTRAL and LANDING)

	private packet rcvPacket;
	private packet sndPacket;

	public MOUNT_CONTROL_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public MOUNT_CONTROL_class(MOUNT_CONTROL_class o)
	{
		input_a = o.input_a;
		input_b = o.input_b;
		input_c = o.input_c;
		target_system = o.target_system;
		target_component = o.target_component;
		save_position = o.save_position;
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

			// int[] mavLen = {4, 4, 4, 1, 1, 1};
			// int[] javLen = {4, 4, 4, 2, 2, 2};

			input_a			= rcvPacket.getInt();
			input_b			= rcvPacket.getInt();
			input_c			= rcvPacket.getInt();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			save_position	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  input_a
					 ,input_b
					 ,input_c
					 ,(short)1
					 ,(short)1
					 ,save_position
					 );
	}

	public byte[] encode(
						 int v_input_a
						,int v_input_b
						,int v_input_c
						,short v_save_position
						)
	{
		return encode(
					  v_input_a
					 ,v_input_b
					 ,v_input_c
					 ,  (short)1
					 ,  (short)1
					 ,v_save_position
					 );
	}

	public byte[] encode(
						 int v_input_a
						,int v_input_b
						,int v_input_c
						,short v_target_system
						,short v_target_component
						,short v_save_position
						)
	{
		// int[] mavLen = {4, 4, 4, 1, 1, 1};
		// int[] javLen = {4, 4, 4, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putInt(v_input_a);	// Add "input_a" parameter
		sndPacket.putInt(v_input_b);	// Add "input_b" parameter
		sndPacket.putInt(v_input_c);	// Add "input_c" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_save_position);	// Add "save_position" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", MOUNT_CONTROL_input_a"
 				+ ", MOUNT_CONTROL_input_b"
 				+ ", MOUNT_CONTROL_input_c"
 				+ ", MOUNT_CONTROL_target_system"
 				+ ", MOUNT_CONTROL_target_component"
 				+ ", MOUNT_CONTROL_save_position"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + input_a
 				+ ", " + input_b
 				+ ", " + input_c
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + save_position
				);
		return param;
	}
}
