/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: WAYPOINT_REQUEST(40)
--------------------------------------
%%~ Request the information of the waypoint with the sequence number seq. The response 
%%~ of the system to this message should be a WAYPOINT message.
--------------------------------------
*/
public class WAYPOINT_REQUEST_class implements Loggable
{
	public static final int msgID = 40;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public int		 seq;				 	// Sequence

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public WAYPOINT_REQUEST_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public WAYPOINT_REQUEST_class(WAYPOINT_REQUEST_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		seq = o.seq;
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

			// int[] mavLen = {1, 1, 2};
			// int[] javLen = {2, 2, 4};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			seq				= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,seq
					 );
	}

	public byte[] encode(
						 int v_seq
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_seq
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,int v_seq
						)
	{
		// int[] mavLen = {1, 1, 2};
		// int[] javLen = {2, 2, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putShortI(v_seq);	// Add "seq" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", WAYPOINT_REQUEST_target_system"
 				+ ", WAYPOINT_REQUEST_target_component"
 				+ ", WAYPOINT_REQUEST_seq"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + seq
				);
		return param;
	}
}
