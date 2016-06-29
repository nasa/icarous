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
Message ID: AIRSPEED_AUTOCAL(174)
--------------------------------------
%%~ Airspeed auto-calibration
--------------------------------------
*/
public class AIRSPEED_AUTOCAL_class //implements Loggable
{
	public static final int msgID = 174;
	public float	 vx;				 	// GPS velocity north m/s
	public float	 vy;				 	// GPS velocity east m/s
	public float	 vz;				 	// GPS velocity down m/s
	public float	 diff_pressure;	 	// Differential pressure pascals
	public float	 EAS2TAS;			 	// Estimated to true airspeed ratio
	public float	 ratio;			 	// Airspeed ratio
	public float	 state_x;			 	// EKF state x
	public float	 state_y;			 	// EKF state y
	public float	 state_z;			 	// EKF state z
	public float	 Pax;				 	// EKF Pax
	public float	 Pby;				 	// EKF Pby
	public float	 Pcz;				 	// EKF Pcz

	private packet rcvPacket;
	private packet sndPacket;

	public AIRSPEED_AUTOCAL_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public AIRSPEED_AUTOCAL_class(AIRSPEED_AUTOCAL_class o)
	{
		vx = o.vx;
		vy = o.vy;
		vz = o.vz;
		diff_pressure = o.diff_pressure;
		EAS2TAS = o.EAS2TAS;
		ratio = o.ratio;
		state_x = o.state_x;
		state_y = o.state_y;
		state_z = o.state_z;
		Pax = o.Pax;
		Pby = o.Pby;
		Pcz = o.Pcz;
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

			vx				= rcvPacket.getFloat();
			vy				= rcvPacket.getFloat();
			vz				= rcvPacket.getFloat();
			diff_pressure	= rcvPacket.getFloat();
			EAS2TAS			= rcvPacket.getFloat();
			ratio			= rcvPacket.getFloat();
			state_x			= rcvPacket.getFloat();
			state_y			= rcvPacket.getFloat();
			state_z			= rcvPacket.getFloat();
			Pax				= rcvPacket.getFloat();
			Pby				= rcvPacket.getFloat();
			Pcz				= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  vx
					 ,vy
					 ,vz
					 ,diff_pressure
					 ,EAS2TAS
					 ,ratio
					 ,state_x
					 ,state_y
					 ,state_z
					 ,Pax
					 ,Pby
					 ,Pcz
					 );
	}

	public byte[] encode(
						 float v_vx
						,float v_vy
						,float v_vz
						,float v_diff_pressure
						,float v_EAS2TAS
						,float v_ratio
						,float v_state_x
						,float v_state_y
						,float v_state_z
						,float v_Pax
						,float v_Pby
						,float v_Pcz
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_vx);	// Add "vx" parameter
		sndPacket.putFloat(v_vy);	// Add "vy" parameter
		sndPacket.putFloat(v_vz);	// Add "vz" parameter
		sndPacket.putFloat(v_diff_pressure);	// Add "diff_pressure" parameter
		sndPacket.putFloat(v_EAS2TAS);	// Add "EAS2TAS" parameter
		sndPacket.putFloat(v_ratio);	// Add "ratio" parameter
		sndPacket.putFloat(v_state_x);	// Add "state_x" parameter
		sndPacket.putFloat(v_state_y);	// Add "state_y" parameter
		sndPacket.putFloat(v_state_z);	// Add "state_z" parameter
		sndPacket.putFloat(v_Pax);	// Add "Pax" parameter
		sndPacket.putFloat(v_Pby);	// Add "Pby" parameter
		sndPacket.putFloat(v_Pcz);	// Add "Pcz" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", AIRSPEED_AUTOCAL_vx"
 				+ ", AIRSPEED_AUTOCAL_vy"
 				+ ", AIRSPEED_AUTOCAL_vz"
 				+ ", AIRSPEED_AUTOCAL_diff_pressure"
 				+ ", AIRSPEED_AUTOCAL_EAS2TAS"
 				+ ", AIRSPEED_AUTOCAL_ratio"
 				+ ", AIRSPEED_AUTOCAL_state_x"
 				+ ", AIRSPEED_AUTOCAL_state_y"
 				+ ", AIRSPEED_AUTOCAL_state_z"
 				+ ", AIRSPEED_AUTOCAL_Pax"
 				+ ", AIRSPEED_AUTOCAL_Pby"
 				+ ", AIRSPEED_AUTOCAL_Pcz"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + vx
 				+ ", " + vy
 				+ ", " + vz
 				+ ", " + diff_pressure
 				+ ", " + EAS2TAS
 				+ ", " + ratio
 				+ ", " + state_x
 				+ ", " + state_y
 				+ ", " + state_z
 				+ ", " + Pax
 				+ ", " + Pby
 				+ ", " + Pcz
				);
		return param;
	}
}
