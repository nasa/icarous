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
Message ID: V2_EXTENSION(248)
--------------------------------------
%%~ Message implementing parts of the V2 payload specs in V1 frames for transitional 
%%~ support.
--------------------------------------
*/
public class V2_EXTENSION_class //implements Loggable
{
	public static final int msgID = 248;
	public int		 message_type;	 	// A code that identifies the software component that understands this message (analogous to usb device classes or mime type strings).  If this code is less than 32768, it is considered a 'registered' protocol extension and the corresponding entry should be added to https://github.com/mavlink/mavlink/extension-message-ids.xml.  Software creators can register blocks of message IDs as needed (useful for GCS specific metadata, etc...). Message_types greater than 32767 are considered local experiments and should not be checked in to any widely distributed codebase.
	public short	 target_network;	 	// Network ID (0 for broadcast)
	public short	 target_system;	 	// System ID (0 for broadcast)
	public short	 target_component; 	// Component ID (0 for broadcast)
	public short[]	 payload = new short[249];				// Variable length payload. The length is defined by the remaining message length when subtracting the header and other fields.  The entire content of this block is opaque unless you understand any the encoding message_type.  The particular encoding used can be extension specific and might not always be documented as part of the mavlink specification.

	private packet rcvPacket;
	private packet sndPacket;

	public V2_EXTENSION_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public V2_EXTENSION_class(V2_EXTENSION_class o)
	{
		message_type = o.message_type;
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

			// int[] mavLen = {2, 1, 1, 1, 249};
			// int[] javLen = {4, 2, 2, 2, 498};

			message_type		= rcvPacket.getIntS();
			target_network	= rcvPacket.getShortB();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			rcvPacket.getByte(payload, 0, 249);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  message_type
					 ,target_network
					 ,(short)1
					 ,(short)1
					 ,payload
					 );
	}

	public byte[] encode(
						 int v_message_type
						,short v_target_network
						,short[] v_payload
						)
	{
		return encode(
					  v_message_type
					 ,v_target_network
					 ,  (short)1
					 ,  (short)1
					 ,v_payload
					 );
	}

	public byte[] encode(
						 int v_message_type
						,short v_target_network
						,short v_target_system
						,short v_target_component
						,short[] v_payload
						)
	{
		// int[] mavLen = {2, 1, 1, 1, 249};
		// int[] javLen = {4, 2, 2, 2, 498};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_message_type);	// Add "message_type" parameter
		sndPacket.putByteS(v_target_network);	// Add "target_network" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByte(v_payload,0,249);	// Add "payload" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", V2_EXTENSION_message_type"
 				+ ", V2_EXTENSION_target_network"
 				+ ", V2_EXTENSION_target_system"
 				+ ", V2_EXTENSION_target_component"
 				+ ", V2_EXTENSION_payload"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + message_type
 				+ ", " + target_network
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + payload
				);
		return param;
	}
}
