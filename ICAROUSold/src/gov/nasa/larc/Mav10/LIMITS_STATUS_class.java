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
Message ID: LIMITS_STATUS(167)
--------------------------------------
%%~ Status of AP_Limits. Sent in extended      status stream when AP_Limits is enabled
--------------------------------------
*/
public class LIMITS_STATUS_class //implements Loggable
{
	public static final int msgID = 167;
	public long	 last_trigger;	 	// time of last breach in milliseconds since boot
	public long	 last_action;		 	// time of last recovery action in milliseconds since boot
	public long	 last_recovery;	 	// time of last successful recovery in milliseconds since boot
	public long	 last_clear;		 	// time of last all-clear in milliseconds since boot
	public int		 breach_count;	 	// number of fence breaches
	public short	 limits_state;	 	// state of AP_Limits, (see enum LimitState, LIMITS_STATE)
	public short	 mods_enabled;	 	// AP_Limit_Module bitfield of enabled modules, (see enum moduleid or LIMIT_MODULE)
	public short	 mods_required;	 	// AP_Limit_Module bitfield of required modules, (see enum moduleid or LIMIT_MODULE)
	public short	 mods_triggered;	 	// AP_Limit_Module bitfield of triggered modules, (see enum moduleid or LIMIT_MODULE)

	private packet rcvPacket;
	private packet sndPacket;

	public LIMITS_STATUS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public LIMITS_STATUS_class(LIMITS_STATUS_class o)
	{
		last_trigger = o.last_trigger;
		last_action = o.last_action;
		last_recovery = o.last_recovery;
		last_clear = o.last_clear;
		breach_count = o.breach_count;
		limits_state = o.limits_state;
		mods_enabled = o.mods_enabled;
		mods_required = o.mods_required;
		mods_triggered = o.mods_triggered;
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

			// int[] mavLen = {4, 4, 4, 4, 2, 1, 1, 1, 1};
			// int[] javLen = {8, 8, 8, 8, 4, 2, 2, 2, 2};

			last_trigger		= rcvPacket.getLongI();
			last_action		= rcvPacket.getLongI();
			last_recovery	= rcvPacket.getLongI();
			last_clear		= rcvPacket.getLongI();
			breach_count		= rcvPacket.getIntS();
			limits_state		= rcvPacket.getShortB();
			mods_enabled		= rcvPacket.getShortB();
			mods_required	= rcvPacket.getShortB();
			mods_triggered	= rcvPacket.getShortB();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  last_trigger
					 ,last_action
					 ,last_recovery
					 ,last_clear
					 ,breach_count
					 ,limits_state
					 ,mods_enabled
					 ,mods_required
					 ,mods_triggered
					 );
	}

	public byte[] encode(
						 long v_last_trigger
						,long v_last_action
						,long v_last_recovery
						,long v_last_clear
						,int v_breach_count
						,short v_limits_state
						,short v_mods_enabled
						,short v_mods_required
						,short v_mods_triggered
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 2, 1, 1, 1, 1};
		// int[] javLen = {8, 8, 8, 8, 4, 2, 2, 2, 2};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putIntL(v_last_trigger);	// Add "last_trigger" parameter
		sndPacket.putIntL(v_last_action);	// Add "last_action" parameter
		sndPacket.putIntL(v_last_recovery);	// Add "last_recovery" parameter
		sndPacket.putIntL(v_last_clear);	// Add "last_clear" parameter
		sndPacket.putShortI(v_breach_count);	// Add "breach_count" parameter
		sndPacket.putByteS(v_limits_state);	// Add "limits_state" parameter
		sndPacket.putByteS(v_mods_enabled);	// Add "mods_enabled" parameter
		sndPacket.putByteS(v_mods_required);	// Add "mods_required" parameter
		sndPacket.putByteS(v_mods_triggered);	// Add "mods_triggered" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", LIMITS_STATUS_last_trigger"
 				+ ", LIMITS_STATUS_last_action"
 				+ ", LIMITS_STATUS_last_recovery"
 				+ ", LIMITS_STATUS_last_clear"
 				+ ", LIMITS_STATUS_breach_count"
 				+ ", LIMITS_STATUS_limits_state"
 				+ ", LIMITS_STATUS_mods_enabled"
 				+ ", LIMITS_STATUS_mods_required"
 				+ ", LIMITS_STATUS_mods_triggered"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + last_trigger
 				+ ", " + last_action
 				+ ", " + last_recovery
 				+ ", " + last_clear
 				+ ", " + breach_count
 				+ ", " + limits_state
 				+ ", " + mods_enabled
 				+ ", " + mods_required
 				+ ", " + mods_triggered
				);
		return param;
	}
}
