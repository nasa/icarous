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
Message ID: AUTOPILOT_VERSION(148)
--------------------------------------
%%~ Version and capability of autopilot software
--------------------------------------
*/
public class AUTOPILOT_VERSION_class //implements Loggable
{
	public static final int msgID = 148;
	public long	 capabilities;				 	// bitmask of capabilities (see MAV_PROTOCOL_CAPABILITY enum)
	public long	 uid;							 	// UID if provided by hardware
	public long	 flight_sw_version;			 	// Firmware version number
	public long	 middleware_sw_version;		 	// Middleware version number
	public long	 os_sw_version;				 	// Operating system version number
	public long	 board_version;				 	// HW / board version (last 8 bytes should be silicon ID, if any)
	public int		 vendor_id;					 	// ID of the board vendor
	public int		 product_id;					 	// ID of the product
	public short[]	 flight_custom_version = new short[8];			// Custom version field, commonly the first 8 bytes of the git hash. This is not an unique identifier, but should allow to identify the commit using the main version number even for very large code bases.
	public short[]	 middleware_custom_version = new short[8];		// Custom version field, commonly the first 8 bytes of the git hash. This is not an unique identifier, but should allow to identify the commit using the main version number even for very large code bases.
	public short[]	 os_custom_version = new short[8];				// Custom version field, commonly the first 8 bytes of the git hash. This is not an unique identifier, but should allow to identify the commit using the main version number even for very large code bases.

	private packet rcvPacket;
	private packet sndPacket;

	public AUTOPILOT_VERSION_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public AUTOPILOT_VERSION_class(AUTOPILOT_VERSION_class o)
	{
		capabilities = o.capabilities;
		uid = o.uid;
		flight_sw_version = o.flight_sw_version;
		middleware_sw_version = o.middleware_sw_version;
		os_sw_version = o.os_sw_version;
		board_version = o.board_version;
		vendor_id = o.vendor_id;
		product_id = o.product_id;
		flight_custom_version = o.flight_custom_version;
		middleware_custom_version = o.middleware_custom_version;
		os_custom_version = o.os_custom_version;
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

			// int[] mavLen = {8, 8, 4, 4, 4, 4, 2, 2, 8, 8, 8};
			// int[] javLen = {8, 8, 8, 8, 8, 8, 4, 4, 16, 16, 16};

			capabilities					= rcvPacket.getLong();
			uid							= rcvPacket.getLong();
			flight_sw_version			= rcvPacket.getLongI();
			middleware_sw_version		= rcvPacket.getLongI();
			os_sw_version				= rcvPacket.getLongI();
			board_version				= rcvPacket.getLongI();
			vendor_id					= rcvPacket.getIntS();
			product_id					= rcvPacket.getIntS();
			rcvPacket.getByte(flight_custom_version, 0, 8);
			rcvPacket.getByte(middleware_custom_version, 0, 8);
			rcvPacket.getByte(os_custom_version, 0, 8);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  capabilities
					 ,uid
					 ,flight_sw_version
					 ,middleware_sw_version
					 ,os_sw_version
					 ,board_version
					 ,vendor_id
					 ,product_id
					 ,flight_custom_version
					 ,middleware_custom_version
					 ,os_custom_version
					 );
	}

	public byte[] encode(
						 long v_capabilities
						,long v_uid
						,long v_flight_sw_version
						,long v_middleware_sw_version
						,long v_os_sw_version
						,long v_board_version
						,int v_vendor_id
						,int v_product_id
						,short[] v_flight_custom_version
						,short[] v_middleware_custom_version
						,short[] v_os_custom_version
						)
	{
		// int[] mavLen = {8, 8, 4, 4, 4, 4, 2, 2, 8, 8, 8};
		// int[] javLen = {8, 8, 8, 8, 8, 8, 4, 4, 16, 16, 16};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putLong(v_capabilities);	// Add "capabilities" parameter
		sndPacket.putLong(v_uid);	// Add "uid" parameter
		sndPacket.putIntL(v_flight_sw_version);	// Add "flight_sw_version" parameter
		sndPacket.putIntL(v_middleware_sw_version);	// Add "middleware_sw_version" parameter
		sndPacket.putIntL(v_os_sw_version);	// Add "os_sw_version" parameter
		sndPacket.putIntL(v_board_version);	// Add "board_version" parameter
		sndPacket.putShortI(v_vendor_id);	// Add "vendor_id" parameter
		sndPacket.putShortI(v_product_id);	// Add "product_id" parameter
		sndPacket.putByte(v_flight_custom_version,0,8);	// Add "flight_custom_version" parameter
		sndPacket.putByte(v_middleware_custom_version,0,8);	// Add "middleware_custom_version" parameter
		sndPacket.putByte(v_os_custom_version,0,8);	// Add "os_custom_version" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", AUTOPILOT_VERSION_capabilities"
 				+ ", AUTOPILOT_VERSION_uid"
 				+ ", AUTOPILOT_VERSION_flight_sw_version"
 				+ ", AUTOPILOT_VERSION_middleware_sw_version"
 				+ ", AUTOPILOT_VERSION_os_sw_version"
 				+ ", AUTOPILOT_VERSION_board_version"
 				+ ", AUTOPILOT_VERSION_vendor_id"
 				+ ", AUTOPILOT_VERSION_product_id"
 				+ ", AUTOPILOT_VERSION_flight_custom_version"
 				+ ", AUTOPILOT_VERSION_middleware_custom_version"
 				+ ", AUTOPILOT_VERSION_os_custom_version"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + capabilities
 				+ ", " + uid
 				+ ", " + flight_sw_version
 				+ ", " + middleware_sw_version
 				+ ", " + os_sw_version
 				+ ", " + board_version
 				+ ", " + vendor_id
 				+ ", " + product_id
 				+ ", " + flight_custom_version
 				+ ", " + middleware_custom_version
 				+ ", " + os_custom_version
				);
		return param;
	}
}
