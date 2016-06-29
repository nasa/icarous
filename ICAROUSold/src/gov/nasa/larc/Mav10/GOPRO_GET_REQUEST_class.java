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
Message ID: GOPRO_GET_REQUEST(216)
--------------------------------------
%%~ Request a GOPRO_COMMAND response from the GoPro
--------------------------------------
*/
public class GOPRO_GET_REQUEST_class //implements Loggable
{
	public static final int msgID = 216;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public short	 cmd_id;			 	// Command ID

	private packet rcvPacket;
	private packet sndPacket;

	public GOPRO_GET_REQUEST_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public GOPRO_GET_REQUEST_class(GOPRO_GET_REQUEST_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		cmd_id = o.cmd_id;
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

			// int[] mavLen = {1, 1, 1};
			// int[] javLen = {2, 2, 2};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			cmd_id			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,cmd_id
					 );
	}

	public byte[] encode(
						 short v_cmd_id
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_cmd_id
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,short v_cmd_id
						)
	{
		// int[] mavLen = {1, 1, 1};
		// int[] javLen = {2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_cmd_id);	// Add "cmd_id" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GOPRO_GET_REQUEST_target_system"
 				+ ", GOPRO_GET_REQUEST_target_component"
 				+ ", GOPRO_GET_REQUEST_cmd_id"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + cmd_id
				);
		return param;
	}
}
