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
Message ID: ACTUATOR_CONTROL_TARGET(140)
--------------------------------------
%%~ Set the vehicle attitude and body angular rates.
--------------------------------------
*/
public class ACTUATOR_CONTROL_TARGET_class //implements Loggable
{
	public static final int msgID = 140;
	public long	 time_usec;	 	// Timestamp (micros since boot or Unix epoch)
	public float[]	 controls = new float[8];		// Actuator controls. Normed to -1..+1 where 0 is neutral position. Throttle for single rotation direction motors is 0..1, negative range for reverse direction. Standard mapping for attitude controls (group 0): (index 0-7): roll, pitch, yaw, throttle, flaps, spoilers, airbrakes, landing gear. Load a pass-through mixer to repurpose them as generic outputs.
	public short	 group_mlx;	 	// Actuator group. The "_mlx" indicates this is a multi-instance message and a MAVLink parser should use this field to difference between instances.

	private packet rcvPacket;
	private packet sndPacket;

	public ACTUATOR_CONTROL_TARGET_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public ACTUATOR_CONTROL_TARGET_class(ACTUATOR_CONTROL_TARGET_class o)
	{
		time_usec = o.time_usec;
		controls = o.controls;
		group_mlx = o.group_mlx;
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

			// int[] mavLen = {8, 32, 1};
			// int[] javLen = {8, 32, 2};

			time_usec	= rcvPacket.getLong();
			rcvPacket.getByte(controls, 0, 8);
			group_mlx	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  time_usec
					 ,controls
					 ,group_mlx
					 );
	}

	public byte[] encode(
						 long v_time_usec
						,float[] v_controls
						,short v_group_mlx
						)
	{
		// int[] mavLen = {8, 32, 1};
		// int[] javLen = {8, 32, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_time_usec);	// Add "time_usec" parameter
		sndPacket.putByte(v_controls,0,8);	// Add "controls" parameter
		sndPacket.putByteS(v_group_mlx);	// Add "group_mlx" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", ACTUATOR_CONTROL_TARGET_time_usec"
 				+ ", ACTUATOR_CONTROL_TARGET_controls"
 				+ ", ACTUATOR_CONTROL_TARGET_group_mlx"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + time_usec
 				+ ", " + controls
 				+ ", " + group_mlx
				);
		return param;
	}
}
