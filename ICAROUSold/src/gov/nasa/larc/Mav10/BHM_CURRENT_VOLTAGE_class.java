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
Message ID: BHM_CURRENT_VOLTAGE(238)
--------------------------------------
%%~ Message that provides Battery Current, Voltage, RPM, and an alert status
--------------------------------------
*/
public class BHM_CURRENT_VOLTAGE_class //implements Loggable
{
	public static final int msgID = 238;
	public float	 fwd_mtr_cur;	 	// Fwd Motor Current (A)
	public float	 aft_mtr_cur;	 	// Aft Motor Current (A)
	public float	 RPM;			 	// Revolutions. Per Minute
	public float	 llf20v;		 	// Lower Left Fwd Mtr 20-Volt Battery Voltage (V)
	public float	 ula20v;		 	// Upper Left Aft Mtr 20-Volt Battery Voltage (V)
	public float	 lrf40v;		 	// Lower Right Fwd Mtr 40-Volt Battery Voltage (V)
	public float	 ura40v;		 	// Upper Right Aft Mtr 40-Volt Battery Voltage (V)
	public float	 llf20c;		 	// Lower Left Fwd Mtr 20-Volt Battery Current (A)
	public float	 ula20c;		 	// Upper Left Aft Mtr 20-Volt Battery Current (A)
	public float	 lrf40c;		 	// Lower Right Fwd Mtr 40-Volt Battery Current (A)
	public float	 ura40c;		 	// Upper Right Aft Mtr 40-Volt Battery Current (A)
	public float	 alert;		 	// Alert Flag (boolean)

	private packet rcvPacket;
	private packet sndPacket;

	public BHM_CURRENT_VOLTAGE_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public BHM_CURRENT_VOLTAGE_class(BHM_CURRENT_VOLTAGE_class o)
	{
		fwd_mtr_cur = o.fwd_mtr_cur;
		aft_mtr_cur = o.aft_mtr_cur;
		RPM = o.RPM;
		llf20v = o.llf20v;
		ula20v = o.ula20v;
		lrf40v = o.lrf40v;
		ura40v = o.ura40v;
		llf20c = o.llf20c;
		ula20c = o.ula20c;
		lrf40c = o.lrf40c;
		ura40c = o.ura40c;
		alert = o.alert;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

			fwd_mtr_cur	= rcvPacket.getFloat();
			aft_mtr_cur	= rcvPacket.getFloat();
			RPM			= rcvPacket.getFloat();
			llf20v		= rcvPacket.getFloat();
			ula20v		= rcvPacket.getFloat();
			lrf40v		= rcvPacket.getFloat();
			ura40v		= rcvPacket.getFloat();
			llf20c		= rcvPacket.getFloat();
			ula20c		= rcvPacket.getFloat();
			lrf40c		= rcvPacket.getFloat();
			ura40c		= rcvPacket.getFloat();
			alert		= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  fwd_mtr_cur
					 ,aft_mtr_cur
					 ,RPM
					 ,llf20v
					 ,ula20v
					 ,lrf40v
					 ,ura40v
					 ,llf20c
					 ,ula20c
					 ,lrf40c
					 ,ura40c
					 ,alert
					 );
	}

	public byte[] encode(
						 float v_fwd_mtr_cur
						,float v_aft_mtr_cur
						,float v_RPM
						,float v_llf20v
						,float v_ula20v
						,float v_lrf40v
						,float v_ura40v
						,float v_llf20c
						,float v_ula20c
						,float v_lrf40c
						,float v_ura40c
						,float v_alert
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_fwd_mtr_cur);	// Add "fwd_mtr_cur" parameter
		sndPacket.putFloat(v_aft_mtr_cur);	// Add "aft_mtr_cur" parameter
		sndPacket.putFloat(v_RPM);	// Add "RPM" parameter
		sndPacket.putFloat(v_llf20v);	// Add "llf20v" parameter
		sndPacket.putFloat(v_ula20v);	// Add "ula20v" parameter
		sndPacket.putFloat(v_lrf40v);	// Add "lrf40v" parameter
		sndPacket.putFloat(v_ura40v);	// Add "ura40v" parameter
		sndPacket.putFloat(v_llf20c);	// Add "llf20c" parameter
		sndPacket.putFloat(v_ula20c);	// Add "ula20c" parameter
		sndPacket.putFloat(v_lrf40c);	// Add "lrf40c" parameter
		sndPacket.putFloat(v_ura40c);	// Add "ura40c" parameter
		sndPacket.putFloat(v_alert);	// Add "alert" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", BHM_CURRENT_VOLTAGE_fwd_mtr_cur"
 				+ ", BHM_CURRENT_VOLTAGE_aft_mtr_cur"
 				+ ", BHM_CURRENT_VOLTAGE_RPM"
 				+ ", BHM_CURRENT_VOLTAGE_llf20v"
 				+ ", BHM_CURRENT_VOLTAGE_ula20v"
 				+ ", BHM_CURRENT_VOLTAGE_lrf40v"
 				+ ", BHM_CURRENT_VOLTAGE_ura40v"
 				+ ", BHM_CURRENT_VOLTAGE_llf20c"
 				+ ", BHM_CURRENT_VOLTAGE_ula20c"
 				+ ", BHM_CURRENT_VOLTAGE_lrf40c"
 				+ ", BHM_CURRENT_VOLTAGE_ura40c"
 				+ ", BHM_CURRENT_VOLTAGE_alert"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + fwd_mtr_cur
 				+ ", " + aft_mtr_cur
 				+ ", " + RPM
 				+ ", " + llf20v
 				+ ", " + ula20v
 				+ ", " + lrf40v
 				+ ", " + ura40v
 				+ ", " + llf20c
 				+ ", " + ula20c
 				+ ", " + lrf40c
 				+ ", " + ura40c
 				+ ", " + alert
				);
		return param;
	}
}
