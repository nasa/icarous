/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: CHANGE_OPERATOR_CONTROL_ACK(6)
--------------------------------------
%%~ Accept / deny control of this MAV
--------------------------------------
*/
public class CHANGE_OPERATOR_CONTROL_ACK_class implements Loggable
{
	public static final int msgID = 6;
	public short	 gcs_system_id;	 	// ID of the GCS this message 
	public short	 control_request;	 	// 0: request control of this MAV, 1: Release control of this MAV
	public short	 ack;				 	// 0: ACK, 1: NACK: Wrong passkey, 2: NACK: Unsupported passkey encryption method, 3: NACK: Already under control

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public CHANGE_OPERATOR_CONTROL_ACK_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public CHANGE_OPERATOR_CONTROL_ACK_class(CHANGE_OPERATOR_CONTROL_ACK_class o)
	{
		gcs_system_id = o.gcs_system_id;
		control_request = o.control_request;
		ack = o.ack;
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

			gcs_system_id	= rcvPacket.getShortB();
			control_request	= rcvPacket.getShortB();
			ack				= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  gcs_system_id
					 ,control_request
					 ,ack
					 );
	}

	public byte[] encode(
						 short v_gcs_system_id
						,short v_control_request
						,short v_ack
						)
	{
		// int[] mavLen = {1, 1, 1};
		// int[] javLen = {2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_gcs_system_id);	// Add "gcs_system_id" parameter
		sndPacket.putByteS(v_control_request);	// Add "control_request" parameter
		sndPacket.putByteS(v_ack);	// Add "ack" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", CHANGE_OPERATOR_CONTROL_ACK_gcs_system_id"
 				+ ", CHANGE_OPERATOR_CONTROL_ACK_control_request"
 				+ ", CHANGE_OPERATOR_CONTROL_ACK_ack"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + gcs_system_id
 				+ ", " + control_request
 				+ ", " + ack
				);
		return param;
	}
}
