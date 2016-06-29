/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: WAYPOINT(39)
--------------------------------------
%%~ Message encoding a waypoint. This message is emitted to announce      the presence 
%%~ of a waypoint and to set a waypoint on the system. The waypoint can be either 
%%~ in x, y, z meters (type: LOCAL) or x:lat, y:lon, z:altitude. Local frame is Z-down, 
%%~ right handed, global frame is Z-up, right handed
--------------------------------------
*/
public class WAYPOINT_class implements Loggable
{
	public static final int msgID = 39;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public int		 seq;				 	// Sequence
	public short	 frame;			 	// The coordinate system of the waypoint. see MAV_FRAME in mavlink_types.h
	public short	 command;			 	// The scheduled action for the waypoint. see MAV_COMMAND in common.xml MAVLink specs
	public short	 current;			 	// false:0, true:1
	public short	 autocontinue;	 	// autocontinue to next wp
	public float	 param1;			 	// PARAM1 / For NAV command waypoints: Radius in which the waypoint is accepted as reached, in meters
	public float	 param2;			 	// PARAM2 / For NAV command waypoints: Time that the MAV should stay inside the PARAM1 radius before advancing, in milliseconds
	public float	 param3;			 	// PARAM3 / For LOITER command waypoints: Orbit to circle around the waypoint, in meters. If positive the orbit direction should be clockwise, if negative the orbit direction should be counter-clockwise.
	public float	 param4;			 	// PARAM4 / For NAV and LOITER command waypoints: Yaw orientation in degrees, [0..360] 0 = NORTH
	public float	 x;				 	// PARAM5 / local: x position, global: latitude
	public float	 y;				 	// PARAM6 / y position: global: longitude
	public float	 z;				 	// PARAM7 / z position: global: altitude

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public WAYPOINT_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public WAYPOINT_class(WAYPOINT_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		seq = o.seq;
		frame = o.frame;
		command = o.command;
		current = o.current;
		autocontinue = o.autocontinue;
		param1 = o.param1;
		param2 = o.param2;
		param3 = o.param3;
		param4 = o.param4;
		x = o.x;
		y = o.y;
		z = o.z;
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

			// int[] mavLen = {1, 1, 2, 1, 1, 1, 1, 4, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {2, 2, 4, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			seq				= rcvPacket.getIntS();
			frame			= rcvPacket.getShortB();
			command			= rcvPacket.getShortB();
			current			= rcvPacket.getShortB();
			autocontinue		= rcvPacket.getShortB();
			param1			= rcvPacket.getFloat();
			param2			= rcvPacket.getFloat();
			param3			= rcvPacket.getFloat();
			param4			= rcvPacket.getFloat();
			x				= rcvPacket.getFloat();
			y				= rcvPacket.getFloat();
			z				= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,seq
					 ,frame
					 ,command
					 ,current
					 ,autocontinue
					 ,param1
					 ,param2
					 ,param3
					 ,param4
					 ,x
					 ,y
					 ,z
					 );
	}

	public byte[] encode(
						 int v_seq
						,short v_frame
						,short v_command
						,short v_current
						,short v_autocontinue
						,float v_param1
						,float v_param2
						,float v_param3
						,float v_param4
						,float v_x
						,float v_y
						,float v_z
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_seq
					 ,v_frame
					 ,v_command
					 ,v_current
					 ,v_autocontinue
					 ,v_param1
					 ,v_param2
					 ,v_param3
					 ,v_param4
					 ,v_x
					 ,v_y
					 ,v_z
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,int v_seq
						,short v_frame
						,short v_command
						,short v_current
						,short v_autocontinue
						,float v_param1
						,float v_param2
						,float v_param3
						,float v_param4
						,float v_x
						,float v_y
						,float v_z
						)
	{
		// int[] mavLen = {1, 1, 2, 1, 1, 1, 1, 4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {2, 2, 4, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putShortI(v_seq);	// Add "seq" parameter
		sndPacket.putByteS(v_frame);	// Add "frame" parameter
		sndPacket.putByteS(v_command);	// Add "command" parameter
		sndPacket.putByteS(v_current);	// Add "current" parameter
		sndPacket.putByteS(v_autocontinue);	// Add "autocontinue" parameter
		sndPacket.putFloat(v_param1);	// Add "param1" parameter
		sndPacket.putFloat(v_param2);	// Add "param2" parameter
		sndPacket.putFloat(v_param3);	// Add "param3" parameter
		sndPacket.putFloat(v_param4);	// Add "param4" parameter
		sndPacket.putFloat(v_x);	// Add "x" parameter
		sndPacket.putFloat(v_y);	// Add "y" parameter
		sndPacket.putFloat(v_z);	// Add "z" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", WAYPOINT_target_system"
 				+ ", WAYPOINT_target_component"
 				+ ", WAYPOINT_seq"
 				+ ", WAYPOINT_frame"
 				+ ", WAYPOINT_command"
 				+ ", WAYPOINT_current"
 				+ ", WAYPOINT_autocontinue"
 				+ ", WAYPOINT_param1"
 				+ ", WAYPOINT_param2"
 				+ ", WAYPOINT_param3"
 				+ ", WAYPOINT_param4"
 				+ ", WAYPOINT_x"
 				+ ", WAYPOINT_y"
 				+ ", WAYPOINT_z"
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
 				+ ", " + frame
 				+ ", " + command
 				+ ", " + current
 				+ ", " + autocontinue
 				+ ", " + param1
 				+ ", " + param2
 				+ ", " + param3
 				+ ", " + param4
 				+ ", " + x
 				+ ", " + y
 				+ ", " + z
				);
		return param;
	}
}
