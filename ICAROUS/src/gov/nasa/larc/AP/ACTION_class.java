/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: ACTION(10)
--------------------------------------
%%~ An action message allows to execute a certain onboard action. These include liftoff, 
%%~ land, storing parameters too EEPROM, shutddown, etc. The action ids are defined 
%%~ in ENUM MAV_ACTION in mavlink/include/mavlink_types.h
--------------------------------------
*/
public class ACTION_class implements Loggable
{
	public static final int msgID = 10;
	public short	 target;			 	// The system executing the action
	public short	 target_component; 	// The component executing the action
	public short	 action;			 	// The action id

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public ACTION_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_component = 1;
	}

	public ACTION_class(ACTION_class o)
	{
		target = o.target;
		target_component = o.target_component;
		action = o.action;
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

			target			= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			action			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  target
					 ,(short)1
					 ,action
					 );
	}

	public byte[] encode(
						 short v_target
						,short v_action
						)
	{
		return encode(
					  v_target
					 ,  (short)1
					 ,v_action
					 );
	}

	public byte[] encode(
						 short v_target
						,short v_target_component
						,short v_action
						)
	{
		// int[] mavLen = {1, 1, 1};
		// int[] javLen = {2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target);	// Add "target" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_action);	// Add "action" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", ACTION_target"
 				+ ", ACTION_target_component"
 				+ ", ACTION_action"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target
 				+ ", " + target_component
 				+ ", " + action
				);
		return param;
	}
}
