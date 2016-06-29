/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: COMMAND(75)
--------------------------------------
%%~ Send a command with up to four parameters to the MAV
--------------------------------------
*/
public class COMMAND_class implements Loggable
{
	public static final int msgID = 75;
	public short	 target_system;	 	// System which should execute the command
	public short	 target_component; 	// Component which should execute the command, 0 for all components
	public short	 command;			 	// Command ID, as defined by MAV_CMD enum.
	public short	 confirmation;	 	// 0: First transmission of this command. 1-255: Confirmation transmissions (e.g. for kill command)
	public float	 param1;			 	// Parameter 1, as defined by MAV_CMD enum.
	public float	 param2;			 	// Parameter 2, as defined by MAV_CMD enum.
	public float	 param3;			 	// Parameter 3, as defined by MAV_CMD enum.
	public float	 param4;			 	// Parameter 4, as defined by MAV_CMD enum.

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public COMMAND_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public COMMAND_class(COMMAND_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		command = o.command;
		confirmation = o.confirmation;
		param1 = o.param1;
		param2 = o.param2;
		param3 = o.param3;
		param4 = o.param4;
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

			// int[] mavLen = {1, 1, 1, 1, 4, 4, 4, 4};
			// int[] javLen = {2, 2, 2, 2, 4, 4, 4, 4};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			command			= rcvPacket.getShortB();
			confirmation		= rcvPacket.getShortB();
			param1			= rcvPacket.getFloat();
			param2			= rcvPacket.getFloat();
			param3			= rcvPacket.getFloat();
			param4			= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,command
					 ,confirmation
					 ,param1
					 ,param2
					 ,param3
					 ,param4
					 );
	}

	public byte[] encode(
						 short v_command
						,short v_confirmation
						,float v_param1
						,float v_param2
						,float v_param3
						,float v_param4
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_command
					 ,v_confirmation
					 ,v_param1
					 ,v_param2
					 ,v_param3
					 ,v_param4
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,short v_command
						,short v_confirmation
						,float v_param1
						,float v_param2
						,float v_param3
						,float v_param4
						)
	{
		// int[] mavLen = {1, 1, 1, 1, 4, 4, 4, 4};
		// int[] javLen = {2, 2, 2, 2, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_command);	// Add "command" parameter
		sndPacket.putByteS(v_confirmation);	// Add "confirmation" parameter
		sndPacket.putFloat(v_param1);	// Add "param1" parameter
		sndPacket.putFloat(v_param2);	// Add "param2" parameter
		sndPacket.putFloat(v_param3);	// Add "param3" parameter
		sndPacket.putFloat(v_param4);	// Add "param4" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", COMMAND_target_system"
 				+ ", COMMAND_target_component"
 				+ ", COMMAND_command"
 				+ ", COMMAND_confirmation"
 				+ ", COMMAND_param1"
 				+ ", COMMAND_param2"
 				+ ", COMMAND_param3"
 				+ ", COMMAND_param4"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + command
 				+ ", " + confirmation
 				+ ", " + param1
 				+ ", " + param2
 				+ ", " + param3
 				+ ", " + param4
				);
		return param;
	}
}
