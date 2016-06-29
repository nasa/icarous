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
Message ID: CHANGE_OPERATOR_CONTROL(5)
--------------------------------------
%%~ Request to control this MAV
--------------------------------------
*/
public class CHANGE_OPERATOR_CONTROL_class //implements Loggable
{
	public static final int msgID = 5;
	public short	 target_system;	 	// System the GCS requests control for
	public short	 control_request;	 	// 0: request control of this MAV, 1: Release control of this MAV
	public short	 version;			 	// 0: key as plaintext, 1-255: future, different hashing/encryption variants. The GCS should in general use the safest mode possible initially and then gradually move down the encryption level if it gets a NACK message indicating an encryption mismatch.
	public byte[]	 passkey = new byte[25];				// Password / Key, depending on version plaintext or encrypted. 25 or less characters, NULL terminated. The characters may involve A-Z, a-z, 0-9, and "!?,.-"

	private packet rcvPacket;
	private packet sndPacket;

	public CHANGE_OPERATOR_CONTROL_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
	}

	public CHANGE_OPERATOR_CONTROL_class(CHANGE_OPERATOR_CONTROL_class o)
	{
		target_system = o.target_system;
		control_request = o.control_request;
		version = o.version;
		passkey = o.passkey;
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

			// int[] mavLen = {1, 1, 1, 25};
			// int[] javLen = {2, 2, 2, 25};

			target_system	= rcvPacket.getShortB();
			control_request	= rcvPacket.getShortB();
			version			= rcvPacket.getShortB();
			rcvPacket.getByte(passkey, 0, 25);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,control_request
					 ,version
					 ,passkey
					 );
	}

	public byte[] encode(
						 short v_control_request
						,short v_version
						,byte[] v_passkey
						)
	{
		return encode(
					    (short)1
					 ,v_control_request
					 ,v_version
					 ,v_passkey
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_control_request
						,short v_version
						,byte[] v_passkey
						)
	{
		// int[] mavLen = {1, 1, 1, 25};
		// int[] javLen = {2, 2, 2, 25};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_control_request);	// Add "control_request" parameter
		sndPacket.putByteS(v_version);	// Add "version" parameter
		sndPacket.putByte(v_passkey,0,25);	// Add "passkey" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", CHANGE_OPERATOR_CONTROL_target_system"
 				+ ", CHANGE_OPERATOR_CONTROL_control_request"
 				+ ", CHANGE_OPERATOR_CONTROL_version"
 				+ ", CHANGE_OPERATOR_CONTROL_passkey"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + control_request
 				+ ", " + version
 				+ ", " + passkey
				);
		return param;
	}
}
