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
Message ID: CDNR_CONTROLLER(232)
--------------------------------------
%%~ Message from Conflict Detection and Resolution monitor to aircraft. This is a command 
%%~ to resolve conflicts and includes flags and values for heading,altitude,and 
%%~ speed changes as well as a max time duration
--------------------------------------
*/
public class CDNR_CONTROLLER_class //implements Loggable
{
	public static final int msgID = 232;
	public short	 new_heading;	 	// value for new heading
	public short	 new_airspeed; 	// value for new airspeed
	public short	 new_altitude; 	// value for new altitude
	public short	 max_time;	 	// maximum time to hold this command
	public byte	 h_flag;		 	// enables/disables new heading command
	public byte	 s_flag;		 	// enables/disables new airspeed command
	public byte	 a_flag;		 	// enables/disables new altitude command
	public byte	 t_flag;		 	// enables/disables max time command

	private packet rcvPacket;
	private packet sndPacket;

	public CDNR_CONTROLLER_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public CDNR_CONTROLLER_class(CDNR_CONTROLLER_class o)
	{
		new_heading = o.new_heading;
		new_airspeed = o.new_airspeed;
		new_altitude = o.new_altitude;
		max_time = o.max_time;
		h_flag = o.h_flag;
		s_flag = o.s_flag;
		a_flag = o.a_flag;
		t_flag = o.t_flag;
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

			// int[] mavLen = {2, 2, 2, 2, 1, 1, 1, 1};
			// int[] javLen = {2, 2, 2, 2, 1, 1, 1, 1};

			new_heading	= rcvPacket.getShort();
			new_airspeed	= rcvPacket.getShort();
			new_altitude	= rcvPacket.getShort();
			max_time		= rcvPacket.getShort();
			h_flag		= rcvPacket.getByte();
			s_flag		= rcvPacket.getByte();
			a_flag		= rcvPacket.getByte();
			t_flag		= rcvPacket.getByte();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  new_heading
					 ,new_airspeed
					 ,new_altitude
					 ,max_time
					 ,h_flag
					 ,s_flag
					 ,a_flag
					 ,t_flag
					 );
	}

	public byte[] encode(
						 short v_new_heading
						,short v_new_airspeed
						,short v_new_altitude
						,short v_max_time
						,byte v_h_flag
						,byte v_s_flag
						,byte v_a_flag
						,byte v_t_flag
						)
	{
		// int[] mavLen = {2, 2, 2, 2, 1, 1, 1, 1};
		// int[] javLen = {2, 2, 2, 2, 1, 1, 1, 1};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShort(v_new_heading);	// Add "new_heading" parameter
		sndPacket.putShort(v_new_airspeed);	// Add "new_airspeed" parameter
		sndPacket.putShort(v_new_altitude);	// Add "new_altitude" parameter
		sndPacket.putShort(v_max_time);	// Add "max_time" parameter
		sndPacket.putByte(v_h_flag);	// Add "h_flag" parameter
		sndPacket.putByte(v_s_flag);	// Add "s_flag" parameter
		sndPacket.putByte(v_a_flag);	// Add "a_flag" parameter
		sndPacket.putByte(v_t_flag);	// Add "t_flag" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", CDNR_CONTROLLER_new_heading"
 				+ ", CDNR_CONTROLLER_new_airspeed"
 				+ ", CDNR_CONTROLLER_new_altitude"
 				+ ", CDNR_CONTROLLER_max_time"
 				+ ", CDNR_CONTROLLER_h_flag"
 				+ ", CDNR_CONTROLLER_s_flag"
 				+ ", CDNR_CONTROLLER_a_flag"
 				+ ", CDNR_CONTROLLER_t_flag"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + new_heading
 				+ ", " + new_airspeed
 				+ ", " + new_altitude
 				+ ", " + max_time
 				+ ", " + h_flag
 				+ ", " + s_flag
 				+ ", " + a_flag
 				+ ", " + t_flag
				);
		return param;
	}
}
