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
Message ID: COMMAND_LONG(76)
--------------------------------------
%%~ Send a command with up to seven parameters to the MAV
--------------------------------------
*/
public class COMMAND_LONG_class //implements Loggable
{
	public static final int msgID = 76;
	public float	 param1;			 	// Parameter 1, as defined by MAV_CMD enum.
	public float	 param2;			 	// Parameter 2, as defined by MAV_CMD enum.
	public float	 param3;			 	// Parameter 3, as defined by MAV_CMD enum.
	public float	 param4;			 	// Parameter 4, as defined by MAV_CMD enum.
	public float	 param5;			 	// Parameter 5, as defined by MAV_CMD enum.
	public float	 param6;			 	// Parameter 6, as defined by MAV_CMD enum.
	public float	 param7;			 	// Parameter 7, as defined by MAV_CMD enum.
	public int		 command;			 	// Command ID, as defined by MAV_CMD enum.
	public short	 target_system;	 	// System which should execute the command
	public short	 target_component; 	// Component which should execute the command, 0 for all components
	public short	 confirmation;	 	// 0: First transmission of this command. 1-255: Confirmation transmissions (e.g. for kill command)

	private packet rcvPacket;
	private packet sndPacket;

	public COMMAND_LONG_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public COMMAND_LONG_class(COMMAND_LONG_class o)
	{
		param1 = o.param1;
		param2 = o.param2;
		param3 = o.param3;
		param4 = o.param4;
		param5 = o.param5;
		param6 = o.param6;
		param7 = o.param7;
		command = o.command;
		target_system = o.target_system;
		target_component = o.target_component;
		confirmation = o.confirmation;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 2, 1, 1, 1};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2};

			param1			= rcvPacket.getFloat();
			param2			= rcvPacket.getFloat();
			param3			= rcvPacket.getFloat();
			param4			= rcvPacket.getFloat();
			param5			= rcvPacket.getFloat();
			param6			= rcvPacket.getFloat();
			param7			= rcvPacket.getFloat();
			command			= rcvPacket.getIntS();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			confirmation		= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  param1
					 ,param2
					 ,param3
					 ,param4
					 ,param5
					 ,param6
					 ,param7
					 ,command
					 ,(short)1
					 ,(short)1
					 ,confirmation
					 );
	}

	public byte[] encode(
						 float v_param1
						,float v_param2
						,float v_param3
						,float v_param4
						,float v_param5
						,float v_param6
						,float v_param7
						,int v_command
						,short v_confirmation
						)
	{
		return encode(
					  v_param1
					 ,v_param2
					 ,v_param3
					 ,v_param4
					 ,v_param5
					 ,v_param6
					 ,v_param7
					 ,v_command
					 ,  (short)1
					 ,  (short)1
					 ,v_confirmation
					 );
	}

	public byte[] encode(
						 float v_param1
						,float v_param2
						,float v_param3
						,float v_param4
						,float v_param5
						,float v_param6
						,float v_param7
						,int v_command
						,short v_target_system
						,short v_target_component
						,short v_confirmation
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 2, 1, 1, 1};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_param1);	// Add "param1" parameter
		sndPacket.putFloat(v_param2);	// Add "param2" parameter
		sndPacket.putFloat(v_param3);	// Add "param3" parameter
		sndPacket.putFloat(v_param4);	// Add "param4" parameter
		sndPacket.putFloat(v_param5);	// Add "param5" parameter
		sndPacket.putFloat(v_param6);	// Add "param6" parameter
		sndPacket.putFloat(v_param7);	// Add "param7" parameter
		sndPacket.putShortI(v_command);	// Add "command" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_confirmation);	// Add "confirmation" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", COMMAND_LONG_param1"
 				+ ", COMMAND_LONG_param2"
 				+ ", COMMAND_LONG_param3"
 				+ ", COMMAND_LONG_param4"
 				+ ", COMMAND_LONG_param5"
 				+ ", COMMAND_LONG_param6"
 				+ ", COMMAND_LONG_param7"
 				+ ", COMMAND_LONG_command"
 				+ ", COMMAND_LONG_target_system"
 				+ ", COMMAND_LONG_target_component"
 				+ ", COMMAND_LONG_confirmation"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + param1
 				+ ", " + param2
 				+ ", " + param3
 				+ ", " + param4
 				+ ", " + param5
 				+ ", " + param6
 				+ ", " + param7
 				+ ", " + command
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + confirmation
				);
		return param;
	}
}
