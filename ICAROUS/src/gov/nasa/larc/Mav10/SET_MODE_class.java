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
Message ID: SET_MODE(11)
--------------------------------------
%%~ Set the system mode, as defined by enum MAV_MODE. There is no target component id 
%%~ as the mode is by definition for the overall aircraft, not only for one component.
--------------------------------------
*/
public class SET_MODE_class //implements Loggable
{
	public static final int msgID = 11;
	public long	 custom_mode;		 	// The new autopilot-specific mode. This field can be ignored by an autopilot.
	public short	 target_system;	 	// The system setting the mode
	public short	 base_mode;		 	// The new base mode

	private packet rcvPacket;
	private packet sndPacket;

	public SET_MODE_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
	}

	public SET_MODE_class(SET_MODE_class o)
	{
		custom_mode = o.custom_mode;
		target_system = o.target_system;
		base_mode = o.base_mode;
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

			// int[] mavLen = {4, 1, 1};
			// int[] javLen = {8, 2, 2};

			custom_mode		= rcvPacket.getLongI();
			target_system	= rcvPacket.getShortB();
			base_mode		= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  custom_mode
					 ,(short)1
					 ,base_mode
					 );
	}

	public byte[] encode(
						 long v_custom_mode
						,short v_base_mode
						)
	{
		return encode(
					  v_custom_mode
					 ,  (short)1
					 ,v_base_mode
					 );
	}

	public byte[] encode(
						 long v_custom_mode
						,short v_target_system
						,short v_base_mode
						)
	{
		// int[] mavLen = {4, 1, 1};
		// int[] javLen = {8, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_custom_mode);	// Add "custom_mode" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_base_mode);	// Add "base_mode" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SET_MODE_custom_mode"
 				+ ", SET_MODE_target_system"
 				+ ", SET_MODE_base_mode"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + custom_mode
 				+ ", " + target_system
 				+ ", " + base_mode
				);
		return param;
	}
}
