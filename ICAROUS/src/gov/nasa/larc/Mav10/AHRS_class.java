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
Message ID: AHRS(163)
--------------------------------------
%%~ Status of DCM attitude estimator
--------------------------------------
*/
public class AHRS_class //implements Loggable
{
	public static final int msgID = 163;
	public float	 omegaIx;		 	// X gyro drift estimate rad/s
	public float	 omegaIy;		 	// Y gyro drift estimate rad/s
	public float	 omegaIz;		 	// Z gyro drift estimate rad/s
	public float	 accel_weight; 	// average accel_weight
	public float	 renorm_val;	 	// average renormalisation value
	public float	 error_rp;	 	// average error_roll_pitch value
	public float	 error_yaw;	 	// average error_yaw value

	private packet rcvPacket;
	private packet sndPacket;

	public AHRS_class()
	{
		rcvPacket = new packet(msgID);
		sndPacket = new packet(msgID);
	}

	public AHRS_class(AHRS_class o)
	{
		omegaIx = o.omegaIx;
		omegaIy = o.omegaIy;
		omegaIz = o.omegaIz;
		accel_weight = o.accel_weight;
		renorm_val = o.renorm_val;
		error_rp = o.error_rp;
		error_yaw = o.error_yaw;
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

			// int[] mavLen = {4, 4, 4, 4, 4, 4, 4};
			// int[] javLen = {4, 4, 4, 4, 4, 4, 4};

			omegaIx		= rcvPacket.getFloat();
			omegaIy		= rcvPacket.getFloat();
			omegaIz		= rcvPacket.getFloat();
			accel_weight	= rcvPacket.getFloat();
			renorm_val	= rcvPacket.getFloat();
			error_rp		= rcvPacket.getFloat();
			error_yaw	= rcvPacket.getFloat();
		}
		return(pstatus);
	}

	public byte[] encode()
	{
		return encode(
					  omegaIx
					 ,omegaIy
					 ,omegaIz
					 ,accel_weight
					 ,renorm_val
					 ,error_rp
					 ,error_yaw
					 );
	}

	public byte[] encode(
						 float v_omegaIx
						,float v_omegaIy
						,float v_omegaIz
						,float v_accel_weight
						,float v_renorm_val
						,float v_error_rp
						,float v_error_yaw
						)
	{
		// int[] mavLen = {4, 4, 4, 4, 4, 4, 4};
		// int[] javLen = {4, 4, 4, 4, 4, 4, 4};

		sndPacket.setSndSeq();

		sndPacket.resetDataIdx();
		sndPacket.putFloat(v_omegaIx);	// Add "omegaIx" parameter
		sndPacket.putFloat(v_omegaIy);	// Add "omegaIy" parameter
		sndPacket.putFloat(v_omegaIz);	// Add "omegaIz" parameter
		sndPacket.putFloat(v_accel_weight);	// Add "accel_weight" parameter
		sndPacket.putFloat(v_renorm_val);	// Add "renorm_val" parameter
		sndPacket.putFloat(v_error_rp);	// Add "error_rp" parameter
		sndPacket.putFloat(v_error_yaw);	// Add "error_yaw" parameter

		// encode the checksum

		sndPacket.putChkSum();

		return sndPacket.getPacket();
	}

	public String getLogHeader()
	{
		String param = (
				  "  time"
 				+ ", AHRS_omegaIx"
 				+ ", AHRS_omegaIy"
 				+ ", AHRS_omegaIz"
 				+ ", AHRS_accel_weight"
 				+ ", AHRS_renorm_val"
 				+ ", AHRS_error_rp"
 				+ ", AHRS_error_yaw"
				);
		return param;
	}

    public String getLogData()
	{
		String param = (
				System.currentTimeMillis()
 				+ ", " + omegaIx
 				+ ", " + omegaIy
 				+ ", " + omegaIz
 				+ ", " + accel_weight
 				+ ", " + renorm_val
 				+ ", " + error_rp
 				+ ", " + error_yaw
				);
		return param;
	}
}
