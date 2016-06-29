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
Message ID: MISSION_SET_CURRENT(41)
--------------------------------------
%%~ Set the mission item with sequence number seq as current item. This means that the 
%%~ MAV will continue to this mission item on the shortest path (not following the 
%%~ mission items in-between).
--------------------------------------
*/
public class MISSION_SET_CURRENT_class //implements Loggable
{
	public static final int msgID = 41;
	public int		 seq;				 	// Sequence
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID

	private packet rcvPacket;
	private packet sndPacket;

	public MISSION_SET_CURRENT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public MISSION_SET_CURRENT_class(MISSION_SET_CURRENT_class o)
	{
		seq = o.seq;
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

			// int[] mavLen = {2, 1, 1};
			// int[] javLen = {4, 2, 2};

			seq				= rcvPacket.getIntS();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  seq
					 ,(short)1
					 ,(short)1
					 );
	}

	public byte[] encode(
						 int v_seq
						)
	{
		return encode(
					  v_seq
					 ,  (short)1
					 ,  (short)1
					 );
	}

	public byte[] encode(
						 int v_seq
						,short v_target_system
						,short v_target_component
						)
	{
		// int[] mavLen = {2, 1, 1};
		// int[] javLen = {4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_seq);	// Add "seq" parameter
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
 				+ ", MISSION_SET_CURRENT_seq"
 				+ ", MISSION_SET_CURRENT_target_system"
 				+ ", MISSION_SET_CURRENT_target_component"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + seq
 				+ ", " + target_system
 				+ ", " + target_component
				);
		return param;
	}
}
