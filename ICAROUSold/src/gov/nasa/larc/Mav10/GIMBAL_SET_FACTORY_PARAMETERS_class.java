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
Message ID: GIMBAL_SET_FACTORY_PARAMETERS(206)
--------------------------------------
%%~              Set factory configuration parameters (such as assembly date and time, 
%%~ and serial number).  This is only intended to be used             during manufacture, 
%%~ not by end users, so it is protected by a simple checksum of sorts (this 
%%~ won't stop anybody determined,             it's mostly just to keep the average 
%%~ user from trying to modify these values.  This will need to be revisited if that 
%%~ isn't             adequate.         
--------------------------------------
*/
public class GIMBAL_SET_FACTORY_PARAMETERS_class //implements Loggable
{
	public static final int msgID = 206;
	public long	 magic_1;				 	// Magic number 1 for validation
	public long	 magic_2;				 	// Magic number 2 for validation
	public long	 magic_3;				 	// Magic number 3 for validation
	public long	 serial_number_pt_1;	 	// Unit Serial Number Part 1 (part code, design, language/country)
	public long	 serial_number_pt_2;	 	// Unit Serial Number Part 2 (option, year, month)
	public long	 serial_number_pt_3;	 	// Unit Serial Number Part 3 (incrementing serial number per month)
	public int		 assembly_year;		 	// Assembly Date Year
	public short	 target_system;		 	// System ID
	public short	 target_component;	 	// Component ID
	public short	 assembly_month;		 	// Assembly Date Month
	public short	 assembly_day;		 	// Assembly Date Day
	public short	 assembly_hour;		 	// Assembly Time Hour
	public short	 assembly_minute;		 	// Assembly Time Minute
	public short	 assembly_second;		 	// Assembly Time Second

	private packet rcvPacket;
	private packet sndPacket;

	public GIMBAL_SET_FACTORY_PARAMETERS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
		target_system = 1;
		target_component = 1;
	}

	public GIMBAL_SET_FACTORY_PARAMETERS_class(GIMBAL_SET_FACTORY_PARAMETERS_class o)
	{
		magic_1 = o.magic_1;
		magic_2 = o.magic_2;
		magic_3 = o.magic_3;
		serial_number_pt_1 = o.serial_number_pt_1;
		serial_number_pt_2 = o.serial_number_pt_2;
		serial_number_pt_3 = o.serial_number_pt_3;
		assembly_year = o.assembly_year;
		target_system = o.target_system;
		target_component = o.target_component;
		assembly_month = o.assembly_month;
		assembly_day = o.assembly_day;
		assembly_hour = o.assembly_hour;
		assembly_minute = o.assembly_minute;
		assembly_second = o.assembly_second;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 1};
			// int[] javLen = {8, 8, 8, 8, 8, 8, 4, 2, 2, 2, 2, 2, 2, 2};

			magic_1				= rcvPacket.getLongI();
			magic_2				= rcvPacket.getLongI();
			magic_3				= rcvPacket.getLongI();
			serial_number_pt_1	= rcvPacket.getLongI();
			serial_number_pt_2	= rcvPacket.getLongI();
			serial_number_pt_3	= rcvPacket.getLongI();
			assembly_year		= rcvPacket.getIntS();
			target_system		= rcvPacket.getShortB();
			target_component		= rcvPacket.getShortB();
			assembly_month		= rcvPacket.getShortB();
			assembly_day			= rcvPacket.getShortB();
			assembly_hour		= rcvPacket.getShortB();
			assembly_minute		= rcvPacket.getShortB();
			assembly_second		= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  magic_1
					 ,magic_2
					 ,magic_3
					 ,serial_number_pt_1
					 ,serial_number_pt_2
					 ,serial_number_pt_3
					 ,assembly_year
					 ,(short)1
					 ,(short)1
					 ,assembly_month
					 ,assembly_day
					 ,assembly_hour
					 ,assembly_minute
					 ,assembly_second
					 );
	}

	public byte[] encode(
						 long v_magic_1
						,long v_magic_2
						,long v_magic_3
						,long v_serial_number_pt_1
						,long v_serial_number_pt_2
						,long v_serial_number_pt_3
						,int v_assembly_year
						,short v_assembly_month
						,short v_assembly_day
						,short v_assembly_hour
						,short v_assembly_minute
						,short v_assembly_second
						)
	{
		return encode(
					  v_magic_1
					 ,v_magic_2
					 ,v_magic_3
					 ,v_serial_number_pt_1
					 ,v_serial_number_pt_2
					 ,v_serial_number_pt_3
					 ,v_assembly_year
					 ,  (short)1
					 ,  (short)1
					 ,v_assembly_month
					 ,v_assembly_day
					 ,v_assembly_hour
					 ,v_assembly_minute
					 ,v_assembly_second
					 );
	}

	public byte[] encode(
						 long v_magic_1
						,long v_magic_2
						,long v_magic_3
						,long v_serial_number_pt_1
						,long v_serial_number_pt_2
						,long v_serial_number_pt_3
						,int v_assembly_year
						,short v_target_system
						,short v_target_component
						,short v_assembly_month
						,short v_assembly_day
						,short v_assembly_hour
						,short v_assembly_minute
						,short v_assembly_second
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 1};
		// int[] javLen = {8, 8, 8, 8, 8, 8, 4, 2, 2, 2, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_magic_1);	// Add "magic_1" parameter
		sndPacket.putIntL(v_magic_2);	// Add "magic_2" parameter
		sndPacket.putIntL(v_magic_3);	// Add "magic_3" parameter
		sndPacket.putIntL(v_serial_number_pt_1);	// Add "serial_number_pt_1" parameter
		sndPacket.putIntL(v_serial_number_pt_2);	// Add "serial_number_pt_2" parameter
		sndPacket.putIntL(v_serial_number_pt_3);	// Add "serial_number_pt_3" parameter
		sndPacket.putShortI(v_assembly_year);	// Add "assembly_year" parameter
		sndPacket.putByteS(v_target_system);	// Add "target_system" parameter
		sndPacket.putByteS(v_target_component);	// Add "target_component" parameter
		sndPacket.putByteS(v_assembly_month);	// Add "assembly_month" parameter
		sndPacket.putByteS(v_assembly_day);	// Add "assembly_day" parameter
		sndPacket.putByteS(v_assembly_hour);	// Add "assembly_hour" parameter
		sndPacket.putByteS(v_assembly_minute);	// Add "assembly_minute" parameter
		sndPacket.putByteS(v_assembly_second);	// Add "assembly_second" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_magic_1"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_magic_2"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_magic_3"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_serial_number_pt_1"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_serial_number_pt_2"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_serial_number_pt_3"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_assembly_year"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_target_system"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_target_component"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_assembly_month"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_assembly_day"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_assembly_hour"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_assembly_minute"
 				+ ", GIMBAL_SET_FACTORY_PARAMETERS_assembly_second"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + magic_1
 				+ ", " + magic_2
 				+ ", " + magic_3
 				+ ", " + serial_number_pt_1
 				+ ", " + serial_number_pt_2
 				+ ", " + serial_number_pt_3
 				+ ", " + assembly_year
 				+ ", " + target_system
 				+ ", " + target_component
 				+ ", " + assembly_month
 				+ ", " + assembly_day
 				+ ", " + assembly_hour
 				+ ", " + assembly_minute
 				+ ", " + assembly_second
				);
		return param;
	}
}
