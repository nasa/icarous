/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: WAYPOINT_COUNT(44)
--------------------------------------
%%~ This message is emitted as response to WAYPOINT_REQUEST_LIST by the MAV. The GCS 
%%~ can then request the individual waypoints based on the knowledge of the total number 
%%~ of waypoints.
--------------------------------------
*/
public class WAYPOINT_COUNT_class implements Loggable
{
	public static final int msgID = 44;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public int		 count;			 	// Number of Waypoints in the Sequence

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public WAYPOINT_COUNT_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public WAYPOINT_COUNT_class(WAYPOINT_COUNT_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		count = o.count;
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
			count			= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,count
					 );
	}

	public byte[] encode(
						 int v_count
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_count
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,int v_count
						)
	{
		// int[] mavLen = {1, 1, 2};
		// int[] javLen = {2, 2, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putShortI(v_count);	// Add "count" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", WAYPOINT_COUNT_target_system"
 				+ ", WAYPOINT_COUNT_target_component"
 				+ ", WAYPOINT_COUNT_count"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + count
				);
		return param;
	}
}
