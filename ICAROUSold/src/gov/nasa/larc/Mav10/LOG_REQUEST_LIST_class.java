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
Message ID: LOG_REQUEST_LIST(117)
--------------------------------------
%%~ Request a list of available logs. On some systems calling this may stop on-board 
%%~ logging until LOG_REQUEST_END is called.
--------------------------------------
*/
public class LOG_REQUEST_LIST_class //implements Loggable
{
	public static final int msgID = 117;
	public int		 start;			 	// First log id (0 for first available)
	public int		 end;				 	// Last log id (0xffff for last available)
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID

	private packet rcvPacket;
	private packet sndPacket;

	public LOG_REQUEST_LIST_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public LOG_REQUEST_LIST_class(LOG_REQUEST_LIST_class o)
	{
		start = o.start;
		end = o.end;
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
			// int[] javLen = {4, 4, 2, 2};

			start			= rcvPacket.getIntS();
			end				= rcvPacket.getIntS();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  start
					 ,end
					 ,(short)1
					 ,(short)1
					 );
	}

	public byte[] encode(
						 int v_start
						,int v_end
						)
	{
		return encode(
					  v_start
					 ,v_end
					 ,  (short)1
					 ,  (short)1
					 );
	}

	public byte[] encode(
						 int v_start
						,int v_end
						,short v_target_system
						,short v_target_component
						)
	{
		// int[] mavLen = {2, 2, 1, 1};
		// int[] javLen = {4, 4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_start);	// Add "start" parameter
		sndPacket.putShortI(v_end);	// Add "end" parameter
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
 				+ ", LOG_REQUEST_LIST_start"
 				+ ", LOG_REQUEST_LIST_end"
 				+ ", LOG_REQUEST_LIST_target_system"
 				+ ", LOG_REQUEST_LIST_target_component"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + start
 				+ ", " + end
 				+ ", " + target_system
 				+ ", " + target_component
				);
		return param;
	}
}
