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
Message ID: MISSION_REQUEST_PARTIAL_LIST(37)
--------------------------------------
%%~ Request a partial list of mission items from the system/component. http://qgroundcontrol.org/mavlink/waypoint_protocol. 
%%~ If start and end index are the same, just 
%%~ send one waypoint.
--------------------------------------
*/
public class MISSION_REQUEST_PARTIAL_LIST_class //implements Loggable
{
	public static final int msgID = 37;
	public short	 start_index;		 	// Start index, 0 by default
	public short	 end_index;		 	// End index, -1 by default (-1: send list to end). Else a valid index of the list
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID

	private packet rcvPacket;
	private packet sndPacket;

	public MISSION_REQUEST_PARTIAL_LIST_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public MISSION_REQUEST_PARTIAL_LIST_class(MISSION_REQUEST_PARTIAL_LIST_class o)
	{
		start_index = o.start_index;
		end_index = o.end_index;
		target_system = o.target_system;
		target_component = o.target_component;
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

			// int[] mavLen = {2, 2, 1, 1};
			// int[] javLen = {2, 2, 2, 2};

			start_index		= rcvPacket.getShort();
			end_index		= rcvPacket.getShort();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  start_index
					 ,end_index
					 ,(short)1
					 ,(short)1
					 );
	}

	public byte[] encode(
						 short v_start_index
						,short v_end_index
						)
	{
		return encode(
					  v_start_index
					 ,v_end_index
					 ,  (short)1
					 ,  (short)1
					 );
	}

	public byte[] encode(
						 short v_start_index
						,short v_end_index
						,short v_target_system
						,short v_target_component
						)
	{
		// int[] mavLen = {2, 2, 1, 1};
		// int[] javLen = {2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShort(v_start_index);	// Add "start_index" parameter
		sndPacket.putShort(v_end_index);	// Add "end_index" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", MISSION_REQUEST_PARTIAL_LIST_start_index"
 				+ ", MISSION_REQUEST_PARTIAL_LIST_end_index"
 				+ ", MISSION_REQUEST_PARTIAL_LIST_target_system"
 				+ ", MISSION_REQUEST_PARTIAL_LIST_target_component"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + start_index
 				+ ", " + end_index
 				+ ", " + target_system
 				+ ", " + target_component
				);
		return param;
	}
}
