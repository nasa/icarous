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
Message ID: DIGICAM_CONTROL(155)
--------------------------------------
%%~ Control on-board Camera Control System to take shots.
--------------------------------------
*/
public class DIGICAM_CONTROL_class //implements Loggable
{
	public static final int msgID = 155;
	public float	 extra_value;		 	// Correspondent value to given extra_param
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public short	 session;			 	// 0: stop, 1: start or keep it up //Session control e.g. show/hide lens
	public short	 zoom_pos;		 	// 1 to N //Zoom's absolute position (0 means ignore)
	public byte	 zoom_step;		 	// -100 to 100 //Zooming step value to offset zoom from the current position
	public short	 focus_lock;		 	// 0: unlock focus or keep unlocked, 1: lock focus or keep locked, 3: re-lock focus
	public short	 shot;			 	// 0: ignore, 1: shot or start filming
	public short	 command_id;		 	// Command Identity (incremental loop: 0 to 255)//A command sent multiple times will be executed or pooled just once
	public short	 extra_param;		 	// Extra parameters enumeration (0 means ignore)

	private packet rcvPacket;
	private packet sndPacket;

	public DIGICAM_CONTROL_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public DIGICAM_CONTROL_class(DIGICAM_CONTROL_class o)
	{
		extra_value = o.extra_value;
		target_system = o.target_system;
		target_component = o.target_component;
		session = o.session;
		zoom_pos = o.zoom_pos;
		zoom_step = o.zoom_step;
		focus_lock = o.focus_lock;
		shot = o.shot;
		command_id = o.command_id;
		extra_param = o.extra_param;
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

			// int[] mavLen = {4, 1, 1, 1, 1, 1, 1, 1, 1, 1};
			// int[] javLen = {4, 2, 2, 2, 2, 1, 2, 2, 2, 2};

			extra_value		= rcvPacket.getFloat();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			session			= rcvPacket.getShortB();
			zoom_pos			= rcvPacket.getShortB();
			zoom_step		= rcvPacket.getByte();
			focus_lock		= rcvPacket.getShortB();
			shot				= rcvPacket.getShortB();
			command_id		= rcvPacket.getShortB();
			extra_param		= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  extra_value
					 ,(short)1
					 ,(short)1
					 ,session
					 ,zoom_pos
					 ,zoom_step
					 ,focus_lock
					 ,shot
					 ,command_id
					 ,extra_param
					 );
	}

	public byte[] encode(
						 float v_extra_value
						,short v_session
						,short v_zoom_pos
						,byte v_zoom_step
						,short v_focus_lock
						,short v_shot
						,short v_command_id
						,short v_extra_param
						)
	{
		return encode(
					  v_extra_value
					 ,  (short)1
					 ,  (short)1
					 ,v_session
					 ,v_zoom_pos
					 ,v_zoom_step
					 ,v_focus_lock
					 ,v_shot
					 ,v_command_id
					 ,v_extra_param
					 );
	}

	public byte[] encode(
						 float v_extra_value
						,short v_target_system
						,short v_target_component
						,short v_session
						,short v_zoom_pos
						,byte v_zoom_step
						,short v_focus_lock
						,short v_shot
						,short v_command_id
						,short v_extra_param
						)
	{
		// int[] mavLen = {4, 1, 1, 1, 1, 1, 1, 1, 1, 1};
		// int[] javLen = {4, 2, 2, 2, 2, 1, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_extra_value);	// Add "extra_value" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_session);	// Add "session" parameter
		sndPacket.putByteS(v_zoom_pos);	// Add "zoom_pos" parameter
		sndPacket.putByte(v_zoom_step);	// Add "zoom_step" parameter
		sndPacket.putByteS(v_focus_lock);	// Add "focus_lock" parameter
		sndPacket.putByteS(v_shot);	// Add "shot" parameter
		sndPacket.putByteS(v_command_id);	// Add "command_id" parameter
		sndPacket.putByteS(v_extra_param);	// Add "extra_param" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", DIGICAM_CONTROL_extra_value"
 				+ ", DIGICAM_CONTROL_target_system"
 				+ ", DIGICAM_CONTROL_target_component"
 				+ ", DIGICAM_CONTROL_session"
 				+ ", DIGICAM_CONTROL_zoom_pos"
 				+ ", DIGICAM_CONTROL_zoom_step"
 				+ ", DIGICAM_CONTROL_focus_lock"
 				+ ", DIGICAM_CONTROL_shot"
 				+ ", DIGICAM_CONTROL_command_id"
 				+ ", DIGICAM_CONTROL_extra_param"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + extra_value
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + session
 				+ ", " + zoom_pos
 				+ ", " + zoom_step
 				+ ", " + focus_lock
 				+ ", " + shot
 				+ ", " + command_id
 				+ ", " + extra_param
				);
		return param;
	}
}
