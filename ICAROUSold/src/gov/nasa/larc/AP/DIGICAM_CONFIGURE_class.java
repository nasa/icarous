/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.serial.Loggable;

/**
Message ID: DIGICAM_CONFIGURE(154)
--------------------------------------
%%~ Configure on-board Camera Control System.
--------------------------------------
*/
public class DIGICAM_CONFIGURE_class implements Loggable
{
	public static final int msgID = 154;
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID
	public short	 mode;			 	// Mode enumeration from 1 to N //P, TV, AV, M, Etc (0 means ignore)
	public int		 shutter_speed;	 	// Divisor number //e.g. 1000 means 1/1000 (0 means ignore)
	public short	 aperture;		 	// F stop number x 10 //e.g. 28 means 2.8 (0 means ignore)
	public short	 iso;				 	// ISO enumeration from 1 to N //e.g. 80, 100, 200, Etc (0 means ignore)
	public short	 exposure_type;	 	// Exposure type enumeration from 1 to N (0 means ignore)
	public short	 command_id;		 	// Command Identity (incremental loop: 0 to 255)//A command sent multiple times will be executed or pooled just once
	public short	 engine_cut_off;	 	// Main engine cut-off time before camera trigger in seconds/10 (0 means no cut-off)
	public short	 extra_param;		 	// Extra parameters enumeration (0 means ignore)
	public float	 extra_value;		 	// Correspondent value to given extra_param

	private APPacket rcvPacket;
	private APPacket sndPacket;

	public DIGICAM_CONFIGURE_class()
	{
		rcvPacket = new APPacket(msgID);
		sndPacket = new APPacket(msgID);
		target_system = 1;
		target_component = 1;
	}

	public DIGICAM_CONFIGURE_class(DIGICAM_CONFIGURE_class o)
	{
		target_system = o.target_system;
		target_component = o.target_component;
		mode = o.mode;
		shutter_speed = o.shutter_speed;
		aperture = o.aperture;
		iso = o.iso;
		exposure_type = o.exposure_type;
		command_id = o.command_id;
		engine_cut_off = o.engine_cut_off;
		extra_param = o.extra_param;
		extra_value = o.extra_value;
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

			// int[] mavLen = {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 4};
			// int[] javLen = {2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 4};

			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
			mode				= rcvPacket.getShortB();
			shutter_speed	= rcvPacket.getIntS();
			aperture			= rcvPacket.getShortB();
			iso				= rcvPacket.getShortB();
			exposure_type	= rcvPacket.getShortB();
			command_id		= rcvPacket.getShortB();
			engine_cut_off	= rcvPacket.getShortB();
			extra_param		= rcvPacket.getShortB();
			extra_value		= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  (short)1
					 ,(short)1
					 ,mode
					 ,shutter_speed
					 ,aperture
					 ,iso
					 ,exposure_type
					 ,command_id
					 ,engine_cut_off
					 ,extra_param
					 ,extra_value
					 );
	}

	public byte[] encode(
						 short v_mode
						,int v_shutter_speed
						,short v_aperture
						,short v_iso
						,short v_exposure_type
						,short v_command_id
						,short v_engine_cut_off
						,short v_extra_param
						,float v_extra_value
						)
	{
		return encode(
					    (short)1
					 ,  (short)1
					 ,v_mode
					 ,v_shutter_speed
					 ,v_aperture
					 ,v_iso
					 ,v_exposure_type
					 ,v_command_id
					 ,v_engine_cut_off
					 ,v_extra_param
					 ,v_extra_value
					 );
	}

	public byte[] encode(
						 short v_target_system
						,short v_target_component
						,short v_mode
						,int v_shutter_speed
						,short v_aperture
						,short v_iso
						,short v_exposure_type
						,short v_command_id
						,short v_engine_cut_off
						,short v_extra_param
						,float v_extra_value
						)
	{
		// int[] mavLen = {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 4};
		// int[] javLen = {2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_mode);	// Add "mode" parameter
		sndPacket.putShortI(v_shutter_speed);	// Add "shutter_speed" parameter
		sndPacket.putByteS(v_aperture);	// Add "aperture" parameter
		sndPacket.putByteS(v_iso);	// Add "iso" parameter
		sndPacket.putByteS(v_exposure_type);	// Add "exposure_type" parameter
		sndPacket.putByteS(v_command_id);	// Add "command_id" parameter
		sndPacket.putByteS(v_engine_cut_off);	// Add "engine_cut_off" parameter
		sndPacket.putByteS(v_extra_param);	// Add "extra_param" parameter
		sndPacket.putFloat(v_extra_value);	// Add "extra_value" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", DIGICAM_CONFIGURE_target_system"
 				+ ", DIGICAM_CONFIGURE_target_component"
 				+ ", DIGICAM_CONFIGURE_mode"
 				+ ", DIGICAM_CONFIGURE_shutter_speed"
 				+ ", DIGICAM_CONFIGURE_aperture"
 				+ ", DIGICAM_CONFIGURE_iso"
 				+ ", DIGICAM_CONFIGURE_exposure_type"
 				+ ", DIGICAM_CONFIGURE_command_id"
 				+ ", DIGICAM_CONFIGURE_engine_cut_off"
 				+ ", DIGICAM_CONFIGURE_extra_param"
 				+ ", DIGICAM_CONFIGURE_extra_value"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + mode
 				+ ", " + shutter_speed
 				+ ", " + aperture
 				+ ", " + iso
 				+ ", " + exposure_type
 				+ ", " + command_id
 				+ ", " + engine_cut_off
 				+ ", " + extra_param
 				+ ", " + extra_value
				);
		return param;
	}
}
