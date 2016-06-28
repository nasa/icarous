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
Message ID: GIMBAL_ERASE_FIRMWARE_AND_CONFIG(208)
--------------------------------------
%%~              Commands the gimbal to erase its firmware image and flash configuration, 
%%~ leaving only the bootloader.  The gimbal will then reboot into the bootloader, 
%%~             ready for the load of a new application firmware image.  Erasing 
%%~ the flash configuration will cause the gimbal to re-perform axis calibration when 
%%~ a             new firmware image is loaded, and will cause all tuning parameters 
%%~ to return to their factory defaults.  WARNING: sending this command will render 
%%~ a             gimbal inoperable until a new firmware image is loaded onto it. 
%%~  For this reason, a particular "knock" value must be sent for the command to 
%%~ take effect.             Use this command at your own risk         
--------------------------------------
*/
public class GIMBAL_ERASE_FIRMWARE_AND_CONFIG_class //implements Loggable
{
	public static final int msgID = 208;
	public long	 knock;			 	// Knock value to confirm this is a valid request
	public short	 target_system;	 	// System ID
	public short	 target_component; 	// Component ID

	private packet rcvPacket;
	private packet sndPacket;

	public GIMBAL_ERASE_FIRMWARE_AND_CONFIG_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public GIMBAL_ERASE_FIRMWARE_AND_CONFIG_class(GIMBAL_ERASE_FIRMWARE_AND_CONFIG_class o)
	{
		knock = o.knock;
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

			// int[] mavLen = {4, 1, 1};
			// int[] javLen = {8, 2, 2};

			knock			= rcvPacket.getLongI();
			target_system	= rcvPacket.getShortB();
			target_component	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  knock
					 ,(short)1
					 ,(short)1
					 );
	}

	public byte[] encode(
						 long v_knock
						)
	{
		return encode(
					  v_knock
					 ,  (short)1
					 ,  (short)1
					 );
	}

	public byte[] encode(
						 long v_knock
						,short v_target_system
						,short v_target_component
						)
	{
		// int[] mavLen = {4, 1, 1};
		// int[] javLen = {8, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_knock);	// Add "knock" parameter
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
 				+ ", GIMBAL_ERASE_FIRMWARE_AND_CONFIG_knock"
 				+ ", GIMBAL_ERASE_FIRMWARE_AND_CONFIG_target_system"
 				+ ", GIMBAL_ERASE_FIRMWARE_AND_CONFIG_target_component"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + knock
 				+ ", " + target_system
 				+ ", " + target_component
				);
		return param;
	}
}
