/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//MavLink 0.9

package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: TRAFFIC_SIM_TIMING(116)
--------------------------------------
%%~ Message that provides timing information about the traffic simulation
--------------------------------------
*/
public class TRAFFIC_SIM_TIMING_class implements Loggable
{
	public static final int msgID = 116;
	public float	 sim_time;	 	// Traffic simulation absolute time
	public float	 time_to_wpt;	 	// Absolute time to reach the next waypoint

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public TRAFFIC_SIM_TIMING_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
	}

	public TRAFFIC_SIM_TIMING_class(TRAFFIC_SIM_TIMING_class o)
	{
		sim_time = o.sim_time;
		time_to_wpt = o.time_to_wpt;
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

			// int[] mavLen = {4, 4};
			// int[] javLen = {4, 4};

			sim_time		= rcvPacket.getFloat();
			time_to_wpt	= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  sim_time
					 ,time_to_wpt
					 );
	}

	public byte[] encode(
						 float v_sim_time
						,float v_time_to_wpt
						)
	{
		// int[] mavLen = {4, 4};
		// int[] javLen = {4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_sim_time);	// Add "sim_time" parameter
		sndPacket.putFloat(v_time_to_wpt);	// Add "time_to_wpt" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", TRAFFIC_SIM_TIMING_sim_time"
 				+ ", TRAFFIC_SIM_TIMING_time_to_wpt"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + sim_time
 				+ ", " + time_to_wpt
				);
		return param;
	}
}
