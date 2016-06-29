/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: PARAM_SET(23)
--------------------------------------
%%~ Set a parameter value TEMPORARILY to RAM. It will be reset to default on system 
%%~ reboot. Send the ACTION MAV_ACTION_STORAGE_WRITE to PERMANENTLY write the RAM contents 
%%~ to EEPROM. IMPORTANT: The receiving component should acknowledge the new 
%%~ parameter value by sending a param_value message to all communication partners. 
%%~ This will also ensure that multiple GCS all have an up-to-date list of all parameters. 
%%~ If the sending GCS did not receive a PARAM_VALUE message within its timeout 
%%~ time, it should re-send the PARAM_SET message.
--------------------------------------
*/
public class PARAM_SET_class implements Loggable
{
	public static final int msgID = 23;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public byte[]	 param_id = new byte[15];			// Onboard parameter id
	public float	 param_value;		 	// Onboard parameter value

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public PARAM_SET_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public PARAM_SET_class(PARAM_SET_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		param_id = o.param_id;
		param_value = o.param_value;
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

			// int[] mavLen = {1, 1, 15, 4};
			// int[] javLen = {2, 2, 15, 4};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			rcvPacket.getByte(param_id, 0, 15);
			param_value		= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,param_id
					 ,param_value
					 );
	}

	public byte[] encode(
						 byte[] v_param_id
						,float v_param_value
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_param_id
					 ,v_param_value
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,byte[] v_param_id
						,float v_param_value
						)
	{
		// int[] mavLen = {1, 1, 15, 4};
		// int[] javLen = {2, 2, 15, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByte(v_param_id,0,15);	// Add "param_id" parameter
		sndPacket.putFloat(v_param_value);	// Add "param_value" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", PARAM_SET_target_system"
 				+ ", PARAM_SET_target_component"
 				+ ", PARAM_SET_param_id"
 				+ ", PARAM_SET_param_value"
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
 				+ ", " + param_value
				);
		return param;
	}
}
