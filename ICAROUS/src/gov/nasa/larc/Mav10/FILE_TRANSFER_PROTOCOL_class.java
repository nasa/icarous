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
Message ID: FILE_TRANSFER_PROTOCOL(110)
--------------------------------------
%%~ File transfer message
--------------------------------------
*/
public class FILE_TRANSFER_PROTOCOL_class //implements Loggable
{
	public static final int msgID = 110;
	public short	 target_network;	 	// Network ID (0 for broadcast)
	public short	 target_system;	 	// System ID (0 for broadcast)
	public short	 target_component; 	// Component ID (0 for broadcast)
	public short[]	 payload = new short[251];				// Variable length payload. The length is defined by the remaining message length when subtracting the header and other fields.  The entire content of this block is opaque unless you understand any the encoding message_type.  The particular encoding used can be extension specific and might not always be documented as part of the mavlink specification.

	private packet rcvPacket;
	private packet sndPacket;

	public FILE_TRANSFER_PROTOCOL_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public FILE_TRANSFER_PROTOCOL_class(FILE_TRANSFER_PROTOCOL_class o)
	{
		target_network = o.target_network;
		target_system = o.target_system;
		target_component = o.target_component;
		payload = o.payload;
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

			// int[] mavLen = {1, 1, 1, 251};
			// int[] javLen = {2, 2, 2, 502};

			target_network	= rcvPacket.getShortB();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			rcvPacket.getByte(payload, 0, 251);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  target_network
					 ,(short)1
					 ,(short)1
					 ,payload
					 );
	}

	public byte[] encode(
						 short v_target_network
						,short[] v_payload
						)
	{
		return encode(
					  v_target_network
					 ,  (short)1
					 ,  (short)1
					 ,v_payload
					 );
	}

	public byte[] encode(
						 short v_target_network
						,short v_target_system
						,short v_target_component
						,short[] v_payload
						)
	{
		// int[] mavLen = {1, 1, 1, 251};
		// int[] javLen = {2, 2, 2, 502};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_network);	// Add "target_network" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByte(v_payload,0,251);	// Add "payload" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", FILE_TRANSFER_PROTOCOL_target_network"
 				+ ", FILE_TRANSFER_PROTOCOL_target_system"
 				+ ", FILE_TRANSFER_PROTOCOL_target_component"
 				+ ", FILE_TRANSFER_PROTOCOL_payload"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_network
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + payload
				);
		return param;
	}
}
