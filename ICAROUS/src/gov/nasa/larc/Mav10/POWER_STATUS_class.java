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
Message ID: POWER_STATUS(125)
--------------------------------------
%%~ Power supply status
--------------------------------------
*/
public class POWER_STATUS_class //implements Loggable
{
	public static final int msgID = 125;
	public int	 Vcc;		 	// 5V rail voltage in millivolts
	public int	 Vservo;	 	// servo rail voltage in millivolts
	public int	 flags;	 	// power supply status flags (see MAV_POWER_STATUS enum)

	private packet rcvPacket;
	private packet sndPacket;

	public POWER_STATUS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public POWER_STATUS_class(POWER_STATUS_class o)
	{
		Vcc = o.Vcc;
		Vservo = o.Vservo;
		flags = o.flags;
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

			// int[] mavLen = {2, 2, 2};
			// int[] javLen = {4, 4, 4};

			Vcc		= rcvPacket.getIntS();
			Vservo	= rcvPacket.getIntS();
			flags	= rcvPacket.getIntS();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  Vcc
					 ,Vservo
					 ,flags
					 );
	}

	public byte[] encode(
						 int v_Vcc
						,int v_Vservo
						,int v_flags
						)
	{
		// int[] mavLen = {2, 2, 2};
		// int[] javLen = {4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putShortI(v_Vcc);	// Add "Vcc" parameter
		sndPacket.putShortI(v_Vservo);	// Add "Vservo" parameter
		sndPacket.putShortI(v_flags);	// Add "flags" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", POWER_STATUS_Vcc"
 				+ ", POWER_STATUS_Vservo"
 				+ ", POWER_STATUS_flags"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + Vcc
 				+ ", " + Vservo
 				+ ", " + flags
				);
		return param;
	}
}
