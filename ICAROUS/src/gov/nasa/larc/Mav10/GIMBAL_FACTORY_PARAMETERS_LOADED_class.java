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
Message ID: GIMBAL_FACTORY_PARAMETERS_LOADED(207)
--------------------------------------
%%~              Sent by the gimbal after the factory parameters are successfully loaded, 
%%~ to inform the factory software that the load is complete         
--------------------------------------
*/
public class GIMBAL_FACTORY_PARAMETERS_LOADED_class //implements Loggable
{
	public static final int msgID = 207;
	public short	 dummy;	 	// Dummy field because mavgen doesn't allow messages with no fields

	private packet rcvPacket;
	private packet sndPacket;

	public GIMBAL_FACTORY_PARAMETERS_LOADED_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public GIMBAL_FACTORY_PARAMETERS_LOADED_class(GIMBAL_FACTORY_PARAMETERS_LOADED_class o)
	{
		dummy = o.dummy;
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

			// int[] mavLen = {1};
			// int[] javLen = {2};

			dummy	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  dummy
					 );
	}

	public byte[] encode(
						 short v_dummy
						)
	{
		// int[] mavLen = {1};
		// int[] javLen = {2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putByteS(v_dummy);	// Add "dummy" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", GIMBAL_FACTORY_PARAMETERS_LOADED_dummy"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + dummy
				);
		return param;
	}
}
