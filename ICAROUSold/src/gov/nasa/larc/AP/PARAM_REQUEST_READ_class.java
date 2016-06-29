/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: PARAM_REQUEST_READ(20)
--------------------------------------
%%~ Request to read the onboard parameter with the param_id string id. Onboard parameters 
%%~ are stored as key[const char*] -> value[float]. This allows to send a parameter 
%%~ to any other component (such as the GCS) without the need of previous knowledge 
%%~ of possible parameter names. Thus the same GCS can store different parameters 
%%~ for different autopilots. See also http://qgroundcontrol.org/parameter_interface 
%%~ for a full documentation of QGroundControl and IMU code.
--------------------------------------
*/
public class PARAM_REQUEST_READ_class implements Loggable
{
	public static final int msgID = 20;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public byte[]	 param_id = new byte[15];			// Onboard parameter id
	public short	 param_index;		 	// Parameter index. Send -1 to use the param ID field as identifier

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public PARAM_REQUEST_READ_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public PARAM_REQUEST_READ_class(PARAM_REQUEST_READ_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		param_id = o.param_id;
		param_index = o.param_index;
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

			// int[] mavLen = {1, 1, 15, 2};
			// int[] javLen = {2, 2, 15, 2};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			rcvPacket.getByte(param_id, 0, 15);
			param_index		= rcvPacket.getShort();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,param_id
					 ,param_index
					 );
	}

	public byte[] encode(
						 byte[] v_param_id
						,short v_param_index
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_param_id
					 ,v_param_index
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,byte[] v_param_id
						,short v_param_index
						)
	{
		// int[] mavLen = {1, 1, 15, 2};
		// int[] javLen = {2, 2, 15, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByte(v_param_id,0,15);	// Add "param_id" parameter
		sndPacket.putShort(v_param_index);	// Add "param_index" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", PARAM_REQUEST_READ_target_system"
 				+ ", PARAM_REQUEST_READ_target_component"
 				+ ", PARAM_REQUEST_READ_param_id"
 				+ ", PARAM_REQUEST_READ_param_index"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + param_id
 				+ ", " + param_index
				);
		return param;
	}
}
