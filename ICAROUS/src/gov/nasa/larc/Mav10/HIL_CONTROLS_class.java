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
Message ID: HIL_CONTROLS(91)
--------------------------------------
%%~ Sent from autopilot to simulation. Hardware in the loop control outputs
--------------------------------------
*/
public class HIL_CONTROLS_class //implements Loggable
{
	public static final int msgID = 91;
	public long	 time_usec;		 	// Timestamp (microseconds since UNIX epoch or microseconds since system boot)
	public float	 roll_ailerons;	 	// Control output -1 .. 1
	public float	 pitch_elevator;	 	// Control output -1 .. 1
	public float	 yaw_rudder;		 	// Control output -1 .. 1
	public float	 throttle;		 	// Throttle 0 .. 1
	public float	 aux1;			 	// Aux 1, -1 .. 1
	public float	 aux2;			 	// Aux 2, -1 .. 1
	public float	 aux3;			 	// Aux 3, -1 .. 1
	public float	 aux4;			 	// Aux 4, -1 .. 1
	public short	 mode;			 	// System mode (MAV_MODE)
	public short	 nav_mode;		 	// Navigation mode (MAV_NAV_MODE)

	private packet rcvPacket;
	private packet sndPacket;

	public HIL_CONTROLS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public HIL_CONTROLS_class(HIL_CONTROLS_class o)
	{
		time_usec = o.time_usec;
		roll_ailerons = o.roll_ailerons;
		pitch_elevator = o.pitch_elevator;
		yaw_rudder = o.yaw_rudder;
		throttle = o.throttle;
		aux1 = o.aux1;
		aux2 = o.aux2;
		aux3 = o.aux3;
		aux4 = o.aux4;
		mode = o.mode;
		nav_mode = o.nav_mode;
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

			// int[] mavLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1};
			// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2};

			time_usec		= rcvPacket.getLong();
			roll_ailerons	= rcvPacket.getFloat();
			pitch_elevator	= rcvPacket.getFloat();
			yaw_rudder		= rcvPacket.getFloat();
			throttle			= rcvPacket.getFloat();
			aux1				= rcvPacket.getFloat();
			aux2				= rcvPacket.getFloat();
			aux3				= rcvPacket.getFloat();
			aux4				= rcvPacket.getFloat();
			mode				= rcvPacket.getShortB();
			nav_mode			= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_usec
					 ,roll_ailerons
					 ,pitch_elevator
					 ,yaw_rudder
					 ,throttle
					 ,aux1
					 ,aux2
					 ,aux3
					 ,aux4
					 ,mode
					 ,nav_mode
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,float v_roll_ailerons
						,float v_pitch_elevator
						,float v_yaw_rudder
						,float v_throttle
						,float v_aux1
						,float v_aux2
						,float v_aux3
						,float v_aux4
						,short v_mode
						,short v_nav_mode
						)
	{
		// int[] mavLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1};
		// int[] javLen = {8, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
		sndPacket.putFloat(v_roll_ailerons);	// Add "roll_ailerons" parameter
		sndPacket.putFloat(v_pitch_elevator);	// Add "pitch_elevator" parameter
		sndPacket.putFloat(v_yaw_rudder);	// Add "yaw_rudder" parameter
		sndPacket.putFloat(v_throttle);	// Add "throttle" parameter
		sndPacket.putFloat(v_aux1);	// Add "aux1" parameter
		sndPacket.putFloat(v_aux2);	// Add "aux2" parameter
		sndPacket.putFloat(v_aux3);	// Add "aux3" parameter
		sndPacket.putFloat(v_aux4);	// Add "aux4" parameter
		sndPacket.putByteS(v_mode);	// Add "mode" parameter
		sndPacket.putByteS(v_nav_mode);	// Add "nav_mode" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", HIL_CONTROLS_time_usec"
 				+ ", HIL_CONTROLS_roll_ailerons"
 				+ ", HIL_CONTROLS_pitch_elevator"
 				+ ", HIL_CONTROLS_yaw_rudder"
 				+ ", HIL_CONTROLS_throttle"
 				+ ", HIL_CONTROLS_aux1"
 				+ ", HIL_CONTROLS_aux2"
 				+ ", HIL_CONTROLS_aux3"
 				+ ", HIL_CONTROLS_aux4"
 				+ ", HIL_CONTROLS_mode"
 				+ ", HIL_CONTROLS_nav_mode"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
 				+ ", " + roll_ailerons
 				+ ", " + pitch_elevator
 				+ ", " + yaw_rudder
 				+ ", " + throttle
 				+ ", " + aux1
 				+ ", " + aux2
 				+ ", " + aux3
 				+ ", " + aux4
 				+ ", " + mode
 				+ ", " + nav_mode
				);
		return param;
	}
}
