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
Message ID: SERIAL_CONTROL(126)
--------------------------------------
%%~ Control a serial port. This can be used for raw access to an onboard serial peripheral 
%%~ such as a GPS or telemetry radio. It is designed to make it possible to update 
%%~ the devices firmware via MAVLink messages or change the devices settings. 
%%~ A message with zero bytes can be used to change just the baudrate.
--------------------------------------
*/
public class SERIAL_CONTROL_class //implements Loggable
{
	public static final int msgID = 126;
	public long	 baudrate; 	// Baudrate of transfer. Zero means no change.
	public int		 timeout;	 	// Timeout for reply data in milliseconds
	public short	 device;	 	// See SERIAL_CONTROL_DEV enum
	public short	 flags;	 	// See SERIAL_CONTROL_FLAG enum
	public short	 count;	 	// how many bytes in this transfer
	public short[]	 data = new short[70];		// serial data

	private packet rcvPacket;
	private packet sndPacket;

	public SERIAL_CONTROL_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public SERIAL_CONTROL_class(SERIAL_CONTROL_class o)
	{
		baudrate = o.baudrate;
		timeout = o.timeout;
		device = o.device;
		flags = o.flags;
		count = o.count;
		data = o.data;
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

			// int[] mavLen = {4, 2, 1, 1, 1, 70};
			// int[] javLen = {8, 4, 2, 2, 2, 140};

			baudrate	= rcvPacket.getLongI();
			timeout	= rcvPacket.getIntS();
			device	= rcvPacket.getShortB();
			flags	= rcvPacket.getShortB();
			count	= rcvPacket.getShortB();
			rcvPacket.getByte(data, 0, 70);
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  baudrate
					 ,timeout
					 ,device
					 ,flags
					 ,count
					 ,data
					 );
	}

	public byte[] encode(
						 long v_baudrate
						,int v_timeout
						,short v_device
						,short v_flags
						,short v_count
						,short[] v_data
						)
	{
		// int[] mavLen = {4, 2, 1, 1, 1, 70};
		// int[] javLen = {8, 4, 2, 2, 2, 140};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_baudrate);	// Add "baudrate" parameter
		sndPacket.putShortI(v_timeout);	// Add "timeout" parameter
		sndPacket.putByteS(v_device);	// Add "device" parameter
		sndPacket.putByteS(v_flags);	// Add "flags" parameter
		sndPacket.putByteS(v_count);	// Add "count" parameter
		sndPacket.putByte(v_data,0,70);	// Add "data" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", SERIAL_CONTROL_baudrate"
 				+ ", SERIAL_CONTROL_timeout"
 				+ ", SERIAL_CONTROL_device"
 				+ ", SERIAL_CONTROL_flags"
 				+ ", SERIAL_CONTROL_count"
 				+ ", SERIAL_CONTROL_data"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + baudrate
 				+ ", " + timeout
 				+ ", " + device
 				+ ", " + flags
 				+ ", " + count
 				+ ", " + data
				);
		return param;
	}
}
