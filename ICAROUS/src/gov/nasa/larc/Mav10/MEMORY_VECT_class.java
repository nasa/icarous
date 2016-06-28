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
Message ID: MEMORY_VECT(249)
--------------------------------------
%%~ Send raw controller memory. The use of this message is discouraged for normal packets, 
%%~ but a quite efficient way for testing new messages and getting experimental 
%%~ debug output.
--------------------------------------
*/
public class MEMORY_VECT_class //implements Loggable
{
	public static final int msgID = 249;
	public int		 address;	 	// Starting address of the debug variables
	public short	 ver;		 	// Version code of the type variable. 0=unknown, type ignored and assumed int16_t. 1=as below
	public short	 type;	 	// Type code of the memory variables. for ver = 1: 0=16 x int16_t, 1=16 x uint16_t, 2=16 x Q15, 3=16 x 1Q14
	public byte[]	 value = new byte[32];		// Memory contents at specified address

	private packet rcvPacket;
	private packet sndPacket;

	public MEMORY_VECT_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public MEMORY_VECT_class(MEMORY_VECT_class o)
	{
		address = o.address;
		ver = o.ver;
		type = o.type;
		value = o.value;
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

			// int[] mavLen = {2, 1, 1, 32};
			// int[] javLen = {4, 2, 2, 32};

			address	= rcvPacket.getIntS();
			ver		= rcvPacket.getShortB();
			type		= rcvPacket.getShortB();
			rcvPacket.getByte(value, 0, 32);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  address
					 ,ver
					 ,type
					 ,value
					 );
	}

	public byte[] encode(
						 int v_address
						,short v_ver
						,short v_type
						,byte[] v_value
						)
	{
		// int[] mavLen = {2, 1, 1, 32};
		// int[] javLen = {4, 2, 2, 32};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_address);	// Add "address" parameter
		sndPacket.putByteS(v_ver);	// Add "ver" parameter
		sndPacket.putByteS(v_type);	// Add "type" parameter
		sndPacket.putByte(v_value,0,32);	// Add "value" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", MEMORY_VECT_address"
 				+ ", MEMORY_VECT_ver"
 				+ ", MEMORY_VECT_type"
 				+ ", MEMORY_VECT_value"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + address
 				+ ", " + ver
 				+ ", " + type
 				+ ", " + value
				);
		return param;
	}
}
