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
Message ID: BHM_SOC_EOD_RUL(237)
--------------------------------------
%%~ Message that provides Battery End-Of-Discharge(EOD), Remaining-Useful-Life(RUL), 
%%~ and State-Of-Charge(SOC) information
--------------------------------------
*/
public class BHM_SOC_EOD_RUL_class //implements Loggable
{
	public static final int msgID = 237;
	public float	 ura_eod;	 	// Upper Right Aft Motor EOD (seconds)
	public float	 ura_rul;	 	// Upper Right Aft Motor RUL (seconds
	public float	 ura_soc;	 	// Upper Right Aft Motor SOC (percent)
	public float	 lrf_eod;	 	// Lower Right Fwd Motor EOD (seconds)
	public float	 lrf_rul;	 	// Lower Right Fwd Motor RUL (seconds
	public float	 lrf_soc;	 	// Lower Right Fwd Motor SOC (percent)
	public float	 ula_eod;	 	// Upper Left Aft Motor EOD (seconds)
	public float	 ula_rul;	 	// Upper Left Aft Motor RUL (seconds
	public float	 ula_soc;	 	// Upper Left Aft Motor SOC (percent)
	public float	 llf_eod;	 	// Lower Left Fwd Motor EOD (seconds)
	public float	 llf_rul;	 	// Lower Left Fwd Motor RUL (seconds
	public float	 llf_soc;	 	// Lower Left Fwd Motor SOC (percent)

	private packet rcvPacket;
	private packet sndPacket;

	public BHM_SOC_EOD_RUL_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public BHM_SOC_EOD_RUL_class(BHM_SOC_EOD_RUL_class o)
	{
		ura_eod = o.ura_eod;
		ura_rul = o.ura_rul;
		ura_soc = o.ura_soc;
		lrf_eod = o.lrf_eod;
		lrf_rul = o.lrf_rul;
		lrf_soc = o.lrf_soc;
		ula_eod = o.ula_eod;
		ula_rul = o.ula_rul;
		ula_soc = o.ula_soc;
		llf_eod = o.llf_eod;
		llf_rul = o.llf_rul;
		llf_soc = o.llf_soc;
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

			ura_eod	= rcvPacket.getFloat();
			ura_rul	= rcvPacket.getFloat();
			ura_soc	= rcvPacket.getFloat();
			lrf_eod	= rcvPacket.getFloat();
			lrf_rul	= rcvPacket.getFloat();
			lrf_soc	= rcvPacket.getFloat();
			ula_eod	= rcvPacket.getFloat();
			ula_rul	= rcvPacket.getFloat();
			ula_soc	= rcvPacket.getFloat();
			llf_eod	= rcvPacket.getFloat();
			llf_rul	= rcvPacket.getFloat();
			llf_soc	= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  ura_eod
					 ,ura_rul
					 ,ura_soc
					 ,lrf_eod
					 ,lrf_rul
					 ,lrf_soc
					 ,ula_eod
					 ,ula_rul
					 ,ula_soc
					 ,llf_eod
					 ,llf_rul
					 ,llf_soc
					 );
	}

	public byte[] encode(
						 float v_ura_eod
						,float v_ura_rul
						,float v_ura_soc
						,float v_lrf_eod
						,float v_lrf_rul
						,float v_lrf_soc
						,float v_ula_eod
						,float v_ula_rul
						,float v_ula_soc
						,float v_llf_eod
						,float v_llf_rul
						,float v_llf_soc
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_ura_eod);	// Add "ura_eod" parameter
		sndPacket.putFloat(v_ura_rul);	// Add "ura_rul" parameter
		sndPacket.putFloat(v_ura_soc);	// Add "ura_soc" parameter
		sndPacket.putFloat(v_lrf_eod);	// Add "lrf_eod" parameter
		sndPacket.putFloat(v_lrf_rul);	// Add "lrf_rul" parameter
		sndPacket.putFloat(v_lrf_soc);	// Add "lrf_soc" parameter
		sndPacket.putFloat(v_ula_eod);	// Add "ula_eod" parameter
		sndPacket.putFloat(v_ula_rul);	// Add "ula_rul" parameter
		sndPacket.putFloat(v_ula_soc);	// Add "ula_soc" parameter
		sndPacket.putFloat(v_llf_eod);	// Add "llf_eod" parameter
		sndPacket.putFloat(v_llf_rul);	// Add "llf_rul" parameter
		sndPacket.putFloat(v_llf_soc);	// Add "llf_soc" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", BHM_SOC_EOD_RUL_ura_eod"
 				+ ", BHM_SOC_EOD_RUL_ura_rul"
 				+ ", BHM_SOC_EOD_RUL_ura_soc"
 				+ ", BHM_SOC_EOD_RUL_lrf_eod"
 				+ ", BHM_SOC_EOD_RUL_lrf_rul"
 				+ ", BHM_SOC_EOD_RUL_lrf_soc"
 				+ ", BHM_SOC_EOD_RUL_ula_eod"
 				+ ", BHM_SOC_EOD_RUL_ula_rul"
 				+ ", BHM_SOC_EOD_RUL_ula_soc"
 				+ ", BHM_SOC_EOD_RUL_llf_eod"
 				+ ", BHM_SOC_EOD_RUL_llf_rul"
 				+ ", BHM_SOC_EOD_RUL_llf_soc"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + ura_eod
 				+ ", " + ura_rul
 				+ ", " + ura_soc
 				+ ", " + lrf_eod
 				+ ", " + lrf_rul
 				+ ", " + lrf_soc
 				+ ", " + ula_eod
 				+ ", " + ula_rul
 				+ ", " + ula_soc
 				+ ", " + llf_eod
 				+ ", " + llf_rul
 				+ ", " + llf_soc
				);
		return param;
	}
}
