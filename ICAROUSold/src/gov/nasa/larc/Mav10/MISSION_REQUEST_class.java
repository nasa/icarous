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
Message ID: MISSION_REQUEST(40)
--------------------------------------
%%~ Request the information of the mission item with the sequence number seq. The response 
%%~ of the system to this message should be a MISSION_ITEM message. http://qgroundcontrol.org/mavlink/waypoint_protocol
--------------------------------------
*/
public class MISSION_REQUEST_class //implements Loggable
{
	public static final int msgID = 40;
	public int		 seq;				 	// Sequence
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID

	private packet rcvPacket;
	private packet sndPacket;

	public MISSION_REQUEST_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public MISSION_REQUEST_class(MISSION_REQUEST_class o)
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
 				+ ", MISSION_REQUEST_seq"
 				+ ", MISSION_REQUEST_target_system"
 				+ ", MISSION_REQUEST_target_component"
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
